//
// Created by Shiki on 2022/6/22.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_AUDIO_ENCODER_H_
#define EDITOR_APP_SRC_MAIN_CPP_AUDIO_ENCODER_H_

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>
};

class AudioEncoder {
 public:
  explicit AudioEncoder(const AVCodec *codec);
  virtual ~AudioEncoder();

  int32_t OpenCodecCtx();
  void DestroyAudioEncoder();

  AVFrame * CreateFrame();
  void DestroyFrame();

  AVCodecContext *codec_ctx() const;
  AVCodecID codec_id() const;

  AVFrame *frame() const;
 private:
  AVFrame *frame_ = nullptr;
  const AVCodec *codec_;
  AVCodecContext *codec_ctx_ = nullptr;
};

std::unique_ptr<AudioEncoder> FindAudioEncoder(const char *codec_name = "AAC");
#endif //EDITOR_APP_SRC_MAIN_CPP_AUDIO_ENCODER_H_
