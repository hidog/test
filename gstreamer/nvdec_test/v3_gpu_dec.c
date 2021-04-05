/*


gst-launch-1.0 filesrc location=~/Videos/test.mp4 ! qtdemux ! h264parse ! nvdec ! glimagesink


改用demux,可以分開處理影音.
使用queue


https://stackoverflow.com/questions/13500893/decode-a-mp4-video-with-gstreamer


*/



#include <gst/gst.h>
#include <stdio.h>

typedef struct _CustomData 
{
    GstElement *pipeline;
    GstElement *source;
    GstElement *demuxer;

    GstElement *audio_queue;
    GstElement *audio_decode;
    GstElement *audio_convert;
    GstElement *audio_sink;

    GstElement *video_queue;
    GstElement *video_parse;
    GstElement *video_decode;
    GstElement *gldl;
    GstElement *video_convert;
    GstElement *video_sink;

} CustomData;




#define PRINT_LINE printf("\n line = %d\n", __LINE__ );
#define GST_GL_DISPLAY_CONTEXT_TYPE "gst.gl.GLDisplay"




void pad_added_handler( GstElement *src, GstPad *pad, CustomData *data )
{
    GstCaps *caps;
    GstStructure *pad_sct;
    const gchar *name;

    caps = gst_pad_get_current_caps(pad);
    pad_sct = gst_caps_get_structure( caps, 0 );
    name = gst_structure_get_name(pad_sct);

    printf( "src name = %s\n", gst_element_get_name(src) );
    printf( "pad name = %s\n", name);
    printf( "Received new pad '%s' from '%s':\n", GST_PAD_NAME (pad), GST_ELEMENT_NAME (src));

    GstPad *sinkpad = NULL;
    if( g_str_has_prefix( name, "audio/mpeg") ) // 可能有不只一個audio type, 看情況
    {
        sinkpad = gst_element_get_static_pad ( data->audio_queue, "sink");
        if(  gst_pad_link ( pad, sinkpad) != GST_PAD_LINK_OK ) // can use GST_PAD_LINK_FAILED to parse ret = gst_pad_link
            printf("not link !!\n\n"); 
        gst_object_unref (sinkpad);
    }
    else if ( g_str_has_prefix (name, "video/x-h264")) 
    {
        sinkpad = gst_element_get_static_pad ( data->video_queue, "sink");
        if(  gst_pad_link ( pad, sinkpad ) != GST_PAD_LINK_OK )
            printf("not link !!\n\n"); 
        gst_object_unref (sinkpad);
    }
    else
        printf("\n !? %s\n", name);

    gst_caps_unref (caps);
}






gboolean bus_call( GstBus *bus, GstMessage *msg, gpointer data )
{
    //PRINT_LINE
    GMainLoop *loop = (GMainLoop *)data;
    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:            
           // PRINT_LINE
            g_print("End of stream\n");
            printf("GST_MESSAGE_EOS");
            g_main_loop_quit(loop);
            break;
        case GST_MESSAGE_ERROR:
        {   
            //PRINT_LINE         
            printf("GST_MESSAGE_ERROR");
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
        case GST_MESSAGE_NEED_CONTEXT:
        {
            PRINT_LINE
            break;
        }
        default:            
            //PRINT_LINE
            //printf("default = %d\n", GST_MESSAGE_TYPE(msg) );
            break;
    }
    return TRUE;
}

                                                    
//gst-launch-1.0 filesrc location=~/Videos/test.mp4 ! qtdemux ! h264parse ! nvdec ! gldownload ! autovideoconvert ! autovideosink


int main( int argc, char *argv[] ) 
{
    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    gboolean terminate = FALSE;

    gst_init ( NULL, NULL );

    GMainLoop *loop = g_main_loop_new ( NULL, FALSE );   

    data.source = gst_element_factory_make( "filesrc", "source" );
    g_object_set( G_OBJECT(data.source), "location", "/home/hidog/Videos/test.mp4", NULL );

    data.demuxer = gst_element_factory_make ("qtdemux", "demuxer");

    data.video_queue = gst_element_factory_make( "queue", "video_queue" );
    data.audio_queue = gst_element_factory_make( "queue", "audio_queue" );

    data.audio_decode = gst_element_factory_make( "faad", "audio_decoder" );  // 最後用faad才成功, 用decodebin也一樣失敗
    data.audio_convert = gst_element_factory_make( "audioconvert", "audio_convert" );
    data.audio_sink    = gst_element_factory_make( "alsasink", "audio_sink" );

    // 如何找到合適的decoder?     data.video_parse = gst_element_factory_make( "h264parse", "video_parse" );
    data.video_decode = gst_element_factory_make( "nvdec", "video_decoder" );   
    data.gldl = gst_element_factory_make( "gldownload", "gldl" );
    data.video_convert = gst_element_factory_make( "autovideoconvert", "video_conv" );
    data.video_sink    = gst_element_factory_make( "autovideosink", "video_sink" );

    data.pipeline = gst_pipeline_new ("test-pipeline");
    bus = gst_element_get_bus( data.pipeline );
    gst_bus_add_watch( bus, bus_call, loop );
    gst_object_unref (bus);

    gst_bin_add_many( GST_BIN(data.pipeline), data.source, data.demuxer, 
                      data.video_queue, data.video_parse, data.video_decode, data.gldl, data.video_convert, data.video_sink,
                      data.audio_queue, data.audio_decode, data.audio_convert, data.audio_sink, NULL );

    gst_element_link_many( data.source, data.demuxer, NULL );
    gst_element_link_many( data.audio_queue, data.audio_decode, data.audio_convert, data.audio_sink, NULL ); 
    gst_element_link_many( data.video_queue, data.video_parse, data.video_decode, data.gldl, data.video_convert, data.video_sink, NULL );


    g_signal_connect( G_OBJECT(data.demuxer), "pad-added", G_CALLBACK(pad_added_handler), &data );

    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );

    g_main_loop_run(loop);


    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref (data.pipeline);

    return 0;
}







