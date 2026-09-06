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

#ifndef RVORBITALDATAUPDATERS_HPP
#define RVORBITALDATAUPDATERS_HPP

#include "RvPlatformUpdater.hpp"

namespace rv
{
class ResultPlatform;
}

namespace RvOrbitalData
{
class EccentricityUpdater : public rv::PlatformUnitlessUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Eccentricity";
   EccentricityUpdater(const std::string& aPlatformName = "")
      : rv::PlatformUnitlessUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class SemiMajorAxisUpdater : public rv::PlatformLengthUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Semi-major Axis";

   SemiMajorAxisUpdater(const std::string& aPlatformName = "")
      : rv::PlatformLengthUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class InclinationUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Inclination";

   InclinationUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class RAAN_Updater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "RAAN";

   RAAN_Updater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class ArgOfPeriapsisUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Argument of Periapsis";

   ArgOfPeriapsisUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};

class TrueAnomalyUpdater : public rv::PlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "True Anomaly";

   TrueAnomalyUpdater(const std::string& aPlatformName = "")
      : rv::PlatformAngleUpdater(cUPDATER_NAME, aPlatformName)
   {
   }

protected:
   bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
};
} // namespace RvOrbitalData

#endif
