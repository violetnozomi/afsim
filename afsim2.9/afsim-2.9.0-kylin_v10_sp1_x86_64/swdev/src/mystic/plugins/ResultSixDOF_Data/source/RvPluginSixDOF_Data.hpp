// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVPLUGINSIXDOFDATA_HPP
#define RVPLUGINSIXDOFDATA_HPP

#include "RvPlugin.hpp"

namespace wkf
{
class PlatformGrouping;
class UnitTreeWidgetItem;
} // namespace wkf

namespace RvSixDOF_Data
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
   /** Triggers when the platform of interest has changed.
    * @param aPlatformPtr pointer to the new platform of interest.
    */
   void PlatformOfInterestChanged(wkf::Platform* aPlatformPtr);

   // Pointer to the interface
   Interface* mInterfacePtr;

   /** This enum handles all SixDOF message types*/
   enum eSixDOF_DataType
   {
      // MsgSixDOF_CoreData
      eALTITUDE,
      eVERT_SPEED_FPM,
      eKCAS,
      eKTAS,
      eMACH,
      eHEADING,
      ePITCH,
      eROLL,
      eGLOAD,
      eALPHA,
      eG_AVAIL,
      eSPEEDBRAKE_ANGLE,

      // MsgSixDOF_Kinematic
      eBETA,
      eALPHA_DOT,
      eBETA_DOT,
      eYAW_RATE,
      ePITCH_RATE,
      eROLL_RATE,
      eDYNAMIC_PRESSURE,
      eNX,
      eNY,
      eFLIGHT_PATH_ANGLE,
      eCENTER_GRAVITY_X,
      eCENTER_GRAVITY_Y,
      eCENTER_GRAVITY_Z,

      // MsgSixDOF_EngineFuel
      eFUELFLOW,
      eFUEL_INTERNAL,
      eFUEL_EXTERNAL,
      eFUEL_INTERNAL_REMAINING,
      eFUEL_EXTERNAL_REMAINING,
      eTOTAL_WEIGHT,
      eAFTERBURNER,
      ePRODUCING_THRUST,
      eENGINE_OPERATING,
      eENGINE_SMOKING,
      eCONTRAILING,
      eJOKER_FUEL_STATE,
      eBINGO_FUEL_STATE,
      eWEIGHT_ON_WHEELS,

      // MsgSixDOF_Autopilot
      eACTIVE_PILOT,
      eLATERAL_MODE_NAME,
      eLATERAL_MODE_VALUE,
      eVERTICAL_MODE_NAME,
      eVERTICAL_MODE_VALUE,
      eSPEED_MODE_NAME,
      eSPEED_MODE_VALUE,
      eCURRENT_MANEUVER,

      // MsgSixDOF_AutopilotLimits
      ePITCH_GLOAD_MIN,
      ePITCH_GLOAD_MAX,
      eALPHA_MIN,
      eALPHA_MAX,
      ePITCH_RATE_MIN,
      ePITCH_RATE_MAX,
      eVERTICAL_SPEED_MIN,
      eVERTICAL_SPEED_MAX,
      eYAW_GLOAD_MAX,
      eBETA_MAX,
      eYAW_RATE_MAX,
      eROLL_RATE_MAX,
      eBANK_ANGLE_MAX,
      eFORWARD_ACCEL_MIN,
      eFORWARD_ACCEL_MAX,
      eTAXI_SPEED_MAX,
      eTAXI_YAW_RATE_MAX,

      // MsgSixDOF_ControlInputs
      eSTICK_BACK,
      eSTICK_RIGHT,
      eRUDDER_RIGHT,
      eTHROTTLE,
      eSPEED_BRAKE_LEVER,
      eFLAPS_LEVER,
      eLANDING_GEAR_LEVER,
      eSPOILERS_LEVER,
      eTHRUST_REVERSER_LEVER,

      // MsgSixDOF_ForceMoment
      eLIFT,
      eDRAG,
      eTHRUST,
      eSIDEFORCE,

      eYAW_MOMENT,
      ePITCH_MOMENT,
      eROLL_MOMENT
   };

   /** Create a tree widget item.
    * @param aParent parent of the new widget
    * @param aType type of widget to make.
    */
   void CreateItem(QTreeWidgetItem* aParent, eSixDOF_DataType aType);

   std::map<eSixDOF_DataType, QString>                  mDataToStringMap;
   QList<QTreeWidgetItem*>                              mTopLevelWidgets;
   std::map<eSixDOF_DataType, QTreeWidgetItem*>         mStringDataWidgets;
   std::map<eSixDOF_DataType, wkf::UnitTreeWidgetItem*> mUnitDataWidgets;
   std::map<std::string, wkf::UnitTreeWidgetItem*>      mControlSurfaceWidgets;

   // Pointers to widget items for setting visibility
   QTreeWidgetItem* mCoreDataItemPtr;
   QTreeWidgetItem* mKinematicItemPtr;
   QTreeWidgetItem* mEngineFuelItemPtr;
   QTreeWidgetItem* mAutopilotItemPtr;
   QTreeWidgetItem* mAutopilotLimitsItemPtr;
   QTreeWidgetItem* mControlInputsItemPtr;
   QTreeWidgetItem* mControlSurfacesItemPtr;
   QTreeWidgetItem* mForceMomentItemPtr;
   bool             mUpdatePosted{true};
};
} // namespace RvSixDOF_Data
#endif
