//
// Created by Shiki on 2022/6/21.
//

#include "muxer.h"
#include "android_log.h"
#include "media_utils.h"
#include <vector>

using namespace std;

Muxer::Muxer() {

}
Muxer::~Muxer() {
  DestroyMuxer();
}
int32_t Muxer::InitMuxer(const std::shared_ptr<Media> &media) {
  media_=media;
  video_encoder_ = FindVideoEncoder();
  if (!video_encoder_) {
	LOGD("Error:FindVideoEncoder fail")
	return -1;
  }

  audio_encoder_ = FindAudioEncoder();
  if (!audio_encoder_) {
	LOGD("Error:FindAudioEncoder fail")
	return -1;
  }

  pkt_ = av_packet_alloc();
  if (!pkt_) {
	LOGD("Error:could not alloc packet")
	return -1;
  }
  return 0;
}
void Muxer::Muxing(int64_t start_time, int64_t end_time, const char *out_filename,std::function<void(const char *out_filename,int64_t curr_millisecond, int64_t total_millisecond)> callback) {
  AVFormatContext *output_fmt_ctx = nullptr;
  int64_t total_millisecond=end_time-start_time;
  const char *filename= strdup(out_filename);
  if(callback){
	callback(filename,total_millisecond,total_millisecond);
  }
  if (media_) {
	int64_t start_pts=0;

	avformat_alloc_output_context2(&output_fmt_ctx, nullptr, nullptr, filename);
	if (!output_fmt_ctx) {
	  LOGE("Error:could not avformat_alloc_output_context2 out_filename:%s",filename)
	  return;
	}

	AVOutputFormat *fmt = output_fmt_ctx->oformat;

	video_encoder_->OpenCodecCtx(media_->video_stream());
	audio_encoder_->OpenCodecCtx();

	if (fmt->video_codec!=video_encoder_->codec_id()) {
	  fmt->video_codec = video_encoder_->codec_id();
	}
	if (fmt->audio_codec!=audio_encoder_->codec_id()) {
	  fmt->audio_codec = audio_encoder_->codec_id();
	}
	LOGD("video codec id:%u\taudio codec codec_id:%u", fmt->video_codec, fmt->audio_codec)

	//创建新的视频流
	AVStream *video_stream = avformat_new_stream(output_fmt_ctx, nullptr);

	if (!video_stream) {
	  LOGE("Error:could not avformat_new_stream for video")
	  goto destroy;
	}

	int out_video_st_idx=video_stream->index;
	video_stream->time_base = media_->video_stream()->time_base;
	video_stream->avg_frame_rate = media_->video_stream()->avg_frame_rate;
	//复制AVCodecParameters中的编解码参数
	int32_t result = avcodec_parameters_copy(video_stream->codecpar, media_->video_stream()->codecpar);
	if (result < 0) {
	  LOGE( "Error: copy video codec paramaters failed!")
	  goto destroy;
	}
	video_stream->id = output_fmt_ctx->nb_streams - 1;

	AVStream *audio_stream = avformat_new_stream(output_fmt_ctx, nullptr);
	if (!audio_stream) {
	  LOGE("Error:could not avformat_new_stream for audio")
	  goto destroy;
	}
	int  out_audio_st_idx = audio_stream->index;
	//复制AVCodecParameters中的编解码参数
	result = avcodec_parameters_copy(audio_stream->codecpar, media_->audio_stream()->codecpar);
	if (result < 0) {
	  LOGE("Error: copy audio codec paramaters failed!")
	  goto destroy;
	}
	audio_stream->id = output_fmt_ctx->nb_streams - 1;
	audio_stream->time_base=media_->audio_stream()->time_base;

	uint8_t nb_streams=output_fmt_ctx->nb_streams;
	vector<int64_t> dts_start(nb_streams);
	vector<int64_t> pts_start(nb_streams);

	if (!(fmt->flags & AVFMT_NOFILE)) {
	  int32_t result = avio_open(&output_fmt_ctx->pb, filename, AVIO_FLAG_WRITE);
	  if (result < 0) {
		LOGE("Error: avio_open output file failed! out_filename:%s",filename)
		goto destroy;
	  }
	}

	//分配输出文件中每一路数据流的私有数据,并将相应的数据写入文件头部
	result = avformat_write_header(output_fmt_ctx, nullptr);
	if (result < 0) {
	  LOGE("Error: avformat_write_header failed!")
	  goto destroy;
	}

	video_encoder_->CreateFrame();
	audio_encoder_->CreateFrame();

	AVCodecContext * encodec_ctx= nullptr;

	media_->decoder()->Decoding(start_time, end_time, [&](const AVFrame *frame,
			int stream_index
			,int64_t curr_millisecond) {
	  AVFrame *new_frame= nullptr;
	  AVStream * stream= nullptr;
	  if(stream_index==media_->video_stream_index()){
		new_frame=video_encoder_->frame();
		encodec_ctx=video_encoder_->codec_ctx();
		stream=video_stream;
	  } else if(stream_index==media_->audio_stream_index()){
		new_frame=audio_encoder_->frame();
		encodec_ctx=audio_encoder_->codec_ctx();
		stream=audio_stream;
	  }
	  int32_t result = av_frame_make_writable(new_frame);
	  if (result) {
		LOGE("Error:could not av_frame_make_writable\t%s", av_err2str(result))
		return result;
	  }
	  uint8_t *data;
	  int linesize=0;
	  for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i) {
		data = frame->data[i];
		linesize = frame->linesize[i];
		if (data) {
		  new_frame->data[i] = data;
		  new_frame->linesize[i] = linesize;
		} else {
		  break;
		}
	  }

	  if (pts_start[stream->id]<=0) {
		pts_start[stream->id] = frame->pts;
		LOGD("[pts_start]id:%d\tpts_start:%ld",stream->id,pts_start[stream->id])
	  }
	  if (dts_start[stream->id]<=0) {
		dts_start[stream->id] = frame->pkt_dts;
	  }

	  start_pts=pts_start[stream->id];
	  new_frame->pts = frame->pts-start_pts;
//	  LOGD("[frame]id:%d\tpts:%ld\tdts:%ld",stream->id,new_frame->pts,new_frame->pkt_dts)
//	  new_frame->pkt_dts=frame->pkt_dts-dts_start[stream->id];
	  if(encodec_ctx){
		result=	WriteFrame(output_fmt_ctx,encodec_ctx, new_frame,pkt_, stream->id,frame->pkt_duration);
	  }
	  if(callback){
		callback(filename,curr_millisecond-start_time-kMillisecondUnit/10,total_millisecond);
	  }
	  return result;
	});

	WriteFrame(output_fmt_ctx,video_encoder_->codec_ctx(), nullptr,pkt_, video_stream->id);
	WriteFrame(output_fmt_ctx,audio_encoder_->codec_ctx(), nullptr,pkt_, audio_stream->id);

	if(output_fmt_ctx){
	  av_write_trailer(output_fmt_ctx);
	}
	if(callback){
	  callback(filename,total_millisecond,total_millisecond);
	}
  }
  goto destroy;
  destroy:
  	delete filename;
  	video_encoder_->DestroyFrame();
	audio_encoder_->DestroyFrame();

	  if (output_fmt_ctx) {
		if (!(output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
		  avio_closep(&output_fmt_ctx->pb);
		}
		avformat_free_context(output_fmt_ctx);
	  }
}
int32_t Muxer::WriteFrame(AVFormatContext *output_fmt_ctx,
						  AVCodecContext *encodec_ctx,
						  const AVFrame *frame,
						  AVPacket *pkt,
						  uint8_t stream_index,
						  int64_t duration){
	int32_t result=-1;
	result=	avcodec_send_frame(encodec_ctx, frame);
	if (result < 0) {
	  LOGE("Error:could not av_frame_make_writable stream_index:%d\t%s",stream_index, av_err2str(result))
	  return result;
	}
	while (result>=0) {
	  result = avcodec_receive_packet(encodec_ctx, pkt);
	  if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
		break;
	  } else if (result < 0) {
		LOGE ("Error:avcodec_receive_packet failed  stream_index:%d\t%s", stream_index, av_err2str(result) )
		break;
	  }
	  pkt->stream_index = stream_index;
	  if (duration > 0) {
		pkt->duration = duration;
	  }
	  av_interleaved_write_frame(output_fmt_ctx, pkt);
	  av_packet_unref(pkt);
	}
	if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
	  result=0;
	}
	return result;
}
void Muxer::DestroyMuxer() {
  if (pkt_) {
	av_packet_free(&pkt_);
	pkt_ = nullptr;
  }
}
