#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform vec3 vtk_sunVec;
uniform mat4 osg_ViewMatrix;
varying float lightIntensity;

void main( void )
{
   vec4 sun = osg_ViewMatrix * vec4(vtk_sunVec, 0.0);
   vec4 p = gl_ModelViewProjectionMatrix * gl_Vertex;
   p.z = min(p.z, p.w);
   gl_Position = p;
   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
   lightIntensity = min(2.0 * max(dot(gl_NormalMatrix * gl_Normal.xyz, sun.xyz), 0.0), 1.0);
}
