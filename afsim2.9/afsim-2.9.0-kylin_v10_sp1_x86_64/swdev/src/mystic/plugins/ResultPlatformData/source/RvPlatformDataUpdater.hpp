// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVPLATFORMDATAUPDATERS_HPP
#define RVPLATFORMDATAUPDATERS_HPP

#include "RvPlatformUpdater.hpp"

namespace rv
{
class ResultPlatform;
}

namespace RvPlatformData
{
class AltitudeUpdater : public rv::PlatformAltitudeUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Altitude";

   AltitudeUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAltitudeUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class HeadingUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Heading";

   HeadingUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   double GetValueInDefaultUnits() const override;
   bool   ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class LatitudeUpdater : public rv::PlatformLatitudeUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Latitude";
   LatitudeUpdater(const std::string& aPlatformName = "")
      : rv::PlatformLatitudeUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class LongitudeUpdater : public rv::PlatformLongitudeUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Longitude";

   LongitudeUpdater(const std::string& aPlatformName = "")
      : rv::PlatformLongitudeUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class PitchUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Pitch";

   PitchUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class RollUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Roll";

   RollUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class SpeedUpdater : public rv::PlatformSpeedUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Speed";

   SpeedUpdater(const std::string& aPlatformName = "")
      : rv::PlatformSpeedUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class PlatformTypeUpdater : public rv::PlatformUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Type";

   PlatformTypeUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUpdater(cUPDATER_NAME, aPlatformName)
      , mPlatformType()
   {
   }

   QString GetValueString() const override;

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;

   std::string mPlatformType;
};

class PlatformSideUpdater : public rv::PlatformUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Side";

   PlatformSideUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUpdater(cUPDATER_NAME, aPlatformName)
      , mSide()
   {
   }

   QString GetValueString() const override;

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;

   std::string mSide;
};

class PlatformIconUpdater : public rv::PlatformUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Icon";

   PlatformIconUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUpdater(cUPDATER_NAME, aPlatformName)
      , mIcon()
   {
   }

   QString GetValueString() const override;

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;

   std::string mIcon;
};

class PlatformDomainUpdater : public rv::PlatformUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Spatial Domain";

   PlatformDomainUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUpdater(cUPDATER_NAME, aPlatformName)
      , mDomain()
   {
   }

   QString GetValueString() const override;

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;

   std::string mDomain;
};

class PlatformStatusUpdater : public rv::PlatformUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Status";

   PlatformStatusUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUpdater(cUPDATER_NAME, aPlatformName)
      , mStatus()
   {
   }

   QString GetValueString() const override;

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;

   std::string mStatus;
};

class StartTimeUpdater : public rv::PlatformTimeUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Start Time";

   StartTimeUpdater(const std::string& aPlatformName = "")
      : rv::PlatformTimeUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class EndTimeUpdater : public rv::PlatformTimeUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "End Time";

   EndTimeUpdater(const std::string& aPlatformName = "")
      : rv::PlatformTimeUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class DamageFactorUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Damage Factor";

   DamageFactorUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class FuelUpdater : public rv::PlatformMassUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Fuel";

   FuelUpdater(const std::string& aPlatformName = "")
      : rv::PlatformMassUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class PositionUpdater : public rv::PlatformUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Position";

   PositionUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUpdater(cUPDATER_NAME, aPlatformName)
      , mLatitude_deg(0.0)
      , mLongitude_deg(0.0)
   {
   }

   QString GetValueString() const override;

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;

   double mLatitude_deg;
   double mLongitude_deg;
};

class LocationX_Updater : public rv::PlatformLengthUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Location X";

   LocationX_Updater(const std::string& aPlatformName = "")
      : rv::PlatformLengthUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class LocationY_Updater : public rv::PlatformLengthUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Location Y";

   LocationY_Updater(const std::string& aPlatformName = "")
      : rv::PlatformLengthUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class LocationZ_Updater : public rv::PlatformLengthUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Location Z";

   LocationZ_Updater(const std::string& aPlatformName = "")
      : rv::PlatformLengthUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class PsiUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Psi";

   PsiUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class ThetaUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Theta";

   ThetaUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class PhiUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Phi";

   PhiUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class VelocityN_Updater : public rv::PlatformSpeedUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Velocity N";

   VelocityN_Updater(const std::string& aPlatformName = "")
      : rv::PlatformSpeedUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class VelocityE_Updater : public rv::PlatformSpeedUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Velocity E";

   VelocityE_Updater(const std::string& aPlatformName = "")
      : rv::PlatformSpeedUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class VelocityD_Updater : public rv::PlatformSpeedUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Velocity D";

   VelocityD_Updater(const std::string& aPlatformName = "")
      : rv::PlatformSpeedUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class VelocityX_Updater : public rv::PlatformSpeedUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Velocity X";

   VelocityX_Updater(const std::string& aPlatformName = "")
      : rv::PlatformSpeedUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class VelocityY_Updater : public rv::PlatformSpeedUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Velocity Y";

   VelocityY_Updater(const std::string& aPlatformName = "")
      : rv::PlatformSpeedUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class VelocityZ_Updater : public rv::PlatformSpeedUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Velocity Z";

   VelocityZ_Updater(const std::string& aPlatformName = "")
      : rv::PlatformSpeedUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class MachUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Mach";

   MachUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
      this->SetPrecision(2);
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AccelerationUpdater : public rv::PlatformAccelerationUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Acceleration";

   AccelerationUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAccelerationUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AccelerationN_Updater : public rv::PlatformAccelerationUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Acceleration N";

   AccelerationN_Updater(const std::string& aPlatformName = "")
      : rv::PlatformAccelerationUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AccelerationE_Updater : public rv::PlatformAccelerationUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Acceleration E";

   AccelerationE_Updater(const std::string& aPlatformName = "")
      : rv::PlatformAccelerationUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AccelerationD_Updater : public rv::PlatformAccelerationUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Acceleration D";

   AccelerationD_Updater(const std::string& aPlatformName = "")
      : rv::PlatformAccelerationUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AccelerationX_Updater : public rv::PlatformAccelerationUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Acceleration X";

   AccelerationX_Updater(const std::string& aPlatformName = "")
      : rv::PlatformAccelerationUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AccelerationY_Updater : public rv::PlatformAccelerationUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Acceleration Y";

   AccelerationY_Updater(const std::string& aPlatformName = "")
      : rv::PlatformAccelerationUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class AccelerationZ_Updater : public rv::PlatformAccelerationUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Acceleration Z";

   AccelerationZ_Updater(const std::string& aPlatformName = "")
      : rv::PlatformAccelerationUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};
} // namespace RvPlatformData
#endif
