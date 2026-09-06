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
uniform sampler2D Texture0;
uniform float height;

void main( void )
{
   float scale = 1.0;//mod(gl_TexCoord[0].y, 1.0 / (height * 0.5)) * height * 0.5;
   vec4 vColor = texture2D(Texture0, gl_TexCoord[0].xy);
   float avg = smoothstep(0.0, 1.0, (vColor.r + vColor.g + vColor.b) / 3.0);
   gl_FragColor = vec4(scale * smoothstep(0.0, 1.0, vColor.r), scale * smoothstep(0.0, 1.0, vColor.g), scale * smoothstep(0.0, 1.0, vColor.b), vColor.a);
}
