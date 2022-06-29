//
// Created by Shiki on 2022/6/8.
//

#include "media_editor.h"
#include "android_log.h"
using namespace std;

MediaEditor::MediaEditor() {
  demuxer_ = make_unique<Demuxer>();
  muxer_ = make_unique<Muxer>();
}
MediaEditor::~MediaEditor() {

}

void MediaEditor::Open(const char *filename) {
  media_ = make_shared<Media>();
  media_->Init(filename);

  demuxer_->InitDemuxer();
  demuxer_->set_media_(media_);

  muxer_->InitMuxer(media_);
}
void MediaEditor::DumpImageList(int64_t start_time,
								int64_t end_time,
								int img_size,
								int img_width,
								int img_height,
								const char *out_filename,
								std::function<void(const char *img_filename, int index)> callback) {
  thread thread_([=] {
	demuxer_->DumpImageList(start_time,
							end_time,
							img_size,
							img_width,
							img_height,
							out_filename,
							callback);
  });
  thread_.detach();
}
int64_t MediaEditor::Duration() {
  return demuxer_->Duration();
}
void MediaEditor::Save(int64_t start_time,
					   int64_t end_time,
					   const char *out_filename,
					   std::function<void(const char *)> callback) {
  thread thread_([=] {
	muxer_->Muxing(start_time,
				   end_time,
				   out_filename);
  });
  thread_.detach();

}
