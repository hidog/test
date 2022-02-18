// gst-launch-1.0 videotestsrc ! x264enc ! gdppay ! tcpclientsink host=127.0.0.1 port=3000 sync=false
// gst-launch-1.0 tcpserversrc host=127.0.0.1 port=3000 ! gdpdepay ! avdec_h264 ! videoconvert ! autovideosink
// 上面這組範例可以播放 用他來改看看

// https://georgy.io/computing/getting-video-from-a-raspberry-pi-on-your-computer
// gst-launch-1.0 videotestsrc horizontal-speed=5  ! vp8enc ! gdppay ! tcpserversink host=127.0.0.1 port=5200
// gst-launch-1.0 -v tcpclientsrc port=5200 ! gdpdepay ! vp8dec ! videoconvert ! autovideosink

// https://stackoverflow.com/questions/50549584/gstreamer-udpsink-udpsrc-versus-tcpserversink-tcpclientsrc


#include <gst/gst.h>
#include <stdio.h>

typedef struct _CustomData 
{
    GstElement *pipeline;
    GstElement *src;
    GstElement *depay;
    GstElement *dec;
    GstElement *conv;
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



// gst-launch-1.0 tcpserversrc host=127.0.0.1 port=3000 ! gdpdepay ! avdec_h264 ! videoconvert ! autovideosink
// gst-launch-1.0 -v tcpclientsrc port=5200 ! gdpdepay ! vp8dec ! videoconvert ! autovideosink


int main( int argc, char *argv[] ) 
{
    PRINT_LINE

    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    gboolean terminate = FALSE;

    gst_init ( NULL, NULL );

    GMainLoop *loop = g_main_loop_new ( NULL, FALSE );   

    data.src = gst_element_factory_make( "tcpserversrc", "source" );
    g_object_set( G_OBJECT(data.src), "host", "127.0.0.1", "port", 3000, NULL );

    data.depay = gst_element_factory_make( "gdpdepay", "depay" );
    
    data.dec   = gst_element_factory_make( "avdec_h264", "decoder" );
    //data.dec = gst_element_factory_make( "vp8dec", "decoder" );
    
    data.conv  = gst_element_factory_make( "videoconvert", "convert" );
    data.sink  = gst_element_factory_make( "autovideosink", "sink" );

    data.pipeline = gst_pipeline_new ("v6-gpu-pipeline");
    bus = gst_element_get_bus( data.pipeline );
    gst_bus_add_watch( bus, bus_call, loop );
    gst_object_unref (bus);

    gst_bin_add_many( GST_BIN(data.pipeline), data.src, data.depay, data.dec, data.conv, data.sink, NULL );
    gst_element_link_many( data.src, data.depay, data.dec, data.conv, data.sink, NULL );

    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );
    g_main_loop_run(loop);

    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref( data.pipeline );

    return 0;
}
