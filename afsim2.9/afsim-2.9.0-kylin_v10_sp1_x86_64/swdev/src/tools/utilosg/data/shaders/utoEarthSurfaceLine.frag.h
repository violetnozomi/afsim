// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
uniform int  UTO_viewport_height;
uniform int  UTO_viewport_width;
varying vec2 linei;
varying vec2 screen_reference; // screen coordinate along line being drawn

vec2 GetArcTextureCoord()
{
   float screenHeight = float(UTO_viewport_height);
   float screenWidth  = float(UTO_viewport_width);
   vec2  fragxy       = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight) * 2.0 - 1.0;
   return vec2(linei.y, -distance(screen_reference, fragxy));
}

// vim: ft=glsl
