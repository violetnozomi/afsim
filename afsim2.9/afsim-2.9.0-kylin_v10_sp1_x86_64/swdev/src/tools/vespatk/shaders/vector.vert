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

uniform vec3 vtk_sunVec;

uniform float modelScalar; // this is the approximate radius of the unscaled model
uniform mat4 osg_ModelViewProjectionMatrix;
uniform float UTO_OrthographicZoom;
uniform float UTO_OrthographicMode;
uniform vec4 UTO_Viewport;
uniform float VA_IconScale;
uniform float VA_TrueScale;

#include "logDepth.h" // logarithmic depth handing

varying float diffuse;

float GetScreenScalar()
{
   // Transform only the (object) origin to clip-space
   vec4 zero = osg_ModelViewProjectionMatrix * vec4(0.0, 0.0, 0.0, 1.0);
   // The w component of a clip-space vertex gives you information about the scaling due to perspective.
   // Here, we extract the w component of the object origin and manipulate it.
   // Orthographic projection works differently, and UTO_OrthographicMode lets you switch between 
   // perspective and orthographic.
   return mix(min(UTO_Viewport.w / zero.w / VA_IconScale * 0.1, 10.0 / modelScalar), 0.05 / UTO_OrthographicZoom / VA_IconScale, UTO_OrthographicMode);
}

void main( void )
{
   vec3 n = normalize(gl_NormalMatrix * gl_Normal);
   diffuse = max(dot(vtk_sunVec, n), 0.0);
   
   GetScreenScalar();
   
      // manipulate the scalar component to cause screen scaling during the usual transform.
   // trueScale allows you to turn off the scalar to the normal fixed pipeline stuff.
   float scalar = mix( GetScreenScalar(), gl_Vertex.w * 10.0 / modelScalar, VA_TrueScale); 
   
   vec4 p = osg_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, scalar);

   vtkLogDepth(p, gl_ProjectionMatrix);
   gl_Position = p;
}
