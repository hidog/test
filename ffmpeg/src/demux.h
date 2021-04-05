#ifndef DEMUX_H
#define DEMUX_H

#include <string>

extern "C" {
    #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/timestamp.h>
    #include <libavformat/avformat.h>
} // end extern "C"





class Demux
{
public:
    Demux();
    ~Demux();

    void    open_file( std::string input, std::string ov, std::string oa );
    void    open_output();
    void    close_output();

    int     init();
    void    run();
    void    flush();
    void    end();

    int     open_codec_context( int *stream_idx, AVCodecContext **dec_ctx, enum AVMediaType type );
    int     decode_packet( AVCodecContext *dec, const AVPacket *pkt );
    int     output_video_frame( AVFrame *frame );
    int     output_audio_frame( AVFrame *frame );
    int     get_format_from_sample_fmt( const char **fmt, enum AVSampleFormat sample_fmt );



private:
    AVFormatContext *fmt_ctx        =   nullptr;
    AVCodecContext  *video_dec_ctx  =   nullptr, 
                    *audio_dec_ctx  =   nullptr;
    AVStream        *video_stream   =   nullptr, 
                    *audio_stream   =   nullptr;
    AVFrame         *frame          =   nullptr;
    AVPacket        pkt;



    enum AVPixelFormat  pix_fmt;


    std::string     input_file, output_video, output_audio;

    int     video_stream_idx = -1, 
            audio_stream_idx = -1;

    int     width, height;
    int     video_dst_linesize[4];
    int     video_dst_bufsize;


    uint8_t *video_dst_data[4]  =   {nullptr};

    int     video_frame_count   =   0;
    int     audio_frame_count   =   0;

    FILE    *v_fp   =   nullptr,  
            *a_fp   =   nullptr;
};



#endif