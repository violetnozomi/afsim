//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#ifndef RVPLUGINORBITALELEMENTDATA_HPP
#define RVPLUGINORBITALELEMENTDATA_HPP

#include "RvPlugin.hpp"

namespace wkf
{
class UnitTreeWidgetItem;
}

namespace RvOrbitalData
{
class Interface;

class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   /** Get the preferences widget. */
   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

   /** Get a list of tree widgets for a platform.
    * @param aPlatformName platform name
    */
   QList<QTreeWidgetItem*> GetPlatformData(const std::string& aPlatformName) override;

   /** Get a plot updater.
    * @param aX x axis
    * @param aY y axis
    * @param aGrouping grouping of platforms
    * @param aSubcategory subcategory
    */
   std::unique_ptr<rv::PlotUpdater> GetPlotUpdater(const QString&               aX,
                                                   const QString&               aY,
                                                   const wkf::PlatformGrouping& aGrouping,
                                                   const QString&               aSubcategory) const override;

   /** Get an updater.
    * @param aPlatformName platform name
    * @param aDatum description/label of updater
    * @param aContext context
    */
   std::unique_ptr<wkf::Updater> GetUpdater(const QString&     aPlatformName,
                                            const QString&     aDatum,
                                            const unsigned int aContext = 0) const override;

private slots:
   /** Advance time and read data.
    * @param aData data to read from
    */
   void AdvanceTimeRead(const rv::ResultData& aData) override;

   /** Read data.
    * @param aData data to read from
    */
   void RegularRead(const rv::ResultData& aData) override;

   /** Update the GUI. */
   void GuiUpdate() override;

private:
   /** This enum handles all orbital data types */
   enum OrbitalDataType
   {
      cECCENTRICITY,
      cSEMI_MAJOR_AXIS,
      cINCLINATION,
      cRAAN,
      cARG_OF_PERIAPSIS,
      cTRUE_ANOMALY
   };

   /** Create a tree widget item.
    * @param aType type of widget to make.
    */
   void CreateItem(OrbitalDataType aType);

   std::map<OrbitalDataType, QString>                  mDataToStringMap;
   std::map<OrbitalDataType, wkf::UnitTreeWidgetItem*> mUnitDataWidgets;
   QList<QTreeWidgetItem*>                             mTopLevelWidgets;
   QTreeWidgetItem*                                    mOrbitalDataItemPtr;
   Interface*                                          mInterfacePtr;
   bool                                                mUpdatePosted;
};
} // namespace RvOrbitalData
#endif
