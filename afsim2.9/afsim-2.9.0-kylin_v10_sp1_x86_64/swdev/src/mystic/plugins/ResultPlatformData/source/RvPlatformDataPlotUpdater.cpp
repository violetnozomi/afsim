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

#include "RvPlatformDataPlotUpdater.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "UtAirspeed.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtSphericalEarth.hpp"
#include "WkfUnitsObject.hpp"

RvPlatformData::PlotUpdater::PlotUpdater(const QString& aX, const QString& aY, const wkf::PlatformGrouping& aGroup)
   : rv::PlotUpdater()
   , mInitialXAxis(aX)
   , mInitialYAxis(aY)
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
}

QStringList RvPlatformData::PlotUpdater::GetPlatforms() const
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

QStringList RvPlatformData::PlotUpdater::GetAxes() const
{
   QStringList axisVariableList;

   axisVariableList.push_back("Time");
   axisVariableList.push_back("Lifetime");
   axisVariableList.push_back("Ground Range");
   axisVariableList.push_back("Damage Factor");
   axisVariableList.push_back("Fuel");

   axisVariableList.push_back("Latitude");
   axisVariableList.push_back("Longitude");
   axisVariableList.push_back("Altitude MSL");

   axisVariableList.push_back("Location X");
   axisVariableList.push_back("Location Y");
   axisVariableList.push_back("Location Z");

   axisVariableList.push_back("Heading");
   axisVariableList.push_back("Pitch");
   axisVariableList.push_back("Roll");

   axisVariableList.push_back("Psi");
   axisVariableList.push_back("Theta");
   axisVariableList.push_back("Phi");

   axisVariableList.push_back("Speed");
   axisVariableList.push_back("Mach");

   axisVariableList.push_back("Velocity N");
   axisVariableList.push_back("Velocity E");
   axisVariableList.push_back("Velocity D");

   axisVariableList.push_back("Velocity X");
   axisVariableList.push_back("Velocity Y");
   axisVariableList.push_back("Velocity Z");

   axisVariableList.push_back("Acceleration");

   axisVariableList.push_back("Acceleration N");
   axisVariableList.push_back("Acceleration E");
   axisVariableList.push_back("Acceleration D");

   axisVariableList.push_back("Acceleration X");
   axisVariableList.push_back("Acceleration Y");
   axisVariableList.push_back("Acceleration Z");

   return axisVariableList;
}

QString RvPlatformData::PlotUpdater::GetUnits(const QString& aAxis) const
{
   if (aAxis == "Time" || aAxis == "Lifetime")
   {
      return WKF_UNIT_NAME(Time);
   }
   else if (aAxis == "Ground Range" || aAxis.startsWith("Location"))
   {
      return WKF_UNIT_NAME(Length);
   }
   else if (aAxis.startsWith("Altitude"))
   {
      return QString::fromStdString(
         UtUnitLength::FindUnitName(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLowAltitudeUnit()));
   }
   else if (aAxis == "Fuel")
   {
      return WKF_UNIT_NAME(Mass);
   }
   else if (aAxis == "Latitude" || aAxis == "Longitude")
   {
      return "degrees";
   }
   else if (aAxis == "Speed" || aAxis.startsWith("Velocity"))
   {
      return WKF_UNIT_NAME(Speed);
   }
   else if (aAxis.startsWith("Acceleration"))
   {
      return WKF_UNIT_NAME(Acceleration);
   }
   else if (QStringList{"Heading", "Pitch", "Roll", "Psi", "Theta", "Phi"}.contains(aAxis))
   {
      return WKF_UNIT_NAME(Angle);
   }
   // Damage factor and mach are unitless.
   return {};
}

const QStringList& RvPlatformData::PlotUpdater::GetInitialSelection() const
{
   return mInitialSelection;
}

const QString& RvPlatformData::PlotUpdater::GetInitialXAxis() const
{
   return mInitialXAxis;
}

const QString& RvPlatformData::PlotUpdater::GetInitialYAxis() const
{
   return mInitialYAxis;
}

std::vector<std::pair<double, double>> RvPlatformData::PlotUpdater::GetSeries(const QString& aItem,
                                                                              const QString& aXAxis,
                                                                              const QString& aYAxis) const
{
   std::vector<std::pair<double, double>> series;

   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         std::string name = aItem.toStdString();

         rv::ResultPlatform* platformPtr = db->FindPlatformByName(name);
         if (platformPtr)
         {
            auto stateArray = platformPtr->GetArray<rv::MsgEntityState>();
            if (stateArray != nullptr && !stateArray->empty())
            {
               auto startMessage = dynamic_cast<const rv::MsgEntityState*>(stateArray->front());
               for (auto* stateMsg : *stateArray)
               {
                  if ((stateMsg->simTime() - GetLastUpdate()) > GetSampleRate())
                  {
                     double x = GetData(aXAxis, dynamic_cast<rv::MsgEntityState*>(stateMsg), startMessage);
                     double y = GetData(aYAxis, dynamic_cast<rv::MsgEntityState*>(stateMsg), startMessage);
                     series.emplace_back(x, y);

                     mLastUpdate = stateMsg->simTime();
                  }
               }
            }
         }
      }
   }
   return series;
}

double RvPlatformData::PlotUpdater::GetData(const QString&            aDesc,
                                            const rv::MsgEntityState* aState,
                                            const rv::MsgEntityState* aStartMessage) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }
   else if (aDesc == "Lifetime")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime() - aStartMessage->simTime(), u);
   }
   else if (aDesc == "Ground Range")
   {
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();

      rv::Vec3d l1      = aState->state().locationWCS();
      double    xyz1[3] = {l1.x(), l1.y(), l1.z()};
      double    lla1[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz1, lla1[0], lla1[1], lla1[2]);

      rv::Vec3d l2      = aStartMessage->state().locationWCS();
      double    xyz2[3] = {l2.x(), l2.y(), l2.z()};
      double    lla2[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz2, lla2[0], lla2[1], lla2[2]);
      double h;
      double d;
      UtSphericalEarth::GreatCircleHeadingAndDistance(lla1[0], lla1[1], lla2[0], lla2[1], h, d);
      return UtUnitLength::ConvertFromStandard(d, u);
   }
   else if (aDesc == "Damage Factor")
   {
      return aState->state().damageFactor();
   }
   else if (aDesc == "Latitude")
   {
      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      double    lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
      return lla[0]; // degrees, no other units available...
   }
   else if (aDesc == "Longitude")
   {
      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      double    lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
      return lla[1]; // degrees, no other units available
   }
   else if (aDesc == "Altitude MSL")
   {
      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      double    lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAltitudeUnit(0);
      return UtUnitLength::ConvertFromStandard(lla[2], u);
   }
   else if (aDesc == "Location X")
   {
      rv::Vec3d              l = aState->state().locationWCS();
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(l.x(), u);
   }
   else if (aDesc == "Location Y")
   {
      rv::Vec3d              l = aState->state().locationWCS();
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(l.y(), u);
   }
   else if (aDesc == "Location Z")
   {
      rv::Vec3d              l = aState->state().locationWCS();
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit();
      return UtUnitLength::ConvertFromStandard(l.z(), u);
   }
   else if (aDesc == "Heading")
   {
      UtEntity tmpEntity;

      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      tmpEntity.SetLocationWCS(xyz);
      tmpEntity.SetOrientationWCS(aState->state().orientationWCS().x(),
                                  aState->state().orientationWCS().y(),
                                  aState->state().orientationWCS().z());
      double ypw[3];
      tmpEntity.GetOrientationNED(ypw[0], ypw[1], ypw[2]);
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(ypw[0], u);
   }
   else if (aDesc == "Pitch")
   {
      UtEntity tmpEntity;

      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      tmpEntity.SetLocationWCS(xyz);
      tmpEntity.SetOrientationWCS(aState->state().orientationWCS().x(),
                                  aState->state().orientationWCS().y(),
                                  aState->state().orientationWCS().z());
      double ypw[3];
      tmpEntity.GetOrientationNED(ypw[0], ypw[1], ypw[2]);
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(ypw[1], u);
   }
   else if (aDesc == "Roll")
   {
      UtEntity tmpEntity;

      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      tmpEntity.SetLocationWCS(xyz);
      tmpEntity.SetOrientationWCS(aState->state().orientationWCS().x(),
                                  aState->state().orientationWCS().y(),
                                  aState->state().orientationWCS().z());
      double ypw[3];
      tmpEntity.GetOrientationNED(ypw[0], ypw[1], ypw[2]);
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(ypw[2], u);
   }
   else if (aDesc == "Psi")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(aState->state().orientationWCS().x(), u);
   }
   else if (aDesc == "Theta")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(aState->state().orientationWCS().y(), u);
   }
   else if (aDesc == "Phi")
   {
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(aState->state().orientationWCS().z(), u);
   }
   else if (aDesc == "Speed")
   {
      rv::Vec3f             v = aState->state().velocityWCS();
      UtUnitSpeed::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit();
      return UtUnitSpeed::ConvertFromStandard(sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z()), u);
   }
   else if (aDesc == "Mach")
   {
      float retval = 0;
      if (aState->state().machNumberValid())
      {
         retval = aState->state().machNumber();
      }

      return retval;
   }
   else if (aDesc == "Velocity N")
   {
      UtEntity tmpEntity;

      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      tmpEntity.SetLocationWCS(xyz);
      double vxyz[3] = {aState->state().velocityWCS().x(),
                        aState->state().velocityWCS().y(),
                        aState->state().velocityWCS().z()};
      tmpEntity.SetVelocityWCS(vxyz);
      double ned[3];
      tmpEntity.GetVelocityNED(ned);
      UtUnitSpeed::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit();
      return UtUnitSpeed::ConvertFromStandard(ned[0], u);
   }
   else if (aDesc == "Velocity E")
   {
      UtEntity tmpEntity;

      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      tmpEntity.SetLocationWCS(xyz);
      double vxyz[3] = {aState->state().velocityWCS().x(),
                        aState->state().velocityWCS().y(),
                        aState->state().velocityWCS().z()};
      tmpEntity.SetVelocityWCS(vxyz);
      double ned[3];
      tmpEntity.GetVelocityNED(ned);
      UtUnitSpeed::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit();
      return UtUnitSpeed::ConvertFromStandard(ned[1], u);
   }
   else if (aDesc == "Velocity D")
   {
      UtEntity tmpEntity;

      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      tmpEntity.SetLocationWCS(xyz);
      double vxyz[3] = {aState->state().velocityWCS().x(),
                        aState->state().velocityWCS().y(),
                        aState->state().velocityWCS().z()};
      tmpEntity.SetVelocityWCS(vxyz);
      double ned[3];
      tmpEntity.GetVelocityNED(ned);
      UtUnitSpeed::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit();
      return UtUnitSpeed::ConvertFromStandard(ned[2], u);
   }
   else if (aDesc == "Velocity X")
   {
      UtUnitSpeed::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit();
      return UtUnitSpeed::ConvertFromStandard(aState->state().velocityWCS().x(), u);
   }
   else if (aDesc == "Velocity Y")
   {
      UtUnitSpeed::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit();
      return UtUnitSpeed::ConvertFromStandard(aState->state().velocityWCS().y(), u);
   }
   else if (aDesc == "Velocity Z")
   {
      UtUnitSpeed::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit();
      return UtUnitSpeed::ConvertFromStandard(aState->state().velocityWCS().z(), u);
   }
   else if (aDesc == "Acceleration")
   {
      rv::Vec3f                    v = aState->state().accelerationWCS();
      UtUnitAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAccelerationUnit();
      return UtUnitAcceleration::ConvertFromStandard(sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z()), u);
   }
   else if (aDesc == "Acceleration N")
   {
      UtEntity tmpEntity;

      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      tmpEntity.SetLocationWCS(xyz);
      double vxyz[3] = {aState->state().accelerationWCS().x(),
                        aState->state().accelerationWCS().y(),
                        aState->state().accelerationWCS().z()};
      tmpEntity.SetAccelerationWCS(vxyz);
      double ned[3];
      tmpEntity.GetAccelerationNED(ned);
      UtUnitAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAccelerationUnit();
      return UtUnitAcceleration::ConvertFromStandard(ned[0], u);
   }
   else if (aDesc == "Acceleration E")
   {
      UtEntity tmpEntity;

      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      tmpEntity.SetLocationWCS(xyz);
      double vxyz[3] = {aState->state().accelerationWCS().x(),
                        aState->state().accelerationWCS().y(),
                        aState->state().accelerationWCS().z()};
      tmpEntity.SetAccelerationWCS(vxyz);
      double ned[3];
      tmpEntity.GetAccelerationNED(ned);
      UtUnitAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAccelerationUnit();
      return UtUnitAcceleration::ConvertFromStandard(ned[1], u);
   }
   else if (aDesc == "Acceleration D")
   {
      UtEntity tmpEntity;

      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      tmpEntity.SetLocationWCS(xyz);
      double vxyz[3] = {aState->state().accelerationWCS().x(),
                        aState->state().accelerationWCS().y(),
                        aState->state().accelerationWCS().z()};
      tmpEntity.SetAccelerationWCS(vxyz);
      double ned[3];
      tmpEntity.GetAccelerationNED(ned);
      UtUnitAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAccelerationUnit();
      return UtUnitAcceleration::ConvertFromStandard(ned[2], u);
   }
   else if (aDesc == "Acceleration X")
   {
      UtUnitAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAccelerationUnit();
      return UtUnitAcceleration::ConvertFromStandard(aState->state().accelerationWCS().x(), u);
   }
   else if (aDesc == "Acceleration Y")
   {
      UtUnitAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAccelerationUnit();
      return UtUnitAcceleration::ConvertFromStandard(aState->state().accelerationWCS().y(), u);
   }
   else if (aDesc == "Acceleration Z")
   {
      UtUnitAcceleration::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAccelerationUnit();
      return UtUnitAcceleration::ConvertFromStandard(aState->state().accelerationWCS().z(), u);
   }
   return 0;
}
