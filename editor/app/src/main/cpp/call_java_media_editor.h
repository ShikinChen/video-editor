//
// Created by Shiki on 2022/6/13.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_CALL_JAVA_MEDIA_EDITOR_H_
#define EDITOR_APP_SRC_MAIN_CPP_CALL_JAVA_MEDIA_EDITOR_H_

#include <jni.h>
enum ThreadType {
  Main,
  Other
};
class CallJavaMediaEditor {
 public:
  CallJavaMediaEditor(JavaVM *java_vm, JNIEnv *jni_env, jobject *j_obj);
  virtual ~CallJavaMediaEditor();

  void DumpImageListCallback(const char *filename, int index, ThreadType thread_type = Main);
  void MuxingListCallback(const char *filename,
						  int64_t curr_millisecond,
						  int64_t total_millisecond,
						  ThreadType thread_type = Main);

 private:
  JavaVM *java_vm_ = nullptr;
  JNIEnv *jni_env_ = nullptr;
  jobject j_obj_ = nullptr;

  jmethodID jmid_dumpImageListCallback_;
  jmethodID jmid_muxingCallback_;
};

#endif //EDITOR_APP_SRC_MAIN_CPP_CALL_JAVA_MEDIA_EDITOR_H_
