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

void main( void )
{
   gl_FrontColor = gl_Color;
   vec4 p = gl_ModelViewProjectionMatrix * gl_Vertex;
   vtkLogDepth(p, gl_ProjectionMatrix);
   gl_Position = p;
}
