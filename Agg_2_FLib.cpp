//////////////////////////////////////////////////////////////////
//																//
//		用途 : AGG封装      									//
//		创建 : [Foolish] / 2004-5-18							//
//		更新 :                      							//
//		主页 : http://www.crazy-bit.com/						//
//		邮箱 : crazybit@263.net									//
//									(c) 1999 - 2004 =USTC= 付黎	//
//////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "StdDefine.h"
#include "ObjImage.h"



#ifdef WIN32
    #pragma warning (disable : 4786) // identifier was truncated to '255' characters in the browser information
#endif
#include "./lib/agg/include/agg_rendering_buffer.h"
#include "./lib/agg/include/agg_renderer_scanline.h"
#include "./lib/agg/include/agg_scanline_bin.h"
#include "./lib/agg/include/agg_pixfmt_gray8.h"
#include "./lib/agg/include/agg_rasterizer_scanline_aa.h"
#include "./lib/agg/include/agg_path_storage.h"
#include "./lib/agg/include/agg_trans_affine.h"
#include "./lib/agg/include/agg_conv_transform.h"
#include "./lib/agg/include/agg_scanline_u.h"
#include "./lib/agg/include/agg_scanline_p.h"
#include "./lib/agg/include/agg_pixfmt_rgba32.h"
#include "./lib/agg/include/agg_pixfmt_rgb24.h"
#include "./lib/agg/include/agg_span_image_filter_rgba32.h"
#include "./lib/agg/include/agg_span_image_filter_rgb24.h"
#include "./lib/agg/include/agg_span_interpolator_linear.h"
#include "./lib/agg/include/agg_rounded_rect.h"
#include "./lib/agg/include/agg_ellipse.h"
#include "./lib/agg/include/agg_conv_curve.h"
#include "./lib/agg/include/agg_conv_stroke.h"
#include "./lib/agg/include/agg_conv_dash.h"
#include "./lib/agg/include/agg_conv_marker.h"
#include "./lib/agg/include/agg_arrowhead.h"
#include "./lib/agg/include/agg_vcgen_markers_term.h"

//=============================================================================
// 多边形填充（8bit）
void  aggFillPolygon_Normal_8Bit (FCObjImage & img, const POINT * ppt, int cNumPoint)
{
    if ((ppt == NULL) || (cNumPoint < 3) || (img.ColorBits() != 8))
        return ;

    // 制作agg多边形
    agg::path_storage     aggPoly ;
    aggPoly.move_to (ppt[0].x, ppt[0].y) ;
    for (int i=1 ; i < cNumPoint ; i++)
    {
        aggPoly.line_to (ppt[i].x, ppt[i].y) ;
    }
    aggPoly.close_polygon() ;

typedef agg::pixfmt_gray8 pixfmt;
typedef agg::renderer_base<pixfmt> renderer_base;
typedef agg::renderer_scanline_bin_solid<renderer_base> renderer_bin;

// 目标image
    agg::rendering_buffer   aggImg (img.GetMemStart(), (unsigned)img.Width(), (unsigned)img.Height(), -(int)img.GetPitch()) ;
    pixfmt          pixf (aggImg) ;
    renderer_base   rb (pixf) ;
    renderer_bin    ren_bin (rb) ;
    ren_bin.color (agg::rgba(1.0, 1.0, 1.0)) ;

    agg::rasterizer_scanline_aa<> ras ;
    ras.add_path (aggPoly) ;
    ras.filling_rule (agg::fill_even_odd) ;
    agg::scanline_bin     aScanlineBin ;
    ras.render (aScanlineBin, ren_bin) ;
}
//=============================================================================
static void  aggDrawPath_AA_8Bit (FCObjImage & img,
                                  agg::path_storage & aggPath,
                                  int nWidth, int nGray,
                                  BOOL bAddArrow, LINE_STYLE lineStyle,
                                  agg::vcgen_stroke::line_cap_e lineCap)
{
    if (img.ColorBits() != 8)
    {
        FAssert(FALSE) ; return ;
    }

typedef agg::pixfmt_gray8 pixfmt;
typedef agg::renderer_base<pixfmt> renderer_base;
typedef agg::renderer_scanline_p_solid<renderer_base> renderer_solid;

    agg::rendering_buffer   aggRenderBuffer (img.GetMemStart(), (unsigned)img.Width(), (unsigned)img.Height(), -(int)img.GetPitch()) ;
    pixfmt            aggPixf (aggRenderBuffer) ;
    renderer_base     aggRenBase (aggPixf) ;
    renderer_solid    ren (aggRenBase) ;

    const double     fGray = nGray / (double)0xFF ;
    ren.color (agg::rgba(fGray,fGray,fGray,1)) ; // 呜呜呜，画出来的是254

    // 准备agg环境
    agg::conv_curve<agg::path_storage>    aggCurve(aggPath) ;
    agg::conv_dash<agg::conv_curve<agg::path_storage>, agg::vcgen_markers_term>   aggDash(aggCurve) ;
    agg::conv_stroke<agg::conv_dash<agg::conv_curve<agg::path_storage>, agg::vcgen_markers_term> >   aggStroke(aggDash) ;
    aggStroke.line_cap (lineCap) ;
    aggStroke.width (nWidth) ;

    // 制作箭头
    const double      k = ::pow ((double)nWidth, 0.7) ;
    agg::arrowhead    aggHeader ;
    aggHeader.head (4*k, 4*k, 3*k, 2*k) ;
    agg::conv_marker<agg::vcgen_markers_term, agg::arrowhead>   aggArrow(aggDash.markers(), aggHeader) ;

    // 线样式
    switch (lineStyle)
    {
        case LINE_STYLE_SOLID : aggDash.add_dash (65535, 10) ; break ;
        case LINE_STYLE_DASH : aggDash.add_dash (2*nWidth, 2*nWidth) ; break ;
        case LINE_STYLE_DOT : aggDash.add_dash (0.1, nWidth*2) ; break ;
    }

    // 画线
    agg::rasterizer_scanline_aa<>   aggRas ;
    aggRas.add_path (aggStroke) ;
    if (bAddArrow)
    {
        aggRas.add_path (aggArrow) ;
    }
    agg::scanline_p8     aScanlineP8 ;
    aggRas.render (aScanlineP8, ren) ;
}
//=============================================================================
void  aggDrawline_AA_8Bit (FCObjImage & img,
                           const POINT & pt1, const POINT & pt2,
                           int nWidth, int nGray,
                           BOOL bAddArrow, LINE_STYLE lineStyle)
{
    // make line path
    agg::path_storage   aggPath ;
    aggPath.move_to (pt1.x, pt1.y) ;
    aggPath.line_to (pt2.x, pt2.y) ;
    ::aggDrawPath_AA_8Bit (img, aggPath, nWidth, nGray, bAddArrow, lineStyle, agg::vcgen_stroke::round_cap) ;
}
//=============================================================================
void  aggDrawRect_AA_8Bit (FCObjImage & img,
                           const RECT & rcRect,
                           int nWidth, int nGray,
                           LINE_STYLE lineStyle)
{
    // make rect path
    const double        fOffset = ((nWidth % 2) ? 0.5 : 0.0) ;
    agg::rounded_rect   aggRectDraw (rcRect.left + fOffset, rcRect.top + fOffset,
                                     rcRect.right - fOffset, rcRect.bottom - fOffset, 0.0) ;
    agg::path_storage   aggPath ;
    aggPath.add_path (aggRectDraw) ;
    ::aggDrawPath_AA_8Bit (img, aggPath, nWidth, nGray, FALSE, lineStyle, agg::vcgen_stroke::square_cap) ;
}
//=============================================================================
void  aggDrawEllipse_AA_8Bit (FCObjImage & img,
                              const RECT & rcEllipse,
                              int nWidth, int nGray,
                              LINE_STYLE lineStyle)
{
    // make ellipse path
    const double        fCenterX = (rcEllipse.left + rcEllipse.right) / 2.0,
                        fCenterY = (rcEllipse.top + rcEllipse.bottom) / 2.0 ;
    agg::ellipse        aggEllipseDraw (fCenterX, fCenterY, RECTWIDTH(rcEllipse)/2.0, RECTHEIGHT(rcEllipse)/2.0, 150) ;
    agg::path_storage   aggPath ;
    aggPath.add_path (aggEllipseDraw) ;
    ::aggDrawPath_AA_8Bit (img, aggPath, nWidth, nGray, FALSE, lineStyle, agg::vcgen_stroke::round_cap) ;
}
//=============================================================================
void  aggStretchImage_AA_24Bit (FCObjImage & img, int nNewWidth, int nNewHeight)
{
    if ((img.ColorBits() != 24) || (nNewWidth == 1) || (nNewHeight == 1))
    {
        FAssert(FALSE) ; return ;
    }

    // 保存原图
    const FCObjImage       imgOld(img) ;
    agg::rendering_buffer  aggRenderOld (imgOld.GetMemStart(), (unsigned)imgOld.Width(), (unsigned)imgOld.Height(), -(int)imgOld.GetPitch()) ;
    if (!img.Create (nNewWidth, nNewHeight, imgOld.ColorBits()))
        return ;

    // 变形matrix
    agg::trans_affine   aggImg_mtx ;
    const double        fScaleX = (imgOld.Width()-1) / (double)(nNewWidth-1),
                        fScaleY = (imgOld.Height()-1) / (double)(nNewHeight-1) ;
    aggImg_mtx *= agg::trans_affine_scaling (fScaleX, fScaleY) ;

    typedef agg::pixfmt_bgr24 pixfmt;
    typedef agg::renderer_base<pixfmt> renderer_base;

    agg::rendering_buffer   aggRenderBuffer (img.GetMemStart(), (unsigned)img.Width(), (unsigned)img.Height(), -(int)img.GetPitch()) ;
    pixfmt          aggPixf (aggRenderBuffer) ;
    renderer_base   rb (aggPixf) ;

    rb.clear (agg::rgba(1.0, 1.0, 1.0, 0.0)) ;

    typedef agg::span_interpolator_linear<> interpolator_type ;
    interpolator_type   interpolator(aggImg_mtx) ;

    typedef agg::span_image_filter_rgb24_bilinear<agg::order_bgr24, interpolator_type> span_gen_type;
    typedef agg::renderer_scanline_u<renderer_base, span_gen_type> renderer_type;

    agg::span_allocator<agg::rgba8>     aggSA ;
    span_gen_type   sg (aggSA, aggRenderOld, agg::rgba(1, 1, 1, 0), interpolator) ;

    agg::rasterizer_scanline_aa<>    aggRas ;
    agg::rounded_rect   aggBound (0, 0, nNewWidth, nNewHeight, 0) ;

    // 这句不能加，否则release有问题而debug没问题
    agg::conv_transform<agg::rounded_rect>   tr(aggBound, agg::trans_affine()) ;

    aggRas.add_path (aggBound) ;
    renderer_type ri(rb, sg);
    agg::scanline_u8     aScanlineU8 ;
    aggRas.render(aScanlineU8, ri);
}
void  aggStretchImage_AA_32Bit (FCObjImage & img, int nNewWidth, int nNewHeight)
{
    if ((img.ColorBits() != 32) || (nNewWidth == 1) || (nNewHeight == 1))
    {
        FAssert(FALSE) ; return ;
    }

    // 保存原图
    const FCObjImage       imgOld(img) ;
    agg::rendering_buffer  aggRenderOld (imgOld.GetMemStart(), (unsigned)imgOld.Width(), (unsigned)imgOld.Height(), -(int)imgOld.GetPitch()) ;
    if (!img.Create (nNewWidth, nNewHeight, imgOld.ColorBits()))
        return ;

    // 变形matrix
    agg::trans_affine   aggImg_mtx ;
    const double        fScaleX = (imgOld.Width()-1) / (double)(nNewWidth-1),
                        fScaleY = (imgOld.Height()-1) / (double)(nNewHeight-1) ;
    aggImg_mtx *= agg::trans_affine_scaling (fScaleX, fScaleY) ;

typedef agg::pixfmt_bgra32 pixfmt;
typedef agg::renderer_base<pixfmt> renderer_base;

    agg::rendering_buffer   aggRenderBuffer (img.GetMemStart(), (unsigned)img.Width(), (unsigned)img.Height(), -(int)img.GetPitch()) ;
    pixfmt          aggPixf (aggRenderBuffer) ;
    renderer_base   rb (aggPixf) ;

    rb.clear (agg::rgba(1.0, 1.0, 1.0, 0.0)) ;

typedef agg::span_interpolator_linear<> interpolator_type ;
    interpolator_type   interpolator(aggImg_mtx) ;

typedef agg::span_image_filter_rgba32_bilinear<agg::order_bgra32, interpolator_type> span_gen_type;
typedef agg::renderer_scanline_u<renderer_base, span_gen_type> renderer_type;

    agg::span_allocator<agg::rgba8>     aggSA ;
    span_gen_type   sg (aggSA, aggRenderOld, agg::rgba(1.0, 1.0, 1.0, 0.0), interpolator) ;

    agg::rasterizer_scanline_aa<>     aggRas ;
    agg::rounded_rect   aggBound (0, 0, nNewWidth, nNewHeight, 0) ;

    // 这句不能加，否则release有问题而debug没问题
    // agg::conv_transform<agg::rounded_rect>   tr(aggBound, agg::trans_affine()) ;

    aggRas.add_path (aggBound) ;
    renderer_type ri(rb, sg);
    agg::scanline_u8    aScanlineU8 ;
    aggRas.render(aScanlineU8, ri);
}
//=============================================================================
void  aggRotateImage_AA_32Bit (FCObjImage & img, int nAngle)
{
    if (img.ColorBits() != 32)
    {
        FAssert(FALSE) ; return ;
    }

	// calc new size
    double      fSin = sin(AngleToRadian(nAngle % 90)),
                fCos = cos(AngleToRadian(nAngle % 90)) ;
    int			nNewWidth, nNewHeight ;
    // 注意这里的宽高计算
    if (nAngle % 180 >= 90)
    {
        nNewWidth = (int)(img.Width() * fSin + img.Height() * fCos) ;
        nNewHeight = (int)(img.Height() * fSin + img.Width() * fCos) ;
    }
    else
    {
        nNewWidth = (int)(img.Width() * fCos + img.Height() * fSin) ;
        nNewHeight = (int)(img.Height() * fCos + img.Width() * fSin) ;
    }

    // 保存原图
    const FCObjImage        imgOld(img) ;
    agg::rendering_buffer   aggRenderOld (imgOld.GetMemStart(), (unsigned)imgOld.Width(), (unsigned)imgOld.Height(), -(int)imgOld.GetPitch()) ;
    if (!img.Create (nNewWidth, nNewHeight, imgOld.ColorBits()))
        return ;

    agg::trans_affine       aggImg_mtx ;
    aggImg_mtx *= agg::trans_affine_translation (-imgOld.Width()/2.0, -imgOld.Height()/2.0) ;
    aggImg_mtx *= agg::trans_affine_rotation (AngleToRadian(nAngle)) ;
    aggImg_mtx *= agg::trans_affine_translation (nNewWidth/2.0, nNewHeight/2.0) ;
    aggImg_mtx.invert() ;

typedef agg::pixfmt_bgra32 pixfmt; 
typedef agg::renderer_base<pixfmt> renderer_base;

    agg::rendering_buffer   aggRenderBuffer (img.GetMemStart(), (unsigned)img.Width(), (unsigned)img.Height(), -(int)img.GetPitch()) ;
    pixfmt          aggPixf (aggRenderBuffer) ;
    renderer_base   rb (aggPixf) ;

    rb.clear (agg::rgba(1.0, 1.0, 1.0, 0)) ;

typedef agg::span_interpolator_linear<> interpolator_type ;
    interpolator_type   interpolator(aggImg_mtx) ;

typedef agg::span_image_filter_rgba32_bilinear<agg::order_bgra32, interpolator_type> span_gen_type;
typedef agg::renderer_scanline_u<renderer_base, span_gen_type> renderer_type;

    agg::span_allocator<agg::rgba8>     aggSA ;
    span_gen_type   sg (aggSA, aggRenderOld, agg::rgba(1, 1, 1, 0), interpolator) ;

    agg::rasterizer_scanline_aa<>     aggRas ;
    agg::rounded_rect   aggBound (0, 0, aggRenderBuffer.width(), aggRenderBuffer.height(), 0) ;

    // 这句不能加，否则release有问题而debug没问题
    agg::conv_transform<agg::rounded_rect>   tr(aggBound, agg::trans_affine()) ;

    aggRas.add_path (aggBound) ;
    renderer_type ri(rb, sg);
    agg::scanline_u8    aScanlineU8 ;
    aggRas.render(aScanlineU8, ri);
}
//=============================================================================
