#version 130
#extension GL_ARB_gpu_shader5 : enable
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "logDepth.h" // logarithmic depth handing
#include "CoordConv.h"

uniform mat4 ecefMatrix; // converts model coordinates to ECEF world
out vec3 ecefCoord;

const float radEarth = 6356752.314;

void main( void )
{
   // 1. convert our vertex into ECEF world coordinates
   vec4 vw = ecefMatrix * gl_Vertex;
   ecefCoord = vw.xyz;
   // 2. convert ECEF world coordinates into transformed world coordinates
   vec4 flatproj = GetEqrCoords(vw);

   vec4 mw = ecefMatrix * vec4(0.0, 0.0, 0.0, 1.0);
   float dmw = dot(mw.xyz, mw.xyz);

   float t = (dmw - dot(mw.xyz, vw.xyz)) / (dot(mw.xyz - vw.xyz, mw.xyz - vw.xyz)); // find the closest point to the earth

   if (t > 0.0)
   {
      if (t <= 1.0)
      {
         vec3 l = mw.xyz * (1.0 - t) + vw.xyz * t; // closest distance to the earth
         if (length(l) < radEarth) // the closest point on the segment is inside the earth
         {
            flatproj.z = -radEarth; // hide it underground
         }
      }
   }

   gl_Position = flatproj;
}
