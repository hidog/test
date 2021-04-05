// gcc tcpclientsink_test.c -o tcpclientsink_test `pkg-config --cflags --libs gstreamer-1.0`

#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>


//声明所用到的element，总线bus，主循环loop
GstElement *pipeline, *source, *sink;
GstBus *bus;
GMainLoop *loop;
 
 
// 消息处理函数(EOS和ERROR)
static gboolean bus_call(GstBus *bus,GstMessage *msg, gpointer data)
{
    GMainLoop *m_loop = (GMainLoop *) data;//这个是主循环的指针，在接受EOS消息时退出循环
    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            printf("GST_MESSAGE_EOS");
            g_main_loop_quit(m_loop);
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
    static guint16 i = 0; 
    printf( "Iter=%d\n", i++ ); 
    
    if( i%5 == 0 )
    { 
        printf("try to stop loop1??\n"); 
        //g_main_loop_quit((GMainLoop*)data); 
    } 
    return TRUE;
}





 
int main(int argc, char *argv[]) 
{
#if 0
  /*
    在gstreamer可以用 gst-launch的方式下命令, 例如    
    gst-launch-1.0 filesrc location=/home/andrea/code/test.mp3 ! tcpclientsink host=127.0.0.1 port=3000
    這行為相當於底下的gst_parse_launch
  */
  GstElement *pipeline;
  GstBus *bus;
  GstMessage *msg;

  /* Initialize GStreamer */
  gst_init ( NULL, NULL );

  /* Build the pipeline */
  pipeline = gst_parse_launch ("filesrc location=/home/andrea/code/test.mp3 ! tcpclientsink host=127.0.0.1 port=3000", NULL);

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
  
#elif 1
    /*
        某些時候,gst-launch的行為可以改寫成C code, 例如底下的範例.
        但不是全部的情況都支援.
    */
    // 初始化
    gst_init ( NULL, NULL );

    //GMainContext* con1 = NULL;
    //con1 = g_main_context_new();

   // gst-launch-1.0 filesrc location=./1.mp3 ! tcpclientsink host="127.0.0.1" port=3000
   

	// 创建主循环
    loop = g_main_loop_new ( NULL, FALSE);
	
	//创建各个element，其中gst_element_factory_make第一个参数为gstreamer插件名称
    pipeline = gst_pipeline_new ("tcp-client-sink");

    source = gst_element_factory_make ("filesrc", "source");
    g_object_set( G_OBJECT(source), "location", "/home/hidog/code/test.mp3", NULL );

    /*
        加入callback後不能播,需要研究原因
    */
    //g_source_set_callback (source, callback, loop, NULL);
    //g_source_attach (source, con1);


    sink = gst_element_factory_make ("tcpclientsink", "sink");
    g_object_set( G_OBJECT(sink), "host", "127.0.0.1", "port", 3000, NULL );



    if ( !pipeline || !source || !sink ) {
        g_print ("One element could not be created\n");
        return -1;
    }
 
	//获取播放管道的消息总线并设置获取消息后的回调函数
    bus = gst_pipeline_get_bus (GST_PIPELINE(pipeline));
    gst_bus_add_watch (bus, bus_call, loop );
 
	//将各element添加到管道中
    gst_bin_add_many( GST_BIN(pipeline), source, sink, NULL);
 
	//依次连接各element
    gst_element_link_many( source, sink, NULL );
 
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
#else
    printf("un defined.");
#endif
    return 0;
}

