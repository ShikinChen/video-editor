### 基于ijkplayer编译ffmpeg
```
git clone https://github.com/ShikinChen/video-editor
cd video-editor

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