//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2021 The Boeing Company, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

inline float checkOn(vec4 fragCoord)
{
   return mod(floor(fragCoord.x / 8.0) + floor(fragCoord.y / 8.0), 2.0);
}

inline vec4 zoneColor(vec4 fragCoord, vec4 baseColor, float inLimits, float alphaAdjust)
{
   float notInLimits = 1.0 - inLimits;
   float alpha = (1.0 - 0.75 * notInLimits) - 0.25 * checkOn(fragCoord) * notInLimits;
   return vec4(
      mix(baseColor.r, 1.0 - baseColor.r, notInLimits),
      mix(baseColor.g, 1.0 - baseColor.g, notInLimits),
      mix(baseColor.b, 1.0 - baseColor.b, notInLimits),
          baseColor.w * alpha * alphaAdjust
      );
}
