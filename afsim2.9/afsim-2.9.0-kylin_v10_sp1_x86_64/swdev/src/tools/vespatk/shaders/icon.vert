#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform vec4 VA_TeamColor;
uniform float VA_SuppressTeamColor;

#include "screenspace.h"

void main(void)
{
   gl_FrontColor = gl_Color * mix(VA_TeamColor, vec4(1.0, 1.0, 1.0, 1.0), VA_SuppressTeamColor);

   gl_Position = ScreenSpaceTransform(gl_Vertex);
}
