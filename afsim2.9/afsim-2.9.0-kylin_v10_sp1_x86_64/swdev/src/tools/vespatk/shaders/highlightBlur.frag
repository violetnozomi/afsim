#version 130
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform sampler2D tex2D;
uniform float osg_FrameTime;

void main( void )
{
   vec4 maxc = textureOffset(tex2D, gl_TexCoord[0].st, ivec2(-1, -2)) * 0.3;
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 0, -2)) * 0.6);
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 1, -2)) * 0.3);
   
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2(-2, -1)) * 0.3);
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2(-1, -1)));
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 0, -1)));
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 1, -1)));
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 2, -1)) * 0.3);
   
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2(-2,  0)) * 0.6);
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2(-1,  0)));
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 1,  0)));
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 2,  0)) * 0.6);
   
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2(-2,  1)) * 0.3);
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2(-1,  1)));
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 0,  1)));
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 1,  1)));
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 2,  1)) * 0.3);
   
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2(-1,  2)) * 0.3);
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 0,  2)) * 0.6);
   maxc = max(maxc, textureOffset(tex2D, gl_TexCoord[0].st, ivec2( 1,  2)) * 0.3);
   gl_FragColor = maxc;
}
