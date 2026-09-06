#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform sampler2D texture0;
varying float lightIntensity;

void main( void )
{
   gl_FragColor = lightIntensity * texture2D(texture0, gl_TexCoord[0].xy);
}
