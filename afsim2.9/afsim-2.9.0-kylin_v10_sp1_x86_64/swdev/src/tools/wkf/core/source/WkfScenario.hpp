// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFVASCENARIO_HPP
#define WKFVASCENARIO_HPP

#include "wkf_export.h"

#include <QString>

#include "VaEntityPtr.hpp"
#include "VaScenario.hpp"
#include "WkfBullseye.hpp"
#include "WkfPointOfInterest.hpp"
#include "WkfTrack.hpp"
#include "WkfVtkEnvironment.hpp"

namespace wkf
{
class Platform;

class WKF_EXPORT Scenario : public vespa::VaScenario
{
public:
   Scenario();

   Platform* FindPlatformByIndex(unsigned int aIndex) const;
   Platform* FindPlatformByName(const std::string& aName) const;
   Platform* FindPlatformByName(const QString& aName) const;

   bool AddEntity(vespa::VaEntity* aEntityPtr) override;
   bool RemoveEntity(vespa::VaEntity* aEntityPtr) override;

   bool DeletePlatformByIndex(unsigned int aIndex);

   bool AddPlatformWithIndex(Platform* aPlatformPtr, unsigned int aIndex);

   const std::map<unsigned int, Platform*>& GetIndexPlatformMap() const { return mIndexedPlatformMap; }

   Platform* CreatePlaceholderPlatform(unsigned int aIndex);

   Track* FindTrack(const TrackId& aTrackId) const;
   void   AddTrack(Track* aTrackPtr);
   void   DeleteTrack(const TrackId& aTrackId);
   void   DeleteAllTracks();

   void EnableMovement(bool aEnable) { mMovementEnabled = aEnable; }

   void MoveSelection(vespa::VaViewer*          aViewerPtr,
                      const vespa::VaEntityMap& aEntityMap,
                      vespa::VaAttachmentList&  aAttachmentList,
                      UtEntity&                 aRefBefore,
                      UtEntity&                 aRefAfter,
                      int                       aFeedbackFormat,
                      double                    aFeedbackPrecision,
                      bool                      aOverrideModifiable = false) override;

   void MoveSelectionComplete(vespa::VaViewer*         aViewerPtr,
                              vespa::VaEntityMap&      aEntityMap,
                              vespa::VaAttachmentList& aAttachmentList) override;

   const BullseyeList&        GetBullseyes() const { return mBullseyeList; }
   bool                       GetRangeBearingFromNearestBullseye(const vespa::VaPosition& aPosition,
                                                                 double&                  aRange_m,
                                                                 double&                  aBearing_deg,
                                                                 std::string&             aBullseyeName) const;
   const PointOfInterestList& GetPointOfInterests() const { return mPOI_List; }

private:
   std::map<unsigned int, Platform*> mIndexedPlatformMap;
   std::map<TrackId, Track*>         mTrackMap;

   BullseyeList        mBullseyeList;
   PointOfInterestList mPOI_List;

   bool mRemovingAllTracks;
   bool mMovementEnabled;
};

using vespa::make_entity_at;
using vespa::make_strong_entity_ptr_at;

template<typename T, typename... Args>
vespa::EntityPtr<T> make_entity(Args&&... aArgs)
{
   assert(vaEnv.GetStandardScenario() != nullptr);
   return make_entity_at<T>(*vaEnv.GetStandardScenario(), std::forward<Args>(aArgs)...);
}

template<typename T, typename... Args>
vespa::StrongEntityPtr<T> make_strong_entity_ptr(Args&&... aArgs)
{
   assert(vaEnv.GetStandardScenario() != nullptr);
   return make_strong_entity_ptr_at(*vaEnv.GetStandardScenario(), std::forward<Args>(aArgs)...);
}
} // namespace wkf
#endif
