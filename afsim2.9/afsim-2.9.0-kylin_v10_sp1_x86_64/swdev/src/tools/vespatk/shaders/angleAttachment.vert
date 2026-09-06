#version 150 compatibility
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform mat4 osg_ViewMatrix;
uniform vec3 uVectorStart;
uniform vec3 uVectorEnd;
uniform float uRadius;

#include "logDepth.h" // logarithmic depth handing

void main( void )
{
   // the model position and scaling
   vec4 zero = gl_ModelViewMatrix * vec4(0.0, 0.0, 0.0, 1.0);
   
   vec3 xvec = normalize(uVectorStart);
   vec3 yvec = normalize(uVectorEnd);
   // rotate to the (non-orthogonal plane)
   mat3 rotation = mat3(xvec.x, xvec.y, xvec.z, yvec.x, yvec.y, yvec.z, 0.0, 0.0, 0.0);
   
   vec3 xv = rotation * gl_Vertex.xyz;
   
   // normalize and scale non-zero length vectors
   float length = length(xv);
   if (length > 0.0)
   {
      xv *= uRadius / length;
   }
   // rotate, add the position, and project to screen
   vec4 p = gl_ProjectionMatrix * (zero + osg_ViewMatrix * vec4(xv, 0.0));
   vtkLogDepth(p, gl_ProjectionMatrix);
   gl_Position = p;
}
