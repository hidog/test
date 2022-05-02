#include "wrap.h"

extern "C" {

#include <libavutil/error.h>
#include <libavutil/imgutils.h>

}



int wap_avpriv_set_systematic_pal2(uint32_t pal[256], enum AVPixelFormat pix_fmt)
{
    int i;

    for (i = 0; i < 256; i++) 
    {
        int r, g, b;

        switch (pix_fmt) 
        {
            case AV_PIX_FMT_RGB8:
                r = (i>>5    )*36;
                g = ((i>>2)&7)*36;
                b = (i&3     )*85;
                break;
            case AV_PIX_FMT_BGR8:
                b = (i>>6    )*85;
                g = ((i>>3)&7)*36;
                r = (i&7     )*36;
                break;
            case AV_PIX_FMT_RGB4_BYTE:
                r = (i>>3    )*255;
                g = ((i>>1)&3)*85;
                b = (i&1     )*255;
                break;
            case AV_PIX_FMT_BGR4_BYTE:
                b = (i>>3    )*255;
                g = ((i>>1)&3)*85;
                r = (i&1     )*255;
                break;
            case AV_PIX_FMT_GRAY8:
                r = b = g = i;
                break;
            default:
                return AVERROR(EINVAL);
        }
        pal[i] = b + (g << 8) + (r << 16) + (0xFFU << 24);
    }

    return 0;
}






int wap_image_get_linesize(int width, int plane, int max_step, int max_step_comp, const AVPixFmtDescriptor *desc)
{
    int s, shifted_w, linesize;

    if (!desc)
        return AVERROR(EINVAL);

    if (width < 0)
        return AVERROR(EINVAL);
    
    s = (max_step_comp == 1 || max_step_comp == 2) ? desc->log2_chroma_w : 0;
    shifted_w = ((width + (1 << s) - 1)) >> s;

    if (shifted_w && max_step > INT_MAX / shifted_w)
        return AVERROR(EINVAL);

    linesize = max_step * shifted_w;

    if (desc->flags & AV_PIX_FMT_FLAG_BITSTREAM)
        linesize = (linesize + 7) >> 3;

    return linesize;
}




void wap_av_image_fill_max_pixsteps(int max_pixsteps[4], int max_pixstep_comps[4], const AVPixFmtDescriptor *pixdesc)
{
    int i;
    memset(max_pixsteps, 0, 4*sizeof(max_pixsteps[0]));

    if (max_pixstep_comps)
        memset(max_pixstep_comps, 0, 4*sizeof(max_pixstep_comps[0]));

    for (i = 0; i < 4; i++) 
    {
        const AVComponentDescriptor *comp = &(pixdesc->comp[i]);
        if (comp->step > max_pixsteps[comp->plane]) 
        {
            max_pixsteps[comp->plane] = comp->step;
            if (max_pixstep_comps)
                max_pixstep_comps[comp->plane] = i;
        }
    }
}




int wap_av_image_fill_linesizes(int linesizes[4], enum AVPixelFormat pix_fmt, int width)
{
    int i, ret;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmt);
    int max_step     [4];       /* max pixel step for each plane */
    int max_step_comp[4];       /* the component for each plane which has the max pixel step */

    memset(linesizes, 0, 4*sizeof(linesizes[0]));

    if (!desc || desc->flags & AV_PIX_FMT_FLAG_HWACCEL)
        return AVERROR(EINVAL);

    wap_av_image_fill_max_pixsteps(max_step, max_step_comp, desc);
    for (i = 0; i < 4; i++) 
    {
        if ((ret = wap_image_get_linesize(width, i, max_step[i], max_step_comp[i], desc)) < 0)
            return ret;
        linesizes[i] = ret;
    }

    return 0;
}










int wap_av_image_alloc( uint8_t *pointers[4], int linesizes[4], int w, int h, enum AVPixelFormat pix_fmt, int align )
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmt);
    int i, ret;
    ptrdiff_t linesizes1[4];
    size_t total_size, sizes[4];
    uint8_t *buf;

    if( !desc )
        return AVERROR(EINVAL);

    if((ret = av_image_check_size(w, h, 0, NULL)) < 0)
        return ret;
    if((ret = wap_av_image_fill_linesizes(linesizes, pix_fmt, align>7 ? FFALIGN(w, 8) : w)) < 0)
        return ret;

    for (i = 0; i < 4; i++) 
    {
        linesizes[i] = FFALIGN(linesizes[i], align);
        linesizes1[i] = linesizes[i];
    }

    if ((ret = av_image_fill_plane_sizes(sizes, pix_fmt, h, linesizes1)) < 0)
        return ret;
    
    total_size = align;
    for (i = 0; i < 4; i++) 
    {
        if (total_size > SIZE_MAX - sizes[i])
            return AVERROR(EINVAL);
        total_size += sizes[i];
    }
    buf = (uint8_t*)av_malloc(total_size);
    
    if (!buf)
        return AVERROR(ENOMEM);
    
    if ((ret = av_image_fill_pointers(pointers, pix_fmt, h, buf, linesizes)) < 0) 
    {
        av_free(buf);
        return ret;
    }
    
    if (desc->flags & AV_PIX_FMT_FLAG_PAL) 
    {
        wap_avpriv_set_systematic_pal2((uint32_t*)pointers[1], pix_fmt);
        if (align < 4) 
        {
            av_log(NULL, AV_LOG_ERROR, "Formats with a palette require a minimum alignment of 4\n");
            av_free(buf);
            return AVERROR(EINVAL);
        }
    }

    if ( desc->flags & AV_PIX_FMT_FLAG_PAL && pointers[1] && pointers[1] - pointers[0] > linesizes[0] * h) 
    {
        /* zero-initialize the padding before the palette */
        memset(pointers[0] + linesizes[0] * h, 0, pointers[1] - pointers[0] - linesizes[0] * h);
    }

    return ret;
}
