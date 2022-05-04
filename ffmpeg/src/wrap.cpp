#include "wrap.h"
#include <cassert>

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







void wap_image_copy(uint8_t *dst_data[4], const ptrdiff_t dst_linesizes[4], const uint8_t *src_data[4], const ptrdiff_t src_linesizes[4], 
                enum AVPixelFormat pix_fmt, int width, int height,
                void (*copy_plane)(uint8_t *, ptrdiff_t, const uint8_t *, ptrdiff_t, ptrdiff_t, int))
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmt);

    if (!desc || desc->flags & AV_PIX_FMT_FLAG_HWACCEL)
        return;

    if (desc->flags & AV_PIX_FMT_FLAG_PAL) 
    {
        copy_plane(dst_data[0], dst_linesizes[0],
                   src_data[0], src_linesizes[0],
                   width, height);
        /* copy the palette */
        if ((desc->flags & AV_PIX_FMT_FLAG_PAL) || (dst_data[1] && src_data[1]))
            memcpy(dst_data[1], src_data[1], 4*256);
    } 
    else 
    {
        int i, planes_nb = 0;

        for (i = 0; i < desc->nb_components; i++)
            planes_nb = FFMAX(planes_nb, desc->comp[i].plane + 1);

        for (i = 0; i < planes_nb; i++) 
        {
            int h = height;
            ptrdiff_t bwidth = av_image_get_linesize(pix_fmt, width, i);
            if (bwidth < 0) 
            {
                av_log(NULL, AV_LOG_ERROR, "av_image_get_linesize failed\n");
                return;
            }
            if (i == 1 || i == 2) 
            {
                h = AV_CEIL_RSHIFT(height, desc->log2_chroma_h);
            }
            copy_plane(dst_data[i], dst_linesizes[i],
                       src_data[i], src_linesizes[i],
                       bwidth, h);
        }
    }
}





void wap_image_copy_plane( uint8_t *dst, ptrdiff_t dst_linesize, const uint8_t *src, ptrdiff_t src_linesize, ptrdiff_t bytewidth, int height)
{
    if (!dst || !src)
        return;
    
    assert(FFABS(src_linesize) >= bytewidth);
    assert(FFABS(dst_linesize) >= bytewidth);
    
    for (;height > 0; height--) 
    {
        memcpy(dst, src, bytewidth);
        dst += dst_linesize;
        src += src_linesize;
    }
}







void wap_av_image_copy(uint8_t *dst_data[4], int dst_linesizes[4], const uint8_t *src_data[4], const int src_linesizes[4], 
                       enum AVPixelFormat pix_fmt, int width, int height)
{
    ptrdiff_t dst_linesizes1[4], src_linesizes1[4];
    int i;

    for (i = 0; i < 4; i++) 
    {
        dst_linesizes1[i] = dst_linesizes[i];
        src_linesizes1[i] = src_linesizes[i];
    }

    wap_image_copy( dst_data, dst_linesizes1, src_data, src_linesizes1, pix_fmt,
                    width, height, wap_image_copy_plane);
}











int wap_sws_scale( SwsContext *c, const uint8_t * const srcSlice[], const int srcStride[], int srcSliceY, int srcSliceH, uint8_t *const dst[], const int dstStride[] )
{
    if( c->nb_slice_ctx )
        c = c->slice_ctx[0];

    //return scale_internal(c, srcSlice, srcStride, srcSliceY, srcSliceH,
      //                    dst, dstStride, 0, c->dstH);
    return 0;
}




static int scale_internal(SwsContext *c,
                          const uint8_t * const srcSlice[], const int srcStride[],
                          int srcSliceY, int srcSliceH,
                          uint8_t *const dstSlice[], const int dstStride[],
                          int dstSliceY, int dstSliceH)
{
    const int scale_dst = dstSliceY > 0 || dstSliceH < c->dstH;
    const int frame_start = scale_dst || !c->sliceDir;
    int i, ret;
    const uint8_t *src2[4];
    uint8_t *dst2[4];
    int macro_height_src = isBayer(c->srcFormat) ? 2 : (1 << c->chrSrcVSubSample);
    int macro_height_dst = isBayer(c->dstFormat) ? 2 : (1 << c->chrDstVSubSample);
    // copy strides, so they can safely be modified
    int srcStride2[4];
    int dstStride2[4];
    int srcSliceY_internal = srcSliceY;

    if (!srcStride || !dstStride || !dstSlice || !srcSlice) {
        av_log(c, AV_LOG_ERROR, "One of the input parameters to sws_scale() is NULL, please check the calling code\n");
        return AVERROR(EINVAL);
    }

    if ((srcSliceY  & (macro_height_src - 1)) ||
        ((srcSliceH & (macro_height_src - 1)) && srcSliceY + srcSliceH != c->srcH) ||
        srcSliceY + srcSliceH > c->srcH) {
        av_log(c, AV_LOG_ERROR, "Slice parameters %d, %d are invalid\n", srcSliceY, srcSliceH);
        return AVERROR(EINVAL);
    }

    if ((dstSliceY  & (macro_height_dst - 1)) ||
        ((dstSliceH & (macro_height_dst - 1)) && dstSliceY + dstSliceH != c->dstH) ||
        dstSliceY + dstSliceH > c->dstH) {
        av_log(c, AV_LOG_ERROR, "Slice parameters %d, %d are invalid\n", dstSliceY, dstSliceH);
        return AVERROR(EINVAL);
    }

    if (!check_image_pointers(srcSlice, c->srcFormat, srcStride)) {
        av_log(c, AV_LOG_ERROR, "bad src image pointers\n");
        return AVERROR(EINVAL);
    }
    if (!check_image_pointers((const uint8_t* const*)dstSlice, c->dstFormat, dstStride)) {
        av_log(c, AV_LOG_ERROR, "bad dst image pointers\n");
        return AVERROR(EINVAL);
    }

    // do not mess up sliceDir if we have a "trailing" 0-size slice
    if (srcSliceH == 0)
        return 0;

    if (c->gamma_flag && c->cascaded_context[0])
        return scale_gamma(c, srcSlice, srcStride, srcSliceY, srcSliceH,
                           dstSlice, dstStride, dstSliceY, dstSliceH);

    if (c->cascaded_context[0] && srcSliceY == 0 && srcSliceH == c->cascaded_context[0]->srcH)
        return scale_cascaded(c, srcSlice, srcStride, srcSliceY, srcSliceH,
                              dstSlice, dstStride, dstSliceY, dstSliceH);

    if (!srcSliceY && (c->flags & SWS_BITEXACT) && c->dither == SWS_DITHER_ED && c->dither_error[0])
        for (i = 0; i < 4; i++)
            memset(c->dither_error[i], 0, sizeof(c->dither_error[0][0]) * (c->dstW+2));

    if (usePal(c->srcFormat))
        update_palette(c, (const uint32_t *)srcSlice[1]);

    memcpy(src2,       srcSlice,  sizeof(src2));
    memcpy(dst2,       dstSlice,  sizeof(dst2));
    memcpy(srcStride2, srcStride, sizeof(srcStride2));
    memcpy(dstStride2, dstStride, sizeof(dstStride2));

    if (frame_start && !scale_dst) {
        if (srcSliceY != 0 && srcSliceY + srcSliceH != c->srcH) {
            av_log(c, AV_LOG_ERROR, "Slices start in the middle!\n");
            return AVERROR(EINVAL);
        }

        c->sliceDir = (srcSliceY == 0) ? 1 : -1;
    } else if (scale_dst)
        c->sliceDir = 1;

    if (c->src0Alpha && !c->dst0Alpha && isALPHA(c->dstFormat)) {
        uint8_t *base;
        int x,y;

        av_fast_malloc(&c->rgb0_scratch, &c->rgb0_scratch_allocated,
                       FFABS(srcStride[0]) * srcSliceH + 32);
        if (!c->rgb0_scratch)
            return AVERROR(ENOMEM);

        base = srcStride[0] < 0 ? c->rgb0_scratch - srcStride[0] * (srcSliceH-1) :
                                  c->rgb0_scratch;
        for (y=0; y<srcSliceH; y++){
            memcpy(base + srcStride[0]*y, src2[0] + srcStride[0]*y, 4*c->srcW);
            for (x=c->src0Alpha-1; x<4*c->srcW; x+=4) {
                base[ srcStride[0]*y + x] = 0xFF;
            }
        }
        src2[0] = base;
    }

    if (c->srcXYZ && !(c->dstXYZ && c->srcW==c->dstW && c->srcH==c->dstH)) {
        uint8_t *base;

        av_fast_malloc(&c->xyz_scratch, &c->xyz_scratch_allocated,
                       FFABS(srcStride[0]) * srcSliceH + 32);
        if (!c->xyz_scratch)
            return AVERROR(ENOMEM);

        base = srcStride[0] < 0 ? c->xyz_scratch - srcStride[0] * (srcSliceH-1) :
                                  c->xyz_scratch;

        xyz12Torgb48(c, (uint16_t*)base, (const uint16_t*)src2[0], srcStride[0]/2, srcSliceH);
        src2[0] = base;
    }

    if (c->sliceDir != 1) {
        // slices go from bottom to top => we flip the image internally
        for (i=0; i<4; i++) {
            srcStride2[i] *= -1;
            dstStride2[i] *= -1;
        }

        src2[0] += (srcSliceH - 1) * srcStride[0];
        if (!usePal(c->srcFormat))
            src2[1] += ((srcSliceH >> c->chrSrcVSubSample) - 1) * srcStride[1];
        src2[2] += ((srcSliceH >> c->chrSrcVSubSample) - 1) * srcStride[2];
        src2[3] += (srcSliceH - 1) * srcStride[3];
        dst2[0] += ( c->dstH                         - 1) * dstStride[0];
        dst2[1] += ((c->dstH >> c->chrDstVSubSample) - 1) * dstStride[1];
        dst2[2] += ((c->dstH >> c->chrDstVSubSample) - 1) * dstStride[2];
        dst2[3] += ( c->dstH                         - 1) * dstStride[3];

        srcSliceY_internal = c->srcH-srcSliceY-srcSliceH;
    }
    reset_ptr(src2, c->srcFormat);
    reset_ptr((void*)dst2, c->dstFormat);

    if (c->convert_unscaled) {
        int offset  = srcSliceY_internal;
        int slice_h = srcSliceH;

        // for dst slice scaling, offset the pointers to match the unscaled API
        if (scale_dst) {
            av_assert0(offset == 0);
            for (i = 0; i < 4 && src2[i]; i++) {
                if (!src2[i] || (i > 0 && usePal(c->srcFormat)))
                    break;
                src2[i] += (dstSliceY >> ((i == 1 || i == 2) ? c->chrSrcVSubSample : 0)) * srcStride2[i];
            }

            for (i = 0; i < 4 && dst2[i]; i++) {
                if (!dst2[i] || (i > 0 && usePal(c->dstFormat)))
                    break;
                dst2[i] -= (dstSliceY >> ((i == 1 || i == 2) ? c->chrDstVSubSample : 0)) * dstStride2[i];
            }
            offset  = dstSliceY;
            slice_h = dstSliceH;
        }

        ret = c->convert_unscaled(c, src2, srcStride2, offset, slice_h,
                                  dst2, dstStride2);
        if (scale_dst)
            dst2[0] += dstSliceY * dstStride2[0];
    } else {
        ret = swscale(c, src2, srcStride2, srcSliceY_internal, srcSliceH,
                      dst2, dstStride2, dstSliceY, dstSliceH);
    }

    if (c->dstXYZ && !(c->srcXYZ && c->srcW==c->dstW && c->srcH==c->dstH)) {
        uint16_t *dst16;

        if (scale_dst) {
            dst16 = (uint16_t *)dst2[0];
        } else {
            int dstY = c->dstY ? c->dstY : srcSliceY + srcSliceH;

            av_assert0(dstY >= ret);
            av_assert0(ret >= 0);
            av_assert0(c->dstH >= dstY);
            dst16 = (uint16_t*)(dst2[0] + (dstY - ret) * dstStride2[0]);
        }

        /* replace on the same data */
        rgb48Toxyz12(c, dst16, dst16, dstStride2[0]/2, ret);
    }

    /* reset slice direction at end of frame */
    if ((srcSliceY_internal + srcSliceH == c->srcH) || scale_dst)
        c->sliceDir = 0;

    return ret;
}