/*

從appsrc讀取資料進來,可以客製化input

*/


/*

gcc v2_jpg_to_mp4.c -o v2_jpg_to_mp4 -lgio-2.0 `pkg-config --cflags --libs gstreamer-1.0`
gcc v2_jpg_to_mp4.c -o v2_jpg_to_mp4 `pkg-config --cflags --libs gstreamer-1.0 gio-2.0`
這兩個編譯指令都能用
因為多了gio,所以編譯選項要修改

https://stackoverflow.com/questions/20207687/muxing-jpeg-to-mkv-using-gstreamer/24620197
https://stackoverflow.com/questions/57575951/how-to-wait-for-x264enc-to-encode-buffered-frames-on-end-of-stream
http://gstreamer-devel.966125.n4.nabble.com/Grabbing-an-image-frame-from-a-running-pipeline-on-demand-td4677938.html
https://opensource.apple.com/source/WebCore/WebCore-7537.77.4/platform/audio/gstreamer/AudioFileReaderGStreamer.cpp.auto.html

*/



#include<sys/time.h>

#include <gst/gst.h>
#include <stdio.h>
#include <gio/gio.h>

typedef struct _CustomData
{
    GstElement *pipeline;
    GstElement *src;

    //GstElement *parser;
    GstElement *dec;
    GstElement *conv;
    GstElement *enc;

    GstElement *a_src;
    GstElement *a_parse;
    GstElement *a_queue;

    GstElement *mux;
    GstElement *sink;
} CustomData;


#define PRINT_LINE printf("\n line = %d\n", __LINE__ );





gboolean bus_call( GstBus *bus, GstMessage *msg, gpointer data )
{
    //PRINT_LINE
    GMainLoop *loop = (GMainLoop *)data;
    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:            
            // PRINT_LINE
            g_print("End of stream\n");
            //printf("GST_MESSAGE_EOS");
            g_main_loop_quit(loop);
            break;
        case GST_MESSAGE_ERROR:
        {   
            //PRINT_LINE         
            //printf("GST_MESSAGE_ERROR");
            gchar *debug;
            GError *error;
			gst_message_parse_error(msg,&error,&debug);
            g_free(debug);
            g_printerr("ERROR:%s\n",error->message);
            g_error_free(error);

            if( loop != NULL )
                g_main_loop_quit(loop);

            break;
        }
        default:            
            //PRINT_LINE
            //printf("default = %d\n", GST_MESSAGE_TYPE(msg) );
            break;
    }
    return TRUE;
}






char g_buffer[300000] = {0};


void cb_need_data( GstElement *appsrc, guint unused_size, gpointer user_data )
{ 
    GMainLoop *loop = (GMainLoop*)user_data;

    static gboolean white = FALSE;
    static GstClockTime timestamp = 0;
    GstBuffer *buffer;
    guint size;
    GstFlowReturn ret;

    //
    FILE *fp;
    char file_path[100];
    static int index = 0;
    sprintf( file_path, "./jpg/file_%d.jpg", index );
    printf( "index = %d\n", index );
    index++;

    //
    fp = fopen( file_path, "rb" );
    if( fp == NULL )
    {
        g_signal_emit_by_name( appsrc, "end-of-stream", &ret );  // 需要傳送eos, 不然檔案播放有機會遇到無法seek的問題 
        printf( "read finish\n" ); 
        return;
    }

    //
    fseek( fp, 0L, SEEK_END );
    int flen = ftell(fp);
    fseek( fp, 0L, SEEK_SET );
    fread( g_buffer, flen, 1, fp );
    fclose(fp);

    //
    buffer = gst_buffer_new_allocate( NULL, flen, NULL );
    gst_buffer_fill( buffer, 0, g_buffer, flen );

    GST_BUFFER_PTS(buffer) = timestamp;
    //GST_BUFFER_DURATION(buffer) = 41708333; // 舊的寫法,當時不知道24000/1001
    GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale_int( 1001, GST_SECOND, 24000 );  // 這邊會控制播放速度
    /*
        因為並不是真正fps = 24
        比較接近23.976 (但也不完全對)
        https://en.wikipedia.org/wiki/24p#23.976p
        理論上可以用24000/1001取得正確的數據
        這是之前還不熟的時候寫的錯誤的程式碼
        23.98實際上可以用24000/1001產生
        這邊重複設置了duration,是否能精簡?
    */
    
    
    timestamp += GST_BUFFER_DURATION(buffer);

    g_signal_emit_by_name( appsrc, "push-buffer", buffer, &ret );
    gst_buffer_unref (buffer);

    if (ret != GST_FLOW_OK)
        g_main_loop_quit (loop);

    //PRINT_LINE

}




#define MP4
//#define MKV
//#define AVI
//#define MPEG



int main( int argc, char *argv[] )
{
    struct timeval start;
    struct timeval end;
    unsigned long timer;
    gettimeofday( &start, NULL );


    PRINT_LINE

    gst_init( NULL, NULL );

    CustomData data;
    GstMessage *msg;
    GstBus *bus;
    GMainLoop *loop = g_main_loop_new( NULL, FALSE );


    data.src = gst_element_factory_make( "appsrc", "src" );
    GstCaps *caps = gst_caps_new_simple( "image/jpeg", "framerate", GST_TYPE_FRACTION, 24000, 1001, NULL );  // 設定了兩次,不知道能不能精簡
    // do-timestamp 
    //GstCaps *caps = gst_caps_new_simple( "image/jpeg", NULL );
    g_object_set( G_OBJECT(data.src), "caps", caps, NULL );
    g_signal_connect( data.src, "need-data", G_CALLBACK(cb_need_data), loop );
    g_object_set( G_OBJECT(data.src), "stream-type", 0, "format", GST_FORMAT_TIME, NULL );  // 0 應該會對到macro GST_APP_STREAM_TYPE_STREAM 
    //g_object_set( G_OBJECT(data.src), "stream-type", 0, NULL );



    data.a_src = gst_element_factory_make( "filesrc", "a_src" );
    g_object_set( G_OBJECT(data.a_src), "location", "./test.mp3", NULL );
    
    data.a_parse = gst_element_factory_make( "mpegaudioparse", "a_parse" );
    data.a_queue = gst_element_factory_make( "queue", "a_queue" );




    /*
        data.parser = gst_element_factory_make( "jpegparse", "parse" );
        data.dec = gst_element_factory_make( "avdec_mjpeg", "dec" );
        原本用這個方式串接,效果看起來沒有太大差異.
    */
    //data.parser = gst_element_factory_make( "jpegparse", "parse" );
    data.dec = gst_element_factory_make( "jpegdec", "dec" );
    data.conv = gst_element_factory_make( "videoconvert", "conv" );
    data.enc = gst_element_factory_make( "x264enc", "enc" );
    //data.enc = gst_element_factory_make( "nvh264enc", "enc" );
    data.sink = gst_element_factory_make( "filesink", "sink" );



#ifdef MP4
    /*
        其他格式都正確,唯獨mp4不能播放,搜尋應該跟CAPS設定有關,但沒找到可用的參考.
    */
    data.mux = gst_element_factory_make( "mp4mux", "mux" );
    g_object_set( G_OBJECT(data.sink), "location", "./test.mp4", NULL );
#elif defined(MKV)
    data.mux = gst_element_factory_make( "matroskamux", "mux" );
    g_object_set( G_OBJECT(data.sink), "location", "./test.mkv", NULL );
#elif defined(AVI)
    data.mux = gst_element_factory_make( "avimux", "mux" );
    g_object_set( G_OBJECT(data.sink), "location", "./test.avi", NULL );
#elif defined(MPEG)
    data.mux = gst_element_factory_make( "mpegtsmux", "mux" );
    g_object_set( G_OBJECT(data.sink), "location", "./test.mpeg", NULL );
#else
#error not defined...
#endif



    //  
    data.pipeline = gst_pipeline_new ("pipeline");
    bus = gst_element_get_bus( data.pipeline );
    gst_bus_add_watch( bus, bus_call, loop );
    gst_object_unref (bus);

    gst_bin_add_many( GST_BIN(data.pipeline), data.src, /*data.parser,*/ data.dec, data.conv, data.enc, data.mux, data.sink, NULL );
    gst_bin_add_many( GST_BIN(data.pipeline), data.a_src, data.a_parse, data.a_queue, NULL );
    
    gst_element_link_many( data.src, /*data.parser,*/ data.dec, data.conv, data.enc, data.mux, data.sink, NULL );
    gst_element_link_many( data.a_src, data.a_parse, data.a_queue, data.mux, NULL );

    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );
    g_main_loop_run(loop);

    //
    gst_element_set_state( data.pipeline, GST_STATE_NULL );
    gst_object_unref( data.pipeline );    

    gettimeofday( &end, NULL );
    timer = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("timer = %ld ms %ld s %ld m\n", timer/1000, timer/1000000, timer/60000000 );
    // timer = 255783 ms 255 s 4 m

    return 0;
}






