/*
可以讀取jpg檔案,軟解方式壓成mp4

可以用這個指令來轉
gst-launch-1.0 -e multifilesrc location="./jpg/file_%d.jpg" caps="image/jpeg,framerate=24000/1001" ! jpegdec ! videoconvert ! queue ! x264enc ! queue ! mp4mux ! filesink location=./test.mp4
https://stackoverflow.com/questions/55901428/how-to-create-a-mp4-video-file-from-png-images-using-gstreamer
24000/1001 : fps = 23.98
*/




/*
不明作用的指令
gst-launch-1.0 videotestsrc ! mux. audiotestsrc ! mux. avimux name=mux ! filesink location=./test.avi
http://trac.gateworks.com/wiki/Yocto/gstreamer/multimedia
*/





#include <gst/gst.h>
#include <stdio.h>


typedef struct _CustomData
{
    GstElement *pipeline;

    GstElement *v_src;
    GstElement *v_dec;
    GstElement *v_conv;
    GstElement *v_enc;
    GstElement *v_queue;
   
    GstElement *a_src;
    GstElement *a_parse;
    GstElement *a_queue;

    GstElement *mux;
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
            g_main_loop_quit(loop);
            break;
        case GST_MESSAGE_ERROR:
        {   
            //PRINT_LINE         
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



gboolean callback( gpointer ptr )
{
    CustomData *data = (CustomData*)ptr;
    gint index;
    g_object_get( G_OBJECT(data->v_src), "index", &index, NULL );
    printf( "index = %d\n", index );
    return TRUE;
}




/*
    網路搜尋到的範例有queue, 實測就算沒queue也能work. 
*/

int main( int argc, char *argv[] )
{
    PRINT_LINE

    gst_init( NULL, NULL );

    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    
    GMainLoop *loop = g_main_loop_new( NULL, FALSE );
     
    data.v_src = gst_element_factory_make( "multifilesrc", "v_src" );
    GstCaps *caps = gst_caps_new_simple( "image/jpeg", "framerate", GST_TYPE_FRACTION, 24000, 1001, NULL );
    g_object_set( G_OBJECT(data.v_src), "location", "./jpg/file_%d.jpg", "caps", caps, NULL );	

    /*
        目前沒有找到印出進度的作法,暫時用timer.
        印象有看到gstreamer有相關的物件.
    */
    g_timeout_add( 100, callback, &data );

    data.v_dec = gst_element_factory_make( "jpegdec", "v_dec" );
    data.v_conv = gst_element_factory_make( "videoconvert", "v_conv" );    
    data.v_enc = gst_element_factory_make( "x264enc", "v_enc" );
    data.v_queue = gst_element_factory_make( "queue", "v_queue" );

    data.a_src = gst_element_factory_make( "filesrc", "a_src" );
    g_object_set( G_OBJECT(data.a_src), "location", "./test.mp3", NULL );
    
    data.a_parse = gst_element_factory_make( "mpegaudioparse", "a_parse" );
    data.a_queue = gst_element_factory_make( "queue", "a_queue" );

    data.mux = gst_element_factory_make( "mp4mux", "mux" );
    data.sink = gst_element_factory_make( "filesink", "sink" );
    g_object_set( G_OBJECT(data.sink), "location", "./test.mp4", "sync", TRUE, NULL );

    //
    data.pipeline = gst_pipeline_new ("pipeline");
    bus = gst_element_get_bus( data.pipeline );
    gst_bus_add_watch( bus, bus_call, loop );
    gst_object_unref (bus);

    //
    gst_bin_add_many( GST_BIN(data.pipeline), data.v_src, data.v_dec, data.v_conv, data.v_enc, data.v_queue, 
                                              data.a_src, data.a_parse, data.a_queue,
                                              data.mux, data.sink, NULL );
    
    gst_element_link_many( data.v_src, data.v_dec, data.v_conv, data.v_enc, data.v_queue, data.mux, NULL ); 
    gst_element_link_many( data.a_src, data.a_parse, data.a_queue, data.mux, NULL );
    gst_element_link_many( data.mux, data.sink, NULL );

    // 
    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );
    g_main_loop_run(loop);

    //
    gst_element_set_state( data.pipeline, GST_STATE_NULL );
    gst_object_unref( data.pipeline );    


    return 0;
}






