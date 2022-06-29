//
// Created by Shiki on 2022/6/10.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_MEDIA_UTILS_H_
#define EDITOR_APP_SRC_MAIN_CPP_MEDIA_UTILS_H_

extern "C" {
#include <libavformat/avformat.h>
};

#include "constant.h"

class MediaUtils {
 public:
  static long ToMillisecondByAVStream(const AVStream *stream);
  static long ToMillisecondByAVFrame(const AVFrame *frame, AVRational time_base);

  static long ToMillisecond(long av_time, AVRational time_base);

  static double ToSecondByAVStream(const AVStream *stream);
  static double ToSecond(long av_time, AVRational time_base);

  static int64_t ToAVTimeByTimeBase(int64_t time, AVRational time_base);

  static int64_t ToAVTime(int64_t millisecond);
};

#endif //EDITOR_APP_SRC_MAIN_CPP_MEDIA_UTILS_H_
