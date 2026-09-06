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
#include "RvPluginSixDOF_Data.hpp"

#include <memory>

#include <QMenu>

#include "RvPlottingWidget.hpp"
#include "RvSixDOF_DataInterface.hpp"
#include "RvSixDOF_DataPlotUpdater.hpp"
#include "RvSixDOF_DataUpdater.hpp"
#include "UtMemory.hpp"
#include "WkfAction.hpp"
#include "WkfDefines.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfUnitTypes.hpp"
#include "WkfVtkEnvironment.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvSixDOF_Data::Plugin,
                          "SixDOF Data",
                          "The SixDOF Data plugin is responsible for retrieving and displaying data from a SixDOF "
                          "Mover in the Platform Details dialog.",
                          "mystic")

RvSixDOF_Data::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mInterfacePtr(new Interface(this))
{
   connect(&wkfEnv, &wkf::Environment::PlatformOfInterestChanged, this, &Plugin::PlatformOfInterestChanged);
   // Create the string mappings
   // MsgSixDOF_CoreData
   mDataToStringMap[eALTITUDE]         = "Altitude (MSL)";
   mDataToStringMap[eVERT_SPEED_FPM]   = "Vertical Speed (FPM)";
   mDataToStringMap[eKCAS]             = "Airspeed, Knots Calibrated (KCAS)";
   mDataToStringMap[eKTAS]             = "Airspeed, Knots True (KTAS)";
   mDataToStringMap[eMACH]             = "Airspeed, Mach";
   mDataToStringMap[eHEADING]          = "Local Heading";
   mDataToStringMap[ePITCH]            = "Local Pitch";
   mDataToStringMap[eROLL]             = "Local Roll";
   mDataToStringMap[eGLOAD]            = "Body Up Accel (Gs), G-Load";
   mDataToStringMap[eALPHA]            = "Angle of Attack, Alpha";
   mDataToStringMap[eG_AVAIL]          = "Maneuver Gs, G-Available";
   mDataToStringMap[eSPEEDBRAKE_ANGLE] = "Speed Brake Surface Angle";

   // MsgSixDOF_Kinematic
   mDataToStringMap[eBETA]              = "Angle of Sideslip, Beta";
   mDataToStringMap[eALPHA_DOT]         = "Angle of Attack Rate, AlphaDot";
   mDataToStringMap[eBETA_DOT]          = "Angle of Sideslip Rate, BetaDot";
   mDataToStringMap[eYAW_RATE]          = "Body Yaw Rate, r";
   mDataToStringMap[ePITCH_RATE]        = "Body Pitch Rate, q";
   mDataToStringMap[eROLL_RATE]         = "Body Roll Rate, p";
   mDataToStringMap[eDYNAMIC_PRESSURE]  = "Dynamic Pressure, Q";
   mDataToStringMap[eNX]                = "Body Forward Accel (Gs), Nx";
   mDataToStringMap[eNY]                = "Body Right Accel (Gs), Ny";
   mDataToStringMap[eFLIGHT_PATH_ANGLE] = "Flight Path Angle, Gamma";
   mDataToStringMap[eCENTER_GRAVITY_X]  = "Center of Gravity - X, CGx";
   mDataToStringMap[eCENTER_GRAVITY_Y]  = "Center of Gravity - Y, CGy";
   mDataToStringMap[eCENTER_GRAVITY_Z]  = "Center of Gravity - Z, CGz";

   // MsgSixDOF_EngineFuel
   mDataToStringMap[eFUELFLOW]                = "Fuel Flow (PPH)";
   mDataToStringMap[eFUEL_INTERNAL]           = "Internal Fuel Capacity";
   mDataToStringMap[eFUEL_EXTERNAL]           = "External Fuel Capacity";
   mDataToStringMap[eFUEL_INTERNAL_REMAINING] = "Internal Fuel Remaining";
   mDataToStringMap[eFUEL_EXTERNAL_REMAINING] = "External Fuel Remaining";
   mDataToStringMap[eTOTAL_WEIGHT]            = "Current Total Weight/Mass";
   mDataToStringMap[eAFTERBURNER]             = "Afterburner On";
   mDataToStringMap[ePRODUCING_THRUST]        = "Producing Thrust";
   mDataToStringMap[eENGINE_OPERATING]        = "Engine Operating";
   mDataToStringMap[eENGINE_SMOKING]          = "Engine Smoking";
   mDataToStringMap[eCONTRAILING]             = "Contrailing";
   mDataToStringMap[eJOKER_FUEL_STATE]        = "Joker Fuel State";
   mDataToStringMap[eBINGO_FUEL_STATE]        = "Bingo Fuel State";
   mDataToStringMap[eWEIGHT_ON_WHEELS]        = "Weight on Wheels";

   // MsgSixDOF_Autopilot
   mDataToStringMap[eACTIVE_PILOT]        = "Active Pilot";
   mDataToStringMap[eLATERAL_MODE_NAME]   = "Lateral Mode Name";
   mDataToStringMap[eLATERAL_MODE_VALUE]  = "Lateral Mode Value";
   mDataToStringMap[eVERTICAL_MODE_NAME]  = "Vertical Mode Name";
   mDataToStringMap[eVERTICAL_MODE_VALUE] = "Vertical Mode Value";
   mDataToStringMap[eSPEED_MODE_NAME]     = "Speed Mode Name";
   mDataToStringMap[eSPEED_MODE_VALUE]    = "Speed Mode Value";
   mDataToStringMap[eCURRENT_MANEUVER]    = "Current Maneuver";

   // MsgSixDOF_AutopilotLimits
   mDataToStringMap[ePITCH_GLOAD_MIN]    = "Pitch G-Load Min (Gs)";
   mDataToStringMap[ePITCH_GLOAD_MAX]    = "Pitch G-Load Max (Gs)";
   mDataToStringMap[eALPHA_MIN]          = "Alpha Min";
   mDataToStringMap[eALPHA_MAX]          = "Alpha Max";
   mDataToStringMap[ePITCH_RATE_MIN]     = "Pitch Rate Min";
   mDataToStringMap[ePITCH_RATE_MAX]     = "Pitch Rate Max";
   mDataToStringMap[eVERTICAL_SPEED_MIN] = "Vertical Speed Min (FPM)";
   mDataToStringMap[eVERTICAL_SPEED_MAX] = "Vertical Speed Max (FPM)";
   mDataToStringMap[eYAW_GLOAD_MAX]      = "Yaw G-Load Max (Gs)";
   mDataToStringMap[eBETA_MAX]           = "Beta Max";
   mDataToStringMap[eYAW_RATE_MAX]       = "Yaw Rate Max";
   mDataToStringMap[eROLL_RATE_MAX]      = "Roll Rate Max";
   mDataToStringMap[eBANK_ANGLE_MAX]     = "Bank Angle Max";
   mDataToStringMap[eFORWARD_ACCEL_MIN]  = "Forward Accel Min (Gs)";
   mDataToStringMap[eFORWARD_ACCEL_MAX]  = "Forward Accel Max (Gs)";
   mDataToStringMap[eTAXI_SPEED_MAX]     = "Taxi Speed Max";
   mDataToStringMap[eTAXI_YAW_RATE_MAX]  = "Taxi Yaw Rate Max";

   // MsgSixDOF_ControlInputs
   mDataToStringMap[eSTICK_BACK]            = "Stick Control Back";
   mDataToStringMap[eSTICK_RIGHT]           = "Stick Control Right";
   mDataToStringMap[eRUDDER_RIGHT]          = "Rudder Control Right";
   mDataToStringMap[eTHROTTLE]              = "Throttle Lever (0-2)";
   mDataToStringMap[eSPEED_BRAKE_LEVER]     = "Speed Brake Lever";
   mDataToStringMap[eFLAPS_LEVER]           = "Flaps Lever";
   mDataToStringMap[eLANDING_GEAR_LEVER]    = "Landing Gear Lever";
   mDataToStringMap[eSPOILERS_LEVER]        = "Spoilers Lever";
   mDataToStringMap[eTHRUST_REVERSER_LEVER] = "Thrust Reverser Lever";

   // MsgSixDOF_ForceMoment
   mDataToStringMap[eLIFT]         = "Lift Force";
   mDataToStringMap[eDRAG]         = "Drag Force";
   mDataToStringMap[eTHRUST]       = "Thrust Force";
   mDataToStringMap[eSIDEFORCE]    = "Side Force";
   mDataToStringMap[eYAW_MOMENT]   = "Yaw Moment";
   mDataToStringMap[ePITCH_MOMENT] = "Pitch Moment";
   mDataToStringMap[eROLL_MOMENT]  = "Roll Moment";

   // Create the widget trees
   QTreeWidgetItem* sixDofParent = new QTreeWidgetItem();
   sixDofParent->setText(0, "SixDOF");
   mTopLevelWidgets.push_back(sixDofParent);

   // MsgSixDOF_CoreData
   mCoreDataItemPtr = new QTreeWidgetItem(sixDofParent);
   mCoreDataItemPtr->setText(0, "Core");
   CreateItem(mCoreDataItemPtr, eALTITUDE);
   CreateItem(mCoreDataItemPtr, eVERT_SPEED_FPM);
   CreateItem(mCoreDataItemPtr, eKCAS);
   CreateItem(mCoreDataItemPtr, eKTAS);
   CreateItem(mCoreDataItemPtr, eMACH);
   CreateItem(mCoreDataItemPtr, eHEADING);
   CreateItem(mCoreDataItemPtr, ePITCH);
   CreateItem(mCoreDataItemPtr, eROLL);
   CreateItem(mCoreDataItemPtr, eGLOAD);
   CreateItem(mCoreDataItemPtr, eALPHA);
   CreateItem(mCoreDataItemPtr, eG_AVAIL);
   CreateItem(mCoreDataItemPtr, eSPEEDBRAKE_ANGLE);

   // MsgSixDOF_Kinematic
   mKinematicItemPtr = new QTreeWidgetItem(sixDofParent);
   mKinematicItemPtr->setText(0, "Kinematic");
   CreateItem(mKinematicItemPtr, eBETA);
   CreateItem(mKinematicItemPtr, eALPHA_DOT);
   CreateItem(mKinematicItemPtr, eBETA_DOT);
   CreateItem(mKinematicItemPtr, eYAW_RATE);
   CreateItem(mKinematicItemPtr, ePITCH_RATE);
   CreateItem(mKinematicItemPtr, eROLL_RATE);
   CreateItem(mKinematicItemPtr, eDYNAMIC_PRESSURE);
   CreateItem(mKinematicItemPtr, eNX);
   CreateItem(mKinematicItemPtr, eNY);
   CreateItem(mKinematicItemPtr, eFLIGHT_PATH_ANGLE);
   CreateItem(mKinematicItemPtr, eCENTER_GRAVITY_X);
   CreateItem(mKinematicItemPtr, eCENTER_GRAVITY_Y);
   CreateItem(mKinematicItemPtr, eCENTER_GRAVITY_Z);

   // MsgSixDOF_EngineFuel
   mEngineFuelItemPtr = new QTreeWidgetItem(sixDofParent);
   mEngineFuelItemPtr->setText(0, "Engine and Fuel");
   CreateItem(mEngineFuelItemPtr, eFUELFLOW);
   CreateItem(mEngineFuelItemPtr, eFUEL_INTERNAL);
   CreateItem(mEngineFuelItemPtr, eFUEL_EXTERNAL);
   CreateItem(mEngineFuelItemPtr, eFUEL_INTERNAL_REMAINING);
   CreateItem(mEngineFuelItemPtr, eFUEL_EXTERNAL_REMAINING);
   CreateItem(mEngineFuelItemPtr, eTOTAL_WEIGHT);
   CreateItem(mEngineFuelItemPtr, eAFTERBURNER);
   CreateItem(mEngineFuelItemPtr, ePRODUCING_THRUST);
   CreateItem(mEngineFuelItemPtr, eENGINE_OPERATING);
   CreateItem(mEngineFuelItemPtr, eENGINE_SMOKING);
   CreateItem(mEngineFuelItemPtr, eCONTRAILING);
   CreateItem(mEngineFuelItemPtr, eJOKER_FUEL_STATE);
   CreateItem(mEngineFuelItemPtr, eBINGO_FUEL_STATE);
   CreateItem(mEngineFuelItemPtr, eWEIGHT_ON_WHEELS);

   // MsgSixDOF_Autopilot
   mAutopilotItemPtr = new QTreeWidgetItem(sixDofParent);
   mAutopilotItemPtr->setText(0, "Autopilot");
   CreateItem(mAutopilotItemPtr, eACTIVE_PILOT);
   CreateItem(mAutopilotItemPtr, eLATERAL_MODE_NAME);
   CreateItem(mAutopilotItemPtr, eLATERAL_MODE_VALUE);
   CreateItem(mAutopilotItemPtr, eVERTICAL_MODE_NAME);
   CreateItem(mAutopilotItemPtr, eVERTICAL_MODE_VALUE);
   CreateItem(mAutopilotItemPtr, eSPEED_MODE_NAME);
   CreateItem(mAutopilotItemPtr, eSPEED_MODE_VALUE);
   CreateItem(mAutopilotItemPtr, eCURRENT_MANEUVER);

   // MsgSixDOF_AutopilotLimits
   mAutopilotLimitsItemPtr = new QTreeWidgetItem(sixDofParent);
   mAutopilotLimitsItemPtr->setText(0, "Autopilot Limits");
   CreateItem(mAutopilotLimitsItemPtr, ePITCH_GLOAD_MIN);
   CreateItem(mAutopilotLimitsItemPtr, ePITCH_GLOAD_MAX);
   CreateItem(mAutopilotLimitsItemPtr, eALPHA_MIN);
   CreateItem(mAutopilotLimitsItemPtr, eALPHA_MAX);
   CreateItem(mAutopilotLimitsItemPtr, ePITCH_RATE_MIN);
   CreateItem(mAutopilotLimitsItemPtr, ePITCH_RATE_MAX);
   CreateItem(mAutopilotLimitsItemPtr, eVERTICAL_SPEED_MIN);
   CreateItem(mAutopilotLimitsItemPtr, eVERTICAL_SPEED_MAX);
   CreateItem(mAutopilotLimitsItemPtr, eYAW_GLOAD_MAX);
   CreateItem(mAutopilotLimitsItemPtr, eBETA_MAX);
   CreateItem(mAutopilotLimitsItemPtr, eYAW_RATE_MAX);
   CreateItem(mAutopilotLimitsItemPtr, eROLL_RATE_MAX);
   CreateItem(mAutopilotLimitsItemPtr, eBANK_ANGLE_MAX);
   CreateItem(mAutopilotLimitsItemPtr, eFORWARD_ACCEL_MIN);
   CreateItem(mAutopilotLimitsItemPtr, eFORWARD_ACCEL_MAX);
   CreateItem(mAutopilotLimitsItemPtr, eTAXI_SPEED_MAX);
   CreateItem(mAutopilotLimitsItemPtr, eTAXI_YAW_RATE_MAX);

   // MsgSixDOF_ControlInputs
   mControlInputsItemPtr = new QTreeWidgetItem(sixDofParent);
   mControlInputsItemPtr->setText(0, "Control Inputs");
   CreateItem(mControlInputsItemPtr, eSTICK_BACK);
   CreateItem(mControlInputsItemPtr, eSTICK_RIGHT);
   CreateItem(mControlInputsItemPtr, eRUDDER_RIGHT);
   CreateItem(mControlInputsItemPtr, eTHROTTLE);
   CreateItem(mControlInputsItemPtr, eSPEED_BRAKE_LEVER);
   CreateItem(mControlInputsItemPtr, eFLAPS_LEVER);
   CreateItem(mControlInputsItemPtr, eLANDING_GEAR_LEVER);
   CreateItem(mControlInputsItemPtr, eSPOILERS_LEVER);
   CreateItem(mControlInputsItemPtr, eTHRUST_REVERSER_LEVER);

   // MsgSixDOF_ControlSurfaces
   mControlSurfacesItemPtr = new QTreeWidgetItem(sixDofParent);
   mControlSurfacesItemPtr->setText(0, "Control Surfaces");

   mForceMomentItemPtr = new QTreeWidgetItem(sixDofParent);
   mForceMomentItemPtr->setText(0, "Force and Moment (Rigid-Body)");
   CreateItem(mForceMomentItemPtr, eLIFT);
   CreateItem(mForceMomentItemPtr, eDRAG);
   CreateItem(mForceMomentItemPtr, eTHRUST);
   CreateItem(mForceMomentItemPtr, eSIDEFORCE);
   CreateItem(mForceMomentItemPtr, eYAW_MOMENT);
   CreateItem(mForceMomentItemPtr, ePITCH_MOMENT);
   CreateItem(mForceMomentItemPtr, eROLL_MOMENT);

   // Register platform updaters with Environment
   // MsgSixDOF_CoreData
   wkfEnv.RegisterUpdater<AltitudeUpdater>(*this);
   wkfEnv.RegisterUpdater<VerticalSpeedUpdater>(*this);
   wkfEnv.RegisterUpdater<KCAS_Updater>(*this);
   wkfEnv.RegisterUpdater<KTAS_Updater>(*this);
   wkfEnv.RegisterUpdater<MachUpdater>(*this);
   wkfEnv.RegisterUpdater<HeadingUpdater>(*this);
   wkfEnv.RegisterUpdater<PitchUpdater>(*this);
   wkfEnv.RegisterUpdater<RollUpdater>(*this);
   wkfEnv.RegisterUpdater<GLoadUpdater>(*this);
   wkfEnv.RegisterUpdater<AlphaUpdater>(*this);
   wkfEnv.RegisterUpdater<AvailGsUpdater>(*this);
   wkfEnv.RegisterUpdater<SpeedBrakeAngleUpdater>(*this);

   // MsgSixDOF_Kinematic
   wkfEnv.RegisterUpdater<BetaUpdater>(*this);
   wkfEnv.RegisterUpdater<AlphaDotUpdater>(*this);
   wkfEnv.RegisterUpdater<BetaDotUpdater>(*this);
   wkfEnv.RegisterUpdater<YawRateUpdater>(*this);
   wkfEnv.RegisterUpdater<PitchRateUpdater>(*this);
   wkfEnv.RegisterUpdater<RollRateUpdater>(*this);
   wkfEnv.RegisterUpdater<DynamicPressureUpdater>(*this);
   wkfEnv.RegisterUpdater<NxUpdater>(*this);
   wkfEnv.RegisterUpdater<NyUpdater>(*this);
   wkfEnv.RegisterUpdater<FlightPathAngleUpdater>(*this);
   wkfEnv.RegisterUpdater<CenterGravityXUpdater>(*this);
   wkfEnv.RegisterUpdater<CenterGravityYUpdater>(*this);
   wkfEnv.RegisterUpdater<CenterGravityZUpdater>(*this);

   // MsgSixDOF_EngineFuel
   wkfEnv.RegisterUpdater<FuelFlowUpdater>(*this);
   wkfEnv.RegisterUpdater<FuelInternalUpdater>(*this);
   wkfEnv.RegisterUpdater<FuelExternalUpdater>(*this);
   wkfEnv.RegisterUpdater<FuelInternalRemainingUpdater>(*this);
   wkfEnv.RegisterUpdater<FuelExternalRemainingUpdater>(*this);
   wkfEnv.RegisterUpdater<WeightUpdater>(*this);
   wkfEnv.RegisterUpdater<AfterburnerUpdater>(*this);
   wkfEnv.RegisterUpdater<ProducingThrustUpdater>(*this);
   wkfEnv.RegisterUpdater<EngineOperatingUpdater>(*this);
   wkfEnv.RegisterUpdater<EngineSmokingUpdater>(*this);
   wkfEnv.RegisterUpdater<ContrailingUpdater>(*this);
   wkfEnv.RegisterUpdater<JokerFuelStateUpdater>(*this);
   wkfEnv.RegisterUpdater<BingoFuelStateUpdater>(*this);
   wkfEnv.RegisterUpdater<WeightOnWheelsUpdater>(*this);

   // MsgSixDOF_Autopilot
   wkfEnv.RegisterUpdater<ActivePilotUpdater>(*this);
   wkfEnv.RegisterUpdater<LateralModeNameUpdater>(*this);
   wkfEnv.RegisterUpdater<LateralModeValueUpdater>(*this);
   wkfEnv.RegisterUpdater<VerticalModeNameUpdater>(*this);
   wkfEnv.RegisterUpdater<VerticalModeValueUpdater>(*this);
   wkfEnv.RegisterUpdater<SpeedModeNameUpdater>(*this);
   wkfEnv.RegisterUpdater<SpeedModeValueUpdater>(*this);
   wkfEnv.RegisterUpdater<CurrentManeuverUpdater>(*this);

   // MsgSixDOF_AutopilotLimits
   wkfEnv.RegisterUpdater<PitchGLoadMinUpdater>(*this);
   wkfEnv.RegisterUpdater<PitchGLoadMaxUpdater>(*this);
   wkfEnv.RegisterUpdater<AlphaMinUpdater>(*this);
   wkfEnv.RegisterUpdater<AlphaMaxUpdater>(*this);
   wkfEnv.RegisterUpdater<PitchRateMinUpdater>(*this);
   wkfEnv.RegisterUpdater<PitchRateMaxUpdater>(*this);
   wkfEnv.RegisterUpdater<VerticalSpeedMinUpdater>(*this);
   wkfEnv.RegisterUpdater<VerticalSpeedMaxUpdater>(*this);
   wkfEnv.RegisterUpdater<YawGLoadMaxUpdater>(*this);
   wkfEnv.RegisterUpdater<BetaMaxUpdater>(*this);
   wkfEnv.RegisterUpdater<YawRateMaxUpdater>(*this);
   wkfEnv.RegisterUpdater<RollRateMaxUpdater>(*this);
   wkfEnv.RegisterUpdater<BankAngleMaxUpdater>(*this);
   wkfEnv.RegisterUpdater<ForwardAccelMinUpdater>(*this);
   wkfEnv.RegisterUpdater<ForwardAccelMaxUpdater>(*this);
   wkfEnv.RegisterUpdater<TaxiSpeedMaxUpdater>(*this);
   wkfEnv.RegisterUpdater<TaxiYawRateMaxUpdater>(*this);

   // MsgSixDOF_ControlInputs
   wkfEnv.RegisterUpdater<StickBackUpdater>(*this);
   wkfEnv.RegisterUpdater<StickRightUpdater>(*this);
   wkfEnv.RegisterUpdater<RudderUpdater>(*this);
   wkfEnv.RegisterUpdater<ThrottleUpdater>(*this);
   wkfEnv.RegisterUpdater<SpeedBrakeAngleUpdater>(*this);
   wkfEnv.RegisterUpdater<FlapsLeverUpdater>(*this);
   wkfEnv.RegisterUpdater<LandingGearLeverUpdater>(*this);
   wkfEnv.RegisterUpdater<SpoilersLeverUpdater>(*this);
   wkfEnv.RegisterUpdater<ThrustReverserLeverUpdater>(*this);

   // MsgSixDOF_ControlSurfaces
   // wkfEnv.RegisterUpdater<ControlSurfaceAngleUpdater>(*this);

   // MsgSixDOF_ForceMoment
   wkfEnv.RegisterUpdater<LiftUpdater>(*this);
   wkfEnv.RegisterUpdater<DragUpdater>(*this);
   wkfEnv.RegisterUpdater<ThrustUpdater>(*this);
   wkfEnv.RegisterUpdater<SideForceUpdater>(*this);
   wkfEnv.RegisterUpdater<YawMomentUpdater>(*this);
   wkfEnv.RegisterUpdater<PitchMomentUpdater>(*this);
   wkfEnv.RegisterUpdater<RollMomentUpdater>(*this);
}

void RvSixDOF_Data::Plugin::PlatformOfInterestChanged(wkf::Platform* aPlatform)
{
   // Remove children from the tree widget item
   while (mControlSurfacesItemPtr->childCount())
   {
      mControlSurfacesItemPtr->removeChild(mControlSurfacesItemPtr->child(0));
   }
   // Clear the list of children
   mControlSurfaceWidgets.clear();

   // Set update posted to false so we wait for the data container
   // update once before GUIUpdate
   mUpdatePosted = false;
}

/** Get an updater. */
std::unique_ptr<wkf::Updater> RvSixDOF_Data::Plugin::GetUpdater(const QString&     aPlatformName,
                                                                const QString&     aDatum,
                                                                const unsigned int aContext) const
{
   // MsgSixDOF_CoreData
   if (aDatum == mDataToStringMap.at(eALTITUDE))
   {
      return wkf::make_updater<AltitudeUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eVERT_SPEED_FPM))
   {
      return wkf::make_updater<VerticalSpeedUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eKCAS))
   {
      return wkf::make_updater<KCAS_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eKTAS))
   {
      return wkf::make_updater<KTAS_Updater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eMACH))
   {
      return wkf::make_updater<MachUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eHEADING))
   {
      return wkf::make_updater<HeadingUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(ePITCH))
   {
      return wkf::make_updater<PitchUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eROLL))
   {
      return wkf::make_updater<RollUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eGLOAD))
   {
      return wkf::make_updater<GLoadUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eALPHA))
   {
      return wkf::make_updater<AlphaUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eG_AVAIL))
   {
      return wkf::make_updater<AvailGsUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eSPEEDBRAKE_ANGLE))
   {
      return wkf::make_updater<SpeedBrakeAngleUpdater>(aPlatformName.toStdString());
   }
   // MsgSixDOF_Kinematic
   else if (aDatum == mDataToStringMap.at(eBETA))
   {
      return wkf::make_updater<BetaUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eALPHA_DOT))
   {
      return wkf::make_updater<AlphaDotUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eBETA_DOT))
   {
      return wkf::make_updater<BetaDotUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eYAW_RATE))
   {
      return wkf::make_updater<YawRateUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(ePITCH_RATE))
   {
      return wkf::make_updater<PitchRateUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eROLL_RATE))
   {
      return wkf::make_updater<RollRateUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eDYNAMIC_PRESSURE))
   {
      return wkf::make_updater<DynamicPressureUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eNX))
   {
      return wkf::make_updater<NxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eNY))
   {
      return wkf::make_updater<NyUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eFLIGHT_PATH_ANGLE))
   {
      return wkf::make_updater<FlightPathAngleUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eCENTER_GRAVITY_X))
   {
      return wkf::make_updater<CenterGravityXUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eCENTER_GRAVITY_Y))
   {
      return wkf::make_updater<CenterGravityYUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eCENTER_GRAVITY_Z))
   {
      return wkf::make_updater<CenterGravityZUpdater>(aPlatformName.toStdString());
   }
   // MsgSixDOF_EngineFuel
   else if (aDatum == mDataToStringMap.at(eFUELFLOW))
   {
      return wkf::make_updater<FuelFlowUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eFUEL_INTERNAL))
   {
      return wkf::make_updater<FuelInternalUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eFUEL_EXTERNAL))
   {
      return wkf::make_updater<FuelExternalUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eFUEL_INTERNAL_REMAINING))
   {
      return wkf::make_updater<FuelInternalRemainingUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eFUEL_EXTERNAL_REMAINING))
   {
      return wkf::make_updater<FuelExternalRemainingUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eTOTAL_WEIGHT))
   {
      return wkf::make_updater<WeightUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eAFTERBURNER))
   {
      return wkf::make_updater<AfterburnerUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(ePRODUCING_THRUST))
   {
      return wkf::make_updater<ProducingThrustUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eENGINE_OPERATING))
   {
      return wkf::make_updater<EngineOperatingUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eENGINE_SMOKING))
   {
      return wkf::make_updater<EngineSmokingUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eCONTRAILING))
   {
      return wkf::make_updater<ContrailingUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eJOKER_FUEL_STATE))
   {
      return wkf::make_updater<JokerFuelStateUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eBINGO_FUEL_STATE))
   {
      return wkf::make_updater<BingoFuelStateUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eWEIGHT_ON_WHEELS))
   {
      return wkf::make_updater<WeightOnWheelsUpdater>(aPlatformName.toStdString());
   }
   // MsgSixDOF_Autopilot
   else if (aDatum == mDataToStringMap.at(eACTIVE_PILOT))
   {
      return wkf::make_updater<ActivePilotUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eLATERAL_MODE_NAME))
   {
      return wkf::make_updater<LateralModeNameUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eLATERAL_MODE_VALUE))
   {
      return wkf::make_updater<LateralModeValueUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eVERTICAL_MODE_NAME))
   {
      return wkf::make_updater<VerticalModeNameUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eVERTICAL_MODE_VALUE))
   {
      return wkf::make_updater<VerticalModeValueUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eSPEED_MODE_NAME))
   {
      return wkf::make_updater<SpeedModeNameUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eSPEED_MODE_VALUE))
   {
      return wkf::make_updater<SpeedModeValueUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eCURRENT_MANEUVER))
   {
      return wkf::make_updater<CurrentManeuverUpdater>(aPlatformName.toStdString());
   }
   // MsgSixDOF_AutopilotLimits
   else if (aDatum == mDataToStringMap.at(ePITCH_GLOAD_MIN))
   {
      return wkf::make_updater<PitchGLoadMinUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(ePITCH_GLOAD_MAX))
   {
      return wkf::make_updater<PitchGLoadMaxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eALPHA_MIN))
   {
      return wkf::make_updater<AlphaMinUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eALPHA_MAX))
   {
      return wkf::make_updater<AlphaMaxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(ePITCH_RATE_MIN))
   {
      return wkf::make_updater<PitchRateMinUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(ePITCH_RATE_MAX))
   {
      return wkf::make_updater<PitchRateMaxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eVERTICAL_SPEED_MIN))
   {
      return wkf::make_updater<VerticalSpeedMinUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eVERTICAL_SPEED_MAX))
   {
      return wkf::make_updater<VerticalSpeedMaxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eYAW_GLOAD_MAX))
   {
      return wkf::make_updater<YawGLoadMaxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eBETA_MAX))
   {
      return wkf::make_updater<BetaMaxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eYAW_RATE_MAX))
   {
      return wkf::make_updater<YawRateMaxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eROLL_RATE_MAX))
   {
      return wkf::make_updater<RollRateMaxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eBANK_ANGLE_MAX))
   {
      return wkf::make_updater<BankAngleMaxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eFORWARD_ACCEL_MIN))
   {
      return wkf::make_updater<ForwardAccelMinUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eFORWARD_ACCEL_MAX))
   {
      return wkf::make_updater<ForwardAccelMaxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eTAXI_SPEED_MAX))
   {
      return wkf::make_updater<TaxiSpeedMaxUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eTAXI_YAW_RATE_MAX))
   {
      return wkf::make_updater<TaxiYawRateMaxUpdater>(aPlatformName.toStdString());
   }
   // MsgSixDOF_ControlInputs
   else if (aDatum == mDataToStringMap.at(eSTICK_BACK))
   {
      return wkf::make_updater<StickBackUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eSTICK_RIGHT))
   {
      return wkf::make_updater<StickRightUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eRUDDER_RIGHT))
   {
      return wkf::make_updater<RudderUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eTHROTTLE))
   {
      return wkf::make_updater<ThrottleUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eSPEED_BRAKE_LEVER))
   {
      return wkf::make_updater<SpeedBrakeAngleUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eFLAPS_LEVER))
   {
      return wkf::make_updater<FlapsLeverUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eLANDING_GEAR_LEVER))
   {
      return wkf::make_updater<LandingGearLeverUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eSPOILERS_LEVER))
   {
      return wkf::make_updater<SpoilersLeverUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eTHRUST_REVERSER_LEVER))
   {
      return wkf::make_updater<ThrustReverserLeverUpdater>(aPlatformName.toStdString());
   }
   // MsgSixDOF_ForceMoment
   else if (aDatum == mDataToStringMap.at(eLIFT))
   {
      return wkf::make_updater<LiftUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eDRAG))
   {
      return wkf::make_updater<DragUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eTHRUST))
   {
      return wkf::make_updater<ThrustUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eSIDEFORCE))
   {
      return wkf::make_updater<SideForceUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eYAW_MOMENT))
   {
      return wkf::make_updater<YawMomentUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(ePITCH_MOMENT))
   {
      return wkf::make_updater<PitchMomentUpdater>(aPlatformName.toStdString());
   }
   else if (aDatum == mDataToStringMap.at(eROLL_MOMENT))
   {
      return wkf::make_updater<RollMomentUpdater>(aPlatformName.toStdString());
   }
   return nullptr;
}

/** Get the preferences widget. */
QList<wkf::PrefWidget*> RvSixDOF_Data::Plugin::GetPreferencesWidgets() const
{
   return QList<wkf::PrefWidget*>();
}

/** Get a list of tree widgets for a platform.
 * @param aPlatformName platform name
 */
QList<QTreeWidgetItem*> RvSixDOF_Data::Plugin::GetPlatformData(const std::string& aPlatformName)
{
   mInterfacePtr->SetPlatformOfInterest(aPlatformName);
   mUpdatePosted = true;
   return mTopLevelWidgets;
}

/** Advance time and read data. */
void RvSixDOF_Data::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   mInterfacePtr->AdvanceTimeRead(aData);
   mUpdatePosted = true;
}

/** Read data. */
void RvSixDOF_Data::Plugin::RegularRead(const rv::ResultData& aData)
{
   mInterfacePtr->AdvanceTimeRead(aData);
   mUpdatePosted = true;
}

/** Update the GUI. */
void RvSixDOF_Data::Plugin::GuiUpdate()
{
   if (mUpdatePosted)
   {
      RvSixDOF_Data::Interface::SixDOF_DataContainer data = mInterfacePtr->GetSixDOF_Data();

      if (mInterfacePtr->IsSixDOF_Visible())
      {
         mTopLevelWidgets.first()->setHidden(false);
         if (mInterfacePtr->IsCoreDataVisible())
         {
            mCoreDataItemPtr->setHidden(false);
            // MsgSixDOF_CoreData
            mUnitDataWidgets[eALTITUDE]->SetValue(data.m_altitude_m);
            mUnitDataWidgets[eVERT_SPEED_FPM]->SetValue(data.m_vertSpeed_fpm);
            mUnitDataWidgets[eKCAS]->SetValue(data.m_KCAS);
            mUnitDataWidgets[eKTAS]->SetValue(data.m_KTAS);
            mUnitDataWidgets[eMACH]->SetValue(data.m_mach);
            mUnitDataWidgets[eHEADING]->SetValue(data.m_heading_rad);
            mUnitDataWidgets[ePITCH]->SetValue(data.m_pitch_rad);
            mUnitDataWidgets[eROLL]->SetValue(data.m_roll_rad);
            mUnitDataWidgets[eGLOAD]->SetValue(data.m_gLoad);
            mUnitDataWidgets[eALPHA]->SetValue(data.m_alpha_rad);
            mUnitDataWidgets[eG_AVAIL]->SetValue(data.m_gAvail);
            mUnitDataWidgets[eSPEEDBRAKE_ANGLE]->SetValue(data.m_speedbrakeAngle_rad);
         }
         else
         {
            mCoreDataItemPtr->setHidden(true);
         }

         if (mInterfacePtr->IsKinematicVisible())
         {
            mKinematicItemPtr->setHidden(false);
            // MsgSixDOF_Kinematic
            mUnitDataWidgets[eBETA]->SetValue(data.m_beta_rad);
            mUnitDataWidgets[eALPHA_DOT]->SetValue(data.m_alphaDot_rps);
            mUnitDataWidgets[eBETA_DOT]->SetValue(data.m_betaDot_rps);
            mUnitDataWidgets[eYAW_RATE]->SetValue(data.m_yawRate_rps);
            mUnitDataWidgets[ePITCH_RATE]->SetValue(data.m_pitchRate_rps);
            mUnitDataWidgets[eROLL_RATE]->SetValue(data.m_rollRate_rps);
            mUnitDataWidgets[eDYNAMIC_PRESSURE]->SetValue(data.m_dynamicPressure_pa);
            mUnitDataWidgets[eNX]->SetValue(data.m_nx);
            mUnitDataWidgets[eNY]->SetValue(data.m_ny);
            mUnitDataWidgets[eFLIGHT_PATH_ANGLE]->SetValue(data.m_flightPathAngle_rad);
            mUnitDataWidgets[eCENTER_GRAVITY_X]->SetValue(data.m_centerGravity_x);
            mUnitDataWidgets[eCENTER_GRAVITY_Y]->SetValue(data.m_centerGravity_y);
            mUnitDataWidgets[eCENTER_GRAVITY_Z]->SetValue(data.m_centerGravity_z);
         }
         else
         {
            mKinematicItemPtr->setHidden(true);
         }

         if (mInterfacePtr->IsEngineFuelVisible())
         {
            mEngineFuelItemPtr->setHidden(false);
            // MsgSixDOF_EngineFuel
            mUnitDataWidgets[eFUELFLOW]->SetValue(data.m_fuelFlow_pph);
            mUnitDataWidgets[eFUEL_INTERNAL]->SetValue(data.m_fuelInternal_kg);
            mUnitDataWidgets[eFUEL_EXTERNAL]->SetValue(data.m_fuelExternal_kg);
            mUnitDataWidgets[eFUEL_INTERNAL_REMAINING]->SetValue(data.m_fuelInternalRemaining_kg);
            mUnitDataWidgets[eFUEL_EXTERNAL_REMAINING]->SetValue(data.m_fuelExternalRemaining_kg);
            mUnitDataWidgets[eTOTAL_WEIGHT]->SetValue(data.m_totalWeight_kg);
            mStringDataWidgets[eAFTERBURNER]->setText(1, bool(data.mBits.test(0)) ? "true" : "false");
            mStringDataWidgets[ePRODUCING_THRUST]->setText(1, bool(data.mBits.test(1)) ? "true" : "false");
            mStringDataWidgets[eENGINE_OPERATING]->setText(1, bool(data.mBits.test(2)) ? "true" : "false");
            mStringDataWidgets[eENGINE_SMOKING]->setText(1, bool(data.mBits.test(3)) ? "true" : "false");
            mStringDataWidgets[eCONTRAILING]->setText(1, bool(data.mBits.test(4)) ? "true" : "false");
            mUnitDataWidgets[eJOKER_FUEL_STATE]->SetValue(data.m_jokerFuelState_kg);
            mUnitDataWidgets[eBINGO_FUEL_STATE]->SetValue(data.m_bingoFuelState_kg);
            mStringDataWidgets[eWEIGHT_ON_WHEELS]->setText(1, bool(data.mBits.test(5)) ? "true" : "false");
         }
         else
         {
            mEngineFuelItemPtr->setHidden(true);
         }

         if (mInterfacePtr->IsAutopilotVisible())
         {
            mAutopilotItemPtr->setHidden(false);
            // MsgSixDOF_Autopilot
            mStringDataWidgets[eACTIVE_PILOT]->setText(1, QString::fromStdString(data.m_activePilot));
            mStringDataWidgets[eLATERAL_MODE_NAME]->setText(1, QString::fromStdString(data.m_lateralModeName));
            mUnitDataWidgets[eLATERAL_MODE_VALUE]->SetValue(data.m_lateralModeValue);
            mStringDataWidgets[eVERTICAL_MODE_NAME]->setText(1, QString::fromStdString(data.m_verticalModeName));
            mUnitDataWidgets[eVERTICAL_MODE_VALUE]->SetValue(data.m_verticalModeValue);
            mStringDataWidgets[eSPEED_MODE_NAME]->setText(1, QString::fromStdString(data.m_speedModeName));
            mUnitDataWidgets[eSPEED_MODE_VALUE]->SetValue(data.m_speedModeValue);
            mStringDataWidgets[eCURRENT_MANEUVER]->setText(1, QString::fromStdString(data.m_currentManeuver));
         }
         else
         {
            mAutopilotItemPtr->setHidden(true);
         }

         if (mInterfacePtr->IsAutopilotLimitsVisible())
         {
            mAutopilotLimitsItemPtr->setHidden(false);
            // MsgSixDOF_AutopilotLimits
            mUnitDataWidgets[ePITCH_GLOAD_MIN]->SetValue(data.m_pitchGLoadMin);
            mUnitDataWidgets[ePITCH_GLOAD_MAX]->SetValue(data.m_pitchGLoadMax);
            mUnitDataWidgets[eALPHA_MIN]->SetValue(data.m_alphaMin);
            mUnitDataWidgets[eALPHA_MAX]->SetValue(data.m_alphaMax);
            mUnitDataWidgets[ePITCH_RATE_MIN]->SetValue(data.m_pitchRateMin);
            mUnitDataWidgets[ePITCH_RATE_MAX]->SetValue(data.m_pitchRateMax);
            mUnitDataWidgets[eVERTICAL_SPEED_MIN]->SetValue(data.m_verticalSpeedMin);
            mUnitDataWidgets[eVERTICAL_SPEED_MAX]->SetValue(data.m_verticalSpeedMax);
            mUnitDataWidgets[eYAW_GLOAD_MAX]->SetValue(data.m_yawGLoadMax);
            mUnitDataWidgets[eBETA_MAX]->SetValue(data.m_betaMax);
            mUnitDataWidgets[eYAW_RATE_MAX]->SetValue(data.m_yawRateMax);
            mUnitDataWidgets[eROLL_RATE_MAX]->SetValue(data.m_rollRateMax);
            mUnitDataWidgets[eBANK_ANGLE_MAX]->SetValue(data.m_bankAngleMax);
            mUnitDataWidgets[eFORWARD_ACCEL_MIN]->SetValue(data.m_forwardAccelMin);
            mUnitDataWidgets[eFORWARD_ACCEL_MAX]->SetValue(data.m_forwardAccelMax);
            mUnitDataWidgets[eTAXI_SPEED_MAX]->SetValue(data.m_taxiSpeedMax);
            mUnitDataWidgets[eTAXI_YAW_RATE_MAX]->SetValue(data.m_taxiYawRateMax);
         }
         else
         {
            mAutopilotLimitsItemPtr->setHidden(true);
         }

         if (mInterfacePtr->IsControlInputsVisible())
         {
            mControlInputsItemPtr->setHidden(false);
            // MsgSixDOF_ControlInputs
            mUnitDataWidgets[eSTICK_BACK]->SetValue(data.m_stickBack);
            mUnitDataWidgets[eSTICK_RIGHT]->SetValue(data.m_stickRight);
            mUnitDataWidgets[eRUDDER_RIGHT]->SetValue(data.m_rudderRight);
            mUnitDataWidgets[eTHROTTLE]->SetValue(data.m_throttle);
            mUnitDataWidgets[eSPEED_BRAKE_LEVER]->SetValue(data.m_speedBrakeLever);
            mUnitDataWidgets[eFLAPS_LEVER]->SetValue(data.m_flapsLever);
            mUnitDataWidgets[eLANDING_GEAR_LEVER]->SetValue(data.m_landingGearLever);
            mUnitDataWidgets[eSPOILERS_LEVER]->SetValue(data.m_spoilersLever);
            mUnitDataWidgets[eTHRUST_REVERSER_LEVER]->SetValue(data.m_thrustReverserLever);
         }
         else
         {
            mControlInputsItemPtr->setHidden(true);
         }

         if (mInterfacePtr->IsControlSurfacesVisible())
         {
            mControlSurfacesItemPtr->setHidden(false);

            for (size_t i = 0; i < data.m_surfaceNames.size(); i++)
            {
               // If there isn't a tree widget item for the surface name, add it.
               if (mControlSurfaceWidgets.find(data.m_surfaceNames[i]) == mControlSurfaceWidgets.end())
               {
                  mControlSurfaceWidgets[data.m_surfaceNames[i]] =
                     new wkf::AngleTreeWidgetItem(mControlSurfacesItemPtr, QString::fromStdString(data.m_surfaceNames[i]));
               }
               // Update the value of the control surface.
               mControlSurfaceWidgets[data.m_surfaceNames[i]]->SetValue(data.m_surfaceAngles[i]);
            }
         }
         else
         {
            mControlSurfacesItemPtr->setHidden(true);
         }

         if (mInterfacePtr->IsForceMomentVisible())
         {
            mForceMomentItemPtr->setHidden(false);
            // MsgSixDOF_ForceMoment
            mUnitDataWidgets[eLIFT]->SetValue(data.m_liftForce_nt);
            mUnitDataWidgets[eDRAG]->SetValue(data.m_dragForce_nt);
            mUnitDataWidgets[eTHRUST]->SetValue(data.m_thrustForce_nt);
            mUnitDataWidgets[eSIDEFORCE]->SetValue(data.m_sideForce_nt);
            mUnitDataWidgets[eYAW_MOMENT]->SetValue(data.m_yawMoment);
            mUnitDataWidgets[ePITCH_MOMENT]->SetValue(data.m_pitchMoment);
            mUnitDataWidgets[eROLL_MOMENT]->SetValue(data.m_rollMoment);
         }
         else
         {
            mForceMomentItemPtr->setHidden(true);
         }
      } // if (mInterfacePtr->IsSixDOF_Visible())
      else
      {
         mTopLevelWidgets.first()->setHidden(true);
      }

      mUpdatePosted = false;
   } // if (mUpdatePosted)
}

/** Create a tree widget item. */
void RvSixDOF_Data::Plugin::CreateItem(QTreeWidgetItem* parent, eSixDOF_DataType aType)
{
   QTreeWidgetItem* item = nullptr;
   // Switch through all known types and create a widget with the correct type
   switch (aType)
   {
   case eALTITUDE:
      mUnitDataWidgets[aType] = new wkf::AltitudeTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eHEADING:
   case ePITCH:
   case eROLL:
   case eALPHA:
   case eBETA:
   case eSPEEDBRAKE_ANGLE:
   case eFLIGHT_PATH_ANGLE:
   case eALPHA_MIN:
   case eALPHA_MAX:
   case eBETA_MAX:
   case eBANK_ANGLE_MAX:
      mUnitDataWidgets[aType] = new wkf::AngleTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eCENTER_GRAVITY_X:
   case eCENTER_GRAVITY_Y:
   case eCENTER_GRAVITY_Z:
      mUnitDataWidgets[aType] = new wkf::LengthTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eYAW_MOMENT:
   case ePITCH_MOMENT:
   case eROLL_MOMENT:
      mUnitDataWidgets[aType] = new wkf::TorqueTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eMACH:
   case eSTICK_BACK:
   case eSTICK_RIGHT:
   case eRUDDER_RIGHT:
   case eTHROTTLE:
   case eSPEED_BRAKE_LEVER:
   case eLATERAL_MODE_VALUE:
   case eVERTICAL_MODE_VALUE:
   case eSPEED_MODE_VALUE:
   case eFLAPS_LEVER:
   case eLANDING_GEAR_LEVER:
   case eSPOILERS_LEVER:
   case eTHRUST_REVERSER_LEVER:
   case eFORWARD_ACCEL_MIN:
   case eFORWARD_ACCEL_MAX:
   case eGLOAD:
   case eG_AVAIL:
   case eNX:
   case eNY:
   case ePITCH_GLOAD_MIN:
   case ePITCH_GLOAD_MAX:
   case eYAW_GLOAD_MAX:
   case eVERTICAL_SPEED_MIN:
   case eVERTICAL_SPEED_MAX:
   case eVERT_SPEED_FPM:
   case eFUELFLOW:
   case eKCAS:
   case eKTAS:
      mUnitDataWidgets[aType] = new wkf::UnitlessTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eDYNAMIC_PRESSURE:
      mUnitDataWidgets[aType] = new wkf::PressureTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eALPHA_DOT:
   case eBETA_DOT:
   case eYAW_RATE:
   case ePITCH_RATE:
   case eROLL_RATE:
   case ePITCH_RATE_MIN:
   case ePITCH_RATE_MAX:
   case eYAW_RATE_MAX:
   case eROLL_RATE_MAX:
   case eTAXI_YAW_RATE_MAX:
      mUnitDataWidgets[aType] = new wkf::AngularRateTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eLIFT:
   case eDRAG:
   case eTHRUST:
   case eSIDEFORCE:
      mUnitDataWidgets[aType] = new wkf::ForceTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eFUEL_INTERNAL:
   case eFUEL_INTERNAL_REMAINING:
   case eFUEL_EXTERNAL:
   case eFUEL_EXTERNAL_REMAINING:
   case eTOTAL_WEIGHT:
   case eJOKER_FUEL_STATE:
   case eBINGO_FUEL_STATE:
      mUnitDataWidgets[aType] = new wkf::MassTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eTAXI_SPEED_MAX:
      mUnitDataWidgets[aType] = new wkf::SpeedTreeWidgetItem(item, mDataToStringMap[aType]);
      item                    = mUnitDataWidgets[aType];
      break;
   case eAFTERBURNER:
   case ePRODUCING_THRUST:
   case eENGINE_OPERATING:
   case eENGINE_SMOKING:
   case eCONTRAILING:
   case eWEIGHT_ON_WHEELS:
   case eLATERAL_MODE_NAME:
   case eVERTICAL_MODE_NAME:
   case eSPEED_MODE_NAME:
   case eACTIVE_PILOT:
   case eCURRENT_MANEUVER:
      mStringDataWidgets[aType] = new QTreeWidgetItem(parent);
      mStringDataWidgets[aType]->setText(0, mDataToStringMap[aType]);
      item = mStringDataWidgets[aType];
      break;
   default:
      // error
      break;
   } // switch (aType)

   if (item != nullptr)
   {
      item->setData(0, wkf::UPDATER_ROLE, true);
      parent->addChild(item);
   }
}

/** Get a plot updater. */
std::unique_ptr<rv::PlotUpdater> RvSixDOF_Data::Plugin::GetPlotUpdater(const QString&               aX,
                                                                       const QString&               aY,
                                                                       const wkf::PlatformGrouping& aGrouping,
                                                                       const QString&               aSubcategory) const
{
   return ut::make_unique<PlotUpdater>(aX, aY, aGrouping);
}
