// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSURFACEWAVERADARSENSORERRORMODEL_HPP
#define WSFSURFACEWAVERADARSENSORERRORMODEL_HPP

#include "wsf_mil_export.h"

#include <memory>

#include "WsfStandardSensorErrorModel.hpp"
#include "WsfSurfaceWaveRadarSensor.hpp"

namespace wsf
{

//! A sensor error model for the Radar Sensor Specific Error Model functionality.
class WSF_MIL_EXPORT SurfaceWaveRadarSensorErrorModel : public StandardSensorErrorModel
{
public:
   static std::unique_ptr<wsf::SensorErrorModelBase> ObjectFactory(const std::string& aTypeName);

   SurfaceWaveRadarSensorErrorModel* Clone() const override;
   bool                              Initialize(WsfSensorMode* aSensorModePtr) override;
   bool                              ProcessInput(UtInput& aInput) override;
   void ApplyMeasurementErrors(const wsf::SphericalMeasurementErrors& aErrors, WsfSensorResult& aResult) override;

protected:
   SphericalMeasurementErrors GetSphericalErrorStdDev(ut::Random&      aRandom,
                                                      WsfSensorResult& aResult,
                                                      const Sigmas&    aSigmas) const override;

private:
   WsfSurfaceWaveRadarSensor::RadarMode* mSurfaceWaveRadarModePtr{nullptr};

   //! Additional parameters associated with measurement errors
   double mErrorModelAzBeamwidth{0.0};       // radians
   double mErrorModelElBeamwidth{0.0};       // radians
   double mErrorModelPulseWidth{0.0};        // seconds
   double mErrorModelDopplerResolution{0.0}; // Hz (1/sec)
};

} // namespace wsf

#endif
