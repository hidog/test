#ifndef DECODE_H
#define DECODE_H


extern "C" {

#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

}


int output_video_frame(AVFrame* frame);
int output_audio_frame(AVFrame* frame);
int open_codec_context( int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, AVMediaType type );
int get_format_from_sample_fmt( const char **fmt, enum AVSampleFormat sample_fmt );




int     decode_test();





#endif