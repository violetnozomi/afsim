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
#ifndef L16_INTERVALUPDATER_HPP
#define L16_INTERVALUPDATER_HPP

#include "UtCallback.hpp"
#include "UtInput.hpp"
class WsfSimulation;

namespace WsfL16
{
namespace IntervalUpdaterPrivate
{
class UpdateEvent;
}

// Classes such as Processors and Movers have an Update() function called
// on a regular interval.  This feature is useful in other places.
// This class may either be used as a base class by overriding the Update() method
// or using callbacks added to the OnUpdate callback list.
class IntervalUpdater
{
public:
   IntervalUpdater();
   explicit IntervalUpdater(double aTimeInterval);
   IntervalUpdater(const IntervalUpdater& aSrc);
   virtual ~IntervalUpdater();

   IntervalUpdater& operator=(const IntervalUpdater& aRhs) = delete;

   double GetUpdateInterval() const { return mUpdateInterval; }

   //! Changes the update interval.
   //! Change will take effect immediately if called within Update(),
   //! otherwise, next time Update() is called.
   void SetUpdateInterval(double aUpdateInterval);

   double GetLastUpdateTime() { return mLastUpdateTime; }

   virtual void Update(double aSimTime);

   void Initialize(double aSimTime, WsfSimulation& aSimulation, bool aAutoStart = true);

   bool ProcessInput(UtInput& aInput);

   UtCallbackListN<void(double)> OnUpdate;

   double UpdatePrivate(double aSimTime);

   void TurnOn(double aSimTime);

   void TurnOff(double aSimTime);

private:
   void HandleSimulationComplete(double aSimTime);

   void StartUpdates(double aSimTime);

   void DeleteEvent();

   bool                                 mIsInitialized;
   double                               mLastUpdateTime;
   double                               mUpdateInterval;
   IntervalUpdaterPrivate::UpdateEvent* mEventPtr;
   std::unique_ptr<UtCallback>          mSimCompleteCallbackPtr;
   WsfSimulation*                       mSimulationPtr;
};
} // namespace WsfL16

#endif
