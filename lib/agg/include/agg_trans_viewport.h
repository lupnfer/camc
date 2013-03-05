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
// Viewport transformer - simple orthogonal conversions from world coordinates
//                        to screen (device) ones.
//
//----------------------------------------------------------------------------

#ifndef AGG_TRANS_VIEWPORT_INCLUDED
#define AGG_TRANS_VIEWPORT_INCLUDED

#include "agg_basics.h"


namespace agg
{

    //----------------------------------------------------------trans_viewport
    class trans_viewport
    {
    public:
        trans_viewport() : 
            m_world_x1(0), 
            m_world_y1(0), 
            m_world_w(1),
            m_world_h(1),
            m_device_x1(0),
            m_device_y1(0),
            m_device_w(1),
            m_device_h(1),
            m_kx(1),
            m_ky(1)
        {}

        void device_viewport(double x1, double y1, double x2, double y2)
        {
            m_world_x1 = m_device_x1 = x1;
            m_world_y1 = m_device_y1 = y1;
            m_world_w  = m_device_w  = x2 - x1;
            m_world_h  = m_device_h  = y2 - y1;
            m_kx = m_device_w / m_world_w;
            m_ky = m_device_h / m_world_h;
        }

        void world_viewport(double x1, double y1, double x2, double y2)
        {
            m_world_x1 = x1;
            m_world_y1 = y1;
            m_world_w  = x2 - x1;
            m_world_h  = y2 - y1;
            m_kx = m_device_w / m_world_w;
            m_ky = m_device_h / m_world_h;
        }

        void world_viewport(double* x1, double* y1, double* x2, double* y2) const
        {
            *x1 = m_world_x1;
            *y1 = m_world_y1;
            *x2 = m_world_x1 + m_world_w;
            *y2 = m_world_y1 + m_world_h;
        }

        void transform(double* x, double* y) const
        {
            *x = (*x - m_world_x1) * m_kx + m_device_x1;
            *y = (*y - m_world_y1) * m_ky + m_device_y1;
        }

        void inverse_transform(double* x, double* y) const
        {
            *x = (*x - m_device_x1) / m_kx + m_world_x1;
            *y = (*y - m_device_y1) / m_ky + m_world_y1;
        }

    private:
        double m_world_x1;
        double m_world_y1;
        double m_world_w;
        double m_world_h;
        double m_device_x1;
        double m_device_y1;
        double m_device_w;
        double m_device_h;
        double m_kx;
        double m_ky;
    };


}


#endif
