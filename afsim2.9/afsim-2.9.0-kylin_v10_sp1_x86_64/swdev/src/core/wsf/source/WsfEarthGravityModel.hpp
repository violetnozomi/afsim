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

#ifndef WSFEARTHGRAVITYMODEL_HPP
#define WSFEARTHGRAVITYMODEL_HPP

#include "wsf_export.h"

#include <memory>
#include <vector>

class UtInput;

#include "GeoidUndulation.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"

namespace wsf
{

//! The Earth Gravitational Model (EGM) scenario extension.
class WSF_EXPORT EarthGravityModelExtension : public WsfScenarioExtension
{
public:
   enum EGMComplianceType
   {
      cEGM_UNKNOWN,
      cEGM2008,
      cEGM96
   };

   EarthGravityModelExtension()                                       = default;
   EarthGravityModelExtension(const EarthGravityModelExtension& aSrc) = default;
   ~EarthGravityModelExtension() override                             = default;

   static EarthGravityModelExtension& Get(const WsfScenario& aScenario);

   bool Complete() override;
   void SimulationCreated(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

   float GetGeoidUndulation(double aLat, double aLon) const;

   EGMComplianceType GetEGM_ComplianceType() const { return mEGM_Type; }
   bool              IsEGM2008Compliant() const { return (mEGM_Type == cEGM2008); }
   bool              IsEGM96Compliant() const { return (mEGM_Type == cEGM96); }
   bool              IsEGM_Unknown() const { return (mEGM_Type == cEGM_UNKNOWN); }


private:
   bool ProcessInputData(UtInput& aInput);

   std::shared_ptr<GeoidUndulation>       mGeoidUndulationPtr = nullptr;
   std::string                            mEGM_DataFile;
   std::vector<std::pair<double, double>> mTestLatLons;
   EGMComplianceType                      mEGM_Type = cEGM_UNKNOWN;
};

//!
class WSF_EXPORT EarthGravityModel : public WsfSimulationExtension
{
public:
   EarthGravityModel(EarthGravityModelExtension* aExtensionPtr);

   static EarthGravityModel* Find(const WsfSimulation& aSimulation);

   float GetGeoidUndulation(double aLat, double aLon) const;

   EarthGravityModelExtension::EGMComplianceType GetEGM_ComplianceType() const;
   bool                                          IsEGM2008Compliant() const;
   bool                                          IsEGM96Compliant() const;
   bool                                          IsEGM_Unknown() const;

private:
   EarthGravityModelExtension* mEGM_ExtensionPtr;
};

} // namespace wsf

#endif
