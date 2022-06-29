//
// Created by Shiki on 2022/6/22.
//

#include "video_encoder.h"
#include "android_log.h"
#include "constant.h"

using namespace std;

VideoEncoder::VideoEncoder(const AVCodec *codec) : codec_(codec) {

}
VideoEncoder::~VideoEncoder() {
  DestroyVideoEncoder();
}
void VideoEncoder::DestroyVideoEncoder() {
  DestroyFrame();
  if (codec_ctx_) {
	avcodec_free_context(&codec_ctx_);
	codec_ctx_ = nullptr;
  }
}
AVCodecID VideoEncoder::codec_id() const {
  if (codec_) {
	return codec_->id;
  }
  return AV_CODEC_ID_NONE;
}
AVCodecContext *VideoEncoder::codec_ctx() const {
  return codec_ctx_;
}
int32_t VideoEncoder::OpenCodecCtx(const AVStream *video_stream) {
  int32_t result = -1;
  if (!codec_ctx_) {
	codec_ctx_ = avcodec_alloc_context3(codec_);
  }
  if (!codec_ctx_) {
	LOGE("Error:could not alloc codec");
	return result;
  }
  avcodec_close(codec_ctx_);

  codec_ctx_->profile = FF_PROFILE_H264_HIGH;
  codec_ctx_->level = 50;

  codec_ctx_->gop_size = 50;

  codec_ctx_->max_b_frames = 3;

  codec_ctx_->bit_rate = 1000000;

  if (codec_->id==AV_CODEC_ID_H264) {
	av_opt_set(codec_ctx_->priv_data, "preset", "slow", 0);
//	av_opt_set(codec_ctx_->priv_data, "tune", "zerolatency", 0);
  }

  auto codecpar = video_stream->codecpar;
  codec_ctx_->width = codecpar->width;
  codec_ctx_->height = codecpar->height;
  codec_ctx_->pix_fmt = static_cast<AVPixelFormat>(codecpar->format);

  codec_ctx_->time_base = video_stream->time_base;
  codec_ctx_->framerate = video_stream->avg_frame_rate;

  result = avcodec_open2(codec_ctx_, codec_, nullptr);
  if (result < 0) {
	LOGE("Error:could not open codec:%s", av_err2str(result))
	return result;
  }
  return result;
}
AVFrame *VideoEncoder::CreateFrame() {
  if (frame_) {
	DestroyFrame();
  }
  if (codec_ctx_) {
	frame_ = av_frame_alloc();
	if (frame_) {
	  frame_->width = codec_ctx_->width;
	  frame_->height = codec_ctx_->height;
	  frame_->format = codec_ctx_->pix_fmt;
	  av_frame_get_buffer(frame_, 0);
	}
  }
  return frame_;
}
void VideoEncoder::DestroyFrame() {
  if (frame_) {
	av_frame_free(&frame_);
	frame_ = nullptr;
  }
}
AVFrame *VideoEncoder::frame() const {
  return frame_;
}

std::unique_ptr<VideoEncoder> FindVideoEncoder(const char *codec_name) {
  if (strlen(codec_name) <= 0) {
	LOGE("Error:empty codec name:%s", codec_name);
	return nullptr;
  }
  const AVCodec *codec = avcodec_find_encoder_by_name(codec_name);
  if (!codec) {
	return nullptr;
  }
  return make_unique<VideoEncoder>(codec);
}
