//
// Created by Shiki on 2022/6/22.
//

#include "audio_encoder.h"
#include "android_log.h"

using namespace std;

AudioEncoder::AudioEncoder(const AVCodec *codec) : codec_(codec) {

}
AudioEncoder::~AudioEncoder() {

}

std::unique_ptr<AudioEncoder> FindAudioEncoder(const char *codec_name) {
  AVCodecID audio_codec_id = AV_CODEC_ID_NONE;
  if (strcasecmp(codec_name, "MP3")==0) {
	audio_codec_id = AV_CODEC_ID_MP3;
	LOGE ("codec id is:MP3");
  } else if (strcasecmp(codec_name, "AAC")==0) {
	audio_codec_id = AV_CODEC_ID_AAC;
	LOGE ("codec id is:AAC");
  } else {
	LOGE ("Error:invalid audio format");
	return nullptr;
  }
  auto codec = avcodec_find_encoder(audio_codec_id);
  if (!codec) {
	LOGE ("Error:could not find codec");
	return nullptr;
  }
  return make_unique<AudioEncoder>(codec);
}

