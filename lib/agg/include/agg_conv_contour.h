//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.1
// Copyright (C) 2002-2004 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// conv_stroke
//
//----------------------------------------------------------------------------
#ifndef AGG_CONV_CONTOUR_INCLUDED
#define AGG_CONV_CONTOUR_INCLUDED

#include "agg_basics.h"
#include "agg_vcgen_contour.h"
#include "agg_conv_adaptor_vcgen.h"

namespace agg
{

    //-----------------------------------------------------------conv_contour
    template<class VertexSource> 
    struct conv_contour : public conv_adaptor_vcgen<VertexSource, vcgen_contour>
    {
        conv_contour(VertexSource& vs) : 
            conv_adaptor_vcgen<VertexSource, vcgen_contour>(vs)
        {
        }

        void width(double w) { generator().width(w); }
        void miter_limit(double ml) { generator().miter_limit(ml); }
        void miter_limit_theta(double t) { generator().miter_limit_theta(t); }
        void auto_detect_orientation(bool v) { generator().auto_detect_orientation(v); }

        double width() const { return generator().width(); }
        double miter_limit() const { return generator().miter_limit(); }
        bool auto_detect_orientation() const { return generator().auto_detect_orientation(); }

    private:
        conv_contour(const conv_contour<VertexSource>&);
        const conv_contour<VertexSource>& 
            operator = (const conv_contour<VertexSource>&);
    };

}

#endif
