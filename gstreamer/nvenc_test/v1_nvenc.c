/*

產生測試影像, encode後decode並顯示
gst-launch-1.0 videotestsrc pattern=snow ! video/x-raw,width=500,height=500 ! videoconvert ! video/x-raw ! nvh264enc ! 
video/x-h264,stream-format=byte-stream ! avdec_h264 ! videoconvert ! autovideosink

效果同上
gst-launch-1.0 videotestsrc pattern=snow ! video/x-raw,width=500,height=500 ! nvh264enc  !  avdec_h264  ! autovideosink


C code效果同上

*/




#include <gst/gst.h>
#include <stdio.h>


typedef struct _CustomData
{
    GstElement *pipeline;
    GstElement *src;
    GstElement *enc;
    GstElement *dec;
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



/// gst-launch-1.0 videotestsrc pattern=snow ! video/x-raw,width=2500,height=2000 ! nvh264enc  !  avdec_h264  ! autovideosink

int main( int argc, char *argv[] )
{
    PRINT_LINE

    gst_init( NULL, NULL );

    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    
    GMainLoop *loop = g_main_loop_new( NULL, FALSE );
     
    data.src = gst_element_factory_make( "videotestsrc", "src" );
    g_object_set( G_OBJECT(data.src), "pattern", 1, NULL ); // 1 = snow

    GstCaps *caps = gst_caps_new_simple( "video/x-raw", 
                                         "width", G_TYPE_INT, 500, 
                                         "height", G_TYPE_INT, 500, NULL );

    data.enc = gst_element_factory_make( "nvh264enc", "enc");
    data.dec = gst_element_factory_make( "avdec_h264", "dec" );
    data.sink = gst_element_factory_make( "autovideosink", "sink" );

    //
    data.pipeline = gst_pipeline_new("pipeline");
    bus = gst_element_get_bus( data.pipeline );
    gst_bus_add_watch( bus, bus_call, loop );
    gst_object_unref (bus);

    //
    gst_bin_add_many( GST_BIN(data.pipeline), data.src, data.enc, data.dec, data.sink, NULL );

    //
    gst_element_link_filtered( data.src, data.enc, caps );
    gst_element_link_many( data.enc, data.dec, data.sink, NULL );


    // 
    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );
    g_main_loop_run(loop);

    //
    gst_element_set_state( data.pipeline, GST_STATE_NULL );
    gst_object_unref( data.pipeline );    


    return 0;
}






