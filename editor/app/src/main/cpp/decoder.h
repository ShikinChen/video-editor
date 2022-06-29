//
// Created by Shiki on 2022/6/21.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_DECODER_H_
#define EDITOR_APP_SRC_MAIN_CPP_DECODER_H_
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

class Media;

class Decoder {
 public:
  explicit Decoder(const std::shared_ptr<Media> &media);
  virtual ~Decoder();

  int32_t InitDecoder(int *video_stream_index, int *audio_stream_index);
  void DestroyDecoder();

  void Decoding(int64_t start_time,
				   int64_t end_time, std::function<int32_t(const AVFrame *frame,int stream_index)> callback);

  AVCodecContext *video_dec_ctx() const;
  AVCodecContext *audio_dec_ctx() const;

 private:
  int32_t OpenCodecContext(int *stream_idx, AVCodecContext **dec_ctx,
						   AVFormatContext *fmt_ctx, AVMediaType type);

 private:
  std::shared_ptr<Media> media_;

  AVCodecContext *video_dec_ctx_ = nullptr;
  AVCodecContext *audio_dec_ctx_ = nullptr;
};

#endif //EDITOR_APP_SRC_MAIN_CPP_DECODER_H_
