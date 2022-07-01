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
#include "video_encoder.h"
#include "audio_encoder.h"

#include "call_java_media_editor.h"

class Muxer {
 public:
  explicit Muxer();
  virtual ~Muxer();

  int32_t InitMuxer(const std::shared_ptr<Media> &media);
  void DestroyMuxer();

  void Muxing(int64_t start_time,
			  int64_t end_time,
			  int rotate_degrees,
			  const char *out_filename,
			  std::function<void(const char *out_filename,
								 int64_t curr_millisecond,
								 int64_t total_millisecond)> callback);

 private:
  int32_t WriteFrame(AVFormatContext *output_fmt_ctx,
					 AVCodecContext *encodec_ctx,
					 const AVFrame *frame,
					 AVPacket *pkt,
					 uint8_t stream_index,
					 int64_t duration = -1
  );
 private:
//  AVFrame *frame_ = nullptr;
  AVPacket *pkt_ = nullptr;

  std::shared_ptr<Media> media_;
  std::unique_ptr<VideoEncoder> video_encoder_;
  std::unique_ptr<AudioEncoder> audio_encoder_;

};

#endif //EDITOR_APP_SRC_MAIN_CPP_MUXER_H_
