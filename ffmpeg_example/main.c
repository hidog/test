#include <stdio.h>
#include "media.h"


void  convert_aac_to_opus( char *input_file, char *output_file )
{
   int      ret;
   Decode   dec;
   
   ret   =  open_input( input_file, &dec );
   if( ret < 0 )
   {
      fprintf( stderr, "open input fail at line %d.\n", -ret );
      exit(0);
   }
   
   Encode   enc;
   ret   =  open_output( output_file, dec, &enc );
   if( ret < 0 )
   {
      fprintf( stderr, "open output fail at line %d.\n", -ret );
      exit(0);
   }
   
   FifoBuffer  fifobuf;
   ret   =  init_fifo( dec, enc, &fifobuf );
   if( ret < 0 )
   {
      fprintf( stderr, "open fifo fail at line %d.\n", -ret );
      exit(0);
   }   
   
   int   count    =  0;
   while( av_read_frame( dec.fmt_ctx, dec.pkt ) >= 0 )
   {
      if( (count++ % 30000) == 0 )
         printf( "read pkt count = %d\n", count );

      // audio
      if( dec.pkt->stream_index == dec.audio_index )
      {
         ret   =  audio_decode( &dec );
         if( ret == SUCCESS )
            write_audio_frame( dec, &enc, fifobuf );
      }
      // video
      else
      {
         dec.pkt->stream_index   =  enc.video_stream->index;
         av_packet_rescale_ts( dec.pkt, dec.video_stream->time_base, enc.video_stream->time_base );
         av_interleaved_write_frame( enc.fmt_ctx, dec.pkt );
      }

      av_packet_unref( dec.pkt );
   }
   
   // flush audio
   flush_audio( dec, &enc, fifobuf );
   close_decode(&dec);

   av_write_trailer( enc.fmt_ctx );
   close_encode(&enc);
   close_fifo(&fifobuf);

   printf("finish.\n");
}




void  merge_g711_to_opus( char *input_video, char *input_audio, char *output_file )
{
   int      ret;
   Decode   video_dec, audio_dec;
   
   ret   =  open_video_input( input_video, &video_dec );
   if( ret < 0 )
   {
      fprintf( stderr, "open input fail at line %d.\n", -ret );
      exit(0);
   }

   enum AVCodecID codeid   =  get_g711_codeid_from_filename(input_audio);

   //ret   =  open_g711_input( input_audio, &audio_dec, codeid, 2, 48000 );
   ret   =  open_g711_input( input_audio, &audio_dec, codeid, 1, 8000 );
   if( ret < 0 )
   {
      fprintf( stderr, "open input fail at line %d.\n", -ret );
      exit(0);
   }

   Encode   enc;
   ret   =  open_merge_output( output_file, video_dec, audio_dec, &enc );
   if( ret < 0 )
   {
      fprintf( stderr, "open output fail at line %d.\n", -ret );
      exit(0);
   }
   
   FifoBuffer  fifobuf;
   ret   =  init_fifo( audio_dec, enc, &fifobuf );
   if( ret < 0 )
   {
      fprintf( stderr, "open fifo fail at line %d.\n", -ret );
      exit(0);
   }   
   
   int   count    =  0;
   int   read_video, read_audio;
   while( 1 )
   {
      if( (count++ % 30000) == 0 )
         printf( "read pkt count = %d\n", count );

      // audio
      read_audio  =  av_read_frame( audio_dec.fmt_ctx, audio_dec.pkt );
      if( read_audio >= 0 )
      {
         ret   =  audio_decode( &audio_dec );
         av_packet_unref( audio_dec.pkt );
         if( ret == SUCCESS )
            write_audio_frame( audio_dec, &enc, fifobuf );
      }

      // video
      read_video  =   av_read_frame( video_dec.fmt_ctx, video_dec.pkt );
      if( read_video >= 0 )
      {
         video_dec.pkt->stream_index   =  enc.video_stream->index;
         av_packet_rescale_ts( video_dec.pkt, video_dec.video_stream->time_base, enc.video_stream->time_base );
         av_interleaved_write_frame( enc.fmt_ctx, video_dec.pkt );
         av_packet_unref( video_dec.pkt );
      }

      if( read_video < 0 && read_audio < 0 )
         break;
   }

   // flush audio
   flush_audio( audio_dec, &enc, fifobuf );
   close_decode(&audio_dec);

   av_write_trailer( enc.fmt_ctx );

   close_encode(&enc);
   close_fifo(&fifobuf);

   printf("finish.\n");
}




int main(int argc, char *argv[])
{
   if( argc == 1 )
   {
      printf( "use ./video_test input.mp4 output.mp4 or ./video_test input.mp4 input.g711a output.mp4\n" );
   }
   else if( argc == 3 )
   {
      char  *input_file    =  argv[1];
      char  *output_file   =  argv[2];
      convert_aac_to_opus( input_file, output_file );
   }
   else if( argc == 4 )
   {
      char  *input_video   =  argv[1];
      char  *input_audio   =  argv[2];
      char  *output_file   =  argv[3];
      merge_g711_to_opus( input_video, input_audio, output_file );
   }
   else
   {
      fprintf( stderr, "input parameter error.\n" );
   }

   return 0;
}
