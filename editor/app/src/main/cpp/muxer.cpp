//
// Created by Shiki on 2022/6/21.
//

#include "muxer.h"
#include "android_log.h"

Muxer::Muxer() {

}
Muxer::~Muxer() {

}
void Muxer::set_media_(const std::shared_ptr<Media> &media) {
  media_ = media;
}
int32_t Muxer::InitMuxer() {
  pkt_ = av_packet_alloc();
  if (!pkt_) {
	LOGD("Error:could not alloc packet")
	return -1;
  }

  frame_ = av_frame_alloc();
  if (!frame_) {
	LOGD("Error:could not alloc frame")
	return -1;
  }
  return 0;
}
