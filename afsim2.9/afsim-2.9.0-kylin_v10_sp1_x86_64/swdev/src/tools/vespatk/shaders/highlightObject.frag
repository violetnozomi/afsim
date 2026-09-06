#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform vec4 highlightColor;
uniform sampler2D texture0;

void main( void )
{
   vec4 vColor = texture2D(texture0, gl_TexCoord[0].xy);
   gl_FragColor = vec4(highlightColor.rgb, step(0.1, vColor.a));
}
