
#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>


GstElement *pipeline;
 
 
// 消息处理函数(EOS和ERROR)
static gboolean bus_call(GstBus *bus,GstMessage *msg,gpointer data)
{
    GMainLoop *loop = (GMainLoop *) data;//这个是主循环的指针，在接受EOS消息时退出循环
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
        case GST_MESSAGE_ASYNC_DONE :
            gst_element_set_state( pipeline, GST_STATE_PLAYING );
            break;
        default:
            printf("default = %d\n", GST_MESSAGE_TYPE(msg) );
             break;
    }
    return TRUE;
}



//vhttps://hant-kb.kutu66.com/gstreamer/post_13147630


void test( GstElement *element, GstPad *pad, gpointer data )
{
    printf( "get name = %s\n", gst_element_get_name(element) );
    GstElement *pipeline = (GstElement*)data;
    printf("\n\nTEst\n\n");
    gst_element_set_state( pipeline, GST_STATE_PLAYING );
}

/*

gst-launch-1.0 -v filesrc location=/home/andrea/code/test.mp3 ! audioparse ! tcpserversink port=3000 host=127.0.0.1
gst-launch-1.0 tcpclientsrc port=3000 host=127.0.0.1 ! decodebin ! autoaudiosink
上面這組可以播放,但沒辦法repeat.

gst-launch-1.0 -v multifilesrc location=/home/andrea/code/test.mp3 loop=true ! audioparse ! tcpserversink port=3000 host=127.0.0.1
server改這樣可以repeat,但是播放的時候會慢一段時間才開始播放

目前的c code尚有一些問題  (補充client是cmd line)
播放的時候會缺少一部分的音訊
另外在call back的時候不能改變pipeline狀態,會跳錯誤訊息.


*/

 
int main(int argc, char *argv[]) 
{
#if 0
  GstElement *pipeline;
  GstBus *bus;
  GstMessage *msg;

  // Initialize GStreamer 
  gst_init ( NULL, NULL );

  // Build the pipeline 
  pipeline = gst_parse_launch ("filesrc location=/home/andrea/code/test.mp3 ! tcpserversink host=127.0.0.1 port=3000", NULL);

  // Start playing 
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  // Wait until error or EOS 
  bus = gst_element_get_bus (pipeline);
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  // Free resources 
  if (msg != NULL)
    gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;


#elif 1
    // 初始化
    gst_init ( NULL, NULL );

    //
    GstElement *source, *parse, *sink;
    GstBus *bus;
    GMainLoop *loop;

   // gst-launch-1.0 tcpserversrc host="127.0.0.1" port=3000 ! decodebin ! autoaudiosink


	// 创建主循环
    loop = g_main_loop_new ( NULL, FALSE );
	
	//
    pipeline = gst_pipeline_new ("tcp-server");

    source = gst_element_factory_make ( "filesrc", "source" );
    g_object_set( G_OBJECT(source), "location", "/home/hidog/code/test.mp3", NULL );

    parse = gst_element_factory_make( "audioparse", "parse" );

    sink = gst_element_factory_make ("tcpserversink", "sink");
    g_object_set( G_OBJECT(sink), "host", "127.0.0.1", "port", 3000, NULL );

    if ( !pipeline || !source || !sink  ) 
    {
        g_print ("One element could not be created\n");
        return -1;
    }
 
	//
    bus = gst_pipeline_get_bus (GST_PIPELINE(pipeline));
    gst_bus_add_watch (bus, bus_call, NULL);
 
	//
    gst_bin_add_many( GST_BIN(pipeline), source, parse, sink, NULL);
    gst_element_link_many( source, parse, sink, NULL );

    //client_added_callback( sink, test, source );
    g_signal_connect( sink, "client-added", G_CALLBACK(test), pipeline );
 
 	//
    gst_element_set_state( pipeline, GST_STATE_PAUSED );
    g_print("Running\n");
 

    //开始主循环
    g_main_loop_run(loop);
 
   	//循环退出后，释放资源
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);


#endif

    return 0;
}

