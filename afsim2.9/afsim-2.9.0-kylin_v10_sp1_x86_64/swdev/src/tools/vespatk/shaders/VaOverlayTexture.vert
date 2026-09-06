#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

void main( void )
{
   gl_Position = ftransform();
   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
