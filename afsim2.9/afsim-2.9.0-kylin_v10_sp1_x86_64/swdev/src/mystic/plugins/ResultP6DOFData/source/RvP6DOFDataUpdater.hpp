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
#ifndef RVP6DOFDATAUPDATERS_HPP
#define RVP6DOFDATAUPDATERS_HPP

#include "RvPlatformUpdater.hpp"

namespace rv
{
class ResultPlatform;
}

namespace RvP6DOFData
{
class AltitudeUpdater : public rv::PlatformAltitudeUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Altitude";

   explicit AltitudeUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAltitudeUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class VerticalSpeedUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Vertical Speed (FPM)";

   explicit VerticalSpeedUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class KCAS_Updater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|KCAS";

   explicit KCAS_Updater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class KTAS_Updater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|KTAS";

   explicit KTAS_Updater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class MachUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Mach";

   explicit MachUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class DynamicPressureUpdater : public rv::PlatformPressureUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Q";

   explicit DynamicPressureUpdater(const std::string& aPlatformName = "")
      : rv::PlatformPressureUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class HeadingUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Heading";

   explicit HeadingUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class PitchUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Pitch";

   explicit PitchUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class RollUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Roll";

   explicit RollUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class NxUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Nx";

   explicit NxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class NyUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Ny";

   explicit NyUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class GLoadUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|G-Load";

   explicit GLoadUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AvailGsUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Avail Gs";

   explicit AvailGsUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AlphaUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Alpha";

   explicit AlphaUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class BetaUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Beta";

   explicit BetaUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AlphaDotUpdater : public rv::PlatformAngularRateUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Alpha-Dot";

   explicit AlphaDotUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngularRateUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class BetaDotUpdater : public rv::PlatformAngularRateUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Beta-Dot";

   explicit BetaDotUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngularRateUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class YawRateUpdater : public rv::PlatformAngularRateUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Yaw Rate";

   explicit YawRateUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngularRateUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class PitchRateUpdater : public rv::PlatformAngularRateUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Pitch Rate";

   explicit PitchRateUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngularRateUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class RollRateUpdater : public rv::PlatformAngularRateUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Roll Rate";

   explicit RollRateUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngularRateUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class ActivePilotUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Active Pilot";

   explicit ActivePilotUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
      , mPilotType("Unknown")
   {
   }

protected:
   /** Get the value as a string (mPilotType). */
   QString GetValueString() const override;

   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;

private:
   QString mPilotType;
};

class LiftUpdater : public rv::PlatformForceUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Lift";

   explicit LiftUpdater(const std::string& aPlatformName = "")
      : rv::PlatformForceUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class DragUpdater : public rv::PlatformForceUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Drag";

   explicit DragUpdater(const std::string& aPlatformName = "")
      : rv::PlatformForceUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class ThrustUpdater : public rv::PlatformForceUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Thrust";

   explicit ThrustUpdater(const std::string& aPlatformName = "")
      : rv::PlatformForceUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class FuelFlowUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Fuel Flow (PPH)";

   explicit FuelFlowUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class FuelInternalUpdater : public rv::PlatformMassUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Fuel Internal";

   explicit FuelInternalUpdater(const std::string& aPlatformName = "")
      : rv::PlatformMassUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class FuelInternalRemainingUpdater : public rv::PlatformMassUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Internal Fuel Remaining";

   explicit FuelInternalRemainingUpdater(const std::string& aPlatformName = "")
      : rv::PlatformMassUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class FuelExternalUpdater : public rv::PlatformMassUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Fuel External";

   explicit FuelExternalUpdater(const std::string& aPlatformName = "")
      : rv::PlatformMassUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class FuelExternalRemainingUpdater : public rv::PlatformMassUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|External Fuel Remaining";

   explicit FuelExternalRemainingUpdater(const std::string& aPlatformName = "")
      : rv::PlatformMassUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class StickBackUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Stick Back";

   explicit StickBackUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class StickRightUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Stick Right";

   explicit StickRightUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class RudderUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Rudder";

   explicit RudderUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class ThrottleUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Throttle";

   explicit ThrottleUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AfterburnerUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Afterburner";

   explicit AfterburnerUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class SpeedBrakeAngleUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Speedbrake Lever Angle";

   explicit SpeedBrakeAngleUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class SpeedBrakeLeverUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Speedbrake Lever";

   explicit SpeedBrakeLeverUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class WeightUpdater : public rv::PlatformMassUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Total Mass";

   explicit WeightUpdater(const std::string& aPlatformName = "")
      : rv::PlatformMassUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class WeightOnWheelsUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Weight on Wheels";

   explicit WeightOnWheelsUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class FlightPathAngleUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Flight Path Angle";

   explicit FlightPathAngleUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class CenterGravityXUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Center of Gravity - X";

   explicit CenterGravityXUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class CenterGravityYUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Center of Gravity - Y";

   explicit CenterGravityYUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class CenterGravityZUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Center of Gravity - Z";

   explicit CenterGravityZUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class ProducingThrustUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Producing Thrust";

   explicit ProducingThrustUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class EngineOperatingUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Engine Operating";

   explicit EngineOperatingUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class EngineSmokingUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Engine Smoking";

   explicit EngineSmokingUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class ContrailingUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Contrailing";

   explicit ContrailingUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class JokerFuelStateUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Joker Fuel State";

   explicit JokerFuelStateUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class BingoFuelStateUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Bingo Fuel State";

   explicit BingoFuelStateUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class LateralModeNameUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Lateral Mode Name";

   explicit LateralModeNameUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
      , mLateralModeName("Unknown")
   {
   }

protected:
   /** Get the value as a string (mLateralModeName). */
   QString GetValueString() const override;

   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;

private:
   QString mLateralModeName;
};

class LateralModeValueUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Lateral Mode Value";

   explicit LateralModeValueUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class VerticalModeNameUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Vertical Mode Name";

   explicit VerticalModeNameUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
      , mVerticalModeName("Unknown")
   {
   }

protected:
   /** Get the value as a string (mVerticalModeName). */
   QString GetValueString() const override;

   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;

private:
   QString mVerticalModeName;
};

class VerticalModeValueUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Vertical Mode Value";

   explicit VerticalModeValueUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class SpeedModeNameUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Speed Mode Name";

   explicit SpeedModeNameUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
      , mSpeedModeName("Unknown")
   {
   }

protected:
   /** Get the value as a string (mSpeedModeName). */
   QString GetValueString() const override;

   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;

private:
   QString mSpeedModeName;
};

class SpeedModeValueUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Speed Mode Value";

   explicit SpeedModeValueUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class CurrentManeuverUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Current Maneuver";

   explicit CurrentManeuverUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Get the value as a string (mCurrentManeuver). */
   QString GetValueString() const override;

   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;

private:
   QString mCurrentManeuver;
};

class PitchGLoadMinUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|G-Load Min";

   explicit PitchGLoadMinUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class PitchGLoadMaxUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|G-Load Max";

   explicit PitchGLoadMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AlphaMinUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Alpha Min";

   explicit AlphaMinUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AlphaMaxUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Alpha Max";

   explicit AlphaMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class PitchRateMinUpdater : public rv::PlatformAngularRateUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Pitch Rate Min";

   explicit PitchRateMinUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngularRateUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class PitchRateMaxUpdater : public rv::PlatformAngularRateUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Pitch Rate Max";

   explicit PitchRateMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngularRateUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class VerticalSpeedMinUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Vertical Speed Min (FPM)";

   explicit VerticalSpeedMinUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class VerticalSpeedMaxUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Vertical Speed Max (FPM)";

   explicit VerticalSpeedMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class YawGLoadMaxUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Yaw G-Load Max";

   explicit YawGLoadMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class BetaMaxUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Beta Max";

   explicit BetaMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class YawRateMaxUpdater : public rv::PlatformAngularRateUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Yaw Rate Max";

   explicit YawRateMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngularRateUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class RollRateMaxUpdater : public rv::PlatformAngularRateUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Roll Rate Max";

   explicit RollRateMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngularRateUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class BankAngleMaxUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Bank Angle Max";

   explicit BankAngleMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class ForwardAccelMinUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Forward Accel Min (Gs)";

   explicit ForwardAccelMinUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class ForwardAccelMaxUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Forward Accel Max (Gs)";

   explicit ForwardAccelMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class TaxiSpeedMaxUpdater : public rv::PlatformSpeedUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Taxi Speed Max";

   explicit TaxiSpeedMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformSpeedUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class TaxiYawRateMaxUpdater : public rv::PlatformAngularRateUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Taxi Yaw Rate Max";

   explicit TaxiYawRateMaxUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngularRateUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class FlapsLeverUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Flaps Lever";

   explicit FlapsLeverUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class LandingGearLeverUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Landing Gear Lever";

   explicit LandingGearLeverUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class SpoilersLeverUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Spoilers Lever";

   explicit SpoilersLeverUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class ThrustReverserLeverUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Thrust Reverser Lever";

   explicit ThrustReverserLeverUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class SideForceUpdater : public rv::PlatformForceUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Side Force";

   explicit SideForceUpdater(const std::string& aPlatformName = "")
      : rv::PlatformForceUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class YawMomentUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Yaw Moment";

   explicit YawMomentUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class PitchMomentUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Pitch Moment";

   explicit PitchMomentUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class RollMomentUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RvP6DOFData|Roll Moment";

   explicit RollMomentUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   /** Read data from a platform at a sim time.
    * @param aPlatform platform to read data for.
    * @param aSimTime sim time to read at.
    */
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};
} // namespace RvP6DOFData
#endif
