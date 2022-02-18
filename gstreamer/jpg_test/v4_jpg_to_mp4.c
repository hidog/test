/*

測試feeding功能
encode後的檔案不能seek (不知道是container還是其他的問題)
但有WARNING
需要研究

gcc v4_jpg_to_mp4.c -o v4_jpg_to_mp4 -lgio-2.0 `pkg-config --cflags --libs gstreamer-1.0`
gcc v4_jpg_to_mp4.c -o v4_jpg_to_mp4 `pkg-config --cflags --libs gstreamer-1.0 gio-2.0`

https://stackoverflow.com/questions/20207687/muxing-jpeg-to-mkv-using-gstreamer/24620197
https://stackoverflow.com/questions/57575951/how-to-wait-for-x264enc-to-encode-buffered-frames-on-end-of-stream
http://gstreamer-devel.966125.n4.nabble.com/Grabbing-an-image-frame-from-a-running-pipeline-on-demand-td4677938.html
https://opensource.apple.com/source/WebCore/WebCore-7537.77.4/platform/audio/gstreamer/AudioFileReaderGStreamer.cpp.auto.html

*/





#include <gst/gst.h>
#include <stdio.h>
#include <gio/gio.h>

typedef struct _CustomData
{
    GstElement *pipeline;
    GstElement *src;
    GstElement *queue;
    GstElement *parser;
    GstElement *dec;
    GstElement *enc;
    GstElement *mux;
    GstElement *sink;

    guint sourceid;
} CustomData;


#define PRINT_LINE printf("\n line = %d\n", __LINE__ );


GMainLoop *loop;


char g_buffer[300000] = {0};


gboolean cb_need_data( GstElement *appsrc, guint unused_size, gpointer user_data )
{ 
    printf(".");
    //GMainLoop *loop = (GMainLoop*)user_data;

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
    //printf( "index = %d\n", index );
    index++;

    //
    fp = fopen( file_path, "rb" );
    if( fp == NULL )
    {
        g_signal_emit_by_name( appsrc, "end-of-stream", NULL );
        printf( "read finish\n" ); 
        g_main_loop_quit(loop);
        return FALSE;
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
    return TRUE;
}




void start_feed( GstElement *appsrc, guint size, CustomData *data ) 
{
    //cb_need_data(appsrc);
    //PRINT_LINE
    if (data->sourceid == 0) 
    {
        g_print ("Start feeding\n");
        data->sourceid = g_idle_add( (GSourceFunc)cb_need_data, appsrc );
    }
}




/* This callback triggers when appsrc has enough data and we can stop sending.
 * We remove the idle handler from the mainloop */
void stop_feed( GstElement *source, CustomData *data ) 
{
    //PRINT_LINE
    if (data->sourceid != 0) 
    {
        g_print ("Stop feeding\n");
        g_source_remove(data->sourceid);
        data->sourceid = 0;
    }
}





int main( int argc, char *argv[] )
{
    PRINT_LINE

    gst_init( NULL, NULL );

    CustomData data;
    data.sourceid = 0;

    GstMessage *msg;
    
    loop = g_main_loop_new( NULL, FALSE );


    data.src = gst_element_factory_make( "appsrc", "src" );
    GstCaps *caps = gst_caps_new_simple( "image/jpeg", "framerate", GST_TYPE_FRACTION, 24000, 1001, NULL );
    //GstCaps *caps = gst_caps_new_simple( "image/jpeg", NULL );
    g_object_set( G_OBJECT(data.src), "caps", caps, NULL );

    //g_signal_connect( data.src, "need-data", G_CALLBACK(cb_need_data), loop );
    g_signal_connect( data.src, "need-data", G_CALLBACK(start_feed), &data );
    g_signal_connect( data.src, "enough-data", G_CALLBACK (stop_feed), &data );

    g_object_set( G_OBJECT(data.src), "stream-type", 0, "format", GST_FORMAT_TIME, NULL );  // 0 應該會對到macro GST_APP_STREAM_TYPE_STREAM 
    //g_object_set( G_OBJECT(data.src), "stream-type", 0, NULL );

    data.queue = gst_element_factory_make( "queue", "queue" );
    data.parser = gst_element_factory_make( "jpegparse", "parse" );
    data.dec = gst_element_factory_make( "avdec_mjpeg", "dec" );
    data.enc = gst_element_factory_make( "x264enc", "enc" );
    data.mux = gst_element_factory_make( "avimux", "mux" );

    data.sink = gst_element_factory_make( "filesink", "sink" );
    g_object_set( G_OBJECT(data.sink), "location", "./test.avi", NULL );

    //  
    data.pipeline = gst_pipeline_new ("pipeline");
    gst_bin_add_many( GST_BIN(data.pipeline), data.src, data.queue, data.parser, data.dec, data.enc, data.mux, data.sink, NULL );
    gst_element_link_many( data.src, data.queue, data.parser, data.dec, data.enc, data.mux, data.sink, NULL );
    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );
    g_main_loop_run(loop);

    //
    gst_element_set_state( data.pipeline, GST_STATE_NULL );
    gst_object_unref( data.pipeline );    

    return 0;
}






