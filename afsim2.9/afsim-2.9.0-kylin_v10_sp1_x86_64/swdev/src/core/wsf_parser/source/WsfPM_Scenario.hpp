// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPM_SCENARIO_HPP
#define WSFPM_SCENARIO_HPP

#include "wsf_parser_export.h"

#include <set>

#include "UtColor.hpp"
#include "WsfPProxyNode.hpp"

class WSF_PARSER_EXPORT WsfPM_Scenario : public WsfPProxyNode
{
public:
   WsfPM_Scenario() = default;
   WsfPM_Scenario(const WsfPProxyNode& aNode)
      : WsfPProxyNode(aNode)
   {
   }

   WsfProxy::String GetClassificationLabel();

   const UtColor& GetClassificationColor() const { return mColor; }

private:
   std::string FormatTrigraphsString() const;
   std::string FormatCaveatsString() const;
   void        DetermineLevel(const std::string& aLevelString);

   size_t                mClassificationLevel{0};
   std::string           mClassificationString{};
   std::set<std::string> mTrigraphsSet;
   std::set<std::string> mCaveatsSet;
   bool                  mLevelSet{false};
   UtColor               mColor;
};
#endif
