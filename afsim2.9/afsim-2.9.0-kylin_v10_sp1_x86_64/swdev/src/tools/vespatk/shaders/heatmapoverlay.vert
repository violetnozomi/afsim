#version 130
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

in vec3 position;
in float fieldValue;

out float colorValue;

uniform float minValue;
uniform float maxValue;

void main(void)
{
   float delta = maxValue - minValue;
   if (delta <= 0.0)
   {
      colorValue = 1.0;
   }
   else
   {
      colorValue = (clamp(fieldValue, minValue, maxValue) - minValue) / delta;
   }

   gl_Position = vec4(position, 1.0);
}
