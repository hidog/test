/*
可以讀取連續的jpg來做播放,效果類似播放影片
讀取memory資料進去pipeline
*/

// https://stackoverflow.com/questions/29205920/push-images-into-gstreamer-pipeline


#include <gst/gst.h>
#include <stdio.h>


char g_buffer[300000] = {0};


void cb_need_data( GstElement *appsrc, guint unused_size, gpointer user_data )
{ 
    //printf( "unused_size = %d\n", unused_size ); 固定是4096,估計是保留參數

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
    sprintf( file_path, "/home/hidog/code/example_gstreamer/jpg_test/jpg/file_%d.jpg", index );
    printf( "index = %d\n", index );
    index++;

#if 0
    /*
        故意讓callback卡住,想模擬如果網路不順的情況
        看起來會丟棄部份影像.
    */
    if( index % 100 == 0 )
    {
        for( int i = 0; i < 10000000; i++ )
            printf(".");
            //printf( "wait for i = %d", i );
    }
#endif

    //
    fp = fopen( file_path, "rb" );
    if( fp == NULL )
    {
        printf( "read finish\n" ); 
        g_main_loop_quit (loop);
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

    GST_BUFFER_PTS (buffer) = timestamp;
    GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int( 1, GST_SECOND, 30 );  // 這邊會控制播放速度

    timestamp += GST_BUFFER_DURATION(buffer);

    g_signal_emit_by_name( appsrc, "push-buffer", buffer, &ret );
    gst_buffer_unref (buffer);

    if (ret != GST_FLOW_OK)
        g_main_loop_quit (loop);
}




int main( int argc, char *argv[] )
{
    GstElement *pipeline, *appsrc, *dec, *conv, *videosink;
    GMainLoop *loop;

    /* init GStreamer */
    gst_init (NULL, NULL);
    loop = g_main_loop_new( NULL, FALSE );

    /* setup pipeline */
    pipeline  = gst_pipeline_new("pipeline");
    appsrc    = gst_element_factory_make( "appsrc",       "source" );
    dec       = gst_element_factory_make( "jpegdec",      "dec" );
    conv      = gst_element_factory_make( "videoconvert", "conv" );
    videosink = gst_element_factory_make( "xvimagesink",  "videosink" );

    /* setup */
    //GstCaps *caps = gst_caps_new_simple( "image/jpeg", "framerate", GST_TYPE_FRACTION, 60, 1, NULL );
    GstCaps *caps = gst_caps_new_simple( "image/jpeg", NULL );
    // video/x-raw  image/jpeg
    g_object_set( G_OBJECT(appsrc), "caps", caps, NULL );

    gst_bin_add_many( GST_BIN(pipeline), appsrc, dec, conv, videosink, NULL );
    gst_element_link_many( appsrc, dec, conv, videosink, NULL );

    /* setup appsrc */
    g_object_set( G_OBJECT(appsrc), "stream-type", 0, "format", GST_FORMAT_TIME, NULL );  // 0 應該會對到macro GST_APP_STREAM_TYPE_STREAM 
    g_signal_connect( appsrc, "need-data", G_CALLBACK(cb_need_data), loop );

    /* play */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_main_loop_run(loop);

    /* clean up */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (pipeline));
    g_main_loop_unref (loop);

    return 0;
}
