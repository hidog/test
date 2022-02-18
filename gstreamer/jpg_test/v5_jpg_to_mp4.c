/*
沒有執行過得程式
看起來應該是實驗用
有空研究
*/



#include <gst/gst.h>
//#include <stddef.h>
//#include <stdlib.h>
//#include <string.h>
#include <stdio.h>
//#include <png.h>
//#include <zlib.h>
//#include <gst/app/gstappsrc.h>
//#include <gst/app/gstappsink.h>
//#include <malloc.h>

#define TOP 1
#define HEIGHT 512
#define WIDTH 640




/* Functions below print the Capabilities in a human-friendly format */
static gboolean print_field (GQuark field, const GValue * value, gpointer pfx) {
  gchar *str = gst_value_serialize (value);

  g_print ("%s  %15s: %s\n", (gchar *) pfx, g_quark_to_string (field), str);
  g_free (str);
  return TRUE;
}

static void print_caps (const GstCaps * caps, const gchar * pfx) {
  guint i;

  g_return_if_fail (caps != NULL);

  if (gst_caps_is_any (caps)) {
    g_print ("%sANY\n", pfx);
    return;
  }
  if (gst_caps_is_empty (caps)) {
    g_print ("%sEMPTY\n", pfx);
    return;
  }

  for (i = 0; i < gst_caps_get_size (caps); i++) {
    GstStructure *structure = gst_caps_get_structure (caps, i);

    g_print ("%s%s\n", pfx, gst_structure_get_name (structure));
    gst_structure_foreach (structure, print_field, (gpointer) pfx);
  }
}

/* Prints information about a Pad Template, including its Capabilities */
static void print_pad_templates_information (GstElementFactory * factory) {
  const GList *pads;
  GstStaticPadTemplate *padtemplate;

  g_print ("Pad Templates for %s:\n", gst_element_factory_get_longname (factory));
  if (!gst_element_factory_get_num_pad_templates (factory)) {
    g_print ("  none\n");
    return;
  }

  pads = gst_element_factory_get_static_pad_templates (factory);
  while (pads) {
    padtemplate = pads->data;
    pads = g_list_next (pads);

    if (padtemplate->direction == GST_PAD_SRC)
      g_print ("  SRC template: '%s'\n", padtemplate->name_template);
    else if (padtemplate->direction == GST_PAD_SINK)
      g_print ("  SINK template: '%s'\n", padtemplate->name_template);
    else
      g_print ("  UNKNOWN!!! template: '%s'\n", padtemplate->name_template);

    if (padtemplate->presence == GST_PAD_ALWAYS)
      g_print ("    Availability: Always\n");
    else if (padtemplate->presence == GST_PAD_SOMETIMES)
      g_print ("    Availability: Sometimes\n");
    else if (padtemplate->presence == GST_PAD_REQUEST)
      g_print ("    Availability: On request\n");
    else
      g_print ("    Availability: UNKNOWN!!!\n");

    if (padtemplate->static_caps.string) {
      GstCaps *caps;
      g_print ("    Capabilities:\n");
      caps = gst_static_caps_get (&padtemplate->static_caps);
      print_caps (caps, "      ");
      gst_caps_unref (caps);

    }

    g_print ("\n");
  }
}

/* Shows the CURRENT capabilities of the requested pad in the given element */
static void print_pad_capabilities (GstElement *element, gchar *pad_name) {
  GstPad *pad = NULL;
  GstCaps *caps = NULL;

  /* Retrieve pad */
  pad = gst_element_get_static_pad (element, pad_name);
  if (!pad) {
    g_printerr ("Could not retrieve pad '%s'\n", pad_name);
    return;
  }

  /* Retrieve negotiated caps (or acceptable caps if negotiation is not finished yet) */
  caps = gst_pad_get_current_caps (pad);
  if (!caps)
    caps = gst_pad_query_caps (pad, NULL);

  /* Print and free */
  g_print ("Caps for the %s pad:\n", pad_name);
  print_caps (caps, "      ");
  gst_caps_unref (caps);
  gst_object_unref (pad);
}




int main( int argc, char *argv[] )
{
    	
    GstElement *pipeline, *source, *convert, *enc, *parse, *sink, *capsFilter2, *capsFilter1;
 	GstCaps *capsConvert, *capsSource;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    		  
    /* Initialize GStreamer */
    gst_init ( NULL, NULL );

    /* Create the elements */
    //1-SOURCE
    source = gst_element_factory_make( "multifilesrc", "source" );
    g_object_set (G_OBJECT (source), "location", "./jpg/file_%d.jpg",  NULL);
    //g_object_set (G_OBJECT (source), "start-index", 0,  NULL);	
    //g_object_set (G_OBJECT (source), "stop-index", 30,  NULL);	
    //CAPS Filter Source 
    capsFilter1 = gst_element_factory_make( "capsfilter", NULL );
    capsSource = gst_caps_new_simple ( "video/x-raw",
                                       "format",  G_TYPE_STRING, "I420",
                                       "width", G_TYPE_INT, 1280,
                                       "height", G_TYPE_INT, 720,
                                       "framerate", GST_TYPE_FRACTION, 30, 1, NULL );
    g_object_set( capsFilter1, "caps", capsSource, NULL );


    //2-CONVERSION 
    convert = gst_element_factory_make( "videoconvert", NULL );
    capsFilter2 = gst_element_factory_make( "capsfilter", NULL );
    capsConvert = gst_caps_new_simple( "video/x-raw",
                                       "format",  G_TYPE_STRING, "I420",
                                       "width", G_TYPE_INT, 1280,
                                       "height", G_TYPE_INT, 720, 
                                       "framerate", GST_TYPE_FRACTION, 30, 1, NULL );
    g_object_set(capsFilter2, "caps", capsConvert, NULL);

    //3-PARSSING
    parse = gst_element_factory_make( "videoparse", "parse" ); 
    g_object_set( G_OBJECT (parse), "format", 2,  NULL);  // i420 (2) – GST_VIDEO_FORMAT_I420 
    g_object_set( G_OBJECT (parse), "width", 1280,  NULL );
    g_object_set( G_OBJECT (parse), "height", 720,  NULL );

    	
    //4-CODEC mp4 file
    // enc = gst_element_factory_make ("omxh264enc", "enc"); 未來似乎有機會用到這物件,研究一下是否跟gpu有關
    enc = gst_element_factory_make( "x264enc", "enc" );

     
    //5-DESTINATION
    sink = gst_element_factory_make( "filesink", "sink" );
    g_object_set( G_OBJECT(sink), "location", "./test.mp4", NULL );


    /* Create the empty pipeline */
    pipeline = gst_pipeline_new ("encode-264-pipeline");

    if( !pipeline || !source || !capsFilter1 || !convert  || !capsFilter2 || !parse || !enc || !sink ) 
    { 
        g_printerr ("Not all elements could be created.\n"); 
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many( GST_BIN (pipeline), source, capsFilter1, convert, capsFilter2, parse, enc, sink, NULL );
    if( gst_element_link_many (source, capsFilter1, convert, capsFilter2, parse, enc, sink, NULL ) != TRUE ) 
    { 
        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (pipeline);
        return -1;
    }


    print_pad_capabilities( source, "src" );


    /* Start playing */
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if( ret == GST_STATE_CHANGE_FAILURE ) 
    {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline);
        return -1;
    }
    	
    /* Wait until error or EOS */
    bus = gst_element_get_bus (pipeline);
    msg = gst_bus_timed_pop_filtered( bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS );

    /* Parse message */
    if (msg != NULL) 
    {
        GError *err;
        gchar *debug_info;

    	switch( GST_MESSAGE_TYPE(msg) ) 
        {
    		case GST_MESSAGE_ERROR:
    		    gst_message_parse_error (msg, &err, &debug_info);
    		    g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
    		    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
    		    g_clear_error (&err);
    		    g_free (debug_info);
    		    break;
    	    case GST_MESSAGE_EOS:
    		    g_print ("End-Of-Stream reached.\n");
    		    break;
    	    default:
    		    /* We should not reach here because we only asked for ERRORs and EOS */
    		    g_printerr ("Unexpected message received.\n");
    		    break;
        }
        gst_message_unref (msg);
    }

    /* Free resources */
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);/* Create the empty pipeline */
    //pipeline = gst_pipeline_new ("test-pipeline");


    return 0;
}

