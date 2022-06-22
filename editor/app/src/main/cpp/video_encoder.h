//
// Created by Shiki on 2022/6/22.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_VIDEO_ENCODER_H_
#define EDITOR_APP_SRC_MAIN_CPP_VIDEO_ENCODER_H_

#include <cstdint>
#include <memory>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

class VideoEncoder {
 public:
  explicit VideoEncoder(const AVCodec *codec);
  virtual ~VideoEncoder();

  int32_t InitVideoEncoder();
  void DestroyVideoEncoder();

 private:
  const AVCodec *codec_;
  AVCodecContext *codec_ctx_ = nullptr;
};

std::unique_ptr<VideoEncoder> FindVideoEncoder(const char *codec_name = "libx264");

#endif //EDITOR_APP_SRC_MAIN_CPP_VIDEO_ENCODER_H_
