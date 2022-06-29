//
// Created by Shiki on 2022/6/21.
//

#include "decoder.h"
#include "media.h"
#include "android_log.h"
#include "media_utils.h"

Decoder::Decoder(const std::shared_ptr<Media> &media) : media_(media) {

}
Decoder::~Decoder() {
  DestroyDecoder();
}

int32_t Decoder::InitDecoder(int *video_stream_index, int *audio_stream_index) {
  auto format_ctx = media_->format_ctx();
  int32_t result = OpenCodecContext(video_stream_index, &video_dec_ctx_, format_ctx, AVMEDIA_TYPE_VIDEO);
  if (result < 0) {
	LOGE("Error: OpenCodecContext AVMEDIA_TYPE_VIDEO failed.")
	return result;
  }
  result = OpenCodecContext(audio_stream_index, &audio_dec_ctx_, format_ctx, AVMEDIA_TYPE_AUDIO);
  if (result < 0) {
	LOGE("Error: OpenCodecContext AVMEDIA_TYPE_AUDIO failed.")
	return result;
  }
  return 0;
}

int32_t Decoder::OpenCodecContext(int *stream_idx,
								  AVCodecContext **dec_ctx,
								  AVFormatContext *fmt_ctx,
								  AVMediaType type) {
  int32_t result = av_find_best_stream(fmt_ctx, type, -1, -1, nullptr, 0);
  if (result < 0) {
	LOGE("Error: Could not find %s stream in input file.", av_get_media_type_string(type))
	return result;
  }
  int stream_index = result;
  result = -1;
  AVStream *st = fmt_ctx->streams[stream_index];

  AVCodec *dec = nullptr;
  bool is_h264_mediacodec = true;
  if (st->codecpar->codec_id==AV_CODEC_ID_H264) {
	//TODO 暂时无法使用硬编码
//	dec = avcodec_find_decoder_by_name("h264_mediacodec");
  }
  if (!dec) {
	is_h264_mediacodec = false;
	dec = avcodec_find_decoder(st->codecpar->codec_id);
  }

  if (!dec) {
	LOGE("Error: Failed to find codec:%s", av_get_media_type_string(type))
	return result;
  }

  LOGD("codec name:%s", dec->name)
  *dec_ctx = avcodec_alloc_context3(dec);
  if (!*dec_ctx) {
	LOGE("Error: Failed to alloc codec context:%s", av_get_media_type_string(type))
	return result;
  }
  result = avcodec_parameters_to_context(*dec_ctx, st->codecpar);
  if (result < 0) {
	LOGE("Error: Failed to copy codec parameters to decoder context.")
	return result;
  }
  if (is_h264_mediacodec) {
//	(*dec_ctx)->thread_count = 1;
  }

  result = avcodec_open2(*dec_ctx, dec, nullptr);

  if (result < 0) {
	LOGE("Error: Error: Could not open %s codec\t%s", av_get_media_type_string(type), av_err2str(result))
	return result;
  }

  *stream_idx = stream_index;
  return result;
}

void Decoder::DestroyDecoder() {
  if (audio_dec_ctx_) {
	avcodec_free_context(&audio_dec_ctx_);
	audio_dec_ctx_ = nullptr;
  }
  if (video_dec_ctx_) {
	avcodec_free_context(&video_dec_ctx_);
	video_dec_ctx_ = nullptr;
  }
}
AVCodecContext *Decoder::video_dec_ctx() const {
  return video_dec_ctx_;
}
AVCodecContext *Decoder::audio_dec_ctx() const {
  return audio_dec_ctx_;
}
void Decoder::Decoding(int64_t start_time,
					   int64_t end_time, std::function<int32_t(const AVFrame *, int, int64_t)> callback) {
  AVPacket *pkt = av_packet_alloc();
  AVFrame *frame = av_frame_alloc();
  int32_t result = -1;
  int8_t retry_count = 3;
  bool is_video_end = false;
  bool is_audio_end = false;
  //TODO 提高截取精度
  if (start_time > 0) {
	start_time -= video_dec_ctx_->gop_size*av_q2d(video_dec_ctx_->time_base)*kMillisecondUnit;
  }

  int64_t start = MediaUtils::ToAVTime(start_time);
  avformat_seek_file(media_->format_ctx(), -1, start, start, INT64_MAX, AVSEEK_FLAG_BACKWARD);

  while (av_read_frame(media_->format_ctx(), pkt) >= 0) {
	AVCodecContext *codec_ctx = nullptr;
	AVRational time_base;
	if (pkt->stream_index==media_->video_stream_index()) {
	  codec_ctx = video_dec_ctx_;
	  time_base = media_->video_stream()->time_base;
	} else if (pkt->stream_index==media_->audio_stream_index()) {
	  codec_ctx = audio_dec_ctx_;
	  time_base = media_->audio_stream()->time_base;
	}

	result = avcodec_send_packet(codec_ctx, pkt);
	while (result >= 0) {
	  result = avcodec_receive_frame(codec_ctx, frame);
	  if (result < 0) {
		if (result==AVERROR_EOF || result==AVERROR(EAGAIN)) {
		  av_frame_unref(frame);
		  break;
		}
		av_frame_unref(frame);
		LOGE("Error:during decoding:%s", av_err2str(result))
		break;
	  }

	  int64_t curr_millisecond = MediaUtils::ToMillisecond(frame->pts, time_base);
	  if ((curr_millisecond > end_time || curr_millisecond < start_time) && retry_count > 0) {
		retry_count -= 1;
		break;
	  }
	  LOGD("[frame]curr_millisecond:%ld,pts:%ld", curr_millisecond, frame->pts)
	  if (!callback || callback(frame, pkt->stream_index, curr_millisecond) < 0) {
		is_video_end = true;
		is_audio_end = true;
	  }
	  av_frame_unref(frame);
	  if (curr_millisecond >= end_time) {
		if (!is_video_end && pkt->stream_index==media_->video_stream_index()) {
		  is_video_end = true;
		}
		if (!is_audio_end && pkt->stream_index==media_->audio_stream_index()) {
		  is_audio_end = true;
		}
	  }
	}
	av_packet_unref(pkt);
	if (is_video_end && is_audio_end) {
	  break;
	}
  }
  if (pkt) {
	av_packet_free(&pkt);
  }
  if (frame) {
	av_frame_free(&frame);
  }
}
