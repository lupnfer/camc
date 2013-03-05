#pragma once

#include <vector>
#include "GraphicsGems.h"

typedef struct tag_StrokeStylePoint{
	double m_u;
	double m_width;
} SSP;

typedef std::vector<SSP> STROKESTYLE;