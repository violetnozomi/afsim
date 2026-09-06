// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "ColorProxyUtil.hpp"

#include "UtColor.hpp"
#include "UtOptional.hpp"
#include "WsfPProxyNode.hpp"
#include "WsfPProxyValue.hpp"

ut::optional<UtColor> ColorProxyUtil::GetColor(const WsfPProxyNode& aColorNode)
{
   ut::optional<UtColor> color;
   if (aColorNode.IsValid() && !aColorNode.GetValue().GetAttr("red").IsInherited())
   {
      WsfPProxyValue valR = aColorNode.GetValue().GetAttr("red");
      WsfPProxyValue valG = aColorNode.GetValue().GetAttr("green");
      WsfPProxyValue valB = aColorNode.GetValue().GetAttr("blue");
      WsfPProxyValue valA = aColorNode.GetValue().GetAttr("alpha");
      if (valR.IsValid() && valG.IsValid() && valB.IsValid())
      {
         int r;
         int g;
         int b;
         int a = 255;
         if (valA.IsValid())
         {
            valA.GetIntegerValue(a);
         }
         valR.GetIntegerValue(r);
         valG.GetIntegerValue(g);
         valB.GetIntegerValue(b);
         color = UtColor(r / 255.0F, g / 255.0F, b / 255.0F, a / 255.0F);
      }
   }
   return color;
}
