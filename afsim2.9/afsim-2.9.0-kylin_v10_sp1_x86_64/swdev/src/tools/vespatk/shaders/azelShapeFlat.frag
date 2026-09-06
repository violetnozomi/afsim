#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// unused and untested with this shader version
uniform sampler1D Texture0;
uniform int projected;
varying vec4 vertColor;

void main( void )
{
   vec4 texColor = vec4(texture1D(Texture0, gl_TexCoord[0].x).rgb, vertColor.a);
   vec4 vColor = mix(texColor, vertColor, (float)projected);
   gl_FragColor = vColor;
}
