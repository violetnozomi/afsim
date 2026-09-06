// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "logDepth.h" 

varying vec4 timerColor;

void main(void)
{
   timerColor = gl_Color;
   vec4 p = gl_ModelViewProjectionMatrix * gl_Vertex;
   vtkLogDepth(p, gl_ProjectionMatrix);
   gl_Position = p;
}
