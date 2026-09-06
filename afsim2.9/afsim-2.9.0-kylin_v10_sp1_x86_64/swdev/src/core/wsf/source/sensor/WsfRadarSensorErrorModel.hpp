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

#ifndef WSFRADARSENSORERRORMODEL_HPP
#define WSFRADARSENSORERRORMODEL_HPP

#include "wsf_export.h"

#include <memory>

#include "UtRandom.hpp"
#include "WsfStandardSensorErrorModel.hpp"

namespace wsf
{

//! A sensor error model for the Radar Sensor Specific Error Model functionality.
class WSF_EXPORT RadarSensorErrorModel : public StandardSensorErrorModel
{
public:
   static std::unique_ptr<SensorErrorModelBase> ObjectFactory(const std::string& aTypeName);

   RadarSensorErrorModel* Clone() const override;
   bool                   ProcessInput(UtInput& aInput) override;
   bool                   Initialize(WsfSensorMode* aSensorModePtr) override;

   void ApplyMeasurementErrors(const SphericalMeasurementErrors& aErrors, WsfSensorResult& aResult) override;

private:
   SphericalMeasurementErrors GetSphericalErrorStdDev(ut::Random&      aRandom,
                                                      WsfSensorResult& aResult,
                                                      const Sigmas&    aSigmas) const override;
   SphericalMeasurementErrors GetRadarSphericalErrorStdDev(ut::Random& aRandom, WsfSensorResult& aResult) const;

   bool mOverrideMeasurementWithTruth{false};
   bool mComputeMeasurementErrors{false};
};

} // namespace wsf

#endif
