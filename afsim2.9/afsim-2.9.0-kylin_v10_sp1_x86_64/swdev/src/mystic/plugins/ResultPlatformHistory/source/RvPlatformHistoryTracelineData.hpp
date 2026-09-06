// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RvPlatformHistoryTracelineData_HPP
#define RvPlatformHistoryTracelineData_HPP

#include "RvPlatformHistoryStateData.hpp"
#include "UtColor.hpp"
#include "platform_history/WkfPlatformHistoryPrefWidget.hpp"

namespace wkf
{
class Platform;
}

namespace vespa
{
class VaViewer;
}

namespace RvPlatformHistory
{
/**
 * Manages the traceline for a platform.
 */
class TracelineData
{
public:
   TracelineData(wkf::Platform& aPlatform, vespa::VaViewer* aViewerPtr);
   virtual ~TracelineData();

   void                 SetVisible(bool aVisible);
   void                 SetColoration(const wkf::TraceStateValue& aColoration, const std::vector<UtColor>& aColorArray);
   void                 SetLineWidth(unsigned int aWidth);
   bool                 IsVisible() const;
   void                 Update(wkf::TraceStateValue           aStateSet,
                               rv::ResultPlatform*            aPlatformData,
                               float                          aPruneBefore,
                               float                          aPruneAfter,
                               const std::pair<float, float>& aLowRange,
                               const std::pair<float, float>& aHighRange);
   bool                 GetFullUpdateRequired() const { return mFullUpdateRequired; }
   void                 SetFullUpdateRequired(bool aState) { mFullUpdateRequired = aState; }
   wkf::TraceStateValue GetColoration() { return mColoration; }
   wkf::Platform*       GetPlatform() { return mPlatformPtr; }
   // Ownership of aStateSet transfers to this TracelineData.
   void AddStateSet(wkf::TraceStateValue aStateValue, std::unique_ptr<StateData> aStateSet)
   {
      mStateSets.emplace(aStateValue, std::move(aStateSet));
   }
   std::unique_ptr<StateData> PopStateSet(wkf::TraceStateValue aStateValue);

private:
   std::map<wkf::TraceStateValue, std::unique_ptr<StateData>> mStateSets;
   wkf::Platform*                                             mPlatformPtr;
   //! id of trace line attachment for fetching the attachment from the platform pointer
   unsigned int         mTraceLineUniqueId;
   double               mLastUpdateTime{0.0};
   bool                 mFullUpdateRequired{false};
   wkf::TraceStateValue mColoration{wkf::eTRACE_STATE};
   unsigned int         mLineWidth{0};
};
} // namespace RvPlatformHistory
#endif // RvPlatformHistoryTracelineData_HPP
