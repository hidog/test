/*
使用tcpclientsink
可以讀取一個mp4檔案,傳給tcpserversrc做播放
這邊的程式碼設計,需要先執行server
*/


#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>



 
 
// 消息处理函数(EOS和ERROR)
static gboolean bus_call(GstBus *bus,GstMessage *msg, gpointer data)
{
    GMainLoop *loop = (GMainLoop *) data;//这个是主循环的指针，在接受EOS消息时退出循环
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






 
int main(int argc, char *argv[]) 
{
    // 初始化
    gst_init ( NULL, NULL );   

    GstElement *pipeline, *source, *sink;
    GstBus *bus;
    GMainLoop *loop;

	// 创建主循环
    loop = g_main_loop_new ( NULL, FALSE);
	
	// 创建各个element，其中gst_element_factory_make第一个参数为gstreamer插件名称
    pipeline = gst_pipeline_new ("tcp-client-sink");

    source = gst_element_factory_make( "filesrc", "source" );
    g_object_set( G_OBJECT(source), "location", "/home/hidog/Videos/test.mp4", NULL );

    sink = gst_element_factory_make ("tcpclientsink", "sink");
    g_object_set( G_OBJECT(sink), "host", "127.0.0.1", "port", 3000, NULL );


    if ( !pipeline || !source || !sink ) 
    {
        g_print ("One element could not be created\n");
        return -1;
    }
 
	//获取播放管道的消息总线并设置获取消息后的回调函数
    bus = gst_pipeline_get_bus (GST_PIPELINE(pipeline));
    gst_bus_add_watch (bus, bus_call, loop );
 
	//将各element添加到管道中
    gst_bin_add_many( GST_BIN(pipeline), source, sink, NULL );
 
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


    return 0;
}

