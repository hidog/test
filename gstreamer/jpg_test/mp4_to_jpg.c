/*
讀取mp4,轉成jpg

轉png的範例
gst-launch-1.0 filesrc location=~/Videos/test.mp4 ! decodebin ! queue ! autovideoconvert ! pngenc ! multifilesink location=./tmp/frame_%d.png   

轉jpg的範例 
gst-launch-1.0 filesrc location="/home/hidog/Videos/test.mp4" ! decodebin ! queue ! videoconvert ! jpegenc ! multifilesink location="./jpg/file_%d.jpg"


分離音軌產生mp3
gst-launch-1.0 filesrc location=~/Videos/test.mp4 ! qtdemux name=demux demux. ! queue ! faad ! lamemp3enc quality=2 target=bitrate bitrate=192 cbr=true ! id3v2mux ! filesink location=./file.mp3

參考 https://pevensay.github.io/2018/08/23/%E4%BD%BF%E7%94%A8gstreamer%E8%BF%9B%E8%A1%8C%E8%A7%86%E9%A2%91%E5%88%86%E5%89%B2%E4%B8%8E%E5%90%88%E6%88%90/

其他參考 https://www.tal.org/tutorials/timelapse-video-gstreamer
         https://www.raspberrypi.org/forums/viewtopic.php?t=134466

研究callback https://stackoverflow.com/questions/42417103/gstreamer-eos-message-handling-in-filesink-to-change-location-on-the-fly
*/


/*
這個失敗沒成功
gst-launch-1.0 filesrc location="/home/hidog/Videos/test.mp4" ! nvdec ! gldownload ! videoconvert ! jpegenc ! multifilesink location="./tmp/file_%d.jpg"

想嘗試轉RGBA, 還沒完成指令
gst-launch-1.0 filesrc location="h264_looped_high.mp4" ! qtdemux ! h264parse ! "video/x-h264, stream-format=(string)byte-stream, alignment=(string)au" 
 ! nvdec ! "video/x-raw(memory:GLMemory),format=(string)NV12" ! glcolorconvert ! "video/x-raw(memory:GLMemory),format=(string)RGBA" ! 
*/










#include<sys/time.h>


// https://stackoverflow.com/questions/13500893/decode-a-mp4-video-with-gstreamer

#include <gst/gst.h>
#include <stdio.h>
#include <assert.h>


typedef struct _CustomData 
{
    GstElement *pipeline;
    GstElement *src;
    GstElement *demuxer;

    GstElement *v_queue;
    GstElement *v_decoder;
    GstElement *v_convert;
    GstElement *v_enc;
    GstElement *v_sink;

    GstElement *a_queue;
    GstElement *a_decoder;
    GstElement *a_enc;
    GstElement *a_mux;
    GstElement *a_sink;


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
        sinkpad = gst_element_get_static_pad ( data->a_queue, "sink");
        if(  gst_pad_link ( pad, sinkpad ) != GST_PAD_LINK_OK )
            printf("not link !!\n\n"); 
        gst_object_unref (sinkpad);
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
    data.v_decoder = gst_element_factory_make( "avdec_h264", "v_decoder" );  
    data.v_convert = gst_element_factory_make( "videoconvert", "v_convert" );
    data.v_enc = gst_element_factory_make( "jpegenc", "v_enc" );
 
    data.v_sink = gst_element_factory_make( "multifilesink", "v_sink" );
    g_object_set( G_OBJECT(data.v_sink), "location", "./jpg/file_%d.jpg", NULL );
    g_object_set( G_OBJECT(data.v_sink), "post-messages", TRUE, NULL );

    data.a_queue = gst_element_factory_make( "queue", "a_queue" );
    data.a_decoder = gst_element_factory_make( "faad", "a_decoder" );

    data.a_enc = gst_element_factory_make( "lamemp3enc", "a_enc" );
    assert( data.a_enc != NULL );
    g_object_set( G_OBJECT(data.a_enc), "quality", 2.0,
                                        "target", 1,              //bitrate (1) – Bitrate   
                                        "bitrate", 192,
                                        "cbr", TRUE, NULL );

    data.a_mux = gst_element_factory_make( "id3v2mux", "a_mux" );
    
    data.a_sink = gst_element_factory_make( "filesink", "a_sink" );
    g_object_set( G_OBJECT(data.a_sink), "location", "./test.mp3", NULL );

    assert( data.a_queue != NULL );

    data.pipeline = gst_pipeline_new ("pipeline");
    bus = gst_element_get_bus( data.pipeline );
    gst_bus_add_watch( bus, bus_call, loop );
    gst_object_unref (bus);
    gst_bin_add_many( GST_BIN(data.pipeline), data.src, data.demuxer, 
                                              data.v_queue, data.v_decoder, data.v_convert, data.v_enc, data.v_sink,
                                              data.a_queue, data.a_decoder, data.a_enc, data.a_mux, data.a_sink, NULL );
    gst_element_link_many( data.src, data.demuxer, NULL );
    gst_element_link_many( data.v_queue, data.v_decoder, data.v_convert, data.v_enc, data.v_sink, NULL );
    gst_element_link_many( data.a_queue, data.a_decoder, data.a_enc, data.a_mux, data.a_sink, NULL );

    g_signal_connect( G_OBJECT(data.demuxer), "pad-added", G_CALLBACK(pad_added_handler), &data );
    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );

    g_main_loop_run(loop);


    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref (data.pipeline);


    gettimeofday( &end, NULL );
    timer = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("timer = %ld ms %ld s %ld m\n", timer/1000, timer/1000000, timer/60000000 );
    // GST_MESSAGE_EOStimer = 257986 ms 257 s 4 m

    return 0;
}








