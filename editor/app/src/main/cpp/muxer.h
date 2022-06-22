//
// Created by Shiki on 2022/6/21.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_MUXER_H_
#define EDITOR_APP_SRC_MAIN_CPP_MUXER_H_
#include <cstdint>
#include <memory>
#include <thread>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libswscale/swscale.h>
}

#include "media.h"

class Muxer {
 public:
  explicit Muxer();
  virtual ~Muxer();

  int32_t InitMuxer();
  void set_media_(const std::shared_ptr<Media> &media);

 private:
  AVFrame *frame_ = nullptr;
  AVPacket *pkt_ = nullptr;

  std::shared_ptr<Media> media_;
};

#endif //EDITOR_APP_SRC_MAIN_CPP_MUXER_H_
