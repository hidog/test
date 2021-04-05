#include "demux.h"
#include <cassert>
#include <stdio.h>







/*****************************************************************************************************
    Demux
*****************************************************************************************************/
Demux::Demux()
{}



/*****************************************************************************************************
    ~Demux
*****************************************************************************************************/
Demux::~Demux()
{}



/*****************************************************************************************************
    open_file
*****************************************************************************************************/
void    Demux::open_file( std::string input, std::string ov, std::string oa )
{
    input_file      =   input;
    output_video    =   ov;
    output_audio    =   oa;
}


/*****************************************************************************************************
    open_codec_context
*****************************************************************************************************/
int     Demux::open_codec_context( int *stream_idx, AVCodecContext **dec_ctx, enum AVMediaType type )
{
    int     ret, stream_index;

    AVStream        *st;
    AVCodec         *dec   =   NULL;
    AVDictionary    *opts  =   NULL;

    ret = av_find_best_stream( fmt_ctx, type, -1, -1, NULL, 0 );
    if( ret < 0 )
    {
        printf( "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), input_file.c_str() );
        return  ret;
    } 
    else
    {
        stream_index = ret;
        st  =   fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec     =   avcodec_find_decoder( st->codecpar->codec_id );
        if( nullptr == dec )
        {
            printf( "Failed to find %s codec\n",
                    av_get_media_type_string(type) );
            return  AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx    =   avcodec_alloc_context3( dec );
        if( nullptr == *dec_ctx )
        {
            printf( "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return  AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        ret     =   avcodec_parameters_to_context( *dec_ctx, st->codecpar );
        if( ret < 0 )
        {
            printf( "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return  ret;
        }

        /* Init the decoders */
        ret     =   avcodec_open2( *dec_ctx, dec, &opts ); 
        if( ret < 0 )
        {
            printf( "Failed to open %s codec\n",
                    av_get_media_type_string(type) );
            return  ret;
        }
        *stream_idx     =   stream_index;
    }

    return 0;
}



/*****************************************************************************************************
    output_video_frame
*****************************************************************************************************/
void    Demux::open_output()
{
    v_fp    =   fopen( output_video.c_str(), "wb+" );
    a_fp    =   fopen( output_audio.c_str(), "wb+" );
}



/*****************************************************************************************************
    output_video_frame
*****************************************************************************************************/
void    Demux::close_output()
{
    fclose(v_fp);
    fclose(a_fp);
}


/*****************************************************************************************************
    end
*****************************************************************************************************/
void    Demux::end()
{
    avcodec_free_context(&video_dec_ctx);
    avcodec_free_context(&audio_dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    av_free(video_dst_data[0]);
}


/*****************************************************************************************************
    flush
*****************************************************************************************************/
void    Demux::flush()
{
    /* flush the decoders */
    if( video_dec_ctx )
        decode_packet( video_dec_ctx, NULL );
    if( audio_dec_ctx )
        decode_packet( audio_dec_ctx, NULL );

    printf( "Demuxing succeeded.\n" );

    if( video_stream )
    {
        printf( "Play the output video file with the command:\n"
                "ffplay -f rawvideo -pix_fmt %s -video_size %dx%d %s\n",
                av_get_pix_fmt_name(pix_fmt), width, height, output_video.c_str() );
    }

    if( audio_stream )
    {
        enum AVSampleFormat sfmt = audio_dec_ctx->sample_fmt;
        int n_channels = audio_dec_ctx->channels;
        const char *fmt;

        if( av_sample_fmt_is_planar(sfmt) )
        {
            const char *packed = av_get_sample_fmt_name(sfmt);
            printf("Warning: the sample format the decoder produced is planar "
                   "(%s). This example will output the first channel only.\n",
                   packed ? packed : "?");
            sfmt = av_get_packed_sample_fmt(sfmt);
            n_channels = 1;
        }

        int ret = get_format_from_sample_fmt(&fmt, sfmt);

        if( ret < 0 )
            assert(0);

        printf( "Play the output audio file with the command:\n"
                "ffplay -f %s -ac %d -ar %d %s\n",
                fmt, n_channels, audio_dec_ctx->sample_rate,
                output_audio.c_str() );
    }
}



/*****************************************************************************************************
    get_format_from_sample_fmt
*****************************************************************************************************/
int Demux::get_format_from_sample_fmt( const char **fmt, enum AVSampleFormat sample_fmt )
{
    int i;
    struct sample_fmt_entry 
    {
        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
    } 

    sample_fmt_entries[] = 
    {
        { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
        { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
        { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
        { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
        { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
    };
    *fmt = NULL;

    for( i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++ ) 
    {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if( sample_fmt == entry->sample_fmt )
        {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }

    printf( "sample format %s is not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
    return -1;
}




/*****************************************************************************************************
    output_video_frame
*****************************************************************************************************/
int     Demux::output_video_frame( AVFrame *frame )
{
    if( frame->width != width || frame->height != height || frame->format != pix_fmt )
    {
        /* To handle this change, one could call av_image_alloc again and
         * decode the following frames into another rawvideo file. */
        printf( "Error: Width, height and pixel format have to be "
                "constant in a rawvideo file, but the width, height or "
                "pixel format of the input video changed:\n"
                "old: width = %d, height = %d, format = %s\n"
                "new: width = %d, height = %d, format = %s\n",
                width, height, av_get_pix_fmt_name(pix_fmt),
                frame->width, frame->height,
                av_get_pix_fmt_name( (AVPixelFormat)(frame->format) ) );
        return  -1;
    }

    printf( "video_frame n:%d coded_n:%d\n",
            video_frame_count++, frame->coded_picture_number);

    /* copy decoded frame to destination buffer:
     * this is required since rawvideo expects non aligned data */
    av_image_copy( video_dst_data, video_dst_linesize,
                   (const uint8_t **)(frame->data), frame->linesize,
                   pix_fmt, width, height );

    /* write to rawvideo file */
    fwrite( video_dst_data[0], 1, video_dst_bufsize, v_fp );
    return  0;
}



/*****************************************************************************************************
    decode_packet
*****************************************************************************************************/
int     Demux::decode_packet( AVCodecContext *dec, const AVPacket *pkt )
{
    int     ret         =   0;
    char    err_buf[256];
    int     err_size    =   256;

    // submit the packet to the decoder
    ret     =   avcodec_send_packet( dec, pkt );
    if( ret < 0 )
    {
        printf( "Error submitting a packet for decoding (%s)\n", av_make_error_string( err_buf, err_size, ret ) );
        return  ret;
    }

    // get all the available frames from the decoder
    while( ret >= 0 )
    {
        ret     =   avcodec_receive_frame( dec, frame );
        if( ret < 0 )
        {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if( ret == AVERROR_EOF || ret == AVERROR(EAGAIN) )
                return  0;

            printf( "Error during decoding (%s)\n", av_make_error_string( err_buf, err_size, ret ) );
            return  ret;
        }

        // write the frame data to output file
        if( dec->codec->type == AVMEDIA_TYPE_VIDEO )
            ret     =   output_video_frame( frame );
        else
            ret     =   output_audio_frame( frame );

        av_frame_unref( frame );
        if( ret < 0 )
            return  ret;
    }

    return 0;
}



/*****************************************************************************************************
    output_audio_frame
*****************************************************************************************************/
int     Demux::output_audio_frame( AVFrame *frame )
{
    size_t  unpadded_linesize   =   frame->nb_samples * av_get_bytes_per_sample( (AVSampleFormat)(frame->format) );

    char    buf[256];
    printf( "audio_frame n:%d nb_samples:%d pts:%s\n",
            audio_frame_count++, frame->nb_samples,
            av_ts_make_time_string( buf, frame->pts, &audio_dec_ctx->time_base ) );
            //av_ts2timestr(frame->pts, &audio_dec_ctx->time_base));

    /* Write the raw audio data samples of the first plane. This works
     * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
     * most audio decoders output planar audio, which uses a separate
     * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
     * In other words, this code will write only the first audio channel
     * in these cases.
     * You should use libswresample or libavfilter to convert the frame
     * to packed data. */
    fwrite( frame->extended_data[0], 1, unpadded_linesize, a_fp );

    return 0;
}


/*****************************************************************************************************
    run
*****************************************************************************************************/
void    Demux::run()
{
    int     ret;

    /* read frames from the file */
    while( true ) 
    {
        ret     =   av_read_frame( fmt_ctx, &pkt );
        if( ret < 0 )
            break;

        // check if the packet belongs to a stream we are interested in, otherwise
        // skip it
        if( pkt.stream_index == video_stream_idx )
            ret     =   decode_packet( video_dec_ctx, &pkt );
        else if ( pkt.stream_index == audio_stream_idx )
            ret     =   decode_packet( audio_dec_ctx, &pkt );

        av_packet_unref(&pkt);
    }

    printf( "end demux\n");
}



/*****************************************************************************************************
    init
*****************************************************************************************************/
int    Demux::init()
{
    int     ret     =   0;

    /* open input file, and allocate format context */
    ret     =   avformat_open_input( &fmt_ctx, input_file.c_str(), NULL, NULL );
    if( ret < 0 )
    {
        printf( "Could not open source file %s\n", input_file.c_str() );
        assert(0);
        return  -1;
    }

    /* retrieve stream information */
    ret     =   avformat_find_stream_info( fmt_ctx, NULL );
    if( ret < 0 )
    {
        printf( "Could not find stream information\n" );
        return  -1;
    }

    // open video codec
    ret     =   open_codec_context( &video_stream_idx, &video_dec_ctx, AVMEDIA_TYPE_VIDEO );
    if( ret >= 0 )
    {
        video_stream = fmt_ctx->streams[video_stream_idx];

        /* allocate image where the decoded image will be put */
        width   =   video_dec_ctx->width;
        height  =   video_dec_ctx->height;
        pix_fmt =   video_dec_ctx->pix_fmt;
        ret     =   av_image_alloc( video_dst_data, video_dst_linesize,
                                    width, height, pix_fmt, 1 );
        if( ret < 0 ) 
        {
            printf( "Could not allocate raw video buffer\n" );
            assert(0);
            return  -1;
        }
        video_dst_bufsize   =   ret;
    }

    // open audio codec
    ret     =   open_codec_context( &audio_stream_idx, &audio_dec_ctx, AVMEDIA_TYPE_AUDIO );
    if( ret >= 0 )
    {
        audio_stream    =   fmt_ctx->streams[audio_stream_idx];
    }

    /* dump input information to stderr */
    av_dump_format( fmt_ctx, 0, input_file.c_str(), 0 );

    //
    if( nullptr == audio_stream && nullptr == video_stream) 
    {
        printf( "Could not find audio or video stream in the input, aborting\n" );
        assert(0);
        return  -1;
    }

    frame   =   av_frame_alloc();
    if( nullptr == frame )
    {
        printf( "Could not allocate frame\n" );
        assert(0);
        return  AVERROR(ENOMEM);
    }

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet( &pkt );
    pkt.data    =   nullptr;
    pkt.size    =   0;

    /*if( nullptr == video_stream)
        printf("Demuxing video from file '%s' into '%s'\n", src_filename, video_dst_filename);
    if (audio_stream)
        printf("Demuxing audio from file '%s' into '%s'\n", src_filename, audio_dst_filename);*/
}



