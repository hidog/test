#include "media.h"

#include <assert.h>
#include <string.h>


int   audio_decode( Decode *dec )
{  
   int   ret   =  0;
   
   if( avcodec_send_packet( dec->audio_ctx, dec->pkt ) < 0 )
   {
      return ERROR;
   }
   else
   {
      ret  =  avcodec_receive_frame( dec->audio_ctx, dec->frame );   

      if( ret >= 0 )
         return SUCCESS;
      if( ret == AVERROR_EOF || ret == AVERROR(EAGAIN) )
         return NO_FRAME;
      else
         return ERROR;
   }
}


int   open_input( char *filename, Decode *dec )
{
   int   video_index =  -1,
         audio_index =  -1;

   AVFormatContext   *fmt_ctx       =  NULL;    
   AVStream          *video_stream  =  NULL,   
                     *audio_stream  =  NULL;
   AVCodecContext    *audio_ctx  =  NULL;
   AVFrame           *frame      =  NULL;
   AVPacket          *pkt  =  NULL;   
   const AVCodec  *codec   =  NULL;    
   
   if( avformat_open_input( &fmt_ctx, filename, NULL, NULL ) < 0 )
      return ERROR;
   
   if( avformat_find_stream_info( fmt_ctx, NULL ) < 0 ) 
      return ERROR;
   
   // audio
   audio_index =  av_find_best_stream( fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0 );
   if( audio_index < 0 )
   {
      return ERROR;
   }
   else 
   {
      audio_stream   =  fmt_ctx->streams[audio_index];
      codec          =  avcodec_find_decoder( audio_stream->codecpar->codec_id );
      if( NULL == dec )
         return ERROR;     

      audio_ctx   =  avcodec_alloc_context3(codec);
      if( NULL  == audio_ctx )
         return ERROR;
      
      if( avcodec_parameters_to_context( audio_ctx, audio_stream->codecpar ) < 0 )
         return ERROR;
      
      if( avcodec_open2( audio_ctx, codec, NULL ) < 0 )
         return ERROR;
      
      audio_ctx->pkt_timebase =  fmt_ctx->streams[audio_index]->time_base;
   }
   
   // video
   video_index =  av_find_best_stream( fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0 );
   if( video_index < 0 )
      printf( "NOTE: this file has no video stream.\n" );
   else 
      video_stream  =  fmt_ctx->streams[video_index];
   
   av_dump_format( fmt_ctx, 0, filename, 0 );
   
   // allocate frame, packet.
   frame =  av_frame_alloc();
   if( NULL == frame )
      return ERROR;
   
   pkt   =  av_packet_alloc();
   if( NULL == pkt )
      return ERROR;
   
   dec->fmt_ctx      =  fmt_ctx;
   dec->frame        =  frame;
   dec->pkt          =  pkt;
   dec->video_index  =  video_index;
   dec->audio_index  =  audio_index;
   dec->video_stream =  video_stream;
   dec->audio_stream =  audio_stream;
   dec->audio_ctx    =  audio_ctx;
   
   return SUCCESS;
}


int   close_decode( Decode *dec )
{
   avcodec_free_context(&dec->audio_ctx);
   avformat_close_input(&dec->fmt_ctx);
   av_packet_free(&dec->pkt);
   av_frame_free(&dec->frame);
   dec->video_index  =  -1;
   dec->audio_index  =  -1;
}



int open_output( char *filename,  Decode dec, Encode *enc )
{
   AVFormatContext   *fmt_ctx    =  NULL;
   AVCodecContext    *audio_ctx  =  NULL;
   AVStream    *audio_stream  =  NULL, 
               *video_stream  =  NULL;

   const AVCodec  *audio_codec   =  NULL;
   
   avformat_alloc_output_context2( &fmt_ctx, NULL, NULL, filename );
   if( NULL == fmt_ctx )
      return ERROR;
   
   // video
   if( NULL != dec.video_stream )
   {
      video_stream      =  avformat_new_stream( fmt_ctx, NULL);
      if( NULL == video_stream )
         return ERROR;
      video_stream->id  =  fmt_ctx->nb_streams - 1;      
      if( avcodec_parameters_copy( video_stream->codecpar, dec.video_stream->codecpar ) < 0 )
         return ERROR;
   }
   
   // audio
   audio_codec =  avcodec_find_encoder(AV_CODEC_ID_OPUS);
   if( NULL == audio_codec )
      return ERROR;
   
   audio_stream   =  avformat_new_stream( fmt_ctx, NULL );
   if( NULL == audio_stream ) 
      return ERROR;      
   audio_stream->id = fmt_ctx->nb_streams - 1;
   
   audio_ctx   =  avcodec_alloc_context3(audio_codec);
   if ( NULL == audio_ctx )
      return ERROR;
   
   audio_ctx->sample_fmt   =  audio_codec->sample_fmts[0];
   audio_ctx->bit_rate     =  192000;
   audio_ctx->sample_rate  =  48000;
   audio_stream->time_base =  (AVRational){ 1, audio_ctx->sample_rate };
   
   if( av_channel_layout_copy( &audio_ctx->ch_layout, &(AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO ) < 0 )
      return ERROR;
   
   if ( fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER )
      audio_ctx->flags  |= AV_CODEC_FLAG_GLOBAL_HEADER;   
   
   if( avcodec_open2( audio_ctx, audio_codec, NULL ) < 0 )
      return ERROR;

   if( avcodec_parameters_from_context( audio_stream->codecpar, audio_ctx ) < 0 )
      return ERROR;

   av_dump_format( fmt_ctx, 0, filename, 1 );
   
   const AVOutputFormat *ofmt  =  fmt_ctx->oformat;  
   if( !(ofmt->flags & AVFMT_NOFILE) ) 
   {
      if( avio_open( &fmt_ctx->pb, filename, AVIO_FLAG_WRITE) < 0 ) 
         return ERROR;       
   }
   
   if( avformat_write_header( fmt_ctx, NULL ) < 0 )
      return ERROR;
   
   // init packet, frame.
   AVPacket *pkt  =  av_packet_alloc();
   if( NULL == pkt )
      return ERROR;   
   
   AVFrame  *frame   =  av_frame_alloc();
   if( NULL == frame )
      return ERROR;
   frame->format        =  audio_ctx->sample_fmt;
   frame->sample_rate   =  audio_ctx->sample_rate;
   frame->nb_samples    =  audio_ctx->frame_size;
   if( av_channel_layout_copy( &frame->ch_layout, &audio_ctx->ch_layout ) < 0 )
      return ERROR;
   if( frame->nb_samples > 0 ) 
   {
      if( av_frame_get_buffer( frame, 0 ) < 0 ) 
         return ERROR;
   }
   if( av_frame_make_writable(frame) < 0 )
      return ERROR;

   // audio swr context
   SwrContext  *swr_ctx =  swr_alloc();
   if( NULL == swr_ctx )
      return ERROR;
   
   av_opt_set_chlayout  ( swr_ctx, "in_chlayout",     &dec.audio_ctx->ch_layout,  0 );
   av_opt_set_int       ( swr_ctx, "in_sample_rate",  dec.audio_ctx->sample_rate, 0 );
   av_opt_set_sample_fmt( swr_ctx, "in_sample_fmt",   dec.audio_ctx->sample_fmt,  0 );
   av_opt_set_chlayout  ( swr_ctx, "out_chlayout",    &audio_ctx->ch_layout,      0 );
   av_opt_set_int       ( swr_ctx, "out_sample_rate", audio_ctx->sample_rate,     0 );
   av_opt_set_sample_fmt( swr_ctx, "out_sample_fmt",  audio_ctx->sample_fmt,      0 );
   
   if( swr_init(swr_ctx) < 0 )
      return ERROR;  
   
   enc->fmt_ctx   =  fmt_ctx;
   enc->audio_ctx =  audio_ctx;
   enc->swr_ctx   =  swr_ctx;
   enc->audio_stream =  audio_stream;
   enc->video_stream =  video_stream;
   enc->sample_count =  0;
   enc->frame  =  frame;
   enc->ofmt   =  ofmt;
   enc->pkt =  pkt;

   return SUCCESS;
}


int   audio_encode( Encode *enc )
{   
   int ret;
   
   enc->frame->pts   =  av_rescale_q( enc->sample_count, (AVRational){1, enc->audio_ctx->sample_rate}, enc->audio_stream->time_base );
   enc->sample_count += enc->frame->nb_samples; 

   ret   =  avcodec_send_frame( enc->audio_ctx, enc->frame );
   if( ret < 0 )
      return ERROR;  
   ret   =  avcodec_receive_packet( enc->audio_ctx, enc->pkt );
   if( ret >= 0 )
      return SUCCESS;
   if( ret == AVERROR(EAGAIN) || ret == AVERROR_EOF )
      return NO_FRAME;
   else if( ret < 0 )
      return ERROR;
}


int   init_fifo( Decode dec, Encode enc, FifoBuffer *fifobuf )
{
   int   input_nb_samples  =  dec.audio_ctx->frame_size > 0 ? dec.audio_ctx->frame_size : 512;
   int   output_nb_samples =  enc.audio_ctx->frame_size;
   int   frame_size        =  FFMAX( input_nb_samples, output_nb_samples );
   
   // if needed, the fifo buffer will re-allocate.
   AVAudioFifo *fifo =  av_audio_fifo_alloc( enc.audio_ctx->sample_fmt, enc.audio_ctx->ch_layout.nb_channels, frame_size << 1 );
   if( NULL == fifo )
      return ERROR;  

   // if needed, the tmp_buffer will re-allocate.
   uint8_t  **tmp_buffer   =  calloc( enc.audio_ctx->ch_layout.nb_channels, sizeof(uint8_t*) );
   if( av_samples_alloc( tmp_buffer, NULL, enc.audio_ctx->ch_layout.nb_channels, input_nb_samples, enc.audio_ctx->sample_fmt, 0 ) < 0 )
      return ERROR;   
   
   fifobuf->fifo  =  fifo;
   fifobuf->tmp_buffer  =  tmp_buffer;
   fifobuf->tmp_nb_samples    =  input_nb_samples;
   fifobuf->output_nb_samples =  output_nb_samples;   
   
   return SUCCESS;
}


int   push_audio_frame( Decode dec, Encode enc, FifoBuffer fifobuf,  AVFrame *dec_audio_frame )
{  
   int64_t  delay       =  swr_get_delay( enc.swr_ctx, enc.audio_ctx->sample_rate);
   int      re_samples  =  av_rescale_rnd( delay + dec_audio_frame->nb_samples,
                                           enc.audio_ctx->sample_rate, dec.audio_ctx->sample_rate, AV_ROUND_UP );
   
   // need re-allocate.
   if( re_samples > fifobuf.tmp_nb_samples )
   {
      av_freep(&fifobuf.tmp_buffer[0]);
      if( av_samples_alloc( fifobuf.tmp_buffer, NULL, enc.audio_ctx->ch_layout.nb_channels, re_samples, enc.audio_ctx->sample_fmt, 1 ) < 0 )
         return ERROR;
      fifobuf.tmp_nb_samples   =  re_samples;
   }

   int   ret   =  swr_convert( enc.swr_ctx,
                               fifobuf.tmp_buffer, fifobuf.tmp_nb_samples,
                               (const uint8_t **)dec_audio_frame->data, dec_audio_frame->nb_samples );
   if( ret < 0 )
      return ERROR;

   if( av_audio_fifo_space(fifobuf.fifo) < ret )
   {
      // need realloc fifo.
      int   current_fifo_size =  av_audio_fifo_size(fifobuf.fifo);
      if( av_audio_fifo_realloc( fifobuf.fifo, current_fifo_size + ret ) < 0 )
         return ERROR;
   }

   int   write_size  =  av_audio_fifo_write( fifobuf.fifo, (void **)fifobuf.tmp_buffer , ret );
   if( write_size < ret )
      return ERROR;

   return SUCCESS;
}


int pop_audio_frame( Encode enc, FifoBuffer fifobuf )
{
   int   read_size   =  av_audio_fifo_read( fifobuf.fifo, (void **)enc.frame->data, fifobuf.output_nb_samples );
   if( read_size < fifobuf.output_nb_samples ) 
      return ERROR;
   return SUCCESS;
}


int   write_audio_frame( Decode dec, Encode *enc, FifoBuffer fifobuf )
{
   int   ret;

   push_audio_frame( dec, *enc, fifobuf, dec.frame );
   av_frame_unref(dec.frame);
   
   while( av_audio_fifo_size(fifobuf.fifo) >= fifobuf.output_nb_samples )
   {
      pop_audio_frame( *enc, fifobuf );
      ret   =  audio_encode( enc );
      if( ret == SUCCESS )
      {
         av_packet_rescale_ts( enc->pkt, enc->audio_ctx->time_base, enc->audio_stream->time_base );
         enc->pkt->stream_index   =  enc->audio_stream->index;
         av_interleaved_write_frame( enc->fmt_ctx, enc->pkt );
      }
   }
}


int   flush_audio( Decode dec, Encode *enc, FifoBuffer fifobuf )
{
   // flush audio decode.
   int   ret   =  avcodec_send_packet( dec.audio_ctx, NULL );
   if( ret < 0 )
      return ERROR;

   while( ret >= 0 )
   {
      ret   =  avcodec_receive_frame( dec.audio_ctx, dec.frame );
      if( ret >= 0 )
         write_audio_frame( dec, &enc, fifobuf );
      else
         break;
   }

   // flush fifo buffer
   int      sample      =  av_audio_fifo_size(fifobuf.fifo);
   if( sample > 0 )
   {
      AVFrame  *frame      =  av_frame_alloc();
      frame->nb_samples    =  sample;
      frame->format        =  enc->audio_ctx->sample_fmt;
      frame->sample_rate   =  enc->audio_ctx->sample_rate;
      av_channel_layout_copy( &frame->ch_layout, &enc->audio_ctx->ch_layout );
      av_frame_get_buffer( frame, 0 );
      if( av_frame_make_writable(frame) < 0 )
         return ERROR;

      ret   =  av_audio_fifo_read( fifobuf.fifo, (void **)frame->data, sample );
      if( ret < sample )
         return ERROR;

      frame->pts   =  av_rescale_q( enc->sample_count, (AVRational){1, enc->audio_ctx->sample_rate}, enc->audio_stream->time_base );
      enc->sample_count += sample; 

      ret   =  avcodec_send_frame( enc->audio_ctx, frame );
      if( ret < 0 )
         return ERROR;  
      ret   =  avcodec_receive_packet( enc->audio_ctx, enc->pkt );
      if( ret >= 0 )
      {
         av_packet_rescale_ts( enc->pkt, enc->audio_ctx->time_base, enc->audio_stream->time_base );
         enc->pkt->stream_index   =  enc->audio_stream->index;
         av_interleaved_write_frame( enc->fmt_ctx, enc->pkt );
      }
      av_frame_free(&frame);
   }

   // flush audio encoder.
   ret   =  avcodec_send_frame( enc->audio_ctx, NULL );
   if( ret < 0 )
      return ERROR;
   while( ret >= 0 )
   {
      ret   =  avcodec_receive_packet( enc->audio_ctx, enc->pkt );
      if( ret < 0 )
         break;
      av_packet_rescale_ts( enc->pkt, enc->audio_ctx->time_base, enc->audio_stream->time_base );
      enc->pkt->stream_index   =  enc->audio_stream->index;
      av_interleaved_write_frame( enc->fmt_ctx, enc->pkt );
   }
}


int   close_encode( Encode *enc )
{
   avcodec_free_context(&enc->audio_ctx);
   av_frame_free(&enc->frame);
   av_packet_free(&enc->pkt);
   swr_free(&enc->swr_ctx);   
   if( !(enc->ofmt->flags & AVFMT_NOFILE) )
      avio_closep(&enc->fmt_ctx->pb);
   avformat_free_context(enc->fmt_ctx);
   enc->fmt_ctx   =  NULL;
   enc->sample_count =  0;
}


int   close_fifo( FifoBuffer *fifobuf )
{
   av_audio_fifo_free(fifobuf->fifo);
   fifobuf->fifo  =  NULL;
   av_freep(&fifobuf->tmp_buffer[0]);
   free(fifobuf->tmp_buffer);
   fifobuf->tmp_buffer  =  NULL;
   fifobuf->tmp_nb_samples    =  0;
   fifobuf->output_nb_samples =  0;
}


int   open_video_input( char *filename, Decode *dec )
{
   int   video_index =  -1,
         audio_index =  -1;

   AVFormatContext   *fmt_ctx       =  NULL;    
   AVStream          *video_stream  =  NULL,   
                     *audio_stream  =  NULL;
   AVCodecContext    *audio_ctx  =  NULL;
   AVFrame           *frame      =  NULL;
   AVPacket          *pkt  =  NULL;   
   const AVCodec  *codec   =  NULL;    
   
   if( avformat_open_input( &fmt_ctx, filename, NULL, NULL ) < 0 )
      return ERROR;
   
   if( avformat_find_stream_info( fmt_ctx, NULL ) < 0 ) 
      return ERROR;
   
   // audio
   audio_index =  av_find_best_stream( fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0 );
   if( audio_index >= 0 )
   {
      fprintf( stderr, "Please input pure video stream.\n" );
      return ERROR;
   }
   
   // video
   video_index =  av_find_best_stream( fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0 );
   if( video_index < 0 )
   {
      fprintf( stderr, "There is no video stream.\n" );
      return ERROR;
   }
   else 
      video_stream  =  fmt_ctx->streams[video_index];
   
   av_dump_format( fmt_ctx, 0, filename, 0 );
   
   // allocate packet. we don't decode video, so don't need allocate frame.   
   pkt   =  av_packet_alloc();
   if( NULL == pkt )
      return ERROR;
   
   dec->fmt_ctx      =  fmt_ctx;
   dec->frame        =  frame;
   dec->pkt          =  pkt;
   dec->video_index  =  video_index;
   dec->audio_index  =  audio_index;
   dec->video_stream =  video_stream;
   dec->audio_stream =  audio_stream;
   dec->audio_ctx    =  audio_ctx;
   
   return SUCCESS;
}


int   open_g711_input( char *filename, Decode *dec, enum AVCodecID codeid, int channel, int sample_rate )
{
   int   video_index =  -1,
         audio_index =  -1;

   AVFormatContext   *fmt_ctx       =  NULL;    
   AVStream          *video_stream  =  NULL,   
                     *audio_stream  =  NULL;
   AVCodecContext    *audio_ctx  =  NULL;
   AVFrame           *frame      =  NULL;
   AVPacket          *pkt  =  NULL;   
   const AVCodec  *codec   =  NULL;    
   
   char  tmp[100];
   if( codeid == AV_CODEC_ID_PCM_MULAW )
      sprintf( tmp, "mulaw" );
   else if( codeid == AV_CODEC_ID_PCM_ALAW )
      sprintf( tmp, "alaw" );
   else
      return ERROR;
   AVInputFormat  *in_fmt  =  av_find_input_format(tmp);

   if( avformat_open_input( &fmt_ctx, filename, in_fmt, NULL ) < 0 )
      return ERROR;

   AVDictionary *format_opts = NULL, *codec_opts = NULL;

   sprintf( tmp, "%d", sample_rate );
   av_dict_set( &codec_opts, "ar", tmp, 0);

   sprintf( tmp, "%d", channel );
   av_dict_set( &codec_opts, "ac", tmp, 0);
   
   if( avformat_find_stream_info( fmt_ctx, &codec_opts ) < 0 ) 
      return ERROR;
   av_dict_free(&codec_opts);
   
   // audio
   audio_index =  av_find_best_stream( fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0 );
   if( audio_index < 0 )
   {
      fprintf( stderr, "This is not g711 format file.\n" );
      return ERROR;
   }
   else 
   {
      audio_stream   =  fmt_ctx->streams[audio_index];
      codec          =  avcodec_find_decoder( audio_stream->codecpar->codec_id );

      if( NULL == dec )
         return ERROR;     

      audio_ctx   =  avcodec_alloc_context3(codec);
      if( NULL  == audio_ctx )
         return ERROR;
      
      if( avcodec_parameters_to_context( audio_ctx, audio_stream->codecpar ) < 0 )
         return ERROR;
      
      if( avcodec_open2( audio_ctx, codec, NULL ) < 0 )
         return ERROR;
      
      audio_ctx->pkt_timebase =  fmt_ctx->streams[audio_index]->time_base;
   }
   
   // video
   video_index =  av_find_best_stream( fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0 );
   if( video_index >=0 )
   {
      fprintf( stderr, "Not g711 file.\n" );
      return ERROR;
   }
   
   av_dump_format( fmt_ctx, 0, filename, 0 );
   
   // allocate frame, packet.
   frame =  av_frame_alloc();
   if( NULL == frame )
      return ERROR;
   
   pkt   =  av_packet_alloc();
   if( NULL == pkt )
      return ERROR;
   
   dec->fmt_ctx      =  fmt_ctx;
   dec->frame        =  frame;
   dec->pkt          =  pkt;
   dec->video_index  =  video_index;
   dec->audio_index  =  audio_index;
   dec->video_stream =  video_stream;
   dec->audio_stream =  audio_stream;
   dec->audio_ctx    =  audio_ctx;
   
   return SUCCESS;
}


int   open_merge_output( char *filename, Decode video_dec, Decode audio_dec, Encode *enc )
{
   AVFormatContext   *fmt_ctx    =  NULL;
   AVCodecContext    *audio_ctx  =  NULL;
   AVStream    *audio_stream  =  NULL, 
               *video_stream  =  NULL;

   const AVCodec  *audio_codec   =  NULL;
   
   avformat_alloc_output_context2( &fmt_ctx, NULL, NULL, filename );
   if( NULL == fmt_ctx )
      return ERROR;
   
   // video
   if( NULL != video_dec.video_stream )
   {
      video_stream      =  avformat_new_stream( fmt_ctx, NULL);
      if( NULL == video_stream )
         return ERROR;
      video_stream->id  =  fmt_ctx->nb_streams - 1;      
      if( avcodec_parameters_copy( video_stream->codecpar, video_dec.video_stream->codecpar ) < 0 )
         return ERROR;
   }
   
   // audio
   audio_codec =  avcodec_find_encoder(AV_CODEC_ID_OPUS);
   if( NULL == audio_codec )
      return ERROR;
   
   audio_stream   =  avformat_new_stream( fmt_ctx, NULL );
   if( NULL == audio_stream ) 
      return ERROR;      
   audio_stream->id = fmt_ctx->nb_streams - 1;
   
   audio_ctx   =  avcodec_alloc_context3(audio_codec);
   if ( NULL == audio_ctx )
      return ERROR;
   
   audio_ctx->sample_fmt   =  audio_codec->sample_fmts[0];
   audio_ctx->bit_rate     =  192000;
   audio_ctx->sample_rate  =  48000;
   audio_stream->time_base =  (AVRational){ 1, audio_ctx->sample_rate };
   
   if( av_channel_layout_copy( &audio_ctx->ch_layout, &(AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO ) < 0 )
      return ERROR;
   
   if ( fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER )
      audio_ctx->flags  |= AV_CODEC_FLAG_GLOBAL_HEADER;   
   
   if( avcodec_open2( audio_ctx, audio_codec, NULL ) < 0 )
      return ERROR;

   if( avcodec_parameters_from_context( audio_stream->codecpar, audio_ctx ) < 0 )
      return ERROR;

   av_dump_format( fmt_ctx, 0, filename, 1 );
   
   const AVOutputFormat *ofmt  =  fmt_ctx->oformat;  
   if( !(ofmt->flags & AVFMT_NOFILE) ) 
   {
      if( avio_open( &fmt_ctx->pb, filename, AVIO_FLAG_WRITE) < 0 ) 
         return ERROR;       
   }
   
   if( avformat_write_header( fmt_ctx, NULL ) < 0 )
      return ERROR;
   
   // init packet, frame.
   AVPacket *pkt  =  av_packet_alloc();
   if( NULL == pkt )
      return ERROR;   
   
   AVFrame  *frame   =  av_frame_alloc();
   if( NULL == frame )
      return ERROR;
   frame->format        =  audio_ctx->sample_fmt;
   frame->sample_rate   =  audio_ctx->sample_rate;
   frame->nb_samples    =  audio_ctx->frame_size;
   if( av_channel_layout_copy( &frame->ch_layout, &audio_ctx->ch_layout ) < 0 )
      return ERROR;
   if( frame->nb_samples > 0 ) 
   {
      if( av_frame_get_buffer( frame, 0 ) < 0 ) 
         return ERROR;
   }
   if( av_frame_make_writable(frame) < 0 )
      return ERROR;

   // audio swr context
   SwrContext  *swr_ctx =  swr_alloc();
   if( NULL == swr_ctx )
      return ERROR;
   
   av_opt_set_chlayout  ( swr_ctx, "in_chlayout",     &audio_dec.audio_ctx->ch_layout,  0 );
   av_opt_set_int       ( swr_ctx, "in_sample_rate",  audio_dec.audio_ctx->sample_rate, 0 );
   av_opt_set_sample_fmt( swr_ctx, "in_sample_fmt",   audio_dec.audio_ctx->sample_fmt,  0 );
   av_opt_set_chlayout  ( swr_ctx, "out_chlayout",    &audio_ctx->ch_layout,      0 );
   av_opt_set_int       ( swr_ctx, "out_sample_rate", audio_ctx->sample_rate,     0 );
   av_opt_set_sample_fmt( swr_ctx, "out_sample_fmt",  audio_ctx->sample_fmt,      0 );
   
   if( swr_init(swr_ctx) < 0 )
      return ERROR;  
   
   enc->fmt_ctx   =  fmt_ctx;
   enc->audio_ctx =  audio_ctx;
   enc->swr_ctx   =  swr_ctx;
   enc->audio_stream =  audio_stream;
   enc->video_stream =  video_stream;
   enc->sample_count =  0;
   enc->frame  =  frame;
   enc->ofmt   =  ofmt;
   enc->pkt =  pkt;

   return SUCCESS;
}


enum AVCodecID    get_g711_codeid_from_filename( char *filename )
{
   int   size  =  strlen(filename);
   int   index, i;

   for( index = size-1; index >= 0; index-- )
   {
      if( filename[index] == '.' )
         break;
   }

   if( index == -1 )
   {
      fprintf( stderr, "incorrect audio filename.\n" );
      return AV_CODEC_ID_PCM_ALAW;
   }

   if( size - index > 6 )
   {
      fprintf( stderr, "file extension error.\n" );
      return AV_CODEC_ID_PCM_ALAW;
   }

   char  tmp[20];
   for( i = index + 1; i < size; i++ )
      tmp[i-index-1] =  filename[i];
   tmp[i]   =  '\0';

   printf( "file extension = %s\n", tmp );

   if( 0 == strcmp( tmp, "g711a" ) )      
      return   AV_CODEC_ID_PCM_ALAW;
   else if( 0 == strcmp(tmp, "g711u" ) )
      return   AV_CODEC_ID_PCM_MULAW;
   else
   {
      fprintf( stderr, "file extension error.\n" );
      return AV_CODEC_ID_PCM_ALAW;
   }

}
