/*
改用appsrc做測試
這是做encode的範例
*/



#include<sys/time.h>

#include <gst/gst.h>
#include <stdio.h>
#include <gio/gio.h>

//#define USE_CPU
#define USE_GPU

typedef struct _CustomData
{
    GstElement *pipeline;
    GstElement *src;
    GstElement *queue_1;
    GstElement *dec;
#ifdef USE_GPU
    GstElement *gldownload;
#endif
    GstElement *conv;
    GstElement *queue_2;
    GstElement *enc;
    GstElement *parse;
    GstElement *mux;
    GstElement *sink;
} CustomData;


#define PRINT_LINE printf("\n line = %d\n", __LINE__ );



gboolean bus_call( GstBus *bus, GstMessage *msg, gpointer data )
{
    GMainLoop *loop = (GMainLoop *)data;
    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:            
            g_print("End of stream\n");
            g_main_loop_quit(loop);
            break;
        case GST_MESSAGE_ERROR:
        {   
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
            break;
    }
    return TRUE;
}







char g_buffer[50000000] = {0};


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
    sprintf( file_path, "/home/hidog/code/test/gstreamer/nvdec_test/jpg/file_%d.jpg", index );
    printf( "index = %d\n", index );
    index++;

    //
    fp = fopen( file_path, "rb" );
    if( fp == NULL )
    {
        GstFlowReturn ret;
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
    GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale_int( 1001, GST_SECOND, 24000 );  // 這邊會控制播放速度
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


    //
    PRINT_LINE

    gst_init( NULL, NULL );

    CustomData data;
    GstMessage *msg;
    GstBus *bus;    
    GMainLoop *loop = g_main_loop_new( NULL, FALSE );


    data.src = gst_element_factory_make( "appsrc", "src" );
    GstCaps *caps = gst_caps_new_simple( "image/jpeg", "framerate", GST_TYPE_FRACTION, 24000, 1001, NULL );
    g_object_set( G_OBJECT(data.src), "caps", caps, NULL );
    g_signal_connect( data.src, "need-data", G_CALLBACK(cb_need_data), loop );
    g_object_set( G_OBJECT(data.src), "stream-type", 0, "format", GST_FORMAT_TIME, NULL );  // 0 應該會對到macro GST_APP_STREAM_TYPE_STREAM 

    data.queue_1 = gst_element_factory_make( "queue", "queue_1" );

#ifdef USE_CPU
    data.dec = gst_element_factory_make( "jpegdec", "dec" );
#elif defined(USE_GPU)
    data.dec = gst_element_factory_make( "nvdec", "dec" );
    data.gldownload = gst_element_factory_make( "gldownload", "gldownload" );
#else
#error undefined.
#endif

    data.conv = gst_element_factory_make( "videoconvert", "conv" );
    data.queue_2 = gst_element_factory_make( "queue", "queue_2" );

    data.enc = gst_element_factory_make( "nvh264enc", "enc" );
    data.parse = gst_element_factory_make( "h264parse", "parse" );
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


#ifdef USE_CPU
    gst_bin_add_many( GST_BIN(data.pipeline), data.src, data.queue_1, data.dec, data.conv, data.queue_2, data.enc, data.parse, data.mux, data.sink, NULL );
    gst_element_link_many( data.src, data.queue_1, data.dec, data.conv, data.queue_2, data.enc, data.parse, data.mux, data.sink, NULL );
#elif defined(USE_GPU)
    gst_bin_add_many( GST_BIN(data.pipeline), data.src, data.queue_1, data.dec, data.gldownload, data.conv, data.queue_2, data.enc, data.parse, data.mux, data.sink, NULL );
    gst_element_link_many( data.src, data.queue_1, data.dec, data.gldownload, data.conv, data.queue_2, data.enc, data.parse, data.mux, data.sink, NULL );
#else
#error undefined
#endif

    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );
    g_main_loop_run(loop);

    //
    gst_element_set_state( data.pipeline, GST_STATE_NULL );
    gst_object_unref( data.pipeline );    


    gettimeofday( &end, NULL );
    timer = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    unsigned long m = timer/60000000,
                  s = (timer % 60000000) / 1000000,
                  ms = (timer % 1000000) / 1000;
#ifdef USE_GPU
    printf("use GPU\n");
#elif defined(USE_CPU)
    printf("use CPU\n");
#endif
    printf("timer = %ld m %ld s %ld ms\n", m, s, ms );
    // timer = 201521 ms 201 s 3 m

    return 0;
}






