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

#ifndef WSFOTH_RADARSENSORERRORMODEL_HPP
#define WSFOTH_RADARSENSORERRORMODEL_HPP

#include "wsf_mil_export.h"

#include <memory>

#include "WsfStandardSensorErrorModel.hpp"

namespace wsf
{

//! A sensor error model for the OTH Radar Sensor specific error model functionality.
class WSF_MIL_EXPORT OTH_RadarSensorErrorModel : public StandardSensorErrorModel
{
public:
   static std::unique_ptr<wsf::SensorErrorModelBase> ObjectFactory(const std::string& aTypeName);

   OTH_RadarSensorErrorModel* Clone() const override;
   bool                       ProcessInput(UtInput& aInput) override;
   bool                       Initialize(WsfSensorMode* aSensorModePtr) override;

protected:
   SphericalMeasurementErrors GetSphericalErrorStdDev(ut::Random&      aRandom,
                                                      WsfSensorResult& aResult,
                                                      const Sigmas&    aSigmas) const override;

private:
   SphericalMeasurementErrors GetOTH_RadarSphericalErrorStdDev(ut::Random&      aRandom,
                                                               WsfSensorResult& aResult,
                                                               const Sigmas&    aSigmas) const;

   bool mComputeMeasurementErrors{false};
};

} // namespace wsf

#endif
