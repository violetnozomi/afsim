#version 120
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

uniform mat4 osg_ViewMatrixInverse;
const float radEarth = 6356752.314;

void main( void )
{
   vec4 v = gl_Vertex;
   mat4 modelMat = osg_ViewMatrixInverse * gl_ModelViewMatrix;
   vec4 mw = modelMat * vec4(0.0, 0.0, 0.0, 1.0);
   vec4 vw = modelMat * gl_Vertex;
   float dmw = dot(mw.xyz, mw.xyz);
   float t = (dmw - dot(mw.xyz, vw.xyz)) / (dot(mw.xyz - vw.xyz, mw.xyz - vw.xyz)); // find the closest point to the earth

   if (t > 0.0)
   {
      if (t < 1.0)
      {
         vec3 l = mw.xyz * (1.0 - t) + vw.xyz * t; // closest distance to the earth
         if (length(l) < radEarth) // the closest point on the segment is inside the earth
         {
            v.xyz *= length(l - mw.xyz) / length(v.xyz);
         }
      }
   }
   vec4 p = gl_ModelViewProjectionMatrix * v;
   vtkLogDepth(p, gl_ProjectionMatrix);
   p.z = min(p.w, p.z);
   gl_Position = p;
}
