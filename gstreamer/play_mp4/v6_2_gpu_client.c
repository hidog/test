#include <gst/gst.h>
#include <stdio.h>


typedef struct _CustomData
{
    GstElement *pipeline;
    GstElement *src;
    GstElement *dec;
    GstElement *conv;
    GstElement *enc;
    GstElement *pay;
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



// gst-launch-1.0 videotestsrc ! x264enc ! gdppay ! tcpclientsink host=127.0.0.1 port=3000 sync=false
// gst-launch-1.0 videotestsrc horizontal-speed=5  ! vp8enc ! gdppay ! tcpserversink host=127.0.0.1 port=5200
// 用vp8測試發現效能有差


int main( int argc, char *argv[] )
{
    PRINT_LINE

    gst_init( NULL, NULL );

    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    
    GMainLoop *loop = g_main_loop_new( NULL, FALSE );
     
    data.src = gst_element_factory_make( "multifilesrc", "src" );
    GstCaps *caps = gst_caps_new_simple( "image/jpeg", "framerate", GST_TYPE_FRACTION, 30, 1, NULL );
    g_object_set( G_OBJECT(data.src), "location", "../jpg_test/jpg/file_%d.jpg", "caps", caps, NULL );	

    data.dec = gst_element_factory_make( "jpegdec", "dec" );
    data.conv = gst_element_factory_make( "videoconvert", "conv" );

    data.enc = gst_element_factory_make( "nvh264enc", "enc");
    //data.enc = gst_element_factory_make( "x264enc", "enc" );
    //data.enc = gst_element_factory_make( "vp8enc", "enc" );
    
    data.pay = gst_element_factory_make( "gdppay", "pay" );
    
    data.sink = gst_element_factory_make( "tcpclientsink", "sink" );
    g_object_set( G_OBJECT(data.sink), "host", "127.0.0.1", "port", 3000, "sync", FALSE, NULL );

    //
    data.pipeline = gst_pipeline_new("pipeline");
    bus = gst_element_get_bus( data.pipeline );
    gst_bus_add_watch( bus, bus_call, loop );
    gst_object_unref (bus);

    //
    gst_bin_add_many( GST_BIN(data.pipeline), data.src, data.dec, data.conv, data.enc, data.pay, data.sink, NULL );

    //
    gst_element_link_many( data.src, data.dec, data.conv, data.enc, data.pay, data.sink, NULL );
    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );
    g_main_loop_run(loop);

    //
    gst_element_set_state( data.pipeline, GST_STATE_NULL );
    gst_object_unref( data.pipeline );    

    return 0;
}






