#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform vec2 u2D_Param; // {sma, e}
uniform vec3 uRotations;// {raan, incl, arg of peri}
uniform float uTimeAng; // current time

//USE_FLAT_MAP_PROJECTION is set/unset by VaViewer::MapLoadedMapCB
#pragma import_defines (USE_FLAT_MAP_PROJECTION)
#pragma import_defines (USE_ECEF)

#ifdef USE_FLAT_MAP_PROJECTION
#include "CoordConv.h"
#endif

void main( void )
{
   float cosX = cos(gl_Vertex.x);
   float radius = u2D_Param.x * (1.0 - pow(u2D_Param.y, 2.0)) / (1.0 + u2D_Param.y * cosX);
   vec3 v2d = vec3(radius * cosX, radius * sin(gl_Vertex.x), 0.0);

   float sinA = sin(uRotations.x);
   float cosA = cos(uRotations.x);
   float sinB = sin(uRotations.y);
   float cosB = cos(uRotations.y);
   float sinG = sin(uRotations.z);
   float cosG = cos(uRotations.z);

   mat3 p2eci = mat3(cosG*cosA - sinG*cosB*sinA,
                     cosG*sinA + sinG*cosB*cosA,
                     sinG*sinB,
                     -sinG*cosA - cosG*cosB*sinA,
                     -sinG*sinA + cosG*cosB*cosA,
                     cosG*sinB,
                     sinB*sinA,
                     -sinB*cosA,
                     cosB);

   vec3 pos = p2eci * v2d;
   float earthAngle = uTimeAng;
#ifdef USE_ECEF
   earthAngle = gl_Vertex.y;
#endif

   float sEA = sin(earthAngle);
   float cEA = cos(earthAngle);
   pos = vec3(cEA * pos.x - sEA * pos.y,
              sEA * pos.x + cEA * pos.y,
              pos.z);

#ifdef USE_FLAT_MAP_PROJECTION
   pos = GetEqrCoords(vec4(pos, 1.0)).xyz;
#endif

   gl_Position = vec4(pos, 1.0);
}
