/*

播放mp3的簡單範例

gcc tcpserversrc_test.c -o tcpserversrc_test `pkg-config --cflags --libs gstreamer-1.0`
    
    
    
https://gstreamer.freedesktop.org/documentation/application-development/appendix/checklist-element.html?gi-language=c
./a.outo --gst-debug-level=9  
可以印出訊息 
    
*/

#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>


//
GstElement *pipeline, *source, *decoder, *conv, *sink;
GstBus *bus;
GMainLoop *loop;

 
 
//
static gboolean bus_call(GstBus *bus,GstMessage *msg,gpointer data)
{
    GMainLoop *loop = (GMainLoop *) data;
    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            printf("GST_MESSAGE_EOS\n");
            g_main_loop_quit(loop);
            break;
        case GST_MESSAGE_ERROR:
        {
            printf("GST_MESSAGE_ERROR");
            gchar *debug;
            GError *error;
			gst_message_parse_error(msg,&error,&debug);
            g_free(debug);
            g_printerr("ERROR:%s\n",error->message);
            g_error_free(error);
            g_main_loop_quit(loop);
            break;
        }
        default:
            //printf("default = %d\n", GST_MESSAGE_TYPE(msg) );
            break;
    }
    return TRUE;
}




void cb_new_pad (GstElement *element, GstPad *pad, gpointer data)
{
  gchar *name;
  GstElement *other = data;

  name = gst_pad_get_name (pad);
  g_print ("A new pad %s was created for %s\n", name, gst_element_get_name(element));
  g_free (name);

  g_print ("element %s will be linked to %s\n",
           gst_element_get_name(element),
           gst_element_get_name(other));
  gst_element_link(element, other);
}


 
int main(int argc, char *argv[]) 
{
    /* init GStreamer */

    gst_init ( &argc, &argv );  
    loop = g_main_loop_new (NULL, FALSE);

    /* setup */
    pipeline = gst_pipeline_new ("pipeline");

    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

    source = gst_element_factory_make ("filesrc", "src");
    g_object_set( G_OBJECT(source), "location", "/home/hidog/code/test.mp3",NULL);  // 這邊改成mp4檔案,可以只播出聲音. 
    // 把底下物件改成video,可以只撥影像

    decoder = gst_element_factory_make ("decodebin", "dec");
    //decoder = gst_element_factory_make ("audiodecodebin", "dec"); // 這邊會錯,可能沒這個物件?
    conv = gst_element_factory_make ("audioconvert", "conv");
    //sink = gst_element_factory_make ("alsasink", "sink");    // 兩個都能播,有空研究一下差別
    sink = gst_element_factory_make ("autoaudiosink", "sink");

    //GstElement* sink2 = gst_element_factory_make("autovideosink", "sink2");
    gst_bin_add_many( GST_BIN(pipeline), source, decoder, conv, sink,  NULL );  // 這邊隨便增加物件或減少物件,都會造成不能播放 

    // 不能直連,要用動態方式
    gst_element_link( source, decoder );
    gst_element_link( conv, sink );       // 沒串sink會報錯
    //gst_element_link_many( source, decoder, conv, sink, NULL );

    /* you don't link them here */
    /* gst_element_link (dec, conv); */
    /* add call-back, instead */
    g_signal_connect( decoder, "pad-added", G_CALLBACK (cb_new_pad), conv );

    /* run */
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    g_main_loop_run (loop);

    /* cleanup */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (pipeline));


    return 0;
}







