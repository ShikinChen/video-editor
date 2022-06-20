//
// Created by Shiki on 2022/5/20.
//

#ifndef EDITOR_APP_SRC_MAIN_CPP_FFMPEG_FFMPEG_H_
#define EDITOR_APP_SRC_MAIN_CPP_FFMPEG_FFMPEG_H_

#define RAW_GET(field, def) (raw_ ? raw_->field : (def))
#define RAW_SET(field, val) if(raw_) raw_->field = (val)

#define RAW_GET2(cond, field, def) (m_raw && (cond) ? m_raw->field : def)
#define RAW_SET2(cond, field, val) if(m_raw && (cond)) m_raw->field = (val)

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/parseutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavformat/version.h>
}
#include "../android_log.h"

template<typename T>
struct FFWrapperPtr
{
  FFWrapperPtr() = default;
  explicit FFWrapperPtr(T *raw)
	  : raw_(raw) {}

  const T* raw() const             { return raw_; }
  T*       raw()                   { return raw_; }
  void     reset(T *raw = nullptr) { raw_ = raw; }
  bool     isNull() const          { return (raw_ == nullptr); }

  void log(int level, const char *fmt) const
  {
	LOGD(raw_, level, fmt);
  }

  template<typename... Args>
  void log(int level, const char* fmt, const Args&... args) const
  {
	LOGD(raw_, level, fmt, args...);
  }

 protected:
  T *raw_ = nullptr;
};

template<typename WrapperClass, typename T, T NoneValue = static_cast<T>(-1)>
struct PixSampleFmtWrapper
{
  constexpr PixSampleFmtWrapper() = default;
  constexpr PixSampleFmtWrapper(T fmt) noexcept : fmt_(fmt) {}

  // Access to  the stored value
  operator T() const noexcept
  {
	return fmt_;
  }

  T get() const noexcept
  {
	return fmt_;
  }

  operator T&() noexcept
  {
	return fmt_;
  }

  WrapperClass& operator=(T fmt) noexcept
  {
	fmt_ = fmt;
	return *this;
  }

  void set(T fmt) noexcept
  {
	fmt_ = fmt;
  }

  // IO Stream interface
  friend std::ostream& operator<<(std::ostream& ost, WrapperClass fmt)
  {
	ost << fmt.name();
	return ost;
  }

 protected:
  T fmt_ = NoneValue;
};
#endif //EDITOR_APP_SRC_MAIN_CPP_FFMPEG_FFMPEG_H_
