// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef FIRES_LAUNCH_COMPUTER_HPP
#define FIRES_LAUNCH_COMPUTER_HPP

#include "wsf_fires_export.h"

#include <map>

#include "FiresTable.hpp"
#include "UtEntity.hpp"
class UtInput;
#include "WsfLaunchComputer.hpp"
class WsfScenario;
#include "WsfStringId.hpp"

namespace Fires
{

class FiresTables;

//! A launch computer for use with weapons employing the WSF_FIRES_MOVER type.
//! This class stores the fires tables of range, max. ord. (max. height), and
//! time of flight, for classes of Systems + Munitions.  If a weapon uses this
//! launch computer, the fired munition will automatically call "GetMaxOrdAndTOF"
//! to initialize itself.
class WSF_FIRES_EXPORT FiresLaunchComputer : public WsfLaunchComputer
{
public:
   explicit FiresLaunchComputer(WsfScenario* aScenarioPtr);
   FiresLaunchComputer(const FiresLaunchComputer& aSrc) = default;
   FiresLaunchComputer& operator=(const FiresLaunchComputer& aSrc) = delete;
   ~FiresLaunchComputer() override                                 = default;

   WsfLaunchComputer* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   // static void ResetState();

   double EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime) override;

protected:
   bool InitializeTTIData() override;

private:
   WsfScenario* mScenarioPtr;
   FiresTables* mFiresTablesPtr;

   WsfStringId mType;

   bool     mDebug;
   UtEntity mGeom;

   double mMinRange;
   double mMaxRange;
};

}; // namespace Fires

#endif
