#include "decode.h"
#include <thread>
#include "wrap.h"





AVFormatContext *fmt_ctx = NULL;
AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx;
int width, height;
enum AVPixelFormat pix_fmt;
AVStream *video_stream = NULL, *audio_stream = NULL;
const char *src_filename = NULL;
const char *video_dst_filename = NULL;
const char *audio_dst_filename = NULL;
FILE *video_dst_file = NULL;
FILE *audio_dst_file = NULL;

uint8_t *video_dst_data[4] = {NULL};
int      video_dst_linesize[4];
int video_dst_bufsize;

int video_stream_idx = -1, audio_stream_idx = -1;
AVFrame *frame = NULL;
AVPacket *pkt = NULL;
int video_frame_count = 0;
int audio_frame_count = 0;





int output_video_frame(AVFrame *frame)
{
    printf( "video_frame n:%d coded_n:%d\n", video_frame_count++, frame->coded_picture_number );

    av_image_copy( video_dst_data, video_dst_linesize,
                   (const uint8_t **)(frame->data), frame->linesize,
                   pix_fmt, width, height);

    fwrite( video_dst_data[0], 1, video_dst_bufsize, video_dst_file );
    return 0;
}





int output_audio_frame(AVFrame *frame)
{
    size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample( static_cast<AVSampleFormat>(frame->format) );
    printf( "audio_frame n:%d nb_samples:%d \n", audio_frame_count++, frame->nb_samples );
    fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);
    return 0;
}





int decode_packet(AVCodecContext *dec, const AVPacket *pkt)
{
    int ret = 0;

    // submit the packet to the decoder
    ret = avcodec_send_packet(dec, pkt);

    // get all the available frames from the decoder
    while( ret >= 0 )
    {
        ret = avcodec_receive_frame(dec, frame);
        if (ret < 0) 
        {
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;
            fprintf(stderr, "Error during decoding \n" );
            return ret;
        }

        // write the frame data to output file
        if( dec->codec->type == AVMEDIA_TYPE_VIDEO )
            ret = output_video_frame(frame);
        else
            ret = output_audio_frame(frame);

        av_frame_unref(frame);
        if( ret < 0 )
            return ret;
    }

    return 0;
}








int open_codec_context( int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type )
{
    int ret, stream_index;
    AVStream *st;
    const AVCodec *dec = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);

    stream_index = ret;
    st = fmt_ctx->streams[stream_index];

    /* find decoder for the stream */
    dec = avcodec_find_decoder(st->codecpar->codec_id);
    
    /* Allocate a codec context for the decoder */
    *dec_ctx = avcodec_alloc_context3(dec);
    
    /* Copy codec parameters from input stream to output codec context */
    avcodec_parameters_to_context(*dec_ctx, st->codecpar);
    
    /* Init the decoders */
    avcodec_open2(*dec_ctx, dec, NULL);

    *stream_idx = stream_index;    

    return 0;
}







int get_format_from_sample_fmt( const char **fmt, enum AVSampleFormat sample_fmt )
{
    int i;
    struct sample_fmt_entry {
        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
        { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
        { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
        { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
        { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
        { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
    };
    *fmt = NULL;

    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) 
    {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt) 
        {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }

    fprintf(stderr, "sample format %s is not supported as output format\n", av_get_sample_fmt_name(sample_fmt));
    return -1;
}








int decode_test()
{
    int ret = 0;

    src_filename = "D:\\test.rmvb";
    video_dst_filename = "D:\\v.data";
    audio_dst_filename = "D:\\a.data";

    /* open input file, and allocate format context */
    avformat_open_input(&fmt_ctx, src_filename, NULL, NULL);

    /* retrieve stream information */
    avformat_find_stream_info(fmt_ctx, NULL);

    //
    open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO);
    video_stream = fmt_ctx->streams[video_stream_idx];
    video_dst_file = fopen(video_dst_filename, "wb");
    /* allocate image where the decoded image will be put */
    width = video_dec_ctx->width;
    height = video_dec_ctx->height;
    pix_fmt = video_dec_ctx->pix_fmt;

    //ret = av_image_alloc(video_dst_data, video_dst_linesize, width, height, pix_fmt, 1);
    ret = wap_av_image_alloc(video_dst_data, video_dst_linesize, width, height, pix_fmt, 1);

    
    video_dst_bufsize = ret;    

    //
    open_codec_context(&audio_stream_idx, &audio_dec_ctx, fmt_ctx, AVMEDIA_TYPE_AUDIO);
    audio_stream = fmt_ctx->streams[audio_stream_idx];
    audio_dst_file = fopen(audio_dst_filename, "wb");

    /* dump input information to stderr */
    av_dump_format(fmt_ctx, 0, src_filename, 0);

    //
    frame = av_frame_alloc();
    pkt = av_packet_alloc();

    if(video_stream) printf("Demuxing video from file '%s' into '%s'\n", src_filename, video_dst_filename);
    if(audio_stream) printf("Demuxing audio from file '%s' into '%s'\n", src_filename, audio_dst_filename);

    /* read frames from the file */
    while (av_read_frame(fmt_ctx, pkt) >= 0) 
    {
        // check if the packet belongs to a stream we are interested in, otherwise
        // skip it
        if (pkt->stream_index == video_stream_idx)
            ret = decode_packet(video_dec_ctx, pkt);
        else if (pkt->stream_index == audio_stream_idx)
            ret = decode_packet(audio_dec_ctx, pkt);
        av_packet_unref(pkt);
        if (ret < 0)
            break;

        std::this_thread::sleep_for( std::chrono::milliseconds(1) );
    }

    /* flush the decoders */
    if (video_dec_ctx)
        decode_packet(video_dec_ctx, NULL);
    if (audio_dec_ctx)
        decode_packet(audio_dec_ctx, NULL);

    printf("Demuxing succeeded.\n");

    if( video_stream )
    {
        printf( "Play the output video file with the command:\nffplay -f rawvideo -pix_fmt %s -video_size %dx%d %s\n",
                av_get_pix_fmt_name(pix_fmt), width, height, video_dst_filename);
    }

    if( audio_stream )
    {
        AVSampleFormat sfmt = audio_dec_ctx->sample_fmt;
        int n_channels = audio_dec_ctx->channels;
        const char *fmt;

        if(av_sample_fmt_is_planar(sfmt)) 
        {
            const char *packed = av_get_sample_fmt_name(sfmt);
            printf( "Warning: the sample format the decoder produced is planar (%s). This example will output the first channel only.\n", packed ? packed : "?" );
            sfmt = av_get_packed_sample_fmt(sfmt);
            n_channels = 1;
        }

        get_format_from_sample_fmt(&fmt, sfmt);          
        printf( "Play the output audio file with the command:\nffplay -f %s -ac %d -ar %d %s\n",
                fmt, n_channels, audio_dec_ctx->sample_rate, audio_dst_filename );
    }

    //
    avcodec_free_context(&video_dec_ctx);
    avcodec_free_context(&audio_dec_ctx);
    avformat_close_input(&fmt_ctx);
    if (video_dst_file)
        fclose(video_dst_file);
    if (audio_dst_file)
        fclose(audio_dst_file);
    av_packet_free(&pkt);
    av_frame_free(&frame);
    av_free(video_dst_data[0]);

    return 0;
}
