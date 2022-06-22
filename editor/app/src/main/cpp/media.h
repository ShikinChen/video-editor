//
// Created by Shiki on 2022/6/21.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_MEDIA_H_
#define EDITOR_APP_SRC_MAIN_CPP_MEDIA_H_

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
#include "decoder.h"

class Media : public std::enable_shared_from_this<Media> {
 public:
  explicit Media();
  virtual ~Media();

  int32_t Init(const char *filename);
  void Destroy();

  AVFormatContext *format_ctx() const;
  int video_stream_index() const;
  int audio_stream_index() const;
  AVStream *video_stream() const;
  AVStream *audio_stream() const;

  const std::unique_ptr<Decoder> &decoder() const;
 private:
  AVFormatContext *format_ctx_ = nullptr;

  std::unique_ptr<Decoder> decoder_;

  int video_stream_index_ = -1;
  int audio_stream_index_ = -1;

  AVStream *video_stream_ = nullptr;
  AVStream *audio_stream_ = nullptr;
};

#endif //EDITOR_APP_SRC_MAIN_CPP_MEDIA_H_
