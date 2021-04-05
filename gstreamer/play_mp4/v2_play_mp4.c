/*
播放mp4檔案,將控制部份改用bus call-back來實做
*/

#include <gst/gst.h>
#include <stdio.h>

/* Structure to contain all our information, so we can pass it to callbacks */
typedef struct _CustomData 
{
    GstElement *pipeline;
    GstElement *source;
    GstElement *a_convert;
    GstElement *a_sink;
    GstElement *v_convert;
    GstElement *v_sink;
} CustomData;



/* This function will be called by the pad-added signal */
void pad_added_handler (GstElement *src, GstPad *new_pad, CustomData *data) 
{
    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = NULL;
    GstStructure *new_pad_struct = NULL;
    const gchar *new_pad_type = NULL;

    g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

    /* Check the new pad's type */
    new_pad_caps = gst_pad_get_current_caps (new_pad);
    new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
    new_pad_type = gst_structure_get_name (new_pad_struct);

    GstPad *sink_pad = NULL;

    if ( g_str_has_prefix (new_pad_type, "video/x-raw") )
    {
        sink_pad = gst_element_get_static_pad (data->v_convert, "sink");
        /* If our converter is already linked, we have nothing to do here */
        if (gst_pad_is_linked (sink_pad)) 
        {
            g_print ("We are already linked. Ignoring.\n");
            goto exit;
        }

        ret = gst_pad_link (new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED (ret))   
            g_print ("Type is '%s' but link failed.\n", new_pad_type);
        else   
            g_print ("Link succeeded (type '%s').\n", new_pad_type);  
    }
    else if ( g_str_has_prefix (new_pad_type, "audio/x-raw") ) 
    {
        sink_pad = gst_element_get_static_pad (data->a_convert, "sink");
        /* If our converter is already linked, we have nothing to do here */
        if (gst_pad_is_linked (sink_pad)) 
        {
            g_print ("We are already linked. Ignoring.\n");
            goto exit;
        }

        ret = gst_pad_link (new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED (ret))   
            g_print ("Type is '%s' but link failed.\n", new_pad_type);
        else   
            g_print ("Link succeeded (type '%s').\n", new_pad_type);  
    }
    else
        printf("\n\n\n !!! error !!! \n\nn");

exit:
    /* Unreference the new pad's caps, if we got them */
    if (new_pad_caps != NULL)
        gst_caps_unref (new_pad_caps);

    /* Unreference the sink pad */
    gst_object_unref (sink_pad);
}




gboolean bus_call( GstBus *bus, GstMessage *msg, gpointer data )
{
    GMainLoop *loop = (GMainLoop *)data;
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

            if( loop != NULL )
                g_main_loop_quit(loop);
            break;
        }
        default:            
            printf("default = %d\n", GST_MESSAGE_TYPE(msg) );
            break;
    }
    return TRUE;
}



/*
    將v1改寫成用callback方式控制msg
*/
#define PRINT_LINE printf(" line = %d\n", __LINE__);
int main(int argc, char *argv[]) 
{
    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    gboolean terminate = FALSE;

    /* Initialize GStreamer */
    gst_init ( NULL, NULL );

    /* Create the elements */
    data.source = gst_element_factory_make ("uridecodebin", "source");

    data.v_convert = gst_element_factory_make ("videoconvert", "vconvert");
    data.v_sink = gst_element_factory_make ("autovideosink", "vsink");

    data.a_convert = gst_element_factory_make ("audioconvert", "aconvert");
    data.a_sink = gst_element_factory_make ("autoaudiosink", "asink");

    PRINT_LINE

    /* Create the empty pipeline */
    data.pipeline = gst_pipeline_new ("test-pipeline");

    if (!data.pipeline || !data.source || !data.v_convert || !data.v_sink || !data.a_convert || !data.a_sink ) 
    {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

    PRINT_LINE

    /* Build the pipeline. Note that we are NOT linking the source at this
     * point. We will do it later. */
    gst_bin_add_many (GST_BIN (data.pipeline), data.source, data.v_convert , data.v_sink, data.a_convert, data.a_sink, NULL);
    if (!gst_element_link (data.v_convert, data.v_sink)) 
    {
        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (data.pipeline);
        return -1;
    }

    if (!gst_element_link (data.a_convert, data.a_sink)) 
    {
        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (data.pipeline);
        return -1;
    }

    PRINT_LINE

    /* Set the URI to play */
    g_object_set (data.source, "uri", "file:///home/hidog/Videos/test.mp4" , NULL);

    /* Connect to the pad-added signal */
    g_signal_connect (data.source, "pad-added", G_CALLBACK (pad_added_handler), &data);

    /* Start playing */
    ret = gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) 
    {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (data.pipeline);
        return -1;
    }

    GMainLoop *loop = g_main_loop_new ( NULL, FALSE );
    bus = gst_element_get_bus( data.pipeline );
    gst_bus_add_watch( bus, bus_call, loop );

    g_main_loop_run(loop);


    /* Free resources */
    gst_object_unref (bus);
    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref (data.pipeline);
    return 0;
}


