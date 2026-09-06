// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform vec2 textureScalar;

void main()
{
   gl_Position = gl_Vertex;
   gl_TexCoord[0] = vec4(gl_MultiTexCoord0.xy * textureScalar, 0.0, 0.0);
}
