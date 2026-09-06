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

#ifndef RVSATELLITETETHERDOCKWIDGET_HPP
#define RVSATELLITETETHERDOCKWIDGET_HPP

#include "wkf_common_export.h"

#include "WkfAttachmentSatelliteTetherGrid.hpp"
#include "WkfAttachmentSatelliteTetherTrace.hpp"
#include "WkfAttachmentSatelliteTetherTrack.hpp"
#include "misc/WkfImmersiveViewDockWidget.hpp"

namespace wkf
{
class WKF_COMMON_EXPORT SatelliteTetherDockWidget : public ImmersiveViewDockWidget
{
   Q_OBJECT
public:
   SatelliteTetherDockWidget(const QString&  aObjectName,
                             const QString&  aPlatformName,
                             unsigned int    aId,
                             QMainWindow*    aParent = nullptr,
                             Qt::WindowFlags aFlags  = Qt::WindowFlags());
   ~SatelliteTetherDockWidget() override;

   void         BuildContextMenuPrivate(QMenu& aMenu, const QPoint& aPos) override;
   void         SetLockToHeading(bool aState) override;
   virtual void DataUpdate();
   unsigned int GetId() const { return mId; };
   void         ShowNames(bool aState);
   void         ShowFutureTracks(bool aState);
   void         EnableLighting(bool aState);

   void UpdateColor(wkf::Platform* aPlatformPtr, const QColor& aColor);

   std::vector<std::string> GetTrackedPlatforms() const;

   virtual void AddTrackToDock(wkf::Platform* aPlatformPtr);

   QColor GetTrackColor(const std::string& aPlatform);

   template<typename Callable>
   void ApplyToTraces(Callable aOperation) const
   {
      for (auto iter : mTrackList)
      {
         aOperation(iter.second.second);
      }
   }

signals:
   void CleanUp(unsigned int aId);

protected:
   virtual void AddTrack(wkf::Platform* aPlatformPtr);
   virtual void ClearTracks();
   void         ClearGrid();

   std::unordered_map<std::string, QColor> mPlatformColorList;

private:
   bool HaveTrack(const unsigned int aUniqueId) const;
   void ViewerInitializedPrivate() override;
   void ConnectionEstablishedPrivate() override;
   void ClearDataPrivate() override;
   void EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aInTime);

   virtual void UpdateTrace(AttachmentSatelliteTetherTrace* aTracePtr);
   virtual wkf::AttachmentSatelliteTetherTrace*
        MakeTrace(wkf::Platform& aSrc, vespa::VaViewer& aViewerPtr, wkf::Platform& aTgt, const QColor& aColor) = 0;
   void TetherTo(vespa::VaEntity& aEntity);

   void SetAlignment(const AttachmentSatelliteTetherGrid::Alignment& aAlignment);
   void SetCartesian(bool aState);
   void AdjustCamera();
   void TrackedEntityDeleted(vespa::VaEntity* aEntityPtr);


   std::map<vespa::VaEntity*, std::pair<AttachmentSatelliteTetherTrack*, AttachmentSatelliteTetherTrace*>> mTrackList;
   AttachmentSatelliteTetherGrid* mGridPtr{nullptr};
   unsigned int                   mTetherId{0};
   double                         mHeading{0};
   unsigned int                   mId;
   bool                           mCartesianGrid{true};
   bool                           mShowNames{false};
   bool                           mShowFutureTracks{false};
};
} // namespace wkf

#endif
