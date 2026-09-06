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

#include "RvSixDOF_DataPlotUpdater.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "RvSixDOF_EventPipeClasses.hpp"
#include "UtCast.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtSphericalEarth.hpp"
#include "WkfUnitsObject.hpp"

RvSixDOF_Data::PlotUpdater::PlotUpdater(const QString& aX, const QString& aY, const wkf::PlatformGrouping& aGroup)
   : rv::PlotUpdater()
   , mInitialXAxis(aX)
   , mInitialYAxis(aY)
   , mControlSurfacesList()
{
   if (aGroup.Type() == wkf::PlatformGrouping::eIndividual)
   {
      mInitialSelection.push_back(QString::fromStdString(aGroup.Name()));
   }
   else
   {
      // build the selection list
      rv::ResultData* dataPtr = rvEnv.GetData();
      if (dataPtr != nullptr)
      {
         rv::ResultDb* dbPtr = dataPtr->GetDb();
         if (dbPtr != nullptr)
         {
            const std::map<wkf::PlatformGrouping, std::set<int>>& catmap = wkfEnv.GetGroupings();
            auto                                                  it     = catmap.find(aGroup);
            if (it != catmap.end())
            {
               for (auto&& jt : it->second)
               {
                  mInitialSelection.push_back(QString::fromStdString(dbPtr->GetPlatformName(jt)));
               }
            }
         }
      }
   }

   // Get the control surface names
   rv::ResultData*     dataPtr        = rvEnv.GetData();
   rv::ResultPlatform* platPtr        = dataPtr->FindPlatform(dataPtr->FindPlatformIndex(aGroup.Name()));
   auto                msgSurfacesPtr = platPtr->FindFirstAfter<rv::MsgSixDOF_ControlSurfaces>(0.0);
   if (msgSurfacesPtr != nullptr)
   {
      for (const std::string& surface : msgSurfacesPtr->surfaceNames())
      {
         mControlSurfacesList.append(QString::fromStdString(surface));
      }
   }
}

QStringList RvSixDOF_Data::PlotUpdater::GetPlatforms() const
{
   QStringList     list;
   rv::ResultData* dataPtr = rvEnv.GetData();

   // initialize the plot window's information
   if (dataPtr != nullptr)
   {
      rv::ResultDb* dbPtr = dataPtr->GetDb();
      if (dbPtr != nullptr)
      {
         for (unsigned int i = 0; i < dbPtr->GetPlatformCount(); i++)
         {
            list.push_back(QString::fromStdString(dbPtr->GetPlatformName(i)));
         }
      }
   }
   return list;
}

QString RvSixDOF_Data::PlotUpdater::GetUnits(const QString& aAxis) const
{
   if (aAxis == "Time")
   {
      return WKF_UNIT_NAME(Time);
   }

   // MsgSixDOF_CoreData
   if (aAxis == "Altitude (MSL)")
   {
      return WKF_UNIT_NAME(Length);
   }
   else if (aAxis == "Local Heading" || aAxis == "Local Pitch" || aAxis == "Local Roll" ||
            aAxis == "Angle of Attack, Alpha" || aAxis == "Speed Brake Surface Angle")
   {
      return WKF_UNIT_NAME(Angle);
   }

   // MsgSixDOF_Kinematic
   if (aAxis == "Angle of Sideslip, Beta" || aAxis == "Flight Path Angle, Gamma")
   {
      return WKF_UNIT_NAME(Angle);
   }
   else if (aAxis == "Angle of Attack Rate, AlphaDot" || aAxis == "Angle of Sideslip Rate, BetaDot" ||
            aAxis == "Body Yaw Rate, r" || aAxis == "Body Pitch Rate, q" || aAxis == "Body Roll Rate, p")
   {
      return WKF_UNIT_NAME(AngularRate);
   }
   else if (aAxis == "Dynamic Pressure, Q")
   {
      return WKF_UNIT_NAME(Pressure);
   }
   else if (aAxis.startsWith("Center of Gravity - "))
   {
      return WKF_UNIT_NAME(Length);
   }

   // MsgSixDOF_EngineFuel

   if (aAxis.endsWith("Fuel Capacity") || aAxis.endsWith("Fuel Remaining") || aAxis == "Current Total Weight/Mass" ||
       aAxis == "Joker Fuel State" || aAxis == "Bingo Fuel State")
   {
      return WKF_UNIT_NAME(Mass);
   }

   // MsgSixDOF_Autopilot: no units

   // MsgSixDOF_ControlInputs: no units

   // MsgSixDOF_ControlSurfaces

   if (mControlSurfacesList.contains(aAxis))
   {
      return "degrees";
   }

   // MsgSixDOF_ForceMoment

   if (aAxis.endsWith("Force"))
   {
      return WKF_UNIT_NAME(Force);
   }
   else if (aAxis.endsWith("Moment"))
   {
      return WKF_UNIT_NAME(Torque);
   }

   return {};
}

/** Helper function to get the available axes. */
QStringList RvSixDOF_Data::PlotUpdater::GetAxes() const
{
   QStringList axisVariableList;
   axisVariableList.push_back("Time");

   // MsgSixDOF_CoreData
   axisVariableList.push_back("Altitude (MSL)");
   axisVariableList.push_back("Vertical Speed (FPM)");
   axisVariableList.push_back("Airspeed, Knots Calibrated (KCAS)");
   axisVariableList.push_back("Airspeed, Knots True (KTAS)");
   axisVariableList.push_back("Airspeed, Mach");
   axisVariableList.push_back("Local Heading");
   axisVariableList.push_back("Local Pitch");
   axisVariableList.push_back("Local Roll");
   axisVariableList.push_back("Body Up Accel (Gs), G-Load");
   axisVariableList.push_back("Angle of Attack, Alpha");
   axisVariableList.push_back("Maneuver Gs, G-Available");
   axisVariableList.push_back("Speed Brake Surface Angle");

   // MsgSixDOF_Kinematic
   axisVariableList.push_back("Angle of Sideslip, Beta");
   axisVariableList.push_back("Angle of Attack Rate, AlphaDot");
   axisVariableList.push_back("Angle of Sideslip Rate, BetaDot");
   axisVariableList.push_back("Body Yaw Rate, r");
   axisVariableList.push_back("Body Pitch Rate, q");
   axisVariableList.push_back("Body Roll Rate, p");
   axisVariableList.push_back("Dynamic Pressure, Q");
   axisVariableList.push_back("Body Forward Accel (Gs), Nx");
   axisVariableList.push_back("Body Right Accel (Gs), Ny");
   axisVariableList.push_back("Flight Path Angle, Gamma");
   axisVariableList.push_back("Center of Gravity - X, CGx");
   axisVariableList.push_back("Center of Gravity - Y, CGy");
   axisVariableList.push_back("Center of Gravity - Z, CGz");

   // MsgSixDOF_EngineFuel
   axisVariableList.push_back("Fuel Flow (PPH)");
   axisVariableList.push_back("Internal Fuel Capacity");
   axisVariableList.push_back("External Fuel Capacity");
   axisVariableList.push_back("Internal Fuel Remaining");
   axisVariableList.push_back("External Fuel Remaining");
   axisVariableList.push_back("Current Total Weight/Mass");
   axisVariableList.push_back("Afterburner On");
   axisVariableList.push_back("Producing Thrust");
   axisVariableList.push_back("Engine Operating");
   axisVariableList.push_back("Engine Smoking");
   axisVariableList.push_back("Contrailing");
   axisVariableList.push_back("Joker Fuel State");
   axisVariableList.push_back("Bingo Fuel State");
   axisVariableList.push_back("Weight on Wheels");

   // MsgSixDOF_Autopilot
   axisVariableList.push_back("Lateral Mode Value");
   axisVariableList.push_back("Vertical Mode Value");
   axisVariableList.push_back("Speed Mode Value");

   // MsgSixDOF_ControlInputs
   axisVariableList.push_back("Stick Control Back");
   axisVariableList.push_back("Stick Control Right");
   axisVariableList.push_back("Rudder Control Right");
   axisVariableList.push_back("Throttle Lever (0-2)");
   axisVariableList.push_back("Speed Brake Lever");
   axisVariableList.push_back("Flaps Lever");
   axisVariableList.push_back("Landing Gear Lever");
   axisVariableList.push_back("Spoilers Lever");
   axisVariableList.push_back("Thrust Reverser Lever");

   // MsgSixDOF_ControlSurfaces
   axisVariableList.append(mControlSurfacesList);

   // MsgSixDOF_ForceMoment
   axisVariableList.push_back("Lift Force");
   axisVariableList.push_back("Drag Force");
   axisVariableList.push_back("Thrust Force");
   axisVariableList.push_back("Side Force");
   axisVariableList.push_back("Yaw Moment");
   axisVariableList.push_back("Pitch Moment");
   axisVariableList.push_back("Roll Moment");

   return axisVariableList;
}

/** Get the initial selection for the plot. */
const QStringList& RvSixDOF_Data::PlotUpdater::GetInitialSelection() const
{
   return mInitialSelection;
}

/** Get the initial X axis. */
const QString& RvSixDOF_Data::PlotUpdater::GetInitialXAxis() const
{
   return mInitialXAxis;
}

/** Get the initial Y axis. */
const QString& RvSixDOF_Data::PlotUpdater::GetInitialYAxis() const
{
   return mInitialYAxis;
}

/** Get a series(plot) of X vs. Y. */
std::vector<std::pair<double, double>> RvSixDOF_Data::PlotUpdater::GetSeries(const QString& aItem,
                                                                             const QString& aXAxis,
                                                                             const QString& aYAxis) const
{
   // We need to access from different message types for different variables
   // so check aXAxis and aYAxis against a list and call the correct function
   // for the correct message.
   std::vector<std::pair<double, double>> series;

   std::vector<double> xVals;
   std::vector<double> yVals;

   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         rv::ResultPlatform* platformPtr = db->FindPlatformByName(aItem);
         if (platformPtr)
         {
            // Get the message type to make sure the axis loads the right data
            switch (GetMessageType(aXAxis))
            {
            case MsgType::eMsgSixDOF_CoreData:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_CoreData>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetCoreData(aXAxis, dynamic_cast<rv::MsgSixDOF_CoreData*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgSixDOF_Kinematic:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_Kinematic>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetKinematicData(aYAxis, dynamic_cast<rv::MsgSixDOF_Kinematic*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgSixDOF_EngineFuel:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_EngineFuel>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetEngineFuelData(aYAxis, dynamic_cast<rv::MsgSixDOF_EngineFuel*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgSixDOF_Autopilot:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_Autopilot>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetAutopilotData(aYAxis, dynamic_cast<rv::MsgSixDOF_Autopilot*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgSixDOF_AutopilotLimits:
               // Autopilot limit data typically doesn't change during a run and usually isn't suitable for plotting
               break;
            case MsgType::eMsgSixDOF_ControlInputs:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_ControlInputs>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetControlInputsData(aYAxis, dynamic_cast<rv::MsgSixDOF_ControlInputs*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgSixDOF_ForceMoment:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_ForceMoment>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetForceMomentData(aYAxis, dynamic_cast<rv::MsgSixDOF_ForceMoment*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
               break;
            }
            case MsgType::eMsgSixDOF_ControlSurfaces:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_ControlSurfaces>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        // Get the index of the control surface string
                        rv::MsgSixDOF_ControlSurfaces* msg         = dynamic_cast<rv::MsgSixDOF_ControlSurfaces*>(kt);
                        size_t                         indexOfName = ut::npos;
                        for (size_t i = 0; i < msg->surfaceNames().size(); i++)
                        {
                           if (aXAxis.toStdString().compare(msg->surfaceNames()[i]) == 0)
                           {
                              indexOfName = i;
                           }
                        }
                        if (indexOfName == ut::npos)
                        {
                           break;
                        }

                        double x = GetControlSurfacesData(indexOfName, msg);
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
               break;
            }
            default:
               break;
            }


            mLastUpdate = 0.0;

            switch (GetMessageType(aYAxis))
            {
            case MsgType::eMsgSixDOF_CoreData:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_CoreData>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetCoreData(aYAxis, dynamic_cast<rv::MsgSixDOF_CoreData*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgSixDOF_Kinematic:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_Kinematic>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetKinematicData(aYAxis, dynamic_cast<rv::MsgSixDOF_Kinematic*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgSixDOF_EngineFuel:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_EngineFuel>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetEngineFuelData(aYAxis, dynamic_cast<rv::MsgSixDOF_EngineFuel*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgSixDOF_Autopilot:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_Autopilot>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetAutopilotData(aYAxis, dynamic_cast<rv::MsgSixDOF_Autopilot*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgSixDOF_AutopilotLimits:
               // Autopilot limit data typically doesn't change during a run and usually isn't suitable for plotting
               break;
            case MsgType::eMsgSixDOF_ControlInputs:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_ControlInputs>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetControlInputsData(aYAxis, dynamic_cast<rv::MsgSixDOF_ControlInputs*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgSixDOF_ForceMoment:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_ForceMoment>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetForceMomentData(aYAxis, dynamic_cast<rv::MsgSixDOF_ForceMoment*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
               break;
            }
            case MsgType::eMsgSixDOF_ControlSurfaces:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgSixDOF_ControlSurfaces>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        // Get the index of the control surface string
                        rv::MsgSixDOF_ControlSurfaces* msg         = dynamic_cast<rv::MsgSixDOF_ControlSurfaces*>(kt);
                        size_t                         indexOfName = ut::npos;
                        for (size_t i = 0; i < msg->surfaceNames().size(); i++)
                        {
                           if (aYAxis.toStdString().compare(msg->surfaceNames()[i]) == 0)
                           {
                              indexOfName = i;
                           }
                        }
                        if (indexOfName == ut::npos)
                        {
                           break;
                        }

                        double y = GetControlSurfacesData(indexOfName, msg);
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
               break;
            }
            } // switch (GetMessageType(aXAxis))

            // This should be changed. I was comparing x size to y size to see which one to plot from
            // There should be a better way to do this.
            // If the amount of yVars are larger, just use them to plot
            if (xVals.size() > yVals.size())
            {
               for (unsigned int i = 0; i < yVals.size(); i++)
               {
                  series.emplace_back(xVals[i], yVals[i]);
               }
            }
            else
            {
               for (unsigned int i = 0; i < xVals.size(); i++)
               {
                  series.emplace_back(xVals[i], yVals[i]);
               }
            }
         } // if (platformPtr)
      }    // if (nullptr != db)
   }       // if (nullptr != results)
   return series;
}

/** Get SixDOF Core Data. */
double RvSixDOF_Data::PlotUpdater::GetCoreData(const QString& aDesc, const rv::MsgSixDOF_CoreData* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }
   else if (aDesc == "Altitude (MSL)")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAltitudeUnit(0);
      return UtUnitLength::ConvertFromStandard(aState->altitude_m(), u);
   }
   else if (aDesc == "Vertical Speed (FPM)")
   {
      return aState->vertSpeed_msec() * UtMath::cFT_PER_M * 60;
   }
   else if (aDesc == "Airspeed, Knots Calibrated (KCAS)")
   {
      return aState->KCAS();
   }
   else if (aDesc == "Airspeed, Knots True (KTAS)")
   {
      return aState->KTAS();
   }
   else if (aDesc == "Airspeed, Mach")
   {
      return aState->mach();
   }
   else if (aDesc == "Local Heading")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(aState->heading_rad(), u);
   }
   else if (aDesc == "Local Pitch")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(aState->pitch_rad(), u);
   }
   else if (aDesc == "Local Roll")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(aState->roll_rad(), u);
   }
   else if (aDesc == "Body Up Accel (Gs), G-Load")
   {
      return aState->gLoad();
   }
   else if (aDesc == "Angle of Attack, Alpha")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(aState->alpha_rad(), u);
   }
   else if (aDesc == "Maneuver Gs, G-Available")
   {
      return aState->gAvail();
   }
   else if (aDesc == "Speed Brake Surface Angle")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(aState->speedbrakeAngle_rad(), u);
   }
   return 0;
}

/** Get SixDOF Kinematic Data. */
double RvSixDOF_Data::PlotUpdater::GetKinematicData(const QString& aDesc, const rv::MsgSixDOF_Kinematic* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }
   else if (aDesc == "Angle of Sideslip, Beta")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(aState->beta_rad(), u);
   }
   else if (aDesc == "Angle of Attack Rate, AlphaDot")
   {
      return WKF_UNIT_FROM_STANDARD(AngularRate, aState->alphaDot_rps());
   }
   else if (aDesc == "Angle of Sideslip Rate, BetaDot")
   {
      return WKF_UNIT_FROM_STANDARD(AngularRate, aState->betaDot_rps());
   }
   else if (aDesc == "Body Yaw Rate, r")
   {
      return WKF_UNIT_FROM_STANDARD(AngularRate, aState->yawRate_rps());
   }
   else if (aDesc == "Body Pitch Rate, q")
   {
      return WKF_UNIT_FROM_STANDARD(AngularRate, aState->pitchRate_rps());
   }
   else if (aDesc == "Body Roll Rate, p")
   {
      return WKF_UNIT_FROM_STANDARD(AngularRate, aState->rollRate_rps());
   }
   else if (aDesc == "Dynamic Pressure, Q")
   {
      UtUnitPressure::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetPressureUnit();
      return UtUnitPressure::ConvertFromStandard(aState->dynamicPressure_psf() * UtMath::cPASCAL_PER_PSF, u);
   }
   else if (aDesc == "Body Forward Accel (Gs), Nx")
   {
      return aState->nx();
   }
   else if (aDesc == "Body Right Accel (Gs), Ny")
   {
      return aState->ny();
   }
   else if (aDesc == "Flight Path Angle, Gamma")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(aState->flightPathAngle_rad(), u);
   }
   else if (aDesc == "Center of Gravity - X, CGx")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(aState->centerGravity_x(), u);
   }
   else if (aDesc == "Center of Gravity - Y, CGy")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(aState->centerGravity_y(), u);
   }
   else if (aDesc == "Center of Gravity - Z, CGz")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(aState->centerGravity_z(), u);
   }
   return 0;
}

/** Get SixDOF Engine and Fuel Data. */
double RvSixDOF_Data::PlotUpdater::GetEngineFuelData(const QString& aDesc, const rv::MsgSixDOF_EngineFuel* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }
   else if (aDesc == "Fuel Flow (PPH)")
   {
      return aState->fuelFlow_kgsec() * UtMath::cLB_PER_KG * 3600.0;
   }
   else if (aDesc == "Internal Fuel Capacity")
   {
      UtUnitMass::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetMassUnit();
      return UtUnitMass::ConvertFromStandard(aState->fuelInternal_kg(), u);
   }
   else if (aDesc == "External Fuel Capacity")
   {
      UtUnitMass::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetMassUnit();
      return UtUnitMass::ConvertFromStandard(aState->fuelExternal_kg(), u);
   }
   else if (aDesc == "Internal Fuel Remaining")
   {
      UtUnitMass::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetMassUnit();
      return UtUnitMass::ConvertFromStandard(aState->fuelInternalRemaining_kg(), u);
   }
   else if (aDesc == "External Fuel Remaining")
   {
      UtUnitMass::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetMassUnit();
      return UtUnitMass::ConvertFromStandard(aState->fuelExternalRemaining_kg(), u);
   }
   else if (aDesc == "Current Total Weight/Mass")
   {
      UtUnitMass::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetMassUnit();
      return UtUnitMass::ConvertFromStandard(aState->totalWeight_kg(), u);
   }
   else if (aDesc == "Afterburner On")
   {
      return aState->afterburner_on();
   }
   else if (aDesc == "Producing Thrust")
   {
      return aState->producing_thrust();
   }
   else if (aDesc == "Engine Operating")
   {
      return aState->engine_operating();
   }
   else if (aDesc == "Engine Smoking")
   {
      return aState->engine_smoking();
   }
   else if (aDesc == "Contrailing")
   {
      return aState->contrailing();
   }
   else if (aDesc == "Joker Fuel State")
   {
      UtUnitMass::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetMassUnit();
      return UtUnitMass::ConvertFromStandard(aState->jokerFuelState_kg(), u);
   }
   else if (aDesc == "Bingo Fuel State")
   {
      UtUnitMass::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetMassUnit();
      return UtUnitMass::ConvertFromStandard(aState->bingoFuelState_kg(), u);
   }
   else if (aDesc == "Weight on Wheels")
   {
      return aState->weightOnWheels();
   }
   return 0;
}

/** Get SixDOF Autopilot Data. */
double RvSixDOF_Data::PlotUpdater::GetAutopilotData(const QString& aDesc, const rv::MsgSixDOF_Autopilot* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }
   else if (aDesc == "Lateral Mode Value")
   {
      return aState->lateralModeValue();
   }
   else if (aDesc == "Vertical Mode Value")
   {
      return aState->verticalModeValue();
   }
   else if (aDesc == "Speed Mode Value")
   {
      return aState->speedModeValue();
   }
   return 0;
}

/** Get SixDOF Autopilot Limits Data. */
double RvSixDOF_Data::PlotUpdater::GetControlInputsData(const QString& aDesc, const rv::MsgSixDOF_ControlInputs* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }
   else if (aDesc == "Stick Control Back")
   {
      return aState->stickBack();
   }
   else if (aDesc == "Stick Control Right")
   {
      return aState->stickRight();
   }
   else if (aDesc == "Rudder Control Right")
   {
      return aState->rudderRight();
   }
   else if (aDesc == "Throttle Lever (0-2)")
   {
      return aState->throttle();
   }
   else if (aDesc == "Speed Brake Lever")
   {
      return aState->speedBrakeLever();
   }
   else if (aDesc == "Flaps Lever")
   {
      return aState->flapsLever();
   }
   else if (aDesc == "Landing Gear Lever")
   {
      return aState->landingGearLever();
   }
   else if (aDesc == "Spoilers Lever")
   {
      return aState->spoilersLever();
   }
   else if (aDesc == "Thrust Reverser Lever")
   {
      return aState->thrustReverserLever();
   }
   return 0;
}

double RvSixDOF_Data::PlotUpdater::GetControlSurfacesData(const size_t                         aIndex,
                                                          const rv::MsgSixDOF_ControlSurfaces* aState) const
{
   return UtUnitAngle::ConvertFromStandard(aState->surfaceAngles()[aIndex], UtUnitAngle::cDEGREES);
}

/** Get SixDOF Force and Moment Data. */
double RvSixDOF_Data::PlotUpdater::GetForceMomentData(const QString& aDesc, const rv::MsgSixDOF_ForceMoment* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }
   else if (aDesc == "Lift Force")
   {
      UtUnitForce::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetForceUnit();
      return UtUnitForce::ConvertFromStandard(aState->liftForce_nt(), u);
   }
   else if (aDesc == "Drag Force")
   {
      UtUnitForce::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetForceUnit();
      return UtUnitForce::ConvertFromStandard(aState->dragForce_nt(), u);
   }
   else if (aDesc == "Thrust Force")
   {
      UtUnitForce::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetForceUnit();
      return UtUnitForce::ConvertFromStandard(aState->thrustForce_nt(), u);
   }
   else if (aDesc == "Side Force")
   {
      UtUnitForce::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetForceUnit();
      return UtUnitForce::ConvertFromStandard(aState->sideForce_nt(), u);
   }
   else if (aDesc == "Yaw Moment")
   {
      return WKF_UNIT_FROM_STANDARD(Torque, aState->yawMoment());
   }
   else if (aDesc == "Pitch Moment")
   {
      return WKF_UNIT_FROM_STANDARD(Torque, aState->pitchMoment());
   }
   else if (aDesc == "Roll Moment")
   {
      return WKF_UNIT_FROM_STANDARD(Torque, aState->rollMoment());
   }
   return 0;
}

/** Get the message type of a string, so we know what type of message to load. */
RvSixDOF_Data::MsgType RvSixDOF_Data::PlotUpdater::GetMessageType(const QString& aDesc) const
{
   // MsgSixDOF_CoreData
   if (aDesc.compare("Altitude (MSL)") == 0 || aDesc.compare("Vertical Speed (FPM)") == 0 ||
       aDesc.compare("Airspeed, Knots Calibrated (KCAS)") == 0 || aDesc.compare("Airspeed, Knots True (KTAS)") == 0 ||
       aDesc.compare("Airspeed, Mach") == 0 || aDesc.compare("Local Heading") == 0 || aDesc.compare("Local Pitch") == 0 ||
       aDesc.compare("Local Roll") == 0 || aDesc.compare("Angle of Attack, Alpha") == 0 ||
       aDesc.compare("Maneuver Gs, G-Available") == 0 || aDesc.compare("Speed Brake Surface Angle") == 0)
   {
      return MsgType::eMsgSixDOF_CoreData;
   }
   // MsgSixDOF_Kinematic
   else if (aDesc.compare("Angle of Sideslip, Beta") == 0 || aDesc.compare("Angle of Attack Rate, AlphaDot") == 0 ||
            aDesc.compare("Angle of Sideslip Rate, BetaDot") == 0 || aDesc.compare("Body Yaw Rate, r") == 0 ||
            aDesc.compare("Body Pitch Rate, q") == 0 || aDesc.compare("Body Roll Rate, p") == 0 ||
            aDesc.compare("Dynamic Pressure, Q") == 0 || aDesc.compare("Body Forward Accel (Gs), Nx") == 0 ||
            aDesc.compare("Body Right Accel (Gs), Ny") == 0 || aDesc.compare("Flight Path Angle, Gamma") == 0 ||
            aDesc.compare("Center of Gravity - X, CGx") == 0 || aDesc.compare("Center of Gravity - Y, CGy") == 0 ||
            aDesc.compare("Center of Gravity - Z, CGz") == 0)
   {
      return MsgType::eMsgSixDOF_Kinematic;
   }
   // MsgSixDOF_EngineFuel
   else if (aDesc.compare("Fuel Flow (PPH)") == 0 || aDesc.compare("Internal Fuel Capacity") == 0 ||
            aDesc.compare("External Fuel Capacity") == 0 || aDesc.compare("Internal Fuel Remaining") == 0 ||
            aDesc.compare("External Fuel Remaining") == 0 || aDesc.compare("Current Total Weight/Mass") == 0 ||
            aDesc.compare("Afterburner On") == 0 || aDesc.compare("Producing Thrust") == 0 ||
            aDesc.compare("Engine Operating") == 0 || aDesc.compare("Engine Smoking") == 0 ||
            aDesc.compare("Contrailing") == 0 || aDesc.compare("Joker Fuel State") == 0 ||
            aDesc.compare("Bingo Fuel State") == 0 || aDesc.compare("Weight on Wheels") == 0)
   {
      return MsgType::eMsgSixDOF_EngineFuel;
   }
   // MsgSixDOF_Autopilot
   else if (aDesc.compare("Lateral Mode Value") == 0 || aDesc.compare("Vertical Mode Value") == 0 ||
            aDesc.compare("Speed Mode Value") == 0)
   {
      return MsgType::eMsgSixDOF_Autopilot;
   }
   // MsgSixDOF_ControlInputs
   else if (aDesc.compare("Stick Control Back") == 0 || aDesc.compare("Stick Control Right") == 0 ||
            aDesc.compare("Rudder Control Right") == 0 || aDesc.compare("Throttle Lever (0-2)") == 0 ||
            aDesc.compare("Speed Brake Lever") == 0 || aDesc.compare("Flaps Lever") == 0 ||
            aDesc.compare("Landing Gear Lever") == 0 || aDesc.compare("Spoilers Lever") == 0 ||
            aDesc.compare("Thrust Reverser Lever") == 0)
   {
      return MsgType::eMsgSixDOF_ControlInputs;
   }
   // MsgSixDOF_ForceMoment
   else if (aDesc.compare("Lift Force") == 0 || aDesc.compare("Drag Force") == 0 ||
            aDesc.compare("Thrust Force") == 0 || aDesc.compare("Side Force") == 0 ||
            aDesc.compare("Yaw Moment") == 0 || aDesc.compare("Pitch Moment") == 0 || aDesc.compare("Roll Moment") == 0)
   {
      return MsgType::eMsgSixDOF_ForceMoment;
   }
   else // Check for control surfaces
   {
      bool found = false;
      for (const QString& surface : mControlSurfacesList)
      {
         if (aDesc.compare(surface) == 0)
         {
            found = true;
            break;
         }
      }
      if (found)
      {
         return MsgType::eMsgSixDOF_ControlSurfaces;
      }
      else
      {
         return MsgType::eMsgSixDOF_CoreData;
      }
   }
}
