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
#include <libavformat/avformat.h>
}

class VideoEncoder {
 public:
  explicit VideoEncoder(const AVCodec *codec);
  virtual ~VideoEncoder();

  void DestroyVideoEncoder();

  AVFrame *CreateFrame();
  void DestroyFrame();

  int32_t OpenCodecCtx(const AVStream *video_stream, int width = 0, int height = 0);

  AVCodecContext *codec_ctx() const;
  AVCodecID codec_id() const;
  AVFrame *frame() const;

 private:
  AVFrame *frame_ = nullptr;
  const AVCodec *codec_;
  AVCodecContext *codec_ctx_ = nullptr;
};

std::unique_ptr<VideoEncoder> FindVideoEncoder(const char *codec_name = "libx264");

#endif //EDITOR_APP_SRC_MAIN_CPP_VIDEO_ENCODER_H_
