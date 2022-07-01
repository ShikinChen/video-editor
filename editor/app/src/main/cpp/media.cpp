//
// Created by Shiki on 2022/6/21.
//

#include "media.h"
#include "android_log.h"
#include "constant.h"

Media::Media() {

}
Media::~Media() {
  Destroy();
}
int32_t Media::Init(const char *filename) {
  int result = -1;
  if (strlen(filename)==0) {
	LOGE("Error: empty input file name.")
	return result;
  }
  result = avformat_open_input(&format_ctx_, filename, nullptr, nullptr);
  if (result < 0) {
	LOGE("Error: avformat_open_input failed.")
	return result;
  }
  result = avformat_find_stream_info(format_ctx_, nullptr);
  if (result < 0) {
	LOGE("Error: avformat_find_stream_info failed.")
	return result;
  }

  decoder_ = make_unique<Decoder>(shared_from_this());
  result = decoder_->InitDecoder(&video_stream_index_, &audio_stream_index_);
  if (result < 0) {
	LOGE("Error: InitDecoder failed.")
	return result;
  }
  video_stream_ = format_ctx_->streams[video_stream_index_];
  audio_stream_ = format_ctx_->streams[audio_stream_index_];
  av_dump_format(format_ctx_, 0, filename, 0);

  return result;
}
void Media::Destroy() {
  if (format_ctx_) {
	avformat_close_input(&format_ctx_);
	format_ctx_ = nullptr;
  }
}

AVFormatContext *Media::format_ctx() const {
  return format_ctx_;
}
int Media::video_stream_index() const {
  return video_stream_index_;
}
int Media::audio_stream_index() const {
  return audio_stream_index_;
}
AVStream *Media::video_stream() const {
  return video_stream_;
}
AVStream *Media::audio_stream() const {
  return audio_stream_;
}
const std::unique_ptr<Decoder> &Media::decoder() const {
  return decoder_;
}

int64_t Media::Duration() {
  if (format_ctx_) {
	return format_ctx_->duration/kMillisecondUnit;
  }
  return -1;
}

int Media::width() const {
  if (!video_stream_) {
	return 0;
  }
  return video_stream_->codecpar->width;
}

int Media::height() const {
  if (!video_stream_) {
	return 0;
  }
  return video_stream_->codecpar->height;
}
AVPixelFormat Media::pix_fmt() const {
  if (!video_stream_) {
	return AVPixelFormat::AV_PIX_FMT_NONE;
  }
  return static_cast<AVPixelFormat>(video_stream_->codecpar->format);
}
