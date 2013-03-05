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

#ifndef AGG_CONV_SEGMENTATOR_INCLUDED
#define AGG_CONV_SEGMENTATOR_INCLUDED

#include "agg_basics.h"
#include "agg_conv_adaptor_vpgen.h"
#include "agg_vpgen_segmentator.h"

namespace agg
{

    //========================================================conv_segmentator
    template<class VertexSource> 
    struct conv_segmentator : public conv_adaptor_vpgen<VertexSource, vpgen_segmentator>
    {
        conv_segmentator(VertexSource& vs) : 
            conv_adaptor_vpgen<VertexSource, vpgen_segmentator>(vs) {}

        void approximation_scale(double s) { vpgen().approximation_scale(s);        }
        double approximation_scale() const { return vpgen().approximation_scale();  }

    private:
        conv_segmentator(const conv_segmentator<VertexSource>&);
        const conv_segmentator<VertexSource>& 
            operator = (const conv_segmentator<VertexSource>&);
    };


}

#endif
