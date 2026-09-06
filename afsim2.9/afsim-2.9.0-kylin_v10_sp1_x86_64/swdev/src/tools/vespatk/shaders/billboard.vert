#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform float ephemerisLit;
uniform vec4 VA_TeamColor;
varying vec3 lightVec;
varying vec3 eyeVec;
varying vec4 teamColor;

#include "screenspace.h"

void main( void )
{
   teamColor = VA_TeamColor;
   vec4 vsmodelzero = GetModelZero();
   gl_Position = BillboardTransform(gl_Vertex);
   gl_TexCoord[0] = vec4(gl_Vertex.yx * vec2(1, 1) + vec2(0.5, 0.5), 0.0, 0.0);
   eyeVec = vec3(gl_ModelViewMatrix * gl_Vertex);


   vec3 n = normalize(gl_NormalMatrix * gl_Normal);
   vec3 t = cross(n, gl_NormalMatrix * vec3(0.0, 0.0, 1.0));
   vec3 t2 = cross(n, gl_NormalMatrix * vec3(0.0, 1.0, 0.0));
   if (length(t2) > length(t))
      t = t2;
   t = normalize(t);
   vec3 b = normalize(cross(n, t));
   vec3 v;
   vec3 ls = mix(-1.0 * vsmodelzero.xyz, gl_LightSource[0].position.xyz, ephemerisLit);
   v.x = dot(ls, t);
   v.y = dot(ls, b);
   v.z = dot(ls, n);
   lightVec = normalize(v);
   v.x = dot(eyeVec, t);
   v.y = dot(eyeVec, b);
   v.z = dot(eyeVec, n);
   eyeVec = normalize(v);
}
