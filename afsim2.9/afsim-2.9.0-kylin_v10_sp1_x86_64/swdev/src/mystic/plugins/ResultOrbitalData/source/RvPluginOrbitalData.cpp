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

#include "RvPluginOrbitalData.hpp"

#include "RvOrbitalDataInterface.hpp"
#include "RvOrbitalDataPlotUpdaters.hpp"
#include "RvOrbitalDataUpdaters.hpp"
#include "WkfUnitTypes.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvOrbitalData::Plugin,
                          "Orbital Data",
                          "Adds orbital elements for platforms with a space mover to the Platform Data widget.",
                          "mystic",
                          true)

RvOrbitalData::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mInterfacePtr(new Interface(this))
   , mUpdatePosted(true)
{
   // Create the string mappings
   mDataToStringMap[cECCENTRICITY]     = "Eccentricity";
   mDataToStringMap[cSEMI_MAJOR_AXIS]  = "Semi-major Axis";
   mDataToStringMap[cINCLINATION]      = "Inclination";
   mDataToStringMap[cRAAN]             = "RAAN";
   mDataToStringMap[cARG_OF_PERIAPSIS] = "Argument of Periapsis";
   mDataToStringMap[cTRUE_ANOMALY]     = "True Anomaly";

   mOrbitalDataItemPtr = new QTreeWidgetItem();
   mOrbitalDataItemPtr->setText(0, "Orbital Elements");
   mTopLevelWidgets.push_back(mOrbitalDataItemPtr);

   CreateItem(cECCENTRICITY);
   CreateItem(cSEMI_MAJOR_AXIS);
   CreateItem(cINCLINATION);
   CreateItem(cRAAN);
   CreateItem(cARG_OF_PERIAPSIS);
   CreateItem(cTRUE_ANOMALY);
}

/** Get the preferences widget. */
QList<wkf::PrefWidget*> RvOrbitalData::Plugin::GetPreferencesWidgets() const
{
   return QList<wkf::PrefWidget*>();
}

/** Get a list of tree widgets for a platform.
 * @param aPlatformName platform name
 */
QList<QTreeWidgetItem*> RvOrbitalData::Plugin::GetPlatformData(const std::string& aPlatformName)
{
   mInterfacePtr->SetPlatformOfInterest(aPlatformName);
   mUpdatePosted = true;
   return mTopLevelWidgets;
}

/** Advance time and read data. */
void RvOrbitalData::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   mInterfacePtr->AdvanceTimeRead(aData);
   mUpdatePosted = true;
}

/** Read data. */
void RvOrbitalData::Plugin::RegularRead(const rv::ResultData& aData)
{
   mInterfacePtr->AdvanceTimeRead(aData);
   mUpdatePosted = true;
}

/** Get an updater. */
std::unique_ptr<wkf::Updater> RvOrbitalData::Plugin::GetUpdater(const QString&     aPlatformName,
                                                                const QString&     aDatum,
                                                                const unsigned int aContext) const
{
   if (aDatum == mDataToStringMap.at(cECCENTRICITY))
   {
      return wkf::make_updater<EccentricityUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(cSEMI_MAJOR_AXIS))
   {
      return wkf::make_updater<SemiMajorAxisUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(cINCLINATION))
   {
      return wkf::make_updater<InclinationUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(cRAAN))
   {
      return wkf::make_updater<RAAN_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(cARG_OF_PERIAPSIS))
   {
      return wkf::make_updater<ArgOfPeriapsisUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(cTRUE_ANOMALY))
   {
      return wkf::make_updater<TrueAnomalyUpdater>(aPlatformName.toStdString());
   }
   return nullptr;
}

std::unique_ptr<rv::PlotUpdater> RvOrbitalData::Plugin::GetPlotUpdater(const QString&               aX,
                                                                       const QString&               aY,
                                                                       const wkf::PlatformGrouping& aGrouping,
                                                                       const QString&               aSubcategory) const
{
   return ut::make_unique<PlotUpdater>(aX, aY, aGrouping);
}


/** Update the GUI. */
void RvOrbitalData::Plugin::GuiUpdate()
{
   if (mUpdatePosted)
   {
      auto data = mInterfacePtr->GetOrbitalData();

      if (mInterfacePtr->IsOrbitalVisible() && data.Valid())
      {
         mTopLevelWidgets.first()->setHidden(false);
         mOrbitalDataItemPtr->setHidden(false);
         mUnitDataWidgets[cECCENTRICITY]->SetValue(data.mEccentricity);
         mUnitDataWidgets[cSEMI_MAJOR_AXIS]->SetValue(data.mSemiMajorAxis);
         mUnitDataWidgets[cINCLINATION]->SetValue(data.mInclination);
         mUnitDataWidgets[cRAAN]->SetValue(data.mRAAN);
         mUnitDataWidgets[cARG_OF_PERIAPSIS]->SetValue(data.mArgOfPeriapsis);
         mUnitDataWidgets[cTRUE_ANOMALY]->SetValue(data.mTrueAnomaly);
      }
      else
      {
         mTopLevelWidgets.first()->setHidden(true);
      }

      mUpdatePosted = false;
   }
}

/** Create a tree widget item. */
void RvOrbitalData::Plugin::CreateItem(OrbitalDataType aType)
{
   switch (aType)
   {
   case cECCENTRICITY:
      mUnitDataWidgets[aType] = new wkf::UnitlessTreeWidgetItem(nullptr, mDataToStringMap[aType]);
      break;
   case cSEMI_MAJOR_AXIS:
      mUnitDataWidgets[aType] = new wkf::LengthTreeWidgetItem(nullptr, mDataToStringMap[aType]);
      break;
   case cINCLINATION:
   case cRAAN:
   case cARG_OF_PERIAPSIS:
   case cTRUE_ANOMALY:
      mUnitDataWidgets[aType] = new wkf::AngleTreeWidgetItem(nullptr, mDataToStringMap[aType]);
      break;
   default:
      break;
   }

   mUnitDataWidgets[aType]->setData(0, wkf::UPDATER_ROLE, true);
   mOrbitalDataItemPtr->addChild(mUnitDataWidgets[aType]);
}
