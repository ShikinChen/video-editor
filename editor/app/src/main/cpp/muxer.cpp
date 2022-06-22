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
  video_encoder_ = FindVideoEncoder();
  if (!video_encoder_) {
	LOGD("Error:FindVideoEncoder fail")
	return -1;
  }
  audio_encoder_ = FindAudioEncoder();
  if (!audio_encoder_) {
	LOGD("Error:FindAudioEncoder fail")
	return -1;
  }
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
void Muxer::Cut(int64_t start_time, int64_t end_time, const char *out_filename) {
  if (media_) {
	media_->decoder()->Decoding(start_time, end_time, [=](const AVFrame *frame) {

	});
  }
}
