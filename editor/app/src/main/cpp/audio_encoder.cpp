//
// Created by Shiki on 2022/6/22.
//

#include "audio_encoder.h"
#include "android_log.h"

using namespace std;

AudioEncoder::AudioEncoder(const AVCodec *codec) : codec_(codec) {

}
AudioEncoder::~AudioEncoder() {
  DestroyAudioEncoder();
}
AVCodecID AudioEncoder::codec_id() const {
  if (codec_) {
	return codec_->id;
  }
  return AV_CODEC_ID_NONE;
}
AVCodecContext *AudioEncoder::codec_ctx() const {
  return codec_ctx_;
}
int32_t AudioEncoder::OpenCodecCtx() {
  int32_t result = -1;
  if (!codec_ctx_) {
	codec_ctx_ = avcodec_alloc_context3(codec_);
  }
  if (!codec_ctx_) {
	LOGE("Error:could not alloc codec");
	return result;
  }
  avcodec_close(codec_ctx_);
  codec_ctx_->bit_rate = 128000;
  codec_ctx_->sample_fmt = AV_SAMPLE_FMT_FLTP;
  codec_ctx_->sample_rate = 44100;
  codec_ctx_->channel_layout = AV_CH_LAYOUT_STEREO;
  codec_ctx_->channels = 2;

  result = avcodec_open2(codec_ctx_, codec_, nullptr);
  if (result < 0) {
	LOGE("Error:could not open codec")
	return result;
  }

  return result;
}
void AudioEncoder::DestroyAudioEncoder() {
  DestroyFrame();
  if (codec_ctx_) {
	avcodec_free_context(&codec_ctx_);
	codec_ctx_ = nullptr;
  }
}
AVFrame *AudioEncoder::CreateFrame() {
  if (frame_) {
	DestroyFrame();
  }
  if (codec_ctx_) {
	frame_ = av_frame_alloc();
	if (frame_) {
	  frame_->nb_samples = codec_ctx_->frame_size;
	  frame_->format = codec_ctx_->sample_fmt;
	  frame_->channel_layout = codec_ctx_->channel_layout;
	  av_frame_get_buffer(frame_, 0);
	}
  }
  return frame_;
}
void AudioEncoder::DestroyFrame() {
  if (frame_) {
	av_frame_unref(frame_);
	av_frame_free(&frame_);
	frame_ = nullptr;
  }
}
AVFrame *AudioEncoder::frame() const {
  return frame_;
}

std::unique_ptr<AudioEncoder> FindAudioEncoder(const char *codec_name) {
  AVCodecID audio_codec_id = AV_CODEC_ID_NONE;
  if (strcasecmp(codec_name, "MP3")==0) {
	audio_codec_id = AV_CODEC_ID_MP3;
	LOGE ("codec codec_id is:MP3");
  } else if (strcasecmp(codec_name, "AAC")==0) {
	audio_codec_id = AV_CODEC_ID_AAC;
	LOGE ("codec codec_id is:AAC");
  } else {
	LOGE ("Error:invalid audio format");
	return nullptr;
  }
  auto codec = avcodec_find_encoder(audio_codec_id);
  if (!codec) {
	LOGE ("Error:could not find codec");
	return nullptr;
  }
  return make_unique<AudioEncoder>(codec);
}

