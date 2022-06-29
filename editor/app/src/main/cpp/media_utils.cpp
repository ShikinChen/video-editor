//
// Created by Shiki on 2022/6/10.
//

#include "media_utils.h"
extern "C" {

}
long MediaUtils::ToMillisecondByAVStream(const AVStream *stream) {
  return ToMillisecond(stream->duration, stream->time_base);
}

long MediaUtils::ToMillisecond(long av_time, AVRational time_base) {
  return static_cast<long >(ToSecond(av_time, time_base)*kMillisecondUnit);
}

double MediaUtils::ToSecondByAVStream(const AVStream *stream) {
  return ToSecond(stream->duration, stream->time_base);
}

double MediaUtils::ToSecond(long av_time, AVRational time_base) {
  return av_time*av_q2d(time_base);
}
int64_t MediaUtils::ToAVTime(int64_t millisecond) {
  return millisecond*AV_TIME_BASE/kMillisecondUnit;
}
long MediaUtils::ToMillisecondByAVFrame(const AVFrame *frame, AVRational time_base) {
  return ToMillisecond(frame->pts, time_base);
}
int64_t MediaUtils::ToAVTimeByTimeBase(int64_t time, AVRational time_base) {
  return time/(av_q2d(time_base)*kMillisecondUnit);
}


