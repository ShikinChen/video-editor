//
// Created by Shiki on 2022/6/30.
//

#include "video_filter.h"

#include <sstream>
#include "../android_log.h"

using namespace std;

VideoFilter::VideoFilter(const std::shared_ptr<Media> &media) : media_(media) {

}
VideoFilter::~VideoFilter() {
  DestroyRotateFilter();
}
void VideoFilter::DestroyRotateFilter() {
  if (filter_graph_) {
	avfilter_graph_free(&filter_graph_);
	filter_graph_ = nullptr;
  }
}
int32_t VideoFilter::InitVideoFilter(const char *filter_descr) {
  int32_t result = -1;
  const AVRational time_base = media_->video_stream()->time_base;
  const AVRational sample_aspect_ratio = media_->video_stream()->sample_aspect_ratio;

  const AVFilter *buffer_src = avfilter_get_by_name("buffer");
  const AVFilter *buffer_sink = avfilter_get_by_name("buffersink");

  stringstream args;

  AVFilterInOut *inputs = avfilter_inout_alloc();
  if (!inputs) {
	result = AVERROR(ENOMEM);
	LOGE("Error: avfilter_inout_alloc failed.")
	return result;
  }

  AVFilterInOut *outputs = avfilter_inout_alloc();
  if (!outputs) {
	result = AVERROR(ENOMEM);
	LOGE("Error: avfilter_inout_alloc failed.")
	goto destroy;
  }

  filter_graph_ = avfilter_graph_alloc();
  if (!filter_graph_) {
	result = AVERROR(ENOMEM);
	LOGE("Error: avfilter_graph_alloc failed.")
	goto destroy;
  }

  args << "video_size=" << media_->width() << "x" << media_->height()
	   << ":pix_fmt=" << media_->pix_fmt()
	   << ":time_base=" << time_base.num << "/" << time_base.den
	   << ":pixel_aspect=" << sample_aspect_ratio.num << "/"
	   << FFMAX(sample_aspect_ratio.den, 1);
  LOGI("filter args:%s",args.str().c_str())
  result = avfilter_graph_create_filter(&buffersrc_ctx_, buffer_src, "in", args.str().c_str(), nullptr, filter_graph_);
  if (result < 0) {
	LOGE("Error: could not create sink filter.")
	goto destroy;
  }
  result = avfilter_graph_create_filter(&buffersink_ctx_, buffer_sink, "out", nullptr, nullptr, filter_graph_);
  if (result < 0) {
	LOGE("Error: could not create sink filter.")
	goto destroy;
  }

  outputs->name = av_strdup("in");
  outputs->filter_ctx = buffersrc_ctx_;
  outputs->pad_idx = 0;
  outputs->next = nullptr;

  inputs->name = av_strdup("out");
  inputs->filter_ctx = buffersink_ctx_;
  inputs->pad_idx = 0;
  inputs->next = nullptr;

  result = avfilter_graph_parse_ptr(filter_graph_, filter_descr, &inputs, &outputs, nullptr);
  if (result < 0) {
	LOGE("Error: avfilter_graph_parse_ptr failed")
	goto destroy;
  }

  result = avfilter_graph_config(filter_graph_, nullptr);
  if (result < 0) {
	LOGE("Error: Graph config invalid.")
	goto destroy;
  }
//  goto destroy;

  destroy:
	if(inputs){
	  avfilter_inout_free(&inputs);
	}
	if(outputs){
	  avfilter_inout_free(&outputs);
	}
  	return result;
}
int32_t VideoFilter::FilterFrame(AVFrame * in_frame,AVFrame * out_frame) {
  int32_t result = av_buffersrc_add_frame_flags(buffersrc_ctx_, in_frame, AV_BUFFERSRC_FLAG_KEEP_REF);
  if (result < 0) {
	LOGE("Error: add frame to buffer src failed.")
	return result;
  }
  if(out_frame){
	av_frame_unref(out_frame);
  }
  result = av_buffersink_get_frame(buffersink_ctx_, out_frame);
  if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
  } else if (result < 0) {
	LOGE("Error: buffersink_get_frame failed.")
  }
  //将相关帧属性复制到新帧
  av_frame_copy_props(out_frame, in_frame);
  return result;
}
