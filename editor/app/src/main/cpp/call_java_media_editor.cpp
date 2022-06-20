//
// Created by Shiki on 2022/6/13.
//

#include "call_java_media_editor.h"
#include "android_log.h"

CallJavaMediaEditor::CallJavaMediaEditor(JavaVM *java_vm, JNIEnv *jni_env, jobject *j_obj)
	: java_vm_(java_vm), jni_env_(jni_env) {
  j_obj_ = jni_env->NewGlobalRef(*j_obj);
  if (!j_obj_) {
	LOGE("NewGlobalRef fail")
	return;
  }
  jclass j_clz = jni_env->GetObjectClass(j_obj_);
  jmid_dumpImageListCallback_ = jni_env->GetMethodID(j_clz, "dumpImageListCallback", "(Ljava/lang/String;I)V");
}
CallJavaMediaEditor::~CallJavaMediaEditor() {
  if (jni_env_ && j_obj_) {
	jni_env_->DeleteGlobalRef(j_obj_);
	j_obj_ = nullptr;
  }
}
void CallJavaMediaEditor::DumpImageListCallback(const char *filename, int index, ThreadType thread_type) {
  switch (thread_type) {
	case Main: {
	  jstring j_filename = jni_env_->NewStringUTF(filename);
	  jni_env_->CallVoidMethod(j_obj_, jmid_dumpImageListCallback_, filename, index);
	  jni_env_->DeleteLocalRef(j_filename);
	}
	  break;
	case Other: {
	  JNIEnv *jni_env;
	  java_vm_->AttachCurrentThread(&jni_env, nullptr);
	  if (!jni_env) {
		LOGE("AttachCurrentThread fail")
		return;
	  }
	  jstring j_filename = jni_env->NewStringUTF(filename);
	  jni_env->CallVoidMethod(j_obj_, jmid_dumpImageListCallback_, j_filename, index);
	  jni_env->DeleteLocalRef(j_filename);
	  java_vm_->DetachCurrentThread();
	}
	  break;
  }
}
