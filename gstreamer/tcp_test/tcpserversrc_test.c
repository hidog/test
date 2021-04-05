// gcc tcpserversrc_test.c -o tcpserversrc_test `pkg-config --cflags --libs gstreamer-1.0`


#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>


//声明所用到的element，总线bus，主循环loop
GstElement *pipeline, *source, *decoder, *conv, *sink;
GstBus *bus;
GMainLoop *loop;

 
 
// 消息处理函数(EOS和ERROR)
static gboolean bus_call(GstBus *bus,GstMessage *msg,gpointer data)
{
    //GMainLoop *loop = (GMainLoop *) data;//这个是主循环的指针，在接受EOS消息时退出循环
    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            printf("GST_MESSAGE_EOS\n");
            //g_main_loop_quit(loop);
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





/*static gboolean bus_call (G_GNUC_UNUSED GstBus *bus, GstMessage *msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {

  case GST_MESSAGE_EOS:
    g_print ("End of stream\n");
    g_main_loop_quit (loop);
    break;

  case GST_MESSAGE_ERROR: {
    gchar  *debug;
    GError *error;

    gst_message_parse_error (msg, &error, &debug);
    g_free (debug);

    g_printerr ("Error: %s\n", error->message);
    g_error_free (error);

    g_main_loop_quit (loop);
    break;
  }
  default:
    break;
  }

  return TRUE;
}*/


gboolean callback(gpointer data)
{
    printf("\n!! data = %d\n", data );
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
#if 0
    /*
        用 gst-launch 下cmd, 底下的code能work.
    */
  GstElement *pipeline;
  GstBus *bus;
  GstMessage *msg;

  // Initialize GStreamer 
  gst_init ( NULL, NULL );

  // Build the pipeline 
  pipeline = gst_parse_launch ("tcpserversrc host=127.0.0.1 port=3000 ! decodebin ! autoaudiosink", NULL);

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


#elif 0
    /*
        底下這段code不能work, 需要動態使用add-pad
        記得是類似async的概念, 具體原因參考官方文件
    */

    // 初始化
    gst_init ( NULL, NULL );

    //GMainContext* con1 = NULL;
    //con1 = g_main_context_new();

   // gst-launch-1.0 tcpserversrc host="127.0.0.1" port=3000 ! decodebin ! autoaudiosink


	// 创建主循环
    loop = g_main_loop_new ( NULL, FALSE);
	
	//创建各个element，其中gst_element_factory_make第一个参数为gstreamer插件名称
    pipeline = gst_pipeline_new ("tcp-server");

    source = gst_element_factory_make ("tcpserversrc", "source");
    g_object_set( G_OBJECT(source), "host", "127.0.0.1", "port", 3000, NULL );


    //g_source_set_callback (source, callback, loop, NULL);
    //g_source_attach (source, con1);




    decoder = gst_element_factory_make ( "decodebin", "decoder" );

    sink = gst_element_factory_make ("autoaudiosink", "sink");

    if ( !pipeline || !source || !decoder || !sink  ) {
        g_print ("One element could not be created\n");
        return -1;
    }
 
	//获取播放管道的消息总线并设置获取消息后的回调函数
    bus = gst_pipeline_get_bus (GST_PIPELINE(pipeline));
    gst_bus_add_watch (bus, bus_call, NULL);
 
	//将各element添加到管道中
    gst_bin_add_many( GST_BIN(pipeline), source, decoder, sink, NULL);
 
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
#elif 1
    //https://stackoverflow.com/questions/41698656/convert-gstreamer-command-to-c-code

    /* init GStreamer */
    gst_init ( NULL, NULL );
    loop = g_main_loop_new (NULL, FALSE);

    /* setup */
    pipeline = gst_pipeline_new ("pipeline");

    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

    source = gst_element_factory_make ("tcpserversrc", "src");
    g_object_set( G_OBJECT(source), "host", "127.0.0.1",NULL);
    g_object_set( G_OBJECT(source), "port", 3000 ,NULL);

    decoder = gst_element_factory_make ("decodebin", "dec");
    conv = gst_element_factory_make ("audioconvert", "conv");
    sink = gst_element_factory_make ("alsasink", "sink");

    gst_bin_add_many( GST_BIN(pipeline), source, decoder, conv, sink, NULL );

    gst_element_link( source, decoder );
    gst_element_link( conv, sink );
    //gst_element_link_many( src, dec, conv, sink, NULL );

    /* you don't link them here */
    /* gst_element_link (dec, conv); */
    /* add call-back, instead */
    g_signal_connect( decoder, "pad-added", G_CALLBACK (cb_new_pad), conv);

    /* run */
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    g_main_loop_run (loop);

    /* cleanup */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (pipeline));

#else
    printf("not defined");
#endif

    return 0;
}







