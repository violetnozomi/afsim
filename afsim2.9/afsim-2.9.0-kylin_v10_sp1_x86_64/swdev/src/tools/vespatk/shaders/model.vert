#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// unused and untested at this version

uniform float VA_DirectionallyLit;
uniform float VA_ModelsSupportLighting;
uniform vec4 VA_TeamColor;
varying vec3 lightVec;
varying vec3 eyeVec;
varying vec4 teamColor;

#include "screenspace.h"

void main( void )
{
   teamColor = VA_TeamColor;
   vec4 vsmodelzero = GetModelZero();
   gl_Position = ScreenSpaceTransform(gl_Vertex);
   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
   eyeVec = vec3(gl_ModelViewMatrix * gl_Vertex);


   vec3 n = normalize(gl_NormalMatrix * gl_Normal);
   vec3 t = cross(n, gl_NormalMatrix * vec3(0.0, 0.0, 1.0));
   vec3 t2 = cross(n, gl_NormalMatrix * vec3(0.0, 1.0, 0.0));
   if (length(t2) > length(t))
      t = t2;
   t = normalize(t);
   vec3 b = normalize(cross(n, t));
   vec3 v;
   vec3 ls = mix(-1.0 * vsmodelzero.xyz, gl_LightSource[0].position.xyz, VA_DirectionallyLit * VA_ModelsSupportLighting);
   v.x = dot(ls, t);
   v.y = dot(ls, b);
   v.z = dot(ls, n);
   lightVec = normalize(v);
   v.x = dot(eyeVec, t);
   v.y = dot(eyeVec, b);
   v.z = dot(eyeVec, n);
   eyeVec = normalize(v);
}
