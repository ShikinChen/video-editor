//
// Created by Shiki on 2022/6/7.
//

#include "demuxer.h"

#include <iostream>
#include <cstring>

extern "C" {
#include <libavutil/channel_layout.h>
}
#include "android_log.h"
#include "media_utils.h"

using namespace std;

Demuxer::Demuxer() {

}
Demuxer::~Demuxer() {
  DestroyDemuxer();
}

int32_t Demuxer::InitDemuxer() {
  pkt_ = av_packet_alloc();
  if (!pkt_) {
	LOGD("Error:could not alloc packet")
	return -1;
  }

  frame_ = av_frame_alloc();
  if (!frame_) {
	LOGD("Error:could not alloc frame")
	return -1;
  }
  return 0;
}

void Demuxer::DestroyDemuxer() {
  if (frame_) {
	av_frame_free(&frame_);
	frame_ = nullptr;
  }
  if (pkt_) {
	av_packet_free(&pkt_);
	pkt_ = nullptr;
  }
}

void Demuxer::DumpImageList(int64_t start_time,
							int64_t end_time, int img_size,
							int img_width,
							int img_height,
							const char *out_filename,
							std::function<void(const char *img_filename, int index)> callback) {
  if (end_time <= start_time) {
	return;
  }
  char *filename = strdup(out_filename);
  LOGD("out_filename:%s", filename)
  unique_lock<mutex> lock(dump_image_mutex_);
  long time = MediaUtils::ToMillisecondByAVStream(media_->video_stream());
  AVRational time_base = media_->video_stream()->time_base;
  long step = MediaUtils::ToAVTime((end_time - start_time)/img_size);

  LOGD("time:%ld,step:%ld", time, step)

  SwsContext *frame_img_convert_ctx = nullptr;
  AVCodecContext *frame_img_codec_ctx = nullptr;
  CreateImgConvertAndCodecCtx(&frame_img_convert_ctx, &frame_img_codec_ctx, img_width, img_height);
  AVFrame *dst_frame = nullptr;
  bool is_err = false;
  int32_t result = 0;
  int bytes = 0;
  uint8_t *buffer = nullptr;
  int img_count = 0;
  int got_packet = 0;

  if (frame_img_codec_ctx==nullptr || frame_img_convert_ctx==nullptr) {
	result = -1;
  }
  if (result >= 0) {
	dst_frame = av_frame_alloc();
	if (!dst_frame) {
	  result = -1;
	  LOGE("av_frame_alloc failed")
	}
	dst_frame->format = AV_PIX_FMT_RGB24;
	dst_frame->width = img_width;
	dst_frame->height = img_height;
  }

  if (result >= 0) {
	bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, img_width, img_height, 1);
	buffer = static_cast<uint8_t *>(av_malloc(bytes*sizeof(uint8_t)));
	if (!buffer) {
	  result = -1;
	  LOGE("av_image_get_buffer_size failed")
	}
  }

  if (result >= 0) {
	result =
		av_image_fill_arrays(dst_frame->data, dst_frame->linesize, buffer, AV_PIX_FMT_RGB24, img_width, img_height, 1);
	if (result < 0) {
	  LOGE("av_image_fill_arrays failed")
	}
  }

  int64_t start = MediaUtils::ToAVTime(start_time);
  if (start > 0) {
	avformat_seek_file(media_->format_ctx(), -1, start, start, INT64_MAX, AVSEEK_FLAG_BACKWARD);
  }
  while (result >= 0 && av_read_frame(media_->format_ctx(), pkt_) >= 0) {
	if (pkt_->stream_index==media_->video_stream_index()) {
	  int32_t ret = avcodec_send_packet(media_->decoder()->video_dec_ctx(), pkt_);
	  if (ret < 0) {
		LOGE("Error: avcodec_send_packet failed\t%s.", av_err2str(ret))
		break;
	  }
	  while (ret >= 0 && img_count < img_size) {
		ret = avcodec_receive_frame(media_->decoder()->video_dec_ctx(), frame_);
		if (ret < 0) {
		  if (ret==AVERROR_EOF || ret==AVERROR(EAGAIN)) {
			av_frame_unref(frame_);
			av_packet_unref(pkt_);
			break;
		  }
		  av_packet_unref(pkt_);
		  av_frame_unref(frame_);
		  LOGE("Error:during decoding:%s", av_err2str(ret))
		  break;
		}
		if (frame_->pict_type==AV_PICTURE_TYPE_I) {
		  LOGD("pict_type:AV_PICTURE_TYPE_I")
		  long frame_time = MediaUtils::ToMillisecond(frame_->pts, time_base);
		  LOGD("frame_time:%ld,start:%ld", frame_time, start)
		  img_count++;

		  ret = sws_scale(frame_img_convert_ctx,
						  frame_->data,
						  frame_->linesize,
						  0,
						  frame_->height,
						  dst_frame->data,
						  dst_frame->linesize);
		  if (ret < 0) {
			LOGE("sws_scale failed")
			break;
		  }
		  ret = avcodec_send_frame(frame_img_codec_ctx, dst_frame);
		  if (ret < 0) {
			LOGE("avcodec_send_frame failed")
			break;
		  }
		  ret = avcodec_receive_packet(frame_img_codec_ctx, pkt_);
		  if (ret < 0) {
			LOGE("avcodec_receive_packet failed")
			break;
		  }
		  string file_path(filename);
		  file_path.append("/");
		  file_path.append(to_string(img_count));
		  file_path.append(".png");
		  FILE *file = fopen(file_path.c_str(), "wb");
		  if (!file) {
			LOGE("open file failed")
			break;
		  }
		  fwrite(pkt_->data, 1, pkt_->size, file);
		  fclose(file);
		  callback(file_path.c_str(), img_count - 1);
		  av_frame_unref(frame_);
		  start += step;
		  LOGD("start:%ld", start)
		  avformat_seek_file(media_->format_ctx(), -1, start, start, INT64_MAX, AVSEEK_FLAG_BACKWARD);
		} else if (frame_->pict_type==AV_PICTURE_TYPE_B) {
		  LOGD("pict_type:AV_PICTURE_TYPE_B")
		} else if (frame_->pict_type==AV_PICTURE_TYPE_P) {
		  LOGD("pict_type:AV_PICTURE_TYPE_P")
		}

	  }
	}
  }

  if (frame_img_codec_ctx) {
	avcodec_close(frame_img_codec_ctx);
  }
  if (filename && strlen(filename) > 0) {
	free(filename);
  }
  if (dst_frame) {
	av_frame_free(&dst_frame);
  }
  av_packet_unref(pkt_);
  lock.unlock();
}

void Demuxer::CreateImgConvertAndCodecCtx(SwsContext **img_convert_ctx, AVCodecContext **img_codec_ctx, int img_width,
										  int img_height) {
  *img_convert_ctx =
	  sws_getContext(media_->video_stream()->codecpar->width,
					 media_->video_stream()->codecpar->height,
					 static_cast<AVPixelFormat>(media_->video_stream()->codecpar->format),
					 img_width,
					 img_height,
					 AV_PIX_FMT_RGB24,
					 SWS_BICUBIC,
					 nullptr,
					 nullptr,
					 nullptr);
  if (!*img_convert_ctx) {
	LOGE("sws_getContext failed")
	return;
  }
  AVCodec *encoder = avcodec_find_encoder(AV_CODEC_ID_PNG);
  if (!encoder) {
	LOGE("png avcodec_find_encoder failed")
	return;
  }
  *img_codec_ctx = avcodec_alloc_context3(encoder);
  if (!*img_codec_ctx) {
	LOGE("png avcodec_alloc_context3 failed")
	return;
  }
  (*img_codec_ctx)->bit_rate = media_->video_stream()->codecpar->bit_rate;
  (*img_codec_ctx)->width = img_width;
  (*img_codec_ctx)->height = img_height;
  (*img_codec_ctx)->pix_fmt = AV_PIX_FMT_RGB24;
  (*img_codec_ctx)->codec_type = AVMEDIA_TYPE_VIDEO;
  (*img_codec_ctx)->time_base.num = media_->video_stream()->time_base.num;
  (*img_codec_ctx)->time_base.den = media_->video_stream()->time_base.den;
  avcodec_open2(*img_codec_ctx, encoder, nullptr);
}
int64_t Demuxer::Duration() {
  return media_->Duration();
}
void Demuxer::set_media_(const shared_ptr<Media> &media) {
  media_ = media;
}
