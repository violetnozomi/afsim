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
varying float cameraDist;
varying float vertDist;
uniform float maxRange;
void main()
{
    vec4 camPos = gl_ModelViewMatrix * gl_Vertex;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    cameraDist = max(100.0, length(camPos.zw));
    vertDist = length(gl_Vertex.xy);
    gl_FrontColor = gl_Color;
}
