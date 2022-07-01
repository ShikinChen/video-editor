//
// Created by Shiki on 2022/6/8.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_MEDIA_EDITOR_H_
#define EDITOR_APP_SRC_MAIN_CPP_MEDIA_EDITOR_H_
#include <memory>
#include "demuxer.h"
#include "muxer.h"

class MediaEditor {
 public:
  explicit MediaEditor();
  virtual ~MediaEditor();

  void Open(const char *filename);

  void DumpImageList(int64_t start_time, int64_t end_time, int img_size, int img_width,
					 int img_height,
					 const char *out_filename, std::function<void(const char *img_filename, int index)> callback);

  void Save(int64_t start_time,
			int64_t end_time,
			int rotate_degrees,
			const char *out_filename,
			std::function<void(const char *out_filename,
							   int64_t curr_millisecond,
							   int64_t total_millisecond)> callback);

  int64_t Duration();

 private:
  std::unique_ptr<Demuxer> demuxer_;
  std::unique_ptr<Muxer> muxer_;

  std::shared_ptr<Media> media_;

};

#endif //EDITOR_APP_SRC_MAIN_CPP_MEDIA_EDITOR_H_
