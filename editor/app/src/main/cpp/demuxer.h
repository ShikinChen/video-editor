//
// Created by Shiki on 2022/6/7.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_DEMUXER_H_
#define EDITOR_APP_SRC_MAIN_CPP_DEMUXER_H_

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

class Demuxer {
 public:
  explicit Demuxer();
  virtual ~Demuxer();

  int32_t InitDemuxer(const char *filename);
  void DestroyDemuxer();

  void DumpImageList(int64_t start_time,
					 int64_t end_time,
					 int img_size,
					 int img_width,
					 int img_height,
					 const char *out_filename,
					 std::function<void(const char *img_filename, int index)> callback);

  int64_t Duration();
 private:
  int32_t OpenCodecContext(int *stream_idx, AVCodecContext **dec_ctx,
						   AVFormatContext *fmt_ctx, AVMediaType type);

  void CreateImgConvertAndCodecCtx(SwsContext **img_convert_ctx, AVCodecContext **img_codec_ctx, int img_width,
								   int img_height);

 private:
  AVFormatContext *format_ctx_ = nullptr;
  AVCodecContext *video_dec_ctx_ = nullptr;
  AVCodecContext *audio_dec_ctx_ = nullptr;

  int video_stream_index_ = -1;
  int audio_stream_index_ = -1;

  AVStream *video_stream_ = nullptr;
  AVStream *audio_stream_ = nullptr;

  AVFrame *frame_ = nullptr;
  AVPacket *pkt_ = nullptr;

  std::atomic_bool is_dump_image_;
  std::mutex dump_image_mutex_;

};

#endif //EDITOR_APP_SRC_MAIN_CPP_DEMUXER_H_
