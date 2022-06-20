#include <jni.h>
#include <string>
#include "android_log.h"
#include "media_editor.h"
#include "call_java_media_editor.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libavformat/avformat.h>

const char *kNativeRenderClassName = "me/shiki/editor/MediaEditor";
MediaEditor *media_editor_ = nullptr;
CallJavaMediaEditor *call_java_media_editor_ = nullptr;
JavaVM *jvm_ = nullptr;

JNIEXPORT void JNICALL Init(JNIEnv *env, jobject instance) {
  if (!media_editor_) {
	media_editor_ = new MediaEditor();
  }
  if (!call_java_media_editor_) {
	call_java_media_editor_ = new CallJavaMediaEditor(jvm_, env, &instance);
  }
}

JNIEXPORT void JNICALL Open(JNIEnv *env, jobject instance, jstring j_filename) {
  const char *filename = env->GetStringUTFChars(j_filename, nullptr);
  media_editor_->Open(filename);
  env->ReleaseStringUTFChars(j_filename, filename);
}

JNIEXPORT void JNICALL DumpImageList(JNIEnv *env,
									 jobject instance,
									 jlong j_start_time,
									 jlong j_end_time,
									 jint j_img_count, jint j_img_width, jint j_img_height, jstring j_out_filename) {
  if (media_editor_) {
	const char *out_filename = env->GetStringUTFChars(j_out_filename, nullptr);
	media_editor_->DumpImageList(j_start_time,
								 j_end_time,
								 j_img_count,
								 j_img_width,
								 j_img_height,
								 out_filename,
								 [=](const char *filename, int index) {
								   call_java_media_editor_->DumpImageListCallback(filename, index, Other);
								 });
	env->ReleaseStringUTFChars(j_out_filename, out_filename);
  }
}

JNIEXPORT jlong JNICALL Duration(JNIEnv *env,
								 jobject instance) {
  jlong duration = -1;
  if (media_editor_) {
	duration = media_editor_->Duration();
  }
  return duration;
}

JNIEXPORT void JNICALL EncodingDecodingInfo(JNIEnv *env, jobject instance) {
  void *opaque = nullptr;
  const AVCodec *c_tmp;
  while ((c_tmp = av_codec_iterate(&opaque))) {
	switch (c_tmp->type) {
	  case AVMEDIA_TYPE_VIDEO: {
		LOGD("[Video]:%s", c_tmp->name);
	  }
		break;
	  case AVMEDIA_TYPE_AUDIO: {
		LOGD("[Audio]:%s", c_tmp->name);
	  }
		break;
	  default: {
		LOGD("[Other]:%s", c_tmp->name);
	  }
		break;
	}
  }
}

static JNINativeMethod kMethods[] = {
	{"native_Init", "()V", (void *)(Init)},
	{"native_Open", "(Ljava/lang/String;)V", (void *)(Open)},
	{"native_EncodingDecodingInfo", "()V", (void *)(EncodingDecodingInfo)},
	{"native_DumpImageList", "(JJIIILjava/lang/String;)V", (void *)(DumpImageList)},
	{"native_Duration", "()J", (jlong *)(Duration)},
};

static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum) {
  LOGD("RegisterNativeMethods");
  jclass clazz = env->FindClass(className);
  if (clazz==NULL) {
	LOGD("RegisterNativeMethods fail. clazz == NULL");
	return JNI_FALSE;
  }
  if (env->RegisterNatives(clazz, methods, methodNum) < 0) {
	LOGD("RegisterNativeMethods fail");
	return JNI_FALSE;
  }
  return JNI_TRUE;
}

JNIEXPORT JNICALL jint JNI_OnLoad(JavaVM *jvm, void *p) {
  jint jniRet = JNI_ERR;
  JNIEnv *env = nullptr;
  jvm_ = jvm;
  if (jvm->GetEnv((void **)(&env), JNI_VERSION_1_6)!=JNI_OK) {
	return jniRet;
  }

  jint regRet = RegisterNativeMethods(env, kNativeRenderClassName, kMethods,
									  sizeof(kMethods)/
										  sizeof(kMethods[0]));
  if (regRet!=JNI_TRUE) {
	return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM *jvm, void *reserved) {
  jvm_ = nullptr;
  delete media_editor_;
  delete call_java_media_editor_;
}

#ifdef __cplusplus
}
#endif