#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

varying float diffuse;
uniform vec4 uColor;
uniform bool uniformColor;

void main(void)
{
   if (uniformColor)
      gl_FragColor = uColor;
   else 
      gl_FragColor = gl_FrontMaterial.ambient * gl_FrontLightProduct[0].ambient + gl_FrontMaterial.diffuse * gl_FrontLightProduct[0].diffuse * diffuse;
}
