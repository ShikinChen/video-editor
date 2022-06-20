//
// Created by Shiki on 2022/6/10.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_MEDIA_UTILS_H_
#define EDITOR_APP_SRC_MAIN_CPP_MEDIA_UTILS_H_

extern "C" {
#include <libavformat/avformat.h>
};

class MediaUtils {
 public:
  static long ToTimeByMillisecond(const AVStream *stream);
  static long ToTimeByMillisecond(long duration, AVRational time_base);

  static double ToTimeBySecond(const AVStream *stream);
  static double ToTimeBySecond(long duration, AVRational time_base);

  static int64_t ToAVTime(int64_t millisecond);
 public:
  const static long kMillisecondUnit = 1000;
};

#endif //EDITOR_APP_SRC_MAIN_CPP_MEDIA_UTILS_H_
