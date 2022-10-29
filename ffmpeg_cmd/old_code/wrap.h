#ifndef WRAP_H
#define WRAP_H

#include <stdint.h>

extern "C" {
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
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







/* This struct should be aligned on at least a 32-byte boundary. */
typedef struct SwsContext {
    /**
     * info on struct for av_log
     */
    const AVClass *av_class;

    struct SwsContext *parent;

    AVSliceThread      *slicethread;
    struct SwsContext **slice_ctx;
    int                *slice_err;
    int              nb_slice_ctx;

    // values passed to current sws_receive_slice() call
    int dst_slice_start;
    int dst_slice_height;

    /**
     * Note that src, dst, srcStride, dstStride will be copied in the
     * sws_scale() wrapper so they can be freely modified here.
     */
    SwsFunc convert_unscaled;
    int srcW;                     ///< Width  of source      luma/alpha planes.
    int srcH;                     ///< Height of source      luma/alpha planes.
    int dstH;                     ///< Height of destination luma/alpha planes.
    int chrSrcW;                  ///< Width  of source      chroma     planes.
    int chrSrcH;                  ///< Height of source      chroma     planes.
    int chrDstW;                  ///< Width  of destination chroma     planes.
    int chrDstH;                  ///< Height of destination chroma     planes.
    int lumXInc, chrXInc;
    int lumYInc, chrYInc;
    enum AVPixelFormat dstFormat; ///< Destination pixel format.
    enum AVPixelFormat srcFormat; ///< Source      pixel format.
    int dstFormatBpp;             ///< Number of bits per pixel of the destination pixel format.
    int srcFormatBpp;             ///< Number of bits per pixel of the source      pixel format.
    int dstBpc, srcBpc;
    int chrSrcHSubSample;         ///< Binary logarithm of horizontal subsampling factor between luma/alpha and chroma planes in source      image.
    int chrSrcVSubSample;         ///< Binary logarithm of vertical   subsampling factor between luma/alpha and chroma planes in source      image.
    int chrDstHSubSample;         ///< Binary logarithm of horizontal subsampling factor between luma/alpha and chroma planes in destination image.
    int chrDstVSubSample;         ///< Binary logarithm of vertical   subsampling factor between luma/alpha and chroma planes in destination image.
    int vChrDrop;                 ///< Binary logarithm of extra vertical subsampling factor in source image chroma planes specified by user.
    int sliceDir;                 ///< Direction that slices are fed to the scaler (1 = top-to-bottom, -1 = bottom-to-top).
    int nb_threads;               ///< Number of threads used for scaling
    double param[2];              ///< Input parameters for scaling algorithms that need them.

    AVFrame *frame_src;
    AVFrame *frame_dst;

    RangeList src_ranges;

    /* The cascaded_* fields allow spliting a scaler task into multiple
     * sequential steps, this is for example used to limit the maximum
     * downscaling factor that needs to be supported in one scaler.
     */
    struct SwsContext *cascaded_context[3];
    int cascaded_tmpStride[4];
    uint8_t *cascaded_tmp[4];
    int cascaded1_tmpStride[4];
    uint8_t *cascaded1_tmp[4];
    int cascaded_mainindex;

    double gamma_value;
    int gamma_flag;
    int is_internal_gamma;
    uint16_t *gamma;
    uint16_t *inv_gamma;

    int numDesc;
    int descIndex[2];
    int numSlice;
    struct SwsSlice *slice;
    struct SwsFilterDescriptor *desc;

    uint32_t pal_yuv[256];
    uint32_t pal_rgb[256];

    float uint2float_lut[256];

    /**
     * @name Scaled horizontal lines ring buffer.
     * The horizontal scaler keeps just enough scaled lines in a ring buffer
     * so they may be passed to the vertical scaler. The pointers to the
     * allocated buffers for each line are duplicated in sequence in the ring
     * buffer to simplify indexing and avoid wrapping around between lines
     * inside the vertical scaler code. The wrapping is done before the
     * vertical scaler is called.
     */
    //@{
    int lastInLumBuf;             ///< Last scaled horizontal luma/alpha line from source in the ring buffer.
    int lastInChrBuf;             ///< Last scaled horizontal chroma     line from source in the ring buffer.
    //@}

    uint8_t *formatConvBuffer;
    int needAlpha;

    /**
     * @name Horizontal and vertical filters.
     * To better understand the following fields, here is a pseudo-code of
     * their usage in filtering a horizontal line:
     * @code
     * for (i = 0; i < width; i++) {
     *     dst[i] = 0;
     *     for (j = 0; j < filterSize; j++)
     *         dst[i] += src[ filterPos[i] + j ] * filter[ filterSize * i + j ];
     *     dst[i] >>= FRAC_BITS; // The actual implementation is fixed-point.
     * }
     * @endcode
     */
    //@{
    int16_t *hLumFilter;          ///< Array of horizontal filter coefficients for luma/alpha planes.
    int16_t *hChrFilter;          ///< Array of horizontal filter coefficients for chroma     planes.
    int16_t *vLumFilter;          ///< Array of vertical   filter coefficients for luma/alpha planes.
    int16_t *vChrFilter;          ///< Array of vertical   filter coefficients for chroma     planes.
    int32_t *hLumFilterPos;       ///< Array of horizontal filter starting positions for each dst[i] for luma/alpha planes.
    int32_t *hChrFilterPos;       ///< Array of horizontal filter starting positions for each dst[i] for chroma     planes.
    int32_t *vLumFilterPos;       ///< Array of vertical   filter starting positions for each dst[i] for luma/alpha planes.
    int32_t *vChrFilterPos;       ///< Array of vertical   filter starting positions for each dst[i] for chroma     planes.
    int hLumFilterSize;           ///< Horizontal filter size for luma/alpha pixels.
    int hChrFilterSize;           ///< Horizontal filter size for chroma     pixels.
    int vLumFilterSize;           ///< Vertical   filter size for luma/alpha pixels.
    int vChrFilterSize;           ///< Vertical   filter size for chroma     pixels.
    //@}

    int lumMmxextFilterCodeSize;  ///< Runtime-generated MMXEXT horizontal fast bilinear scaler code size for luma/alpha planes.
    int chrMmxextFilterCodeSize;  ///< Runtime-generated MMXEXT horizontal fast bilinear scaler code size for chroma planes.
    uint8_t *lumMmxextFilterCode; ///< Runtime-generated MMXEXT horizontal fast bilinear scaler code for luma/alpha planes.
    uint8_t *chrMmxextFilterCode; ///< Runtime-generated MMXEXT horizontal fast bilinear scaler code for chroma planes.

    int canMMXEXTBeUsed;
    int warned_unuseable_bilinear;

    int dstY;                     ///< Last destination vertical line output from last slice.
    int flags;                    ///< Flags passed by the user to select scaler algorithm, optimizations, subsampling, etc...
    void *yuvTable;             // pointer to the yuv->rgb table start so it can be freed()
    // alignment ensures the offset can be added in a single
    // instruction on e.g. ARM
    DECLARE_ALIGNED(16, int, table_gV)[256 + 2*YUVRGB_TABLE_HEADROOM];
    uint8_t *table_rV[256 + 2*YUVRGB_TABLE_HEADROOM];
    uint8_t *table_gU[256 + 2*YUVRGB_TABLE_HEADROOM];
    uint8_t *table_bU[256 + 2*YUVRGB_TABLE_HEADROOM];
    DECLARE_ALIGNED(16, int32_t, input_rgb2yuv_table)[16+40*4]; // This table can contain both C and SIMD formatted values, the C vales are always at the XY_IDX points
#define RY_IDX 0
#define GY_IDX 1
#define BY_IDX 2
#define RU_IDX 3
#define GU_IDX 4
#define BU_IDX 5
#define RV_IDX 6
#define GV_IDX 7
#define BV_IDX 8
#define RGB2YUV_SHIFT 15

    int *dither_error[4];

    //Colorspace stuff
    int contrast, brightness, saturation;    // for sws_getColorspaceDetails
    int srcColorspaceTable[4];
    int dstColorspaceTable[4];
    int srcRange;                 ///< 0 = MPG YUV range, 1 = JPG YUV range (source      image).
    int dstRange;                 ///< 0 = MPG YUV range, 1 = JPG YUV range (destination image).
    int src0Alpha;
    int dst0Alpha;
    int srcXYZ;
    int dstXYZ;
    int src_h_chr_pos;
    int dst_h_chr_pos;
    int src_v_chr_pos;
    int dst_v_chr_pos;
    int yuv2rgb_y_offset;
    int yuv2rgb_y_coeff;
    int yuv2rgb_v2r_coeff;
    int yuv2rgb_v2g_coeff;
    int yuv2rgb_u2g_coeff;
    int yuv2rgb_u2b_coeff;

#define RED_DITHER            "0*8"
#define GREEN_DITHER          "1*8"
#define BLUE_DITHER           "2*8"
#define Y_COEFF               "3*8"
#define VR_COEFF              "4*8"
#define UB_COEFF              "5*8"
#define VG_COEFF              "6*8"
#define UG_COEFF              "7*8"
#define Y_OFFSET              "8*8"
#define U_OFFSET              "9*8"
#define V_OFFSET              "10*8"
#define LUM_MMX_FILTER_OFFSET "11*8"
#define CHR_MMX_FILTER_OFFSET "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)
#define DSTW_OFFSET           "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)"*2"
#define ESP_OFFSET            "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)"*2+8"
#define VROUNDER_OFFSET       "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)"*2+16"
#define U_TEMP                "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)"*2+24"
#define V_TEMP                "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)"*2+32"
#define Y_TEMP                "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)"*2+40"
#define ALP_MMX_FILTER_OFFSET "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)"*2+48"
#define UV_OFF_PX             "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)"*3+48"
#define UV_OFF_BYTE           "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)"*3+56"
#define DITHER16              "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)"*3+64"
#define DITHER32              "11*8+4*4*"AV_STRINGIFY(MAX_FILTER_SIZE)"*3+80"
#define DITHER32_INT          (11*8+4*4*MAX_FILTER_SIZE*3+80) // value equal to above, used for checking that the struct hasn't been changed by mistake

    DECLARE_ALIGNED(8, uint64_t, redDither);
    DECLARE_ALIGNED(8, uint64_t, greenDither);
    DECLARE_ALIGNED(8, uint64_t, blueDither);

    DECLARE_ALIGNED(8, uint64_t, yCoeff);
    DECLARE_ALIGNED(8, uint64_t, vrCoeff);
    DECLARE_ALIGNED(8, uint64_t, ubCoeff);
    DECLARE_ALIGNED(8, uint64_t, vgCoeff);
    DECLARE_ALIGNED(8, uint64_t, ugCoeff);
    DECLARE_ALIGNED(8, uint64_t, yOffset);
    DECLARE_ALIGNED(8, uint64_t, uOffset);
    DECLARE_ALIGNED(8, uint64_t, vOffset);
    int32_t lumMmxFilter[4 * MAX_FILTER_SIZE];
    int32_t chrMmxFilter[4 * MAX_FILTER_SIZE];
    int dstW;                     ///< Width  of destination luma/alpha planes.
    DECLARE_ALIGNED(8, uint64_t, esp);
    DECLARE_ALIGNED(8, uint64_t, vRounder);
    DECLARE_ALIGNED(8, uint64_t, u_temp);
    DECLARE_ALIGNED(8, uint64_t, v_temp);
    DECLARE_ALIGNED(8, uint64_t, y_temp);
    int32_t alpMmxFilter[4 * MAX_FILTER_SIZE];
    // alignment of these values is not necessary, but merely here
    // to maintain the same offset across x8632 and x86-64. Once we
    // use proper offset macros in the asm, they can be removed.
    DECLARE_ALIGNED(8, ptrdiff_t, uv_off); ///< offset (in pixels) between u and v planes
    DECLARE_ALIGNED(8, ptrdiff_t, uv_offx2); ///< offset (in bytes) between u and v planes
    DECLARE_ALIGNED(8, uint16_t, dither16)[8];
    DECLARE_ALIGNED(8, uint32_t, dither32)[8];

    const uint8_t *chrDither8, *lumDither8;

#if HAVE_ALTIVEC
    vector signed short   CY;
    vector signed short   CRV;
    vector signed short   CBU;
    vector signed short   CGU;
    vector signed short   CGV;
    vector signed short   OY;
    vector unsigned short CSHIFT;
    vector signed short  *vYCoeffsBank, *vCCoeffsBank;
#endif

    int use_mmx_vfilter;

/* pre defined color-spaces gamma */
#define XYZ_GAMMA (2.6f)
#define RGB_GAMMA (2.2f)
    int16_t *xyzgamma;
    int16_t *rgbgamma;
    int16_t *xyzgammainv;
    int16_t *rgbgammainv;
    int16_t xyz2rgb_matrix[3][4];
    int16_t rgb2xyz_matrix[3][4];

    /* function pointers for swscale() */
    yuv2planar1_fn yuv2plane1;
    yuv2planarX_fn yuv2planeX;
    yuv2interleavedX_fn yuv2nv12cX;
    yuv2packed1_fn yuv2packed1;
    yuv2packed2_fn yuv2packed2;
    yuv2packedX_fn yuv2packedX;
    yuv2anyX_fn yuv2anyX;

    /// Unscaled conversion of luma plane to YV12 for horizontal scaler.
    void (*lumToYV12)(uint8_t *dst, const uint8_t *src, const uint8_t *src2, const uint8_t *src3,
                      int width, uint32_t *pal);
    /// Unscaled conversion of alpha plane to YV12 for horizontal scaler.
    void (*alpToYV12)(uint8_t *dst, const uint8_t *src, const uint8_t *src2, const uint8_t *src3,
                      int width, uint32_t *pal);
    /// Unscaled conversion of chroma planes to YV12 for horizontal scaler.
    void (*chrToYV12)(uint8_t *dstU, uint8_t *dstV,
                      const uint8_t *src1, const uint8_t *src2, const uint8_t *src3,
                      int width, uint32_t *pal);

    /**
     * Functions to read planar input, such as planar RGB, and convert
     * internally to Y/UV/A.
     */
    /** @{ */
    void (*readLumPlanar)(uint8_t *dst, const uint8_t *src[4], int width, int32_t *rgb2yuv);
    void (*readChrPlanar)(uint8_t *dstU, uint8_t *dstV, const uint8_t *src[4],
                          int width, int32_t *rgb2yuv);
    void (*readAlpPlanar)(uint8_t *dst, const uint8_t *src[4], int width, int32_t *rgb2yuv);
    /** @} */

    /**
     * Scale one horizontal line of input data using a bilinear filter
     * to produce one line of output data. Compared to SwsContext->hScale(),
     * please take note of the following caveats when using these:
     * - Scaling is done using only 7 bits instead of 14-bit coefficients.
     * - You can use no more than 5 input pixels to produce 4 output
     *   pixels. Therefore, this filter should not be used for downscaling
     *   by more than ~20% in width (because that equals more than 5/4th
     *   downscaling and thus more than 5 pixels input per 4 pixels output).
     * - In general, bilinear filters create artifacts during downscaling
     *   (even when <20%), because one output pixel will span more than one
     *   input pixel, and thus some pixels will need edges of both neighbor
     *   pixels to interpolate the output pixel. Since you can use at most
     *   two input pixels per output pixel in bilinear scaling, this is
     *   impossible and thus downscaling by any size will create artifacts.
     * To enable this type of scaling, set SWS_FLAG_FAST_BILINEAR
     * in SwsContext->flags.
     */
    /** @{ */
    void (*hyscale_fast)(struct SwsContext *c,
                         int16_t *dst, int dstWidth,
                         const uint8_t *src, int srcW, int xInc);
    void (*hcscale_fast)(struct SwsContext *c,
                         int16_t *dst1, int16_t *dst2, int dstWidth,
                         const uint8_t *src1, const uint8_t *src2,
                         int srcW, int xInc);
    /** @} */

    /**
     * Scale one horizontal line of input data using a filter over the input
     * lines, to produce one (differently sized) line of output data.
     *
     * @param dst        pointer to destination buffer for horizontally scaled
     *                   data. If the number of bits per component of one
     *                   destination pixel (SwsContext->dstBpc) is <= 10, data
     *                   will be 15 bpc in 16 bits (int16_t) width. Else (i.e.
     *                   SwsContext->dstBpc == 16), data will be 19bpc in
     *                   32 bits (int32_t) width.
     * @param dstW       width of destination image
     * @param src        pointer to source data to be scaled. If the number of
     *                   bits per component of a source pixel (SwsContext->srcBpc)
     *                   is 8, this is 8bpc in 8 bits (uint8_t) width. Else
     *                   (i.e. SwsContext->dstBpc > 8), this is native depth
     *                   in 16 bits (uint16_t) width. In other words, for 9-bit
     *                   YUV input, this is 9bpc, for 10-bit YUV input, this is
     *                   10bpc, and for 16-bit RGB or YUV, this is 16bpc.
     * @param filter     filter coefficients to be used per output pixel for
     *                   scaling. This contains 14bpp filtering coefficients.
     *                   Guaranteed to contain dstW * filterSize entries.
     * @param filterPos  position of the first input pixel to be used for
     *                   each output pixel during scaling. Guaranteed to
     *                   contain dstW entries.
     * @param filterSize the number of input coefficients to be used (and
     *                   thus the number of input pixels to be used) for
     *                   creating a single output pixel. Is aligned to 4
     *                   (and input coefficients thus padded with zeroes)
     *                   to simplify creating SIMD code.
     */
    /** @{ */
    void (*hyScale)(struct SwsContext *c, int16_t *dst, int dstW,
                    const uint8_t *src, const int16_t *filter,
                    const int32_t *filterPos, int filterSize);
    void (*hcScale)(struct SwsContext *c, int16_t *dst, int dstW,
                    const uint8_t *src, const int16_t *filter,
                    const int32_t *filterPos, int filterSize);
    /** @} */

    /// Color range conversion function for luma plane if needed.
    void (*lumConvertRange)(int16_t *dst, int width);
    /// Color range conversion function for chroma planes if needed.
    void (*chrConvertRange)(int16_t *dst1, int16_t *dst2, int width);

    int needs_hcscale; ///< Set if there are chroma planes to be converted.

    SwsDither dither;

    SwsAlphaBlend alphablend;

    // scratch buffer for converting packed rgb0 sources
    // filled with a copy of the input frame + fully opaque alpha,
    // then passed as input to further conversion
    uint8_t     *rgb0_scratch;
    unsigned int rgb0_scratch_allocated;

    // scratch buffer for converting XYZ sources
    // filled with the input converted to rgb48
    // then passed as input to further conversion
    uint8_t     *xyz_scratch;
    unsigned int xyz_scratch_allocated;

    unsigned int dst_slice_align;
    atomic_int   stride_unaligned_warned;
    atomic_int   data_unaligned_warned;
} SwsContext;
//FIXME check init (where 0)






int wap_sws_scale( SwsContext *c, const uint8_t * const srcSlice[], const int srcStride[], int srcSliceY, int srcSliceH, uint8_t *const dst[], const int dstStride[] );


#endif