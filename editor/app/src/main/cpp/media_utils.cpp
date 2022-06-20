//
// Created by Shiki on 2022/6/10.
//

#include "media_utils.h"
extern "C" {

}
long MediaUtils::ToTimeByMillisecond(const AVStream *stream) {
  return ToTimeByMillisecond(stream->duration, stream->time_base);
}

long MediaUtils::ToTimeByMillisecond(long duration, AVRational time_base) {
  return static_cast<long >(ToTimeBySecond(duration, time_base)*kMillisecondUnit);
}

double MediaUtils::ToTimeBySecond(const AVStream *stream) {
  return ToTimeBySecond(stream->duration, stream->time_base);
}

double MediaUtils::ToTimeBySecond(long duration, AVRational time_base) {
  return duration*av_q2d(time_base);
}
int64_t MediaUtils::ToAVTime(int64_t millisecond) {
  return millisecond*AV_TIME_BASE/MediaUtils::kMillisecondUnit;
}

