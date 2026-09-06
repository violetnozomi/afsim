#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform vec4 UTO_Viewport;

void main( void )
{
   gl_FrontColor = gl_Color;
   
   vec4 zero = gl_ModelViewProjectionMatrix * vec4(0.0, 0.0, 0.0, 1.0);
   float scalar = UTO_Viewport.w / zero.w * 1.5;
   
   mat4 mm = gl_ModelViewMatrix;
   float sx = length(mm[0].xyz);
   float sy = length(mm[1].xyz);
   float sz = length(mm[2].xyz);
   mm[0][0] = 0.0;
   mm[0][1] = sx;
   mm[0][2] = 0.0;
   mm[1][0] = sy;
   mm[1][1] = 0.0;
   mm[1][2] = 0.0;
   mm[2][0] = 0.0;
   mm[2][1] = 0.0;
   mm[2][2] = sz;
   
   // Here we combine the manipulated scalar component with the original xyz and perform the usual transform.
   gl_Position = gl_ProjectionMatrix * mm * vec4(gl_Vertex.xyz, scalar);
//   gl_Position = BillboardTransform(gl_Vertex);
}
