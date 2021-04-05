/*
實驗如果buffer是固定沒有re-allocate是否可以播放
*/

// https://www.itread01.com/content/1547401879.html
// http://gstreamer-devel.966125.n4.nabble.com/GST-BUFFER-DATA-in-gstreamer1-0-td4661658.html

#include <gst/gst.h>
#include <stdio.h>


GstBuffer *buffer;



void cb_need_data( GstElement *appsrc, guint unused_size, gpointer user_data )
{ 
    GMainLoop *loop = (GMainLoop*)user_data;

    static gboolean white = FALSE;
    static GstClockTime timestamp = 0;
    //GstBuffer *buffer;
    guint size;
    GstFlowReturn ret;

    //
    FILE *fp;
    char file_path[100];
    static int index = 0;
    sprintf( file_path, "/home/hidog/code/example_gstreamer/jpg_test/jpg/file_%d.jpg", index );
    printf( "index = %d\n", index );
    index++;

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

    //
    //buffer = gst_buffer_new_allocate( NULL, flen, NULL );    
    GstMapInfo info;
    gst_buffer_map( buffer, &info, GST_MAP_READ | GST_MAP_WRITE );
    fread( info.data, flen, 1, fp );
    gst_buffer_unmap( buffer, &info );
    fclose(fp);

    //
    GST_BUFFER_PTS (buffer) = timestamp;
    GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int( 1, GST_SECOND, 30 );  // 這邊會控制播放速度

    timestamp += GST_BUFFER_DURATION(buffer);

    g_signal_emit_by_name( appsrc, "push-buffer", buffer, &ret );
    //gst_buffer_unref (buffer);

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

    buffer = gst_buffer_new_allocate( NULL, 300000, NULL );

    /* setup pipeline */
    pipeline  = gst_pipeline_new("pipeline");
    appsrc    = gst_element_factory_make( "appsrc",       "source" );
    dec       = gst_element_factory_make( "jpegdec",      "dec" );
    conv      = gst_element_factory_make( "videoconvert", "conv" );
    videosink = gst_element_factory_make( "xvimagesink",  "videosink" );

    /* setup */
    GstCaps *caps = gst_caps_new_simple( "image/jpeg", "framerate", GST_TYPE_FRACTION, 30, 1, NULL );
    //GstCaps *caps = gst_caps_new_simple( "image/jpeg", NULL );
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














#if 0
#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

static FILE * fp;

void _needData( GstAppSrc * src, guint arg1, gpointer user_data )
{
    GstBuffer * _buf;
    GstFlowReturn * _ret;
    int _size;

    _buf    =   gst_buffer_new_and_alloc( arg1 );

    _size   =   fread( GST_BUFFER_DATA( _buf ), 1, arg1, fp );

    if( _size < arg1 )//讀到檔案尾了，從頭再讀
    {
        fseek( fp, 0, SEEK_SET );

        fread( GST_BUFFER_DATA( _buf ) + _size, 1, arg1 - _size, fp );
    }

    g_signal_emit_by_name( src, "push-buffer", _buf, &_ret );//資料送入pipeline

    gst_buffer_unref( _buf );
}

void _enoughData( GstElement * src, gpointer user_data )
{
    g_message( "%s", __func__ );
}
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
    GstElement * _pipeline, * _src, * _codec, * _sink;

    fp  =   fopen( "/root/2.mp3", "r" );

    g_assert( fp != NULL );

    gst_init( &argc, &argv );

    g_assert( _src  =   gst_element_factory_make( "appsrc", "source" ));
    g_assert( _codec    =   gst_element_factory_make( "mad", "codec" ));//mad mp3 decoder
    g_assert( _sink =   gst_element_factory_make( "alsasink", "sink" ));

    _pipeline   =   gst_pipeline_new( "pipeline" );

    gst_bin_add_many( GST_BIN( _pipeline ), _src, _codec, _sink, NULL );

    gst_element_link_many( _src, _codec, _sink, NULL );

    g_signal_connect ( _src, "need-data", G_CALLBACK( _needData ), NULL );

    gst_element_set_state( _pipeline, GST_STATE_PLAYING );

    pause();

    return 0;
}               /* ----------  end of function main  ---------- */
#endif
