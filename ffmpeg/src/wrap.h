#ifndef WRAP_H
#define WRAP_H

#include <stdint.h>

extern "C" {
#include <libavutil/pixdesc.h>
}



void wap_av_image_fill_max_pixsteps(int max_pixsteps[4], int max_pixstep_comps[4], const AVPixFmtDescriptor *pixdesc);
int wap_image_get_linesize(int width, int plane, int max_step, int max_step_comp, const AVPixFmtDescriptor *desc);
int wap_av_image_fill_linesizes(int linesizes[4], enum AVPixelFormat pix_fmt, int width);
int wap_avpriv_set_systematic_pal2(uint32_t pal[256], enum AVPixelFormat pix_fmt);
int wap_av_image_alloc( uint8_t *pointers[4], int linesizes[4], int w, int h, enum AVPixelFormat pix_fmt, int align );



#endif