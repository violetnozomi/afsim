// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFEVENTPIPEINPUT_HPP
#define WSFEVENTPIPEINPUT_HPP

#include "UtOptional.hpp"
#include "WsfEventPipeOptions.hpp"
#include "WsfEventPipeOptionsCriteria.hpp"

//! Input configuration for the event pipe interface
class WsfEventPipeInput
{
public:
   WsfEventPipeInput() = default;
   struct CriteriaToDetail
   {
      WsfEventPipeOptionsCriteria mCriteria;
      std::string                 mDetailSettingName;
   };

   std::string                                mFileName;
   bool                                       mEnable{false};
   bool                                       mTestMode{false};
   std::map<std::string, WsfEventPipeOptions> mDetailSettings;
   std::vector<CriteriaToDetail>              mCriteriaToDetail;

   ut::optional<double> mPositionThresholdSquared{1.0}; // 1 meter
   ut::optional<double> mAngleThreshold{0.052};         // ~ 3 degrees
   double               mEntityStateMaximumInterval{10.0};
   double               mMaximumMoverUpdateInterval{5.0};
};

#endif
