#version 110
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform vec4 uColor;

void main(void)
{
   if (uColor.a == 0.0)
   {
      discard;
   }
   gl_FragColor = uColor;
}
