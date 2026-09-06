#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform vec3 vtk_sunVec;
uniform mat4 osg_ViewMatrix;

const float cB    = 6356752.3142451794975;
const float cA    = 6378137.0;
const float cAtmoDepth = 200000.0;
const float cPi   = 3.141592654;

varying vec4 v4ZeroPos;
varying vec3 sunLight;

void main()
{
   sunLight = vec3(osg_ViewMatrix * vec4(vtk_sunVec, 0)).xyz;
   v4ZeroPos = (gl_ModelViewMatrix * vec4(0.0, 0.0, 0.0, 1.0));
   vec3 east = vec3(0, -v4ZeroPos.z, v4ZeroPos.y);
   mat4 bb;
   bb[0].xyz = normalize(v4ZeroPos.xyz);
   bb[0].w = 0.0;
   bb[1].xyz = normalize(east);
   bb[1].w = 0.0;
   bb[2].xyz = normalize(cross(east, v4ZeroPos.xyz));
   bb[2].w = 0.0;
   bb[3] = vec4(0.0, 0.0, 0.0, 1.0);

   float lookDistance = length(v4ZeroPos.xyz);
   float horizonStart = cPi;
   float horizonEnd = cPi;
   if (cB <= lookDistance)
   {
      horizonStart = asin(cB / lookDistance);
   }
   if ((cAtmoDepth + cA) <= lookDistance)
   {
      horizonEnd = asin((cAtmoDepth + cA) / lookDistance);
   }
   float cosStart = cos(horizonStart);
   float sinStart = sin(horizonStart);
   float cosEnd = cos(horizonEnd);
   float sinEnd = sin(horizonEnd);

   vec3 start = vec3(cosStart, sinStart * gl_Vertex.y, sinStart * gl_Vertex.x);
   vec3 end = vec3(cosEnd, sinEnd * gl_Vertex.y, sinEnd * gl_Vertex.x);
   vec3 place = mix(start, end, gl_Vertex.z);
   gl_Position = gl_ProjectionMatrix * bb * vec4(place * gl_DepthRange.far, 1.0);
   gl_Position.z = 0.5;

   gl_FrontColor = gl_Color;
}
