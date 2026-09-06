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
uniform sampler2D tex2D;

void main( void )
{
   vec4 color = texture2D(tex2D, gl_TexCoord[0].st);
   gl_FragColor = color;
}
