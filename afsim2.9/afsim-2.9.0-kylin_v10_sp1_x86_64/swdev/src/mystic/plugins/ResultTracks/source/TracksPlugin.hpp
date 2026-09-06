// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef TRACKSPLUGIN_HPP
#define TRACKSPLUGIN_HPP

#include <QList>
#include <QMap>
#include <QTreeWidgetItem>

#include "VaCallbackHolder.hpp"
#include "WkfScenario.hpp"
#include "WkfUnitTypes.hpp"

// Note: this order of inclusion for generated headers is required
#include "RvEventPipeClasses.hpp"
#include "RvEventPipeMessages.hpp"
#include "RvPlugin.hpp"
#include "RvTrackDb.hpp"

namespace rv
{
class MsgBase;
class MsgLocalTrackUpdate;
class TrackDb;
} // namespace rv

namespace vespa
{
class VaAttachment;
class VaEntity;
} // namespace vespa

namespace wkf
{
class Platform;
class PlatformDataTreeItem;
class TrackDisplayInterface;
} // namespace wkf

namespace RvTracks
{
class Plugin : public rv::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);

protected:
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void RegularRead(const rv::ResultData& aData) override;

   QList<QTreeWidgetItem*> GetPlatformData(const std::string& aPlatformName) override;
   void                    PlatformDataItemSelected(QTreeWidgetItem* aItem) override;
   void                    PlatformDataItemUnSelected() override;

   std::unique_ptr<wkf::Updater> GetUpdater(const QString&     aPlatformName,
                                            const QString&     aDatum,
                                            const unsigned int aContext = 0) const override;

   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) override;
   void BuildItemContextMenu(QMenu* aMenu, QTreeWidgetItem* aItemPtr) override;

private:
   void TrackVisibilityChangedCB();

   void PlatformOfInterestChanged(wkf::Platform* aPlatform);
   void GuiUpdate() override;
   void DataUpdate(const rv::ResultData& aData);
   void UpdateTracks(double aSimTime, const rv::TrackDb* aTrackDb, int aIndex);
   void UpdateTrack(const wkf::TrackId& aTrackId, rv::MsgLocalTrackUpdate* aTrackMsg);
   void UpdateTrackLabel(wkf::Track* aTrack, bool aVisible, bool aShowColor) const;
   void ClearTracks();

   void        RemoveTrackHighlight();
   void        TraceTrack(QString aPlatformName, int aTrackId);
   static bool TraceSort(const std::pair<rv::MsgBase*, unsigned int>& aLeft,
                         const std::pair<rv::MsgBase*, unsigned int>& aRight);
   void        PlotActionHandler();

   QList<QTreeWidgetItem*> mTopLevelWidgets;

   class TrackItems
   {
   public:
      TrackItems(QTreeWidgetItem* aParentItem, int aTrackId, const QString& aPlatformName, bool aLocal);

      void SetUserDataIdString(QString aPlatformName, int aTrackId);

      wkf::PlatformDataTreeItem*    mParentItem;
      wkf::PlatformDataTreeItem*    mPositionValid;
      wkf::LatitudeTreeWidgetItem*  mLatitude;
      wkf::LongitudeTreeWidgetItem* mLongitude;
      wkf::AltitudeTreeWidgetItem*  mAltitude;
      wkf::PlatformDataTreeItem*    mVelocityValid;
      wkf::SpeedTreeWidgetItem*     mSpeed;
      wkf::AngleTreeWidgetItem*     mHeading;
      wkf::LengthTreeWidgetItem*    mRange{nullptr};
      wkf::AngleTreeWidgetItem*     mBearing{nullptr};
      wkf::AngleTreeWidgetItem*     mElevation{nullptr};
      wkf::PlatformDataTreeItem*    mSide;
      wkf::PlatformDataTreeItem*    mType;
      wkf::PlatformDataTreeItem*    mSpatialDomain;
      wkf::UnitlessTreeWidgetItem*  mQuality;
   };

   using TrackChildItems = std::map<int, TrackItems>;

   template<typename MessageUpdateType, typename MessageCreateType>
   void PlotTrackChildItems(TrackChildItems&                                                    aTrackChildItems,
                            const rv::Track_Id&                                                 aTrackId,
                            const rv::TrackDb::TrackList<MessageUpdateType, MessageCreateType>& aMessages,
                            double                                                              aSimTime,
                            bool                                                                aLocal);

   TrackChildItems      mLocalTrackChildItems;
   TrackChildItems      mSensorTrackChildItems;
   static constexpr int TRACK_ID_ROLE = Qt::UserRole;
   static constexpr int LOCAL_ID_ROLE = Qt::UserRole + 1;
   static const QString TRACK_SEPARATOR;

   wkf::Track*                       mTempHighlightTrack;
   std::vector<vespa::VaAttachment*> mTempHighlightAttachments;
   // managed by Qt
   wkf::TrackDisplayInterface* mTrackDisplayInterface;
};
} // namespace RvTracks
#endif
