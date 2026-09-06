// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFENGAGEMENTSTATISTICS_HPP
#define WKFENGAGEMENTSTATISTICS_HPP

#include <set>

#include <QDialog>
class QMenu;
class QTableWidgetItem;
class Ui_WkfEngagementStatistics;

#include "wkf_common_export.h"

#include <iostream>

#include "WkfAbstractTableModel.hpp"
#include "WkfPlatformGrouping.hpp"
#include "ui_WkfEngagementStatistics.h"

namespace wkf
{
class WKF_COMMON_EXPORT EngagementStatistics : public QDialog
{
   Q_OBJECT
public:
   EngagementStatistics(QWidget* aParent);
   ~EngagementStatistics() override;

   bool InterestedIn(unsigned int aAttackerId, unsigned int aTargetId);
   void AddWeaponFire(float              aSimTime,
                      unsigned int       aFiringPlatformId,
                      unsigned int       aTargetPlatformId,
                      const std::string& aIntendedTargetName,
                      unsigned int       aWeaponPlatformIndex,
                      int                aLocalTrackNumber,
                      const std::string& aLocalTrackOwner,
                      const QString&     aAttackName,
                      const QString&     aTargetName,
                      const QString&     aWeaponName);
   void AddWeaponTerminate(float              aSimTime,
                           unsigned int       aFiringPlatformId,
                           unsigned int       aTargetPlatformId,
                           const std::string& aIntendedTargetName,
                           unsigned int       aWeaponPlatformIndex,
                           char               aResult,
                           const std::string& aExtendedResult,
                           float              aMissDistance,
                           const QString&     aAttackName,
                           const QString&     aTargetName,
                           const QString&     aWeaponName);
signals:
   void FilterChanged();
   void BuildRowContextMenu(QMenu* aMenuPtr, const QStringList& aData);

private:
   void ItemClicked(const QModelIndex& aIndex);
   void ContextMenuEvent(const QModelIndex& aIndex, const QPoint& aPoint);
   void SelectAttacker(const wkf::PlatformGrouping& aAttacker);
   void SelectTarget(const wkf::PlatformGrouping& aTarget);

   class EngagementTableModel : public AbstractTableModel
   {
   public:
      EngagementTableModel(QObject* aParent)
         : AbstractTableModel(aParent)
      {
      }
      ~EngagementTableModel() override = default;
      int rowCount(const QModelIndex&) const override { return (QModelIndex().isValid()) ? 0 : (int)mData.size(); };
      QVariant data(const QModelIndex& aIndex, int aRole) const override;

      void AddWeaponFire(float              aSimTime,
                         unsigned int       aFiringPlatformId,
                         unsigned int       aTargetPlatformId,
                         const std::string& aIntendedTargetName,
                         unsigned int       aWeaponPlatformIndex,
                         int                aLocalTrackNumber,
                         const std::string& aLocalTrackOwner,
                         const QString&     aAttackName,
                         const QString&     aTargetName,
                         const QString&     aWeaponName);

      void AddWeaponTerminate(float              aSimTime,
                              unsigned int       aFiringPlatformId,
                              unsigned int       aTargetPlatformId,
                              const std::string& aIntendedTargetName,
                              unsigned int       aWeaponPlatformIndex,
                              char               aResult,
                              const std::string& aExtendedResult,
                              float              aMissDistance,
                              const QString&     aAttackName,
                              const QString&     aTargetName,
                              const QString&     aWeaponName);
      void Clear();

   private:
      QString BuildPlatformName(const QString& aName, unsigned int aPlatformId);
      struct Engagement
      {
         float    time;
         float    endTime;
         QString  attacker;
         unsigned attackerId;
         QString  target;
         unsigned targetId;
         QString  intendedTarget;
         QString  weapon;
         unsigned weaponId;
         QString  trackOwner;
         int      trackId;
         float    missDist;
         QString  result;
         QString  extResult;
      };
      std::map<int, Engagement> mData;
      std::set<int>             mUnpairedRows;
   };
   bool                       MatchGrouping(unsigned int aPlatformId, const PlatformGrouping& aGrouping);
   Ui_WkfEngagementStatistics mUi;
   wkf::PlatformGrouping      mAttacker;
   wkf::PlatformGrouping      mTarget;

   int                   mPlotMode;
   EngagementTableModel* mItemModel;
};
} // namespace wkf
#endif
