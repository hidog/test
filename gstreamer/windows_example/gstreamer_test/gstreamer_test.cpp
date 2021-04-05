// gstreamer_test.cpp : Defines the entry point for the console application.
//
#if 1

#include "stdafx.h"
#include <gst/gst.h>
#include <cstdlib>



/* Functions below print the Capabilities in a human-friendly format */
gboolean print_field (GQuark field, const GValue * value, gpointer pfx) 
{
	gchar *str = gst_value_serialize (value);

	g_print ("%s  %15s: %s\n", (gchar *) pfx, g_quark_to_string (field), str);
	g_free (str);
	return TRUE;
}





void print_caps (const GstCaps * caps, const gchar * pfx) 
{
	guint i;

	g_return_if_fail (caps != NULL);

	if (gst_caps_is_any (caps)) 
	{
		g_print ("%sANY\n", pfx);
		return;
	}

	if (gst_caps_is_empty (caps)) 
	{
		g_print ("%sEMPTY\n", pfx);
		return;
	}

	for (i = 0; i < gst_caps_get_size (caps); i++) 
	{
		GstStructure *structure = gst_caps_get_structure (caps, i);

		g_print ("%s%s\n", pfx, gst_structure_get_name (structure));
		gst_structure_foreach (structure, print_field, (gpointer) pfx);
	}
}






/* Prints information about a Pad Template, including its Capabilities */
void print_pad_templates_information(GstElementFactory * factory) 
{
	const GList *pads;
	GstStaticPadTemplate *padtemplate;

	g_print("Pad Templates for %s:\n", gst_element_factory_get_longname(factory));
	if (!gst_element_factory_get_num_pad_templates(factory)) 
	{
		g_print("  none\n");
		return;
	}

	pads = gst_element_factory_get_static_pad_templates(factory);
	while (pads) 
	{
		padtemplate = (GstStaticPadTemplate*)pads->data;
		pads = g_list_next(pads);

		if (padtemplate->direction == GST_PAD_SRC)
			g_print("  SRC template: '%s'\n", padtemplate->name_template);
		else if (padtemplate->direction == GST_PAD_SINK)
			g_print("  SINK template: '%s'\n", padtemplate->name_template);
		else
			g_print("  UNKNOWN!!! template: '%s'\n", padtemplate->name_template);

		if (padtemplate->presence == GST_PAD_ALWAYS)
			g_print("    Availability: Always\n");
		else if (padtemplate->presence == GST_PAD_SOMETIMES)
			g_print("    Availability: Sometimes\n");
		else if (padtemplate->presence == GST_PAD_REQUEST)
			g_print("    Availability: On request\n");
		else
			g_print("    Availability: UNKNOWN!!!\n");

		if (padtemplate->static_caps.string) 
		{
			GstCaps *caps;
			g_print("    Capabilities:\n");
			caps = gst_static_caps_get(&padtemplate->static_caps);
			print_caps(caps, "      ");
			gst_caps_unref(caps);

		}

		g_print("\n");
	}
}

/* Shows the CURRENT capabilities of the requested pad in the given element */
void print_pad_capabilities(GstElement *element, gchar *pad_name) 
{
	GstPad *pad = NULL;
	GstCaps *caps = NULL;

	/* Retrieve pad */
	pad = gst_element_get_static_pad(element, pad_name);
	if (!pad) 
	{
		g_printerr("Could not retrieve pad '%s'\n", pad_name);
		return;
	}

	/* Retrieve negotiated caps (or acceptable caps if negotiation is not finished yet) */
	caps = gst_pad_get_current_caps(pad);
	if (!caps)
		caps = gst_pad_query_caps(pad, NULL);

	/* Print and free */
	g_print("Caps for the %s pad:\n", pad_name);
	print_caps(caps, "      ");
	gst_caps_unref(caps);
	gst_object_unref(pad);
}







/* Structure to contain all our information, so we can pass it to callbacks */
typedef struct _CustomData 
{
	GstElement *pipeline;
	GstElement *source;
	GstElement *convert;
	GstElement *sink;
} CustomData;

/* Handler for the pad-added signal */
/* This function will be called by the pad-added signal */
void pad_added_handler(GstElement *src, GstPad *new_pad, CustomData *data) 
{
	GstPad *sink_pad = gst_element_get_static_pad(data->convert, "sink");
	GstPadLinkReturn ret;
	GstCaps *new_pad_caps = NULL;
	GstStructure *new_pad_struct = NULL;
	const gchar *new_pad_type = NULL;

	g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));

	/* If our converter is already linked, we have nothing to do here */
	if (gst_pad_is_linked(sink_pad)) 
	{
		g_print("We are already linked. Ignoring.\n");
		goto exit;
	}

	/* Check the new pad's type */
	new_pad_caps = gst_pad_get_current_caps(new_pad);
	new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
	new_pad_type = gst_structure_get_name(new_pad_struct);
	
	
	if (!g_str_has_prefix(new_pad_type, "audio/x-raw")) 
	{
		g_print("It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type);
		goto exit;
	}

	/* Attempt the link */
	ret = gst_pad_link(new_pad, sink_pad);
	if (GST_PAD_LINK_FAILED(ret)) 
		g_print("Type is '%s' but link failed.\n", new_pad_type);
	else 
		g_print("Link succeeded (type '%s').\n", new_pad_type);

exit:
	/* Unreference the new pad's caps, if we got them */
	if (new_pad_caps != NULL)
		gst_caps_unref(new_pad_caps);

	/* Unreference the sink pad */
	gst_object_unref(sink_pad);
}





/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData2 {
	GstElement *playbin;  /* Our one and only element */
	gboolean playing;      /* Are we in the PLAYING state? */
	gboolean terminate;    /* Should we terminate execution? */
	gboolean seek_enabled; /* Is seeking enabled for this media? */
	gboolean seek_done;    /* Have we performed the seek already? */
	gint64 duration;       /* How long does this media last, in nanoseconds */
} CustomData2;







void handle_message(CustomData2 *data, GstMessage *msg)
{
	GError *err;
	gchar *debug_info;

	switch (GST_MESSAGE_TYPE(msg)) 
	{
		case GST_MESSAGE_ERROR:
			gst_message_parse_error(msg, &err, &debug_info);
			g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
			g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
			g_clear_error(&err);
			g_free(debug_info);
			data->terminate = TRUE;
			break;
		case GST_MESSAGE_EOS:
			g_print("End-Of-Stream reached.\n");
			data->terminate = TRUE;
			break;
		case GST_MESSAGE_DURATION_CHANGED:
			/* The duration has changed, mark the current one as invalid */
			data->duration = GST_CLOCK_TIME_NONE;
			break;
		case GST_MESSAGE_STATE_CHANGED: 
			{
				GstState old_state, new_state, pending_state;
				gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
				if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data->playbin)) 
				{
					g_print("Pipeline state changed from %s to %s:\n",
						gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));

					/* Remember whether we are in the PLAYING state or not */
					data->playing = (new_state == GST_STATE_PLAYING);

					if (data->playing) 
					{
						/* We just moved to PLAYING. Check if seeking is possible */
						GstQuery *query;
						gint64 start, end;
						query = gst_query_new_seeking(GST_FORMAT_TIME);
						if (gst_element_query(data->playbin, query)) 
						{
							gst_query_parse_seeking(query, NULL, &data->seek_enabled, &start, &end);
							if (data->seek_enabled) 
							{
								g_print("Seeking is ENABLED from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n",
									GST_TIME_ARGS(start), GST_TIME_ARGS(end));
							}
							else 
								g_print("Seeking is DISABLED for this stream.\n");
							
						}
						else 
							g_printerr("Seeking query failed.");
						
						gst_query_unref(query);
					}
				}
			} 
			break;
		default:
			/* We should not reach here */
			g_printerr("Unexpected message received.\n");
			break;
	}
	gst_message_unref(msg);
}






void print_one_tag(const GstTagList * list, const gchar * tag, gpointer user_data)
{
	int i, num;

	num = gst_tag_list_get_tag_size(list, tag);
	for (i = 0; i < num; ++i) 
	{
		const GValue *val;

		/* Note: when looking for specific tags, use the gst_tag_list_get_xyz() API,
		* we only use the GValue approach here because it is more generic */
		val = gst_tag_list_get_value_index(list, tag, i);
		if (G_VALUE_HOLDS_STRING(val)) 
			g_print("\t%20s : %s\n", tag, g_value_get_string(val));
		else if (G_VALUE_HOLDS_UINT(val)) 
			g_print("\t%20s : %u\n", tag, g_value_get_uint(val));
		else if (G_VALUE_HOLDS_DOUBLE(val)) 
			g_print("\t%20s : %g\n", tag, g_value_get_double(val));
		else if (G_VALUE_HOLDS_BOOLEAN(val)) 
			g_print("\t%20s : %s\n", tag, (g_value_get_boolean(val)) ? "true" : "false");
		else if (GST_VALUE_HOLDS_BUFFER(val)) 
		{
			GstBuffer *buf = gst_value_get_buffer(val);
			guint buffer_size = gst_buffer_get_size(buf);

			g_print("\t%20s : buffer of size %u\n", tag, buffer_size);
		}
		else if (GST_VALUE_HOLDS_DATE_TIME(val)) 
		{
			GstDateTime *dt = static_cast<GstDateTime*>(g_value_get_boxed(val));
			gchar *dt_str = gst_date_time_to_iso8601_string(dt);

			g_print("\t%20s : %s\n", tag, dt_str);
			g_free(dt_str);
		}
		else 
			g_print("\t%20s : tag of type '%s'\n", tag, G_VALUE_TYPE_NAME(val));
		
	}
}

void on_new_pad(GstElement * dec, GstPad * pad, GstElement * fakesink)
{
	GstPad *sinkpad;

	sinkpad = gst_element_get_static_pad(fakesink, "sink");
	if (!gst_pad_is_linked(sinkpad)) 
	{
		if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK)
			g_error("Failed to link pads!");
	}
	gst_object_unref(sinkpad);
}



typedef struct _CustomData3
{
	GstElement *pipeline;
	GstElement *video_sink;
	GMainLoop *loop;

	gboolean playing;             /* Playing or Paused */
	gdouble rate;                 /* Current playback rate (can be negative) */
} CustomData3;




/* Send seek event to change rate */
void send_seek_event(CustomData3 * data)
{
	gint64 position;
	GstEvent *seek_event;

	/* Obtain the current position, needed for the seek event */
	if (!gst_element_query_position(data->pipeline, GST_FORMAT_TIME, &position))
	{
		g_printerr("Unable to retrieve current position.\n");
		return;
	}

	/* Create the seek event */
	if (data->rate > 0)
	{
		seek_event =
			gst_event_new_seek(data->rate, GST_FORMAT_TIME,
			static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE), GST_SEEK_TYPE_SET,
			position + 300*GST_SECOND/data->rate, GST_SEEK_TYPE_END, 0);
	}
	else
	{
		seek_event =
			gst_event_new_seek(data->rate, GST_FORMAT_TIME,
			static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE), GST_SEEK_TYPE_SET, 0,
			GST_SEEK_TYPE_SET, position);
	}

	if (data->video_sink == NULL)
		/* If we have not done so, obtain the sink through which we will send the seek events */
		g_object_get(data->pipeline, "video-sink", &data->video_sink, NULL);


	/* Send the event */
	gst_element_send_event(data->video_sink, seek_event);

	g_print("Current rate: %g\n", data->rate);
}




/* Process keyboard input */
gboolean handle_keyboard(GIOChannel * source, GIOCondition cond, CustomData3 * data)
{
	gchar *str = NULL;

	if (g_io_channel_read_line(source, &str, NULL, NULL, NULL) != G_IO_STATUS_NORMAL) 
		return TRUE;

	switch (g_ascii_tolower(str[0])) 
	{
		case 'p':
			data->playing = !data->playing;
			gst_element_set_state(data->pipeline,
				data->playing ? GST_STATE_PLAYING : GST_STATE_PAUSED);
			g_print("Setting state to %s\n", data->playing ? "PLAYING" : "PAUSE");
			break;
		case 's':
			if (g_ascii_isupper(str[0])) 
				data->rate *= 2.0;
			else 
				data->rate /= 2.0;
			send_seek_event(data);
			break;
		case 'd':
			data->rate *= -1.0;
			send_seek_event(data);
			break;
		case 'n':
			if (data->video_sink == NULL) 
				/* If we have not done so, obtain the sink through which we will send the step events */
				g_object_get(data->pipeline, "video-sink", &data->video_sink, NULL);

			gst_element_send_event(data->video_sink,
				gst_event_new_step(GST_FORMAT_BUFFERS, 1, ABS(data->rate), TRUE,
				FALSE));
			g_print("Stepping one frame\n");
			break;
		case 'q':
			g_main_loop_quit(data->loop);
			break;
		default:
			break;
	}

	g_free(str);

	return TRUE;
}




int _tmain(int argc, _TCHAR* argv[])
{
#if 0
	GstElement *pipeline;
	GstBus *bus;
	GstMessage *msg;

	/* Initialize GStreamer */
 	int _argc = 0;
	char **_argv;
	//gst_init(&_argc, &_argv);
	gst_init( NULL, NULL );


	/* Build the pipeline */
	pipeline = gst_parse_launch(
		//"playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
		"playbin uri=file:///D:/code/temp/24.mp4",
		NULL);

	/* Start playing */
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* Wait until error or EOS */
	bus = gst_element_get_bus(pipeline);
	msg =
		gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
		static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS) );

	/* Free resources */
	if (msg != NULL)
		gst_message_unref(msg);
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
#endif


#if 0
	GstElement	*pipeline, *source, *filter, *sink;
	GstBus		*bus;
	GstMessage	*msg;
	GstStateChangeReturn ret;
	
	/* Initialize GStreamer */
	gst_init( NULL, NULL );
	
	/* Create the elements */
	source	=	gst_element_factory_make( "videotestsrc",  "source" );
	filter	=	gst_element_factory_make( "timeoverlay",   "filter" );
	sink	=	gst_element_factory_make( "autovideosink", "sink"   );
	
	/* Create the empty pipeline */
	pipeline	=	gst_pipeline_new("test-pipeline");
	
	if (!pipeline || !source || !filter || !sink) 
	{
		g_printerr("Not all elements could be created.\n");
		return -1;
	}
	
	/* Build the pipeline */
	gst_bin_add_many( GST_BIN(pipeline), source, filter, sink, NULL );
	if (gst_element_link_many(source, filter, sink, NULL) != TRUE) 
	{
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	
	/* Modify the source's properties */
	g_object_set(source, "pattern", 10, NULL);
	
	/* Start playing */
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) 
	{
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	
	/* Wait until error or EOS */
	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
	
	/* Parse message */
	if (msg != NULL) 
	{
		GError *err;
		gchar *debug_info;
	
		switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR:
			gst_message_parse_error(msg, &err, &debug_info);
			g_printerr("Error received from element %s: %s\n",
				GST_OBJECT_NAME(msg->src), err->message);
			g_printerr("Debugging information: %s\n",
				debug_info ? debug_info : "none");
			g_clear_error(&err);
			g_free(debug_info);
			break;
		case GST_MESSAGE_EOS:
			g_print("End-Of-Stream reached.\n");
			break;
		default:
			/* We should not reach here because we only asked for ERRORs and EOS */
			g_printerr("Unexpected message received.\n");
			break;
		}
		gst_message_unref(msg);
	}
	
	/* Free resources */
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
#endif

#if 0
	GstElement *pipeline, *source, *sink;
	GstElementFactory *source_factory, *sink_factory;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	gboolean terminate = FALSE;

	/* Initialize GStreamer */
	gst_init( NULL, NULL );

	/* Create the element factories */
	source_factory = gst_element_factory_find("audiotestsrc");
	sink_factory = gst_element_factory_find("autoaudiosink");
	if (!source_factory || !sink_factory) 
	{
		g_printerr("Not all element factories could be created.\n");
		return -1;
	}

	/* Print information about the pad templates of these factories */
	print_pad_templates_information(source_factory);
	print_pad_templates_information(sink_factory);

	/* Ask the factories to instantiate actual elements */
	source = gst_element_factory_create(source_factory, "source");
	sink = gst_element_factory_create(sink_factory, "sink");

	/* Create the empty pipeline */
	pipeline = gst_pipeline_new("test-pipeline");

	if (!pipeline || !source || !sink) 
	{
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	/* Build the pipeline */
	gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
	if (gst_element_link(source, sink) != TRUE) 
	{
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	/* Print initial negotiated caps (in NULL state) */
	g_print("In NULL state:\n");
	print_pad_capabilities(sink, "sink");

	/* Start playing */
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Unable to set the pipeline to the playing state (check the bus for error messages).\n");
	}

	/* Wait until error, EOS or State Change */
	bus = gst_element_get_bus(pipeline);
	do {
		msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, 
			                             static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_STATE_CHANGED));

		/* Parse message */
		if (msg != NULL) {
			GError *err;
			gchar *debug_info;

			switch (GST_MESSAGE_TYPE(msg)) {
			case GST_MESSAGE_ERROR:
				gst_message_parse_error(msg, &err, &debug_info);
				g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
				g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
				g_clear_error(&err);
				g_free(debug_info);
				terminate = TRUE;
				break;
			case GST_MESSAGE_EOS:
				g_print("End-Of-Stream reached.\n");
				terminate = TRUE;
				break;
			case GST_MESSAGE_STATE_CHANGED:
				/* We are only interested in state-changed messages from the pipeline */
				if (GST_MESSAGE_SRC(msg) == GST_OBJECT(pipeline)) {
					GstState old_state, new_state, pending_state;
					gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
					g_print("\nPipeline state changed from %s to %s:\n",
						gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));
					/* Print the current capabilities of the sink element */
					print_pad_capabilities(sink, "sink");
				}
				break;
			default:
				/* We should not reach here because we only asked for ERRORs, EOS and STATE_CHANGED */
				g_printerr("Unexpected message received.\n");
				break;
			}
			gst_message_unref(msg);
		}
	} while (!terminate);

	/* Free resources */
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	gst_object_unref(source_factory);
	gst_object_unref(sink_factory);
#endif

#if 0
	CustomData data;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	gboolean terminate = FALSE;

	/* Initialize GStreamer */
	gst_init( NULL, NULL );

	/* Create the elements */
	data.source  = gst_element_factory_make("uridecodebin",  "source");
	//data.convert = gst_element_factory_make("videoconvert",  "convert");
	data.convert = gst_element_factory_make("audioconvert",  "convert");
	//data.sink    = gst_element_factory_make("autovideosink", "sink");
	data.sink    = gst_element_factory_make("autoaudiosink", "sink");


	/* Create the empty pipeline */
	data.pipeline = gst_pipeline_new("test-pipeline");

	if (!data.pipeline || !data.source || !data.convert || !data.sink) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	/* Build the pipeline. Note that we are NOT linking the source at this
	* point. We will do it later. */
	gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.convert, data.sink, NULL);
	if (!gst_element_link(data.convert, data.sink)) 
	{
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(data.pipeline);
		return -1;
	}

	/* Set the URI to play */
	g_object_set( data.source, "uri", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);
	//g_object_set(data.source, "uri", "file:///D:/code/temp/24.mp4", NULL);

	/* Connect to the pad-added signal */
	g_signal_connect(data.source, "pad-added", G_CALLBACK(pad_added_handler), &data);

	/* Start playing */
	ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) 
	{
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(data.pipeline);
		return -1;
	}

	/* Listen to the bus */
	bus = gst_element_get_bus(data.pipeline);
	do {
		msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
			                             static_cast<GstMessageType>(GST_MESSAGE_STATE_CHANGED| 
										                             GST_MESSAGE_ERROR| 
																	 GST_MESSAGE_EOS) );

		/* Parse message */
		if (msg != NULL) 
		{
			GError *err;
			gchar *debug_info;

			switch (GST_MESSAGE_TYPE(msg)) 
			{
			case GST_MESSAGE_ERROR:
				gst_message_parse_error(msg, &err, &debug_info);
				g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
				g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
				g_clear_error(&err);
				g_free(debug_info);
				terminate = TRUE;
				break;
			case GST_MESSAGE_EOS:
				g_print("End-Of-Stream reached.\n");
				terminate = TRUE;
				break;
			case GST_MESSAGE_STATE_CHANGED:
				/* We are only interested in state-changed messages from the pipeline */
				if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data.pipeline)) 
				{
					GstState old_state, new_state, pending_state;
					gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
					g_print("Pipeline state changed from %s to %s:\n",
						gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));
				}
				break;
			default:
				/* We should not reach here */
				g_printerr("Unexpected message received.\n");
				break;
			}
			gst_message_unref(msg);
		}
	} while (!terminate);

	/* Free resources */
	gst_object_unref(bus);
	gst_element_set_state(data.pipeline, GST_STATE_NULL);
	gst_object_unref(data.pipeline);
#endif

#if 0
	CustomData2 data;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;

	data.playing = FALSE;
	data.terminate = FALSE;
	data.seek_enabled = FALSE;
	data.seek_done = FALSE;
	data.duration = GST_CLOCK_TIME_NONE;

	/* Initialize GStreamer */
	gst_init( NULL, NULL );

	/* Create the elements */
	data.playbin = gst_element_factory_make("playbin", "playbin");

	if (!data.playbin) 
	{
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	/* Set the URI to play */
	//g_object_set(data.playbin, "uri", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);
	g_object_set(data.playbin, "uri", "file:///D:/code/temp/24.mp4", NULL);


	/* Start playing */
	ret = gst_element_set_state(data.playbin, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) 
	{
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(data.playbin);
		return -1;
	}

	/* Listen to the bus */
	bus = gst_element_get_bus(data.playbin);
	do {
		msg = gst_bus_timed_pop_filtered(bus, 100 * GST_MSECOND,
			static_cast<GstMessageType>(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_DURATION_CHANGED) );

		/* Parse message */
		if (msg != NULL) 
			handle_message(&data, msg);
		else 
		{
			/* We got no message, this means the timeout expired */
			if (data.playing) 
			{
				gint64 current = -1;

				/* Query the current position of the stream */
				if (!gst_element_query_position(data.playbin, GST_FORMAT_TIME, &current)) 
					g_printerr("Could not query current position.\n");

				/* If we didn't know it yet, query the stream duration */
				if (!GST_CLOCK_TIME_IS_VALID(data.duration)) 
				{
					if (!gst_element_query_duration(data.playbin, GST_FORMAT_TIME, &data.duration)) 
						g_printerr("Could not query current duration.\n");
				}

				/* Print current position and total duration */
				g_print("Position %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
					GST_TIME_ARGS(current), GST_TIME_ARGS(data.duration));

				/* If seeking is enabled, we have not done it yet, and the time is right, seek */
				if (data.seek_enabled && !data.seek_done && current > 10 * GST_SECOND) 
				{
					g_print("\nReached 10s, performing seek...\n");
					gst_element_seek_simple(data.playbin, GST_FORMAT_TIME,
						static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), 30 * GST_SECOND);
					data.seek_done = TRUE;
				}
			}
		}
	} while (!data.terminate);

	/* Free resources */
	gst_object_unref(bus);
	gst_element_set_state(data.playbin, GST_STATE_NULL);
	gst_object_unref(data.playbin);
#endif

#if 0
	GstElement *pipe, *dec, *sink;
	GstMessage *msg;
	//gchar *uri;

	gst_init( NULL, NULL );

	pipe = gst_pipeline_new("pipeline");

	dec = gst_element_factory_make("uridecodebin", NULL);
	g_object_set(dec, "uri", "file:///D:/code/temp/24.mp4", NULL);
	gst_bin_add(GST_BIN(pipe), dec);

	sink = gst_element_factory_make("fakesink", NULL);
	gst_bin_add(GST_BIN(pipe), sink);

	g_signal_connect(dec, "pad-added", G_CALLBACK(on_new_pad), sink);

	gst_element_set_state(pipe, GST_STATE_PAUSED);

	while (TRUE) 
	{
		GstTagList *tags = NULL;

		msg = gst_bus_timed_pop_filtered(GST_ELEMENT_BUS(pipe),
			GST_CLOCK_TIME_NONE,
			static_cast<GstMessageType>(GST_MESSAGE_ASYNC_DONE | GST_MESSAGE_TAG | GST_MESSAGE_ERROR) );

		if (GST_MESSAGE_TYPE(msg) != GST_MESSAGE_TAG) /* error or async_done */
			break;

		gst_message_parse_tag(msg, &tags);

		g_print("Got tags from element %s:\n", GST_OBJECT_NAME(msg->src));
		gst_tag_list_foreach(tags, print_one_tag, NULL);
		g_print("\n");
		gst_tag_list_unref(tags);

		gst_message_unref(msg);
	}

	if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) 
	{
		GError *err = NULL;

		gst_message_parse_error(msg, &err, NULL);
		g_printerr("Got error: %s\n", err->message);
		g_error_free(err);
	}

	gst_message_unref(msg);
	gst_element_set_state(pipe, GST_STATE_NULL);
	gst_object_unref(pipe);
	//g_free(uri);
#endif


	CustomData3 data;
	GstStateChangeReturn ret;
	GIOChannel *io_stdin;

	/* Initialize GStreamer */
	gst_init( NULL, NULL );

	/* Initialize our data structure */
	memset(&data, 0, sizeof(data));

	/* Print usage map */
	g_print("USAGE: Choose one of the following options, then press enter:\n"
		" 'P' to toggle between PAUSE and PLAY\n"
		" 'S' to increase playback speed, 's' to decrease playback speed\n"
		" 'D' to toggle playback direction\n"
		" 'N' to move to next frame (in the current direction, better in PAUSE)\n"
		" 'Q' to quit\n");

	/* Build the pipeline */
	data.pipeline =
		gst_parse_launch
		("playbin uri=file:///D:/code/test.mp4",
		NULL);

	/* Add a keyboard watch so we get notified of keystrokes */
#ifdef G_OS_WIN32
	io_stdin = g_io_channel_win32_new_fd(fileno(stdin));
#else
	io_stdin = g_io_channel_unix_new(fileno(stdin));
#endif
	g_io_add_watch(io_stdin, G_IO_IN, (GIOFunc)handle_keyboard, &data);

	/* Start playing */
	ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) 
	{
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(data.pipeline);
		return -1;
	}
	data.playing = TRUE;
	data.rate = 1.0;

	/* Create a GLib Main Loop and set it to run */
	data.loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(data.loop);

	/* Free resources */
	g_main_loop_unref(data.loop);
	g_io_channel_unref(io_stdin);
	gst_element_set_state(data.pipeline, GST_STATE_NULL);
	if (data.video_sink != NULL)
		gst_object_unref(data.video_sink);
	gst_object_unref(data.pipeline);

	system("PAUSE");
	return 0;
}

#endif


#if 0
#include <gst/gst.h>
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	GstElement *pipeline, *audio_source, *tee, *audio_queue, *audio_convert, *audio_resample, *audio_sink;
	GstElement *video_queue, *visual, *video_convert, *video_sink;
	GstBus *bus;
	GstMessage *msg;
	GstPad *tee_audio_pad, *tee_video_pad;
	GstPad *queue_audio_pad, *queue_video_pad;

	/* Initialize GStreamer */
	gst_init( NULL, NULL );

	/* Create the elements */
	audio_source = gst_element_factory_make("audiotestsrc", "audio_source");
	tee = gst_element_factory_make("tee", "tee");
	audio_queue = gst_element_factory_make("queue", "audio_queue");
	audio_convert = gst_element_factory_make("audioconvert", "audio_convert");
	audio_resample = gst_element_factory_make("audioresample", "audio_resample");
	audio_sink = gst_element_factory_make("autoaudiosink", "audio_sink");
	video_queue = gst_element_factory_make("queue", "video_queue");
	visual = gst_element_factory_make("wavescope", "visual");
	video_convert = gst_element_factory_make("videoconvert", "csp");
	video_sink = gst_element_factory_make("autovideosink", "video_sink");

	/* Create the empty pipeline */
	pipeline = gst_pipeline_new("test-pipeline");

	if (!pipeline || !audio_source || !tee || !audio_queue || !audio_convert || !audio_resample || !audio_sink ||
		!video_queue || !visual || !video_convert || !video_sink) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	/* Configure elements */
	g_object_set(audio_source, "freq", 215.0f, NULL);
	g_object_set(visual, "shader", 0, "style", 1, NULL);

	/* Link all elements that can be automatically linked because they have "Always" pads */
	gst_bin_add_many(GST_BIN(pipeline), audio_source, tee, audio_queue, audio_convert, audio_resample, audio_sink,
		video_queue, visual, video_convert, video_sink, NULL);
	if (gst_element_link_many(audio_source, tee, NULL) != TRUE ||
		gst_element_link_many(audio_queue, audio_convert, audio_resample, audio_sink, NULL) != TRUE ||
		gst_element_link_many(video_queue, visual, video_convert, video_sink, NULL) != TRUE) {
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	/* Manually link the Tee, which has "Request" pads */
	tee_audio_pad = gst_element_get_request_pad(tee, "src_%u");
	g_print("Obtained request pad %s for audio branch.\n", gst_pad_get_name(tee_audio_pad));
	queue_audio_pad = gst_element_get_static_pad(audio_queue, "sink");
	tee_video_pad = gst_element_get_request_pad(tee, "src_%u");
	g_print("Obtained request pad %s for video branch.\n", gst_pad_get_name(tee_video_pad));
	queue_video_pad = gst_element_get_static_pad(video_queue, "sink");
	if (gst_pad_link(tee_audio_pad, queue_audio_pad) != GST_PAD_LINK_OK ||
		gst_pad_link(tee_video_pad, queue_video_pad) != GST_PAD_LINK_OK) {
		g_printerr("Tee could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}
	gst_object_unref(queue_audio_pad);
	gst_object_unref(queue_video_pad);

	/* Start playing the pipeline */
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* Wait until error or EOS */
	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, 
		          static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS) );

	/* Release the request pads from the Tee, and unref them */
	gst_element_release_request_pad(tee, tee_audio_pad);
	gst_element_release_request_pad(tee, tee_video_pad);
	gst_object_unref(tee_audio_pad);
	gst_object_unref(tee_video_pad);

	/* Free resources */
	if (msg != NULL)
		gst_message_unref(msg);
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);

	gst_object_unref(pipeline);
	return 0;
}
#endif




#if 0
#include <gst/gst.h>
#include "stdafx.h"
#include <gst/audio/audio.h>
#include <string.h>

#define CHUNK_SIZE 1024   /* Amount of bytes we are sending in each buffer */
#define SAMPLE_RATE 44100 /* Samples per second we are sending */

/* Structure to contain all our information, so we can pass it to callbacks */
typedef struct _CustomData {
	GstElement *pipeline, *app_source, *tee, *audio_queue, *audio_convert1, *audio_resample, *audio_sink;
	GstElement *video_queue, *audio_convert2, *visual, *video_convert, *video_sink;
	GstElement *app_queue, *app_sink;

	guint64 num_samples;   /* Number of samples generated so far (for timestamp generation) */
	gfloat a, b, c, d;     /* For waveform generation */

	guint sourceid;        /* To control the GSource */

	GMainLoop *main_loop;  /* GLib's Main Loop */
} CustomData;

/* This method is called by the idle GSource in the mainloop, to feed CHUNK_SIZE bytes into appsrc.
* The idle handler is added to the mainloop when appsrc requests us to start sending data (need-data signal)
* and is removed when appsrc has enough data (enough-data signal).
*/
static gboolean push_data(CustomData *data) {
	GstBuffer *buffer;
	GstFlowReturn ret;
	int i;
	GstMapInfo map;
	gint16 *raw;
	gint num_samples = CHUNK_SIZE / 2; /* Because each sample is 16 bits */
	gfloat freq;

	/* Create a new empty buffer */
	buffer = gst_buffer_new_and_alloc(CHUNK_SIZE);

	/* Set its timestamp and duration */
	GST_BUFFER_TIMESTAMP(buffer) = gst_util_uint64_scale(data->num_samples, GST_SECOND, SAMPLE_RATE);
	GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale(num_samples, GST_SECOND, SAMPLE_RATE);

	/* Generate some psychodelic waveforms */
	gst_buffer_map(buffer, &map, GST_MAP_WRITE);
	raw = (gint16 *)map.data;
	data->c += data->d;
	data->d -= data->c / 1000;
	freq = 1100 + 1000 * data->d;
	for (i = 0; i < num_samples; i++) {
		data->a += data->b;
		data->b -= data->a / freq;
		raw[i] = (gint16)(500 * data->a);
	}
	gst_buffer_unmap(buffer, &map);
	data->num_samples += num_samples;

	/* Push the buffer into the appsrc */
	g_signal_emit_by_name(data->app_source, "push-buffer", buffer, &ret);

	/* Free the buffer now that we are done with it */
	gst_buffer_unref(buffer);

	if (ret != GST_FLOW_OK) {
		/* We got some error, stop sending data */
		return FALSE;
	}

	return TRUE;
}

/* This signal callback triggers when appsrc needs data. Here, we add an idle handler
* to the mainloop to start pushing data into the appsrc */
static void start_feed(GstElement *source, guint size, CustomData *data) {
	if (data->sourceid == 0) {
		g_print("Start feeding\n");
		data->sourceid = g_idle_add((GSourceFunc)push_data, data);
	}
}

/* This callback triggers when appsrc has enough data and we can stop sending.
* We remove the idle handler from the mainloop */
static void stop_feed(GstElement *source, CustomData *data) {
	if (data->sourceid != 0) {
		g_print("Stop feeding\n");
		g_source_remove(data->sourceid);
		data->sourceid = 0;
	}
}

/* The appsink has received a buffer */
static GstFlowReturn new_sample(GstElement *sink, CustomData *data) {
	GstSample *sample;

	/* Retrieve the buffer */
	g_signal_emit_by_name(sink, "pull-sample", &sample);
	if (sample) {
		/* The only thing we do in this example is print a * to indicate a received buffer */
		g_print("*");
		gst_sample_unref(sample);
		return GST_FLOW_OK;
	}

	return GST_FLOW_ERROR;
}

/* This function is called when an error message is posted on the bus */
static void error_cb(GstBus *bus, GstMessage *msg, CustomData *data) {
	GError *err;
	gchar *debug_info;

	/* Print error details on the screen */
	gst_message_parse_error(msg, &err, &debug_info);
	g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
	g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
	g_clear_error(&err);
	g_free(debug_info);

	g_main_loop_quit(data->main_loop);
}


int _tmain(int argc, _TCHAR* argv[])
{
	CustomData data;
	GstPad *tee_audio_pad, *tee_video_pad, *tee_app_pad;
	GstPad *queue_audio_pad, *queue_video_pad, *queue_app_pad;
	GstAudioInfo info;
	GstCaps *audio_caps;
	GstBus *bus;

	/* Initialize cumstom data structure */
	memset(&data, 0, sizeof(data));
	data.b = 1; /* For waveform generation */
	data.d = 1;

	/* Initialize GStreamer */
	gst_init( NULL, NULL );

	/* Create the elements */
	data.app_source = gst_element_factory_make("appsrc", "audio_source");
	data.tee = gst_element_factory_make("tee", "tee");
	data.audio_queue = gst_element_factory_make("queue", "audio_queue");
	data.audio_convert1 = gst_element_factory_make("audioconvert", "audio_convert1");
	data.audio_resample = gst_element_factory_make("audioresample", "audio_resample");
	data.audio_sink = gst_element_factory_make("autoaudiosink", "audio_sink");
	data.video_queue = gst_element_factory_make("queue", "video_queue");
	data.audio_convert2 = gst_element_factory_make("audioconvert", "audio_convert2");
	data.visual = gst_element_factory_make("wavescope", "visual");
	data.video_convert = gst_element_factory_make("videoconvert", "video_convert");
	data.video_sink = gst_element_factory_make("autovideosink", "video_sink");
	data.app_queue = gst_element_factory_make("queue", "app_queue");
	data.app_sink = gst_element_factory_make("appsink", "app_sink");

	/* Create the empty pipeline */
	data.pipeline = gst_pipeline_new("test-pipeline");

	if (!data.pipeline || !data.app_source || !data.tee || !data.audio_queue || !data.audio_convert1 ||
		!data.audio_resample || !data.audio_sink || !data.video_queue || !data.audio_convert2 || !data.visual ||
		!data.video_convert || !data.video_sink || !data.app_queue || !data.app_sink) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	/* Configure wavescope */
	g_object_set(data.visual, "shader", 0, "style", 0, NULL);

	/* Configure appsrc */
	gst_audio_info_set_format(&info, GST_AUDIO_FORMAT_S16, SAMPLE_RATE, 1, NULL);
	audio_caps = gst_audio_info_to_caps(&info);
	g_object_set(data.app_source, "caps", audio_caps, "format", GST_FORMAT_TIME, NULL);
	g_signal_connect(data.app_source, "need-data", G_CALLBACK(start_feed), &data);
	g_signal_connect(data.app_source, "enough-data", G_CALLBACK(stop_feed), &data);

	/* Configure appsink */
	g_object_set(data.app_sink, "emit-signals", TRUE, "caps", audio_caps, NULL);
	g_signal_connect(data.app_sink, "new-sample", G_CALLBACK(new_sample), &data);
	gst_caps_unref(audio_caps);

	/* Link all elements that can be automatically linked because they have "Always" pads */
	gst_bin_add_many(GST_BIN(data.pipeline), data.app_source, data.tee, data.audio_queue, data.audio_convert1, data.audio_resample,
		data.audio_sink, data.video_queue, data.audio_convert2, data.visual, data.video_convert, data.video_sink, data.app_queue,
		data.app_sink, NULL);
	if (gst_element_link_many(data.app_source, data.tee, NULL) != TRUE ||
		gst_element_link_many(data.audio_queue, data.audio_convert1, data.audio_resample, data.audio_sink, NULL) != TRUE ||
		gst_element_link_many(data.video_queue, data.audio_convert2, data.visual, data.video_convert, data.video_sink, NULL) != TRUE ||
		gst_element_link_many(data.app_queue, data.app_sink, NULL) != TRUE) {
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(data.pipeline);
		return -1;
	}

	/* Manually link the Tee, which has "Request" pads */
	tee_audio_pad = gst_element_get_request_pad(data.tee, "src_%u");
	g_print("Obtained request pad %s for audio branch.\n", gst_pad_get_name(tee_audio_pad));
	queue_audio_pad = gst_element_get_static_pad(data.audio_queue, "sink");
	tee_video_pad = gst_element_get_request_pad(data.tee, "src_%u");
	g_print("Obtained request pad %s for video branch.\n", gst_pad_get_name(tee_video_pad));
	queue_video_pad = gst_element_get_static_pad(data.video_queue, "sink");
	tee_app_pad = gst_element_get_request_pad(data.tee, "src_%u");
	g_print("Obtained request pad %s for app branch.\n", gst_pad_get_name(tee_app_pad));
	queue_app_pad = gst_element_get_static_pad(data.app_queue, "sink");
	if (gst_pad_link(tee_audio_pad, queue_audio_pad) != GST_PAD_LINK_OK ||
		gst_pad_link(tee_video_pad, queue_video_pad) != GST_PAD_LINK_OK ||
		gst_pad_link(tee_app_pad, queue_app_pad) != GST_PAD_LINK_OK) {
		g_printerr("Tee could not be linked\n");
		gst_object_unref(data.pipeline);
		return -1;
	}
	gst_object_unref(queue_audio_pad);
	gst_object_unref(queue_video_pad);
	gst_object_unref(queue_app_pad);

	/* Instruct the bus to emit signals for each received message, and connect to the interesting signals */
	bus = gst_element_get_bus(data.pipeline);
	gst_bus_add_signal_watch(bus);
	g_signal_connect(G_OBJECT(bus), "message::error", (GCallback)error_cb, &data);
	gst_object_unref(bus);

	/* Start playing the pipeline */
	gst_element_set_state(data.pipeline, GST_STATE_PLAYING);

	/* Create a GLib Main Loop and set it to run */
	data.main_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(data.main_loop);

	/* Release the request pads from the Tee, and unref them */
	gst_element_release_request_pad(data.tee, tee_audio_pad);
	gst_element_release_request_pad(data.tee, tee_video_pad);
	gst_element_release_request_pad(data.tee, tee_app_pad);
	gst_object_unref(tee_audio_pad);
	gst_object_unref(tee_video_pad);
	gst_object_unref(tee_app_pad);

	/* Free resources */
	gst_element_set_state(data.pipeline, GST_STATE_NULL);
	gst_object_unref(data.pipeline);
	return 0;
}
#endif



#if 0

#include <string.h>
#include "stdafx.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

/*
* an example application of using appsrc in push mode to create a file.
* from buffers we push into the pipeline.
*/

/* S16LE 10ms frame audio */
#define BUFFER_SIZE 160

/* 300 frames = 5 minutes */
#define TOTAL_FRAMES 30000

#define QUEUED_FRAMES 30

typedef struct
{
	GMainLoop *loop;
	GstElement *pipeline;
	GstElement *source;
	guint source_id;
	guint num_frame;
} AppData;

/* This method is called by the idle GSource in the mainloop. We feed 1 buffer
* of BUFFER_SIZE bytes into appsrc.
* The ide handler is added to the mainloop when appsrc requests us to start
* sending data (need-data signal) and is removed when appsrc has enough data
* (enough-data signal).
*/
static gboolean push_buffer(AppData * app)
{
	gpointer raw_buffer;
	GstBuffer *app_buffer;
	GstMemory *mem;
	GstFlowReturn ret;

	app->num_frame++;

	if (app->num_frame >= TOTAL_FRAMES) {
		/* we are EOS, send end-of-stream and remove the source */
		g_signal_emit_by_name(app->source, "end-of-stream", &ret);
		return FALSE;
	}

	app_buffer = gst_buffer_new();

	mem = gst_allocator_alloc(NULL, BUFFER_SIZE, NULL);

	gst_buffer_append_memory(app_buffer, mem);

	gst_buffer_set_size(app_buffer, BUFFER_SIZE);

	/* Setting the correct timestamp for the buffer is very important, otherwise the
	* resulting file won't be created correctly */
	GST_BUFFER_TIMESTAMP(app_buffer) = (GstClockTime)((app->num_frame / 100.0) * 1e9);

	/* push new buffer */
	g_signal_emit_by_name(app->source, "push-buffer", app_buffer, &ret);

	gst_buffer_unref(app_buffer);

	if (ret != GST_FLOW_OK) {
		/* some error, stop sending data */
		return FALSE;
	}

	return TRUE;
}

/* This signal callback is called when appsrc needs data, we add an idle handler
* to the mainloop to start pushing data into the appsrc */
static void start_feed(GstElement * pipeline, guint size, AppData * app)
{
	if (app->source_id == 0) {
		g_print("start feeding at frame %i\n", app->num_frame);
		app->source_id = g_idle_add((GSourceFunc)push_buffer, app);
	}
}

/* This callback is called when appsrc has enough data and we can stop sending.
* We remove the idle handler from the mainloop */
static void stop_feed(GstElement * pipeline, AppData * app)
{
	if (app->source_id != 0) {
		g_print("stop feeding at frame %i\n", app->num_frame);
		g_source_remove(app->source_id);
		app->source_id = 0;
	}
}

/* called when we get a GstMessage from the pipeline when we get EOS, we
* exit the mainloop and this testapp. */
static gboolean on_pipeline_message(GstBus * bus, GstMessage * message, AppData * app)
{
	GstState state, pending;

	switch (GST_MESSAGE_TYPE(message)) {
	case GST_MESSAGE_EOS:
		g_print("Received End of Stream message\n");
		g_main_loop_quit(app->loop);
		break;
	case GST_MESSAGE_ERROR:
	{
		g_print("Received error\n");

		GError *err = NULL;
		gchar *dbg_info = NULL;

		gst_message_parse_error(message, &err, &dbg_info);
		g_printerr("ERROR from element %s: %s\n",
			GST_OBJECT_NAME(message->src), err->message);
		g_printerr("Debugging info: %s\n", (dbg_info) ? dbg_info : "none");
		g_error_free(err);
		g_free(dbg_info);
	}

	g_main_loop_quit(app->loop);
	break;
	case GST_MESSAGE_STATE_CHANGED:
		gst_element_get_state(app->source, &state, &pending, GST_CLOCK_TIME_NONE);
		/* g_print ("State changed from %i to %i\n", state, pending); */
		break;
	default:
		break;
	}
	return TRUE;
}




int _tmain(int argc, _TCHAR* argv[])
{
	AppData *app = NULL;
	GstBus *bus = NULL;
	GstElement *appsrc = NULL;

	gst_init( NULL, NULL );

	app = g_new0(AppData, 1);

	app->loop = g_main_loop_new(NULL, FALSE);

	/* setting up pipeline, we push media data into this pipeline that will
	* then be recorded to a file, encoded with a codec*/
	//app->pipeline = gst_parse_launch("appsrc is-live=true name=source caps=audio/x-raw,format=S16LE,rate=8000,channels=1,layout=interleaved ! flacenc ! oggmux ! filesink location=flac-audio.ogg", NULL);
	app->pipeline = gst_parse_launch(
		"playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
		//"playbin uri=file:///D:/code/temp/24.mp4",
		NULL);


	if (app->pipeline == NULL) {
		g_print("Bad pipeline\n");
		return -1;
	}

	appsrc = gst_bin_get_by_name(GST_BIN(app->pipeline), "source");

	/* setting maximum of bytes queued */
	gst_app_src_set_max_bytes((GstAppSrc *)appsrc, QUEUED_FRAMES * BUFFER_SIZE);

	/* uncomment the next line to block when appsrc has buffered enough */
	/* g_object_set (appsrc, "block", TRUE, NULL); */
	app->source = appsrc;

	/* add watch for messages */
	bus = gst_element_get_bus(app->pipeline);
	gst_bus_add_watch(bus, (GstBusFunc)on_pipeline_message, app);
	gst_object_unref(bus);

	/* configure the appsrc, we will push data into the appsrc from the
	* mainloop */
	g_signal_connect(app->source, "need-data", G_CALLBACK(start_feed), app);
	g_signal_connect(app->source, "enough-data", G_CALLBACK(stop_feed), app);

	/* go to playing and wait in a mainloop */
	gst_element_set_state(app->pipeline, GST_STATE_PLAYING);

	/* this mainloop is stopped when we receive an error or EOS */
	g_print("Creating movie...\n");
	g_main_loop_run(app->loop);
	g_print("Done.\n");

	gst_app_src_end_of_stream(GST_APP_SRC(app->source));

	gst_element_set_state(app->pipeline, GST_STATE_NULL);

	/* Cleaning up */
	gst_object_unref(app->source);
	gst_object_unref(app->pipeline);
	g_main_loop_unref(app->loop);
	g_free(app);

	return 0;
}
#endif