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



void wap_image_copy_plane( uint8_t *dst, ptrdiff_t dst_linesize, const uint8_t *src, ptrdiff_t src_linesize, ptrdiff_t bytewidth, int height);
void wap_image_copy(uint8_t *dst_data[4], const ptrdiff_t dst_linesizes[4], const uint8_t *src_data[4], const ptrdiff_t src_linesizes[4], 
                    enum AVPixelFormat pix_fmt, int width, int height,
                    void (*copy_plane)(uint8_t *, ptrdiff_t, const uint8_t *, ptrdiff_t, ptrdiff_t, int));
void wap_av_image_copy(uint8_t *dst_data[4], int dst_linesizes[4], const uint8_t *src_data[4], const int src_linesizes[4], enum AVPixelFormat pix_fmt, int width, int height);




#endif