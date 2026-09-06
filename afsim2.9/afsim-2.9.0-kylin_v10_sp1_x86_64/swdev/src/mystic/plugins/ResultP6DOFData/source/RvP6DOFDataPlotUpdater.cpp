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

#include "RvP6DOFDataPlotUpdater.hpp"

#include "RvEnvironment.hpp"
#include "RvP6dofEventPipeClasses.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "UtCast.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtSphericalEarth.hpp"
#include "WkfUnitsObject.hpp"

RvP6DOFData::PlotUpdater::PlotUpdater(const QString& aX, const QString& aY, const wkf::PlatformGrouping& aGroup)
   : rv::PlotUpdater()
   , mInitialXAxis(aX)
   , mInitialYAxis(aY)
   , mControlSurfacesList()
{
   if (aGroup.Type() == wkf::PlatformGrouping::eIndividual)
   {
      mInitialSelection.push_back(aGroup.Name().c_str());
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
                  mInitialSelection.push_back(dbPtr->GetPlatformName(jt).c_str());
               }
            }
         }
      }
   }

   // Get the control surface names
   rv::ResultData*     dataPtr        = rvEnv.GetData();
   rv::ResultPlatform* platPtr        = dataPtr->FindPlatform(dataPtr->FindPlatformIndex(aGroup.Name()));
   auto                msgSurfacesPtr = platPtr->FindFirstAfter<rv::MsgP6dofControlSurfaces>(0.0);
   if (msgSurfacesPtr != nullptr)
   {
      for (const std::string& surface : msgSurfacesPtr->surfaceNames())
      {
         mControlSurfacesList.append(QString::fromStdString(surface));
      }
   }
}

QStringList RvP6DOFData::PlotUpdater::GetPlatforms() const
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
            list.push_back(dbPtr->GetPlatformName(i).c_str());
         }
      }
   }
   return list;
}

/** Helper function to get the available axes. */
QStringList RvP6DOFData::PlotUpdater::GetAxes() const
{
   QStringList axisVariableList;
   axisVariableList.push_back("Time");

   // MsgP6dofCoreData
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

   // MsgP6dofKinematic
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

   // MsgP6dofEngineFuel
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

   // MsgP6dofAutopilot
   axisVariableList.push_back("Lateral Mode Value");
   axisVariableList.push_back("Vertical Mode Value");
   axisVariableList.push_back("Speed Mode Value");

   // MsgP6dofControlInputs
   axisVariableList.push_back("Stick Control Back");
   axisVariableList.push_back("Stick Control Right");
   axisVariableList.push_back("Rudder Control Right");
   axisVariableList.push_back("Throttle Lever (0-2)");
   axisVariableList.push_back("Speed Brake Lever");
   axisVariableList.push_back("Flaps Lever");
   axisVariableList.push_back("Landing Gear Lever");
   axisVariableList.push_back("Spoilers Lever");
   axisVariableList.push_back("Thrust Reverser Lever");

   // MsgP6dofControlSurfaces
   axisVariableList.append(mControlSurfacesList);

   // MsgP6dofForceMoment
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
const QStringList& RvP6DOFData::PlotUpdater::GetInitialSelection() const
{
   return mInitialSelection;
}

/** Get the initial X axis. */
const QString& RvP6DOFData::PlotUpdater::GetInitialXAxis() const
{
   return mInitialXAxis;
}

/** Get the initial Y axis. */
const QString& RvP6DOFData::PlotUpdater::GetInitialYAxis() const
{
   return mInitialYAxis;
}

/** Get a series(plot) of X vs. Y. */
std::vector<std::pair<double, double>> RvP6DOFData::PlotUpdater::GetSeries(const QString& aItem,
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
            case MsgType::eMsgP6dofCoreData:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofCoreData>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetCoreData(aXAxis, dynamic_cast<rv::MsgP6dofCoreData*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgP6dofKinematic:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofKinematic>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetKinematicData(aYAxis, dynamic_cast<rv::MsgP6dofKinematic*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgP6dofEngineFuel:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofEngineFuel>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetEngineFuelData(aYAxis, dynamic_cast<rv::MsgP6dofEngineFuel*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgP6dofAutopilot:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofAutopilot>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetAutopilotData(aYAxis, dynamic_cast<rv::MsgP6dofAutopilot*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgP6dofControlInputs:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofControlInputs>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetControlInputsData(aYAxis, dynamic_cast<rv::MsgP6dofControlInputs*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgP6dofForceMoment:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofForceMoment>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double x = GetForceMomentData(aYAxis, dynamic_cast<rv::MsgP6dofForceMoment*>(kt));
                        xVals.emplace_back(x);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
               break;
            }
            case MsgType::eMsgP6dofControlSurfaces:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofControlSurfaces>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        // Get the index of the control surface string
                        rv::MsgP6dofControlSurfaces* msg         = dynamic_cast<rv::MsgP6dofControlSurfaces*>(kt);
                        size_t                       indexOfName = ut::npos;
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
            case MsgType::eMsgP6dofCoreData:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofCoreData>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetCoreData(aYAxis, dynamic_cast<rv::MsgP6dofCoreData*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgP6dofKinematic:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofKinematic>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetKinematicData(aYAxis, dynamic_cast<rv::MsgP6dofKinematic*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgP6dofEngineFuel:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofEngineFuel>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetEngineFuelData(aYAxis, dynamic_cast<rv::MsgP6dofEngineFuel*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgP6dofAutopilot:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofAutopilot>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetAutopilotData(aYAxis, dynamic_cast<rv::MsgP6dofAutopilot*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgP6dofControlInputs:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofControlInputs>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetControlInputsData(aYAxis, dynamic_cast<rv::MsgP6dofControlInputs*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
            }
            break;
            case MsgType::eMsgP6dofForceMoment:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofForceMoment>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        double y = GetForceMomentData(aYAxis, dynamic_cast<rv::MsgP6dofForceMoment*>(kt));
                        yVals.emplace_back(y);
                        mLastUpdate = kt->simTime();
                     }
                  }
               }
               break;
            }
            case MsgType::eMsgP6dofControlSurfaces:
            {
               const auto dataArray = platformPtr->GetArray<rv::MsgP6dofControlSurfaces>();
               if (dataArray != nullptr)
               {
                  for (auto&& kt : *dataArray)
                  {
                     if ((kt->simTime() - GetLastUpdate()) > GetSampleRate())
                     {
                        // Get the index of the control surface string
                        rv::MsgP6dofControlSurfaces* msg         = dynamic_cast<rv::MsgP6dofControlSurfaces*>(kt);
                        size_t                       indexOfName = ut::npos;
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
            default:
               break;
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

/** Get P6dof Core Data. */
double RvP6DOFData::PlotUpdater::GetCoreData(const QString& aDesc, const rv::MsgP6dofCoreData* aState) const
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

/** Get P6dof Kinematic Data. */
double RvP6DOFData::PlotUpdater::GetKinematicData(const QString& aDesc, const rv::MsgP6dofKinematic* aState) const
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
      UtUnitAngularAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngularAccelerationUnit();
      return UtUnitAngularAcceleration::ConvertFromStandard(aState->alphaDot_rps(), u);
   }
   else if (aDesc == "Angle of Sideslip Rate, BetaDot")
   {
      UtUnitAngularAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngularAccelerationUnit();
      return UtUnitAngularAcceleration::ConvertFromStandard(aState->betaDot_rps(), u);
   }
   else if (aDesc == "Body Yaw Rate, r")
   {
      UtUnitAngularAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngularAccelerationUnit();
      return UtUnitAngularAcceleration::ConvertFromStandard(aState->yawRate_rps(), u);
   }
   else if (aDesc == "Body Pitch Rate, q")
   {
      UtUnitAngularAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngularAccelerationUnit();
      return UtUnitAngularAcceleration::ConvertFromStandard(aState->pitchRate_rps(), u);
   }
   else if (aDesc == "Body Roll Rate, p")
   {
      UtUnitAngularAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngularAccelerationUnit();
      return UtUnitAngularAcceleration::ConvertFromStandard(aState->rollRate_rps(), u);
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

/** Get P6dof Engine and Fuel Data. */
double RvP6DOFData::PlotUpdater::GetEngineFuelData(const QString& aDesc, const rv::MsgP6dofEngineFuel* aState) const
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

/** Get P6dof Autopilot Data. */
double RvP6DOFData::PlotUpdater::GetAutopilotData(const QString& aDesc, const rv::MsgP6dofAutopilot* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }
   else if (aDesc == "Lateral Mode Value")
   {
      return UtUnitMass::ConvertFromStandard(aState->lateralModeValue(), 0);
   }
   else if (aDesc == "Vertical Mode Value")
   {
      return UtUnitMass::ConvertFromStandard(aState->verticalModeValue(), 0);
   }
   else if (aDesc == "Speed Mode Value")
   {
      return UtUnitMass::ConvertFromStandard(aState->speedModeValue(), 0);
   }
   return 0;
}

/** Get P6dof Autopilot Limits Data. */
double RvP6DOFData::PlotUpdater::GetControlInputsData(const QString& aDesc, const rv::MsgP6dofControlInputs* aState) const
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

double RvP6DOFData::PlotUpdater::GetControlSurfacesData(const size_t aIndex, const rv::MsgP6dofControlSurfaces* aState) const
{
   return UtUnitAngle::ConvertFromStandard(aState->surfaceAngles()[aIndex], UtUnitAngle::cDEGREES);
}

/** Get P6dof Force Moment Data. */
double RvP6DOFData::PlotUpdater::GetForceMomentData(const QString& aDesc, const rv::MsgP6dofForceMoment* aState) const
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
      return UtUnitAngularAcceleration::ConvertFromStandard(aState->yawMoment(), 0);
   }
   else if (aDesc == "Pitch Moment")
   {
      return UtUnitAngularAcceleration::ConvertFromStandard(aState->pitchMoment(), 0);
   }
   else if (aDesc == "Roll Moment")
   {
      return UtUnitAngularAcceleration::ConvertFromStandard(aState->rollMoment(), 0);
   }
   return 0;
}

/** Get the message type of a string, so we know what type of message to load. */
RvP6DOFData::MsgType RvP6DOFData::PlotUpdater::GetMessageType(const QString& aDesc) const
{
   // MsgP6dofCoreData
   if (aDesc.compare("Altitude (MSL)") == 0 || aDesc.compare("Vertical Speed (FPM)") == 0 ||
       aDesc.compare("Airspeed, Knots Calibrated (KCAS)") == 0 || aDesc.compare("Airspeed, Knots True (KTAS)") == 0 ||
       aDesc.compare("Airspeed, Mach") == 0 || aDesc.compare("Local Heading") == 0 || aDesc.compare("Local Pitch") == 0 ||
       aDesc.compare("Local Roll") == 0 || aDesc.compare("Angle of Attack, Alpha") == 0 ||
       aDesc.compare("Maneuver Gs, G-Available") == 0 || aDesc.compare("Speed Brake Surface Angle") == 0)
   {
      return MsgType::eMsgP6dofCoreData;
   }
   // MsgP6dofKinematic
   else if (aDesc.compare("Angle of Sideslip, Beta") == 0 || aDesc.compare("Angle of Attack Rate, AlphaDot") == 0 ||
            aDesc.compare("Angle of Sideslip Rate, BetaDot") == 0 || aDesc.compare("Body Yaw Rate, r") == 0 ||
            aDesc.compare("Body Pitch Rate, q") == 0 || aDesc.compare("Body Roll Rate, p") == 0 ||
            aDesc.compare("Dynamic Pressure, Q") == 0 || aDesc.compare("Body Forward Accel (Gs), Nx") == 0 ||
            aDesc.compare("Body Right Accel (Gs), Ny") == 0 || aDesc.compare("Flight Path Angle, Gamma") == 0 ||
            aDesc.compare("Center of Gravity - X, CGx") == 0 || aDesc.compare("Center of Gravity - Y, CGy") == 0 ||
            aDesc.compare("Center of Gravity - Z, CGz") == 0)
   {
      return MsgType::eMsgP6dofKinematic;
   }
   // MsgP6dofEngineFuel
   else if (aDesc.compare("Fuel Flow (PPH)") == 0 || aDesc.compare("Internal Fuel Capacity") == 0 ||
            aDesc.compare("External Fuel Capacity") == 0 || aDesc.compare("Internal Fuel Remaining") == 0 ||
            aDesc.compare("External Fuel Remaining") == 0 || aDesc.compare("Current Total Weight/Mass") == 0 ||
            aDesc.compare("Afterburner On") == 0 || aDesc.compare("Producing Thrust") == 0 ||
            aDesc.compare("Engine Operating") == 0 || aDesc.compare("Engine Smoking") == 0 ||
            aDesc.compare("Contrailing") == 0 || aDesc.compare("Joker Fuel State") == 0 ||
            aDesc.compare("Bingo Fuel State") == 0 || aDesc.compare("Weight on Wheels") == 0)
   {
      return MsgType::eMsgP6dofEngineFuel;
   }
   // MsgP6dofAutopilot
   else if (aDesc.compare("Lateral Mode Value") == 0 || aDesc.compare("Vertical Mode Value") == 0 ||
            aDesc.compare("Speed Mode Value") == 0)
   {
      return MsgType::eMsgP6dofAutopilot;
   }
   // MsgP6dofControlInputs
   else if (aDesc.compare("Stick Control Back") == 0 || aDesc.compare("Stick Control Right") == 0 ||
            aDesc.compare("Rudder Control Right") == 0 || aDesc.compare("Throttle Lever (0-2)") == 0 ||
            aDesc.compare("Speed Brake Lever") == 0 || aDesc.compare("Flaps Lever") == 0 ||
            aDesc.compare("Landing Gear Lever") == 0 || aDesc.compare("Spoilers Lever") == 0 ||
            aDesc.compare("Thrust Reverser Lever") == 0)
   {
      return MsgType::eMsgP6dofControlInputs;
   }
   // MsgP6dofForceMoment
   else if (aDesc.compare("Lift Force") == 0 || aDesc.compare("Drag Force") == 0 ||
            aDesc.compare("Thrust Force") == 0 || aDesc.compare("Side Force") == 0 ||
            aDesc.compare("Yaw Moment") == 0 || aDesc.compare("Pitch Moment") == 0 || aDesc.compare("Roll Moment") == 0)
   {
      return MsgType::eMsgP6dofForceMoment;
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
         return MsgType::eMsgP6dofControlSurfaces;
      }
      else
      {
         return MsgType::eMsgP6dofCoreData;
      }
   }
}
