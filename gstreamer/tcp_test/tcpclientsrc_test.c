#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>


//声明所用到的element，总线bus，主循环loop
GstElement *pipeline, *source, *decoder, *sink;
GstBus *bus;
GMainLoop *loop;
 
 
// 消息处理函数(EOS和ERROR)
static gboolean bus_call(GstBus *bus,GstMessage *msg, gpointer data)
{
    //MainLoop *loop = (GMainLoop *) data;//这个是主循环的指针，在接受EOS消息时退出循环
    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            printf("GST_MESSAGE_EOS");
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
            printf("default = %d\n", GST_MESSAGE_TYPE(msg) );
             break;
    }
    return TRUE;
}



gboolean callback(gpointer data)
{
    printf("\n!! data = %d\n", data );
  return TRUE;
}





 /*
    這邊的範例測試失敗
 */
int main(int argc, char *argv[]) 
{
#if 1
  GstElement *pipeline;
  GstBus *bus;
  GstMessage *msg;

  /* Initialize GStreamer */
  gst_init ( NULL, NULL );

  /* Build the pipeline */
  pipeline = gst_parse_launch ("tcpclientsrc host=127.0.0.1 port=3000 ! decodebin ! autoaudiosink", NULL);

  /* Start playing */
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Wait until error or EOS */
  bus = gst_element_get_bus (pipeline);
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  /* Free resources */
  if (msg != NULL)
    gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
#endif

#if 0
    // 初始化
    gst_init ( NULL, NULL );

    //GMainContext* con1 = NULL;
    //con1 = g_main_context_new();

   // gst-launch-1.0 filesrc location=./1.mp3 ! tcpclientsink host="127.0.0.1" port=3000
   

	// 创建主循环
    loop = g_main_loop_new ( NULL, FALSE);
	
	//创建各个element，其中gst_element_factory_make第一个参数为gstreamer插件名称
    pipeline = gst_pipeline_new ("tcp-client-2");

    source = gst_element_factory_make ("tcpclientsrc", "source");
    g_object_set( G_OBJECT(source), "host", "127.0.0.1", "port", 3000, NULL );


    //g_source_set_callback (source, callback, loop, NULL);
    //g_source_attach (source, con1);

    decoder = gst_element_factory_make ( "decodebin", "decoder" );

    sink = gst_element_factory_make ("autoaudiosink", "sink");



    if ( !pipeline || !source || !sink || !decoder ) {
        g_print ("One element could not be created\n");
        return -1;
    }
 
	//获取播放管道的消息总线并设置获取消息后的回调函数
    bus = gst_pipeline_get_bus (GST_PIPELINE(pipeline));
    gst_bus_add_watch (bus, bus_call, NULL);
 
	//将各element添加到管道中
    gst_bin_add_many( GST_BIN(pipeline), source, sink, NULL);
 
	//依次连接各element
    gst_element_link_many( source, decoder, sink, NULL );
 
 	//开始播放，设置播放状态为GST_STATE_PLAYING
    gst_element_set_state( pipeline, GST_STATE_PLAYING );
    g_print("Running\n");
 





    //开始主循环
    g_main_loop_run(loop);
 
   	//循环退出后，释放资源
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);

    //g_main_context_unref (con1);
    //con1 = NULL;
#endif
    return 0;
}








