# 学习ffmpeg而开发的简单视频编辑app

本身基于ijkplayer进行开发,使用ijkplayer进行预览,然后基于ijkplayer的ffmpeg进行扩展开发视频编辑功能


## 功能包括
1. 截取一段视频
2. 视频旋转

### 项目导入
直接将 editor 目录,导入Android Studio

### 基于ijkplayer编译ffmpeg(可选),本身已经有编译好的ijkplayer的arm64和armv7a库
```
git clone https://github.com/ShikinChen/video-editor
cd video-editor

#初始化项目需要第三方库
./init-android.sh

#编译ffmpeg一定要使用ndk r17
export ANDROID_NDK=NDK r17的路径

cd android/contrib
#先编译x264,如果失败需要先执行clean清除配置和缓存再编译
./compile-x264.sh clean
./compile-x264.sh arm64

#编译ffmpeg
./compile-ffmpeg.sh clean
./compile-ffmpeg.sh arm64

#会报 tools/do-compile-ffmpeg.sh: line 317: 40808 Segmentation fault: 11  make $FF_MAKE_FLAGS > /dev/null 错误再执行一次
./compile-ffmpeg.sh arm64

cd ..
./compile-ijk.sh arm64

#执行完compile-ijk.sh arm64 编译完ijk后 直接将 ./editor 导入Android Studio

```
