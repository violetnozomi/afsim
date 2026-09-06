// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

uniform float stepMeters;
uniform float minStepMeters;
uniform float maxStepMeters;
uniform float colorScalar;
uniform int targetLayer;
uniform int oe_layer_uid;

float Lerp(float value, float valueMin, float valueMax, float rangeMin, float rangeMax)
{
   return rangeMin + ((value - valueMin) * (rangeMax - rangeMin)) / (valueMax - valueMin);
}

void ColorizeTerrain(inout vec4 color)
{
   if (oe_layer_uid != targetLayer)
     return;

   float currentElevationMeters = color.r;

   // Handle no postings (such as north pole), assume sealevel.
   if (isnan(currentElevationMeters) || currentElevationMeters < -1e6)
      currentElevationMeters = 0.0;

   // Round elevation to two decimal places (i.e., 1cm). This helps smooth the elevation
   // steps some (especially in the oceans).
   currentElevationMeters = trunc(currentElevationMeters * 100.0) / 100.0;

   float currentStepMeters = ceil(currentElevationMeters / stepMeters) * stepMeters;
   float v = Lerp(currentStepMeters, minStepMeters, maxStepMeters, 0.0, colorScalar);

   gl_FragData[0] = vec4(v, 1, 1, 1);
}
