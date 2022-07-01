//
// Created by Shiki on 2022/6/30.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_FILTER_VIDEO_FILTER_H_
#define EDITOR_APP_SRC_MAIN_CPP_FILTER_VIDEO_FILTER_H_
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>

extern "C" {
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
};

#include "../media.h"

class VideoFilter {
 public:
  explicit VideoFilter(const std::shared_ptr<Media> &media);
  virtual ~VideoFilter();

  int32_t InitVideoFilter(const char *filter_descr);
  void DestroyRotateFilter();

  int32_t FilterFrame(AVFrame * in_frame,AVFrame * out_frame);

 private:
  AVFilterContext *buffersink_ctx_ = nullptr;
  AVFilterContext *buffersrc_ctx_ = nullptr;
  AVFilterGraph *filter_graph_ = nullptr;

  std::shared_ptr<Media> media_;
};

#endif //EDITOR_APP_SRC_MAIN_CPP_FILTER_VIDEO_FILTER_H_
