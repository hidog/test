/*
網路看到的範例
會播放連續黑白畫面
*/

/*
https://www.itread01.com/content/1548181449.html
https://www.itread01.com/content/1544349262.html
https://www.itread01.com/content/1547413035.html

https://www.cnblogs.com/xleng/p/11611450.html
https://blog.csdn.net/coroutines/article/details/43987357
https://blog.csdn.net/u010312436/article/details/53610599

*/


#include <gst/gst.h>
#include <stdio.h>

#define VIDEO_WIDTH   384
#define VIDEO_HEIGHT  288
#define VIDEO_FORMAT  "RGB16"
#define PIXEL_SIZE    2

#define PRINT_LINE printf( "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__ );


typedef struct _CustomData 
{
    guint sourceid;        /* To control the GSource */
} CustomData;


//GstElement *appsrc;
GMainLoop *loop;



gboolean cb_need_data( GstElement *appsrc )
{
    PRINT_LINE

    //printf( "unused_size = %d\n", unused_size );
    //GMainLoop *loop = (GMainLoop*)user_data;

    static gboolean white = FALSE;
    static GstClockTime timestamp = 0;
    GstBuffer *buffer;
    guint size;
    GstFlowReturn ret;

    PRINT_LINE

    size = VIDEO_WIDTH * VIDEO_HEIGHT * PIXEL_SIZE;
    buffer = gst_buffer_new_allocate( NULL, size, NULL );

    /* this makes the image black/white */
    gst_buffer_memset( buffer, 0, white ? 0xFF : 0x00, size );

    PRINT_LINE

    white = !white;

    GST_BUFFER_PTS (buffer) = timestamp;
    GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int( 1, GST_SECOND, 15 );  // 這邊切換播放頻率 

    timestamp += GST_BUFFER_DURATION(buffer);

    g_signal_emit_by_name( appsrc, "push-buffer", buffer, &ret );
    gst_buffer_unref (buffer);

    PRINT_LINE

    if (ret != GST_FLOW_OK)
    {
        /* something wrong, stop pushing */
        g_main_loop_quit(loop);
    }

    PRINT_LINE
    return TRUE;
}





void start_feed( GstElement *appsrc, guint size, CustomData *data ) 
{
    //cb_need_data(appsrc);
    PRINT_LINE
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
    PRINT_LINE
    if (data->sourceid != 0) 
    {
        g_print ("Stop feeding\n");
        g_source_remove(data->sourceid);
        data->sourceid = 0;
    }
}




int main( int argc, char *argv[] )
{
    CustomData data;
    data.sourceid = 0;

    GstElement *pipeline, *appsrc, *queue, *conv, *videosink;
    //GMainLoop *loop;

    /* init GStreamer */
    gst_init (NULL, NULL);
    loop = g_main_loop_new( NULL, FALSE );

    /* setup pipeline */
    pipeline  = gst_pipeline_new("pipeline");
    appsrc    = gst_element_factory_make( "appsrc",       "source");
    conv      = gst_element_factory_make( "videoconvert", "conv");
    videosink = gst_element_factory_make( "xvimagesink",  "videosink");

    queue = gst_element_factory_make( "queue", "q" );

    /* setup */
    GstCaps *caps = gst_caps_new_simple( "video/x-raw",
                                         "format",    G_TYPE_STRING,     VIDEO_FORMAT,
                                         "width",     G_TYPE_INT,        VIDEO_WIDTH,
                                         "height",    G_TYPE_INT,        VIDEO_HEIGHT,
                                         "framerate", GST_TYPE_FRACTION, 1, 15,
                                         NULL );

    g_object_set( G_OBJECT(appsrc), "caps", caps, NULL );

    gst_bin_add_many( GST_BIN(pipeline), appsrc, queue, conv, videosink, NULL );
    gst_element_link_many( appsrc, queue, conv, videosink, NULL );

    /* setup appsrc */
    g_object_set( G_OBJECT (appsrc), "stream-type", 0, "format", GST_FORMAT_TIME, NULL );
    g_signal_connect( appsrc, "need-data", G_CALLBACK(start_feed), &data );
    g_signal_connect( appsrc, "enough-data", G_CALLBACK (stop_feed), &data );

    /* play */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_main_loop_run(loop);

    /* clean up */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (pipeline));
    g_main_loop_unref (loop);

    return 0;
}
