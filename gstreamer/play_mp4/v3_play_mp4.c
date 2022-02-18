/*
改用filesrc跟decodebin的方式來播放mp4
https://stackoverflow.com/questions/13500893/decode-a-mp4-video-with-gstreamer

簡單的播放範例
gst-launch-1.0 filesrc location=/home/hidog/Videos/test.mp4 ! decodebin name=dec ! videoconvert ! autovideosink dec. ! audioconvert ! audioresample ! alsasink

*/




#include <gst/gst.h>
#include <stdio.h>

typedef struct _CustomData 
{
    GstElement *pipeline;
    GstElement *source;
    GstElement *decoder;
    GstElement *demuxer;
    GstElement *video_convert;
    GstElement *audio_convert;
    GstElement *video_sink;
    GstElement *audio_sink;
} CustomData;




#define PRINT_LINE printf("\n line = %d\n", __LINE__ );

//void pad_added_handler (GstElement *src, GstPad *new_pad, CustomData *data ) 
void pad_added_handler (GstElement *src, GstPad *pad, CustomData *data)
{
    GstCaps *caps;
    GstStructure *pad_sct;
    const gchar *name;

    caps = gst_pad_get_current_caps(pad);
    pad_sct = gst_caps_get_structure( caps, 0 );
    name = gst_structure_get_name(pad_sct);

    printf("name = %s\n", name);

    GstPad *sinkpad = NULL;
    if( g_str_has_prefix( name, "audio/x-raw") ) // 可能有不只一個audio type, 看情況
    {       
        sinkpad = gst_element_get_static_pad ( data->audio_convert, "sink");
        if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK) 
            g_printerr ("Failed to link demux with demux_a_que\n");
        gst_object_unref (sinkpad);
    }
    else if ( g_str_has_prefix (name, "video/x-raw")) 
    {
        sinkpad = gst_element_get_static_pad ( data->video_convert, "sink");
        if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK) 
            g_printerr ("Failed to link demux with demux_v_que\n");
        gst_object_unref (sinkpad);
        PRINT_LINE
    }

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
        default:            
            //PRINT_LINE
            //printf("default = %d\n", GST_MESSAGE_TYPE(msg) );
            break;
    }
    return TRUE;
}





int main( int argc, char *argv[] ) 
{
    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    gboolean terminate = FALSE;

    gst_init ( NULL, NULL );

    GMainLoop *loop = g_main_loop_new ( NULL, FALSE );   

    data.source        = gst_element_factory_make( "filesrc",          "source" );
    g_object_set( G_OBJECT(data.source), "location", "/home/hidog/Videos/test.mp4", NULL );

    data.decoder       = gst_element_factory_make( "decodebin",        "decoder" );

    data.audio_convert = gst_element_factory_make( "audioconvert",     "audio_convert" );
    data.audio_sink    = gst_element_factory_make( "alsasink",    "audio_sink" );

    data.video_convert = gst_element_factory_make( "videoconvert",     "video_convert" );
    data.video_sink    = gst_element_factory_make( "autovideosink",    "video_sink" );

    data.pipeline = gst_pipeline_new ("test-pipeline");
    bus = gst_element_get_bus( data.pipeline );
    gst_bus_add_watch( bus, bus_call, loop );
    gst_object_unref (bus);

    /*if( !data.pipeline || !data.source || !data.demuxer || !data.audioqueue ||!data.audio_decoder ||!data.audio_convert ||
        !data.audio_sink || !data.videoqueue || !data.video_decoder || !data.video_convert || !data.video_sink ) 
    {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }*/


    gst_bin_add_many( GST_BIN(data.pipeline), data.source, data.decoder, data.audio_convert, data.audio_sink, 
                      data.video_convert, data.video_sink, NULL );

    gst_element_link_many( data.source, data.decoder, NULL );
    gst_element_link_many( data.audio_convert, data.audio_sink, NULL ); 
    gst_element_link_many( data.video_convert, data.video_sink, NULL );


    g_signal_connect( G_OBJECT(data.decoder), "pad-added", G_CALLBACK(pad_added_handler), &data );

    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );

    g_main_loop_run(loop);


    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref (data.pipeline);

    return 0;
}
