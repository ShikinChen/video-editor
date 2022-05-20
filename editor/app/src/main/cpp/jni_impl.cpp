#include <jni.h>
#include <string>
#include "AndroidLog.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libavformat/avformat.h>
const char *kNativeRenderClassName = "me/shiki/editor/MainActivity";

JNIEXPORT void JNICALL CodecInfo(JNIEnv *env, jobject instance) {
  void *opaque = NULL;
  const AVCodec *c_tmp;
  while ((c_tmp = av_codec_iterate(&opaque))!=NULL) {
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
	{"native_CodecInfo", "()V", (void *)(CodecInfo)},
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
  JNIEnv *env = NULL;
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
#ifdef __cplusplus
}
#endif