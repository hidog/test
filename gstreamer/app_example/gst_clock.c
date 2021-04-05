// 此程式沒編譯成功

//#include <gst/gst.h>
#include <gst/check/gsttestclock.h>

/*
https://developer.gnome.org/gstreamer-libs/stable/GstTestClock.html
https://developer.gnome.org/gstreamer/stable/GstClock.html#gst-clock-get-time
*/



int main()
{

    GstClock *clock;
    GstTestClock *test_clock;

    clock = gst_test_clock_new ();
    
    test_clock = GST_TEST_CLOCK (clock);
    GST_INFO ("Time: %" GST_TIME_FORMAT, GST_TIME_ARGS (gst_clock_get_time (clock)));
    gst_test_clock_advance_time ( test_clock, 1 * GST_SECOND);
    GST_INFO ("Time: %" GST_TIME_FORMAT, GST_TIME_ARGS (gst_clock_get_time (clock)));
    g_usleep (10 * G_USEC_PER_SEC);
    GST_INFO ("Time: %" GST_TIME_FORMAT, GST_TIME_ARGS (gst_clock_get_time (clock)));
    gst_test_clock_set_time (test_clock, 42 * GST_SECOND);
    GST_INFO ("Time: %" GST_TIME_FORMAT, GST_TIME_ARGS (gst_clock_get_time (clock)));

    return 0;
}
