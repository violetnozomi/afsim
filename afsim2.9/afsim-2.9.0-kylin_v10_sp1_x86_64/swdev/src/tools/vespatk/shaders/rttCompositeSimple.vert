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
void main( void )
{
   gl_Position = vec4(2 * gl_Vertex.xy - 1, -1, 1);
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
