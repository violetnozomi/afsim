// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2021 The Boeing Company, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

//  Show min/max range zone limits
uniform float minRange;
uniform float maxRange;
varying float cameraDist;
varying float vertDist;
#include "zoneLimits.h"

// inverse of step()
float istep(float edge, float x) { return 1.0 - step(edge, x); }

void main(void)
{
   float rampDist = cameraDist / 200.0;
   float inZone = step(minRange, vertDist) * istep(maxRange + 0.1, vertDist);
   gl_FragColor = zoneColor(gl_FragCoord, gl_Color, inZone, 1.0);
}

