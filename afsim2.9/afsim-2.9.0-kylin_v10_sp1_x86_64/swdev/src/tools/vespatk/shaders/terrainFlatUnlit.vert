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
uniform mat4 viewMatrix;
uniform mat4 viewMatrixInverse;
uniform vec4 projTexXform;
const float fMeterPerRad = 6366707.019;
void main( void )
{
   gl_Position = ftransform();
   vec4 p = viewMatrixInverse * gl_ModelViewMatrix * gl_Vertex;
   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
   vec4 geodetic = vec4(p.x / fMeterPerRad, p.y / fMeterPerRad, 0, 0);
   gl_TexCoord[7] = vec4((geodetic.x - projTexXform.x) / projTexXform.y, (geodetic.y - projTexXform.z) / projTexXform.w, 0, 0);
}
