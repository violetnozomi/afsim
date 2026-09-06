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
uniform float UTO_OrthographicZoom;
uniform float UTO_OrthographicMode;
uniform float VA_IconScale;
uniform float VA_TrueScale;

void main( void )
{
   mat4 modelview = gl_ModelViewMatrix;
   modelview[0][0] = 1.0;
   modelview[0][1] = 0.0;
   modelview[0][2] = 0.0;

   modelview[1][0] = 0.0;
   modelview[1][1] = 1.0;
   modelview[1][2] = 0.0;

   modelview[2][0] = 0.0;
   modelview[2][1] = 0.0;
   modelview[2][2] = 1.0;

   // move it over and up by scale size
   vec4 zero = gl_ProjectionMatrix * modelview * vec4(0.0, 0.0, 0.0, 1.0);
   float w = zero.w;
   zero /= w; // normalize... this will happen later anyway, but we want normal coords for...

   // apply the vertex as a pixel offset
   vec4 offset = vec4(2.0 * (gl_Vertex.x / UTO_Viewport.z), 2.0 * (gl_Vertex.y + 24.0 * mix(VA_IconScale, 0.0, VA_TrueScale)) / UTO_Viewport.w, 0.0, 0.0);

   gl_Position = zero + offset;
   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
