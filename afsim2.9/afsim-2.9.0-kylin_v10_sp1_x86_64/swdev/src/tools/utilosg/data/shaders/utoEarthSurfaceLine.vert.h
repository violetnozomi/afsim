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
#include "math.h"
// arc source point
uniform vec3 source;
// arc destination point
uniform vec3 dest;
// Angle between two points.  Computation is done on CPU for accuracy
uniform float arcAngle;
// how much the vertices should arc above the earth.  0.0 is along the earth surface
uniform float arcingFactor = 1.0 / 100.0;

// Interpolating values along arc
varying vec2 linei;
// Screen x,y of a reference point used for computing texture coordinates
varying vec2 screen_reference;

vec4 ComputeArcLine(out vec2 outScreenRef)
{
   linei  = gl_Vertex.xy; // [0-1] from source to dest, [0-1] lateral offset
   vec3 a = dest;
   vec3 b = source;

   vec3  axb          = cross(a, b);
   vec3  axbNorm      = normalize(axb);
   float la           = length(a);
   float lb           = length(b);
   vec3  mid          = (a + b) * 0.5;
   vec3  component    = cross(axbNorm, a);
   component          = normalize(component) * la;
   float avgMagnitude = (la + lb) * 0.5;
   float arcLift      = length(a - b) * arcingFactor;

   float angle = linei.x * arcAngle;

   float lift = sin(linei.x * M_PI) * arcLift;
   float pl   = mix(la, lb, linei.x) + lift;

   vec4 p;
   p.xyz        = (cos(angle) * a + sin(angle) * component) * (pl / la);
   p.w          = 1.0;
   float wcoord = max(1.0, (gl_ModelViewProjectionMatrix * p).w);

   p.xyz += axbNorm * wcoord * .010 * (0.5 - linei.y);
   vec4 screen_a   = gl_ModelViewProjectionMatrix * vec4(a, 1);
   vec4 screen_b   = gl_ModelViewProjectionMatrix * vec4(b, 1);
   vec2 screen_bxy = screen_b.xy / abs(screen_b.w);
   vec2 screenTmp  = screen_a.xy / abs(screen_a.w);
   outScreenRef    = (screenTmp - screen_bxy) * 10.0 + screen_bxy;
   return gl_ModelViewProjectionMatrix * p;
}

// vim: ft=glsl
