/*

gst-launch-1.0 filesrc location=~/Videos/test.mp4 ! qtdemux ! h264parse ! nvdec ! gldownload ! jpegenc ! multifilesink location=./jpg/file_%d.jpg
這個命令可以作到轉檔  (用gpu decode)


這個範例可以用nvdec來把影片轉換成jpg圖檔

*/

#include <sys/time.h>

#include <gst/gst.h>
#include <stdio.h>
#include <assert.h>


typedef struct _CustomData 
{
    GstElement *pipeline;
    GstElement *src;
    GstElement *demuxer;

    GstElement *v_queue;
    GstElement *v_parse;
    GstElement *v_decoder;
    GstElement *v_gld;
    GstElement *v_enc;
    GstElement *v_sink;



} CustomData;




#define PRINT_LINE printf("\n line = %d\n", __LINE__ );


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

    if ( g_str_has_prefix( name, "video/x-h264" ) ) 
    {
        sinkpad = gst_element_get_static_pad ( data->v_queue, "sink");
        if(  gst_pad_link ( pad, sinkpad ) != GST_PAD_LINK_OK )
            printf("not link !!\n\n"); 
        gst_object_unref (sinkpad);
    }
    else if( g_str_has_prefix( name, "audio/mpeg" ) )
    {
        PRINT_LINE
    }
    else
        printf("\n pass %s\n", name);

    gst_caps_unref (caps);
}


gboolean print_field( GQuark field, const GValue* value, gpointer pfx ) 
{
    gchar *str = gst_value_serialize(value);
    g_print("%s  %15s: %s\n", (gchar *)pfx, g_quark_to_string(field), str);
    g_free(str);
    return TRUE;
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
			gst_message_parse_error( msg, &error, &debug );
            g_free(debug);
            g_printerr("ERROR:%s\n",error->message);
            g_error_free(error);

            if( loop != NULL )
                g_main_loop_quit(loop);
            break;
        }
        case GST_MESSAGE_ELEMENT:
        {
            const GstStructure *stc = gst_message_get_structure(msg);
            if( gst_structure_has_name( stc, "GstMultiFileSink") )
                gst_structure_foreach( stc, print_field, "GstMultiFileSink" ); 
            g_print("\n");           
            break;
        }
        default:            
            //PRINT_LINE
            printf("default = %d\n", GST_MESSAGE_TYPE(msg) );
            break;
    }
    return TRUE;
}





int main( int argc, char *argv[] ) 
{
    struct timeval start;
    struct timeval end;
    unsigned long timer;
    gettimeofday( &start, NULL );




    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    gboolean terminate = FALSE;

    gst_init ( NULL, NULL );

    GMainLoop *loop = g_main_loop_new ( NULL, FALSE );   

    data.src = gst_element_factory_make( "filesrc", "source" );
    g_object_set( G_OBJECT(data.src), "location", "/home/hidog/Videos/test.mp4", NULL );

    data.demuxer = gst_element_factory_make ("qtdemux", "demuxer");

    data.v_queue = gst_element_factory_make( "queue", "v_queue" );
    data.v_parse = gst_element_factory_make( "h264parse", "v_parse" );
    data.v_decoder = gst_element_factory_make( "nvdec", "v_decoder" );  
    data.v_gld = gst_element_factory_make( "gldownload", "v_gld" );
    data.v_enc = gst_element_factory_make( "jpegenc", "v_enc" );
    //data.v_enc = gst_element_factory_make( "nvh264enc", "v_enc" );
 
    data.v_sink = gst_element_factory_make( "multifilesink", "v_sink" );
    g_object_set( G_OBJECT(data.v_sink), "location", "./jpg/file_%d.jpg", NULL );
    g_object_set( G_OBJECT(data.v_sink), "post-messages", TRUE, NULL );

    data.pipeline = gst_pipeline_new ("pipeline");
    bus = gst_element_get_bus( data.pipeline );
    gst_bus_add_watch( bus, bus_call, loop );
    gst_object_unref (bus);

    gst_bin_add_many( GST_BIN(data.pipeline), data.src, data.demuxer, 
                                              data.v_queue, data.v_parse, data.v_decoder, data.v_gld, data.v_enc, data.v_sink, NULL );

    gst_element_link_many( data.src, data.demuxer, NULL );
    gst_element_link_many( data.v_queue, data.v_parse, data.v_decoder, data.v_gld, data.v_enc, data.v_sink, NULL );

    g_signal_connect( G_OBJECT(data.demuxer), "pad-added", G_CALLBACK(pad_added_handler), &data );
    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );

    g_main_loop_run(loop);


    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref (data.pipeline);



    gettimeofday( &end, NULL );
    timer = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("timer = %ld ms %ld s %ld m\n", timer/1000, timer/1000000, timer/60000000 );
    // GST_MESSAGE_EOStimer = 73364 ms 73 s 1 m
    // GST_MESSAGE_EOStimer = 129342 ms 129 s 2 m

    return 0;
}








