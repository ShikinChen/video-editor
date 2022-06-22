//
// Created by Shiki on 2022/6/22.
//

#include "video_encoder.h"
#include "android_log.h"

using namespace std;

VideoEncoder::VideoEncoder(const AVCodec *codec) : codec_(codec) {

}
VideoEncoder::~VideoEncoder() {

}
int32_t VideoEncoder::InitVideoEncoder() {
  return NULL;
}
void VideoEncoder::DestroyVideoEncoder() {

}

std::unique_ptr<VideoEncoder> FindVideoEncoder(const char *codec_name) {
  if (strlen(codec_name) <= 0) {
	LOGE("Error:empty codec name:%s", codec_name);
	return nullptr;
  }
  const AVCodec *codec = avcodec_find_encoder_by_name(codec_name);
  if (!codec) {
	return nullptr;
  }
  return make_unique<VideoEncoder>(codec);
}
