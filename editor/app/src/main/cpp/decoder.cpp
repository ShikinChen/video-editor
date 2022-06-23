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
  AVCodec *dec = avcodec_find_decoder(st->codecpar->codec_id);
  if (!dec) {
	LOGE("Error: Failed to find codec:%s", av_get_media_type_string(type))
	return result;
  }
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
  result = avcodec_open2(*dec_ctx, dec, nullptr);
  if (result < 0) {
	LOGE("Error: Error: Could not open %s codec", av_get_media_type_string(type))
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
					   int64_t end_time, std::function<void(const AVFrame *)> callback) {
  AVPacket *pkt = av_packet_alloc();
  AVFrame *frame = av_frame_alloc();
  int32_t result = -1;

  int64_t start = MediaUtils::ToAVTime(start_time);
  if (start > 0) {
	avformat_seek_file(media_->format_ctx(), -1, start, start, INT64_MAX, AVSEEK_FLAG_BACKWARD);
  }

  while (av_read_frame(media_->format_ctx(), pkt) >= 0) {
	AVCodecContext *codec_ctx;
	if (pkt->stream_index==media_->video_stream_index()) {
	  codec_ctx = video_dec_ctx_;
	} else if (pkt->stream_index==media_->audio_stream_index()) {
	  codec_ctx = audio_dec_ctx_;
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
	  callback(frame);
	  int64_t end = frame->pts/AV_TIME_BASE;
	  LOGD("frame->pts:%ld", end);
	  av_frame_unref(frame);
	  if (end >= end_time) {
		break;
	  }
	}
	av_packet_unref(pkt);
  }
  if (pkt) {
	av_packet_free(&pkt);
  }
  if (frame) {
	av_frame_free(&frame);
  }
}