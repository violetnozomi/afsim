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
#ifndef RVPLUGINPLATFORMDATA_HPP
#define RVPLUGINPLATFORMDATA_HPP

#include "RvPlatformDataInterface.hpp"
#include "RvPlugin.hpp"
#include "WkfPlatformDataTreeItem.hpp"

namespace wkf
{
class PlatformGrouping;
class UnitTreeWidgetItem;
} // namespace wkf

namespace RvPlatformData
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) override;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;
   QList<QTreeWidgetItem*> GetPlatformData(const std::string& aPlatformName) override;


   std::unique_ptr<rv::PlotUpdater> GetPlotUpdater(const QString&               aX,
                                                   const QString&               aY,
                                                   const wkf::PlatformGrouping& aGrouping,
                                                   const QString&               aSubcategory) const override;
   void                             MenuPlot();

   std::unique_ptr<wkf::Updater> GetUpdater(const QString&     aPlatformName,
                                            const QString&     aDatum,
                                            const unsigned int aContext = 0) const override;

   void           ResetOptionStates() override;
   void           SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatform) override;
   Qt::CheckState GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatform) const override;

private slots:
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void RegularRead(const rv::ResultData& aData) override;
   void GuiUpdate() override;
   void CheckDataCompatability(Interface::PlatformData& aData);

private:
   // managed by Qt
   Interface* mInterfacePtr;

   enum ePlatformData
   {
      eTYPE,
      eSIDE,
      eICON,
      eSPATIAL_DOMAIN,
      eSTATUS,
      eSTART_TIME,
      eEND_TIME,
      eDAMAGE_FACTOR,
      eFUEL_CAPACITY,
      eFUEL_CURRENT,
      eLATITUDE,
      eLONGITUDE,
      eALTITUDE_MSL,
      eX,
      eY,
      eZ,
      eYAW,
      ePITCH,
      eROLL,
      ePSI,
      eTHETA,
      ePHI,
      eSPEED,
      eMACH,
      eVEL_N,
      eVEL_E,
      eVEL_D,
      eVEL_X,
      eVEL_Y,
      eVEL_Z,
      eACCELERATION,
      eACCEL_N,
      eACCEL_E,
      eACCEL_D,
      eACCEL_X,
      eACCEL_Y,
      eACCEL_Z
   };
   void CreateItem(QTreeWidgetItem* parent, ePlatformData aType);

   std::map<ePlatformData, QString> mDataToStringMap;

   QList<QTreeWidgetItem*>                             mTopLevelWidgets;
   std::map<ePlatformData, wkf::PlatformDataTreeItem*> mStringDataWidgets;
   std::map<ePlatformData, wkf::UnitTreeWidgetItem*>   mUnitDataWidgets;
   wkf::PlatformDataTreeItem*                          mPartsItemPtr;
   wkf::PlatformDataTreeItem*                          mStateItemPtr;
   wkf::PlatformDataTreeItem*                          mCategoriesItemPtr;
   bool                                                mUpdatePosted{true};

   int mDataRingOption;
};
} // namespace RvPlatformData
#endif
