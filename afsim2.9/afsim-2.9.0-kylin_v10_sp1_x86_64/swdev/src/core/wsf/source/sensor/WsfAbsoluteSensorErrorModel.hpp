// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFABSOLUTESENSORERRORMODEL_HPP
#define WSFABSOLUTESENSORERRORMODEL_HPP

#include "wsf_export.h"

#include <memory>

#include "UtVec3.hpp"
#include "WsfSensorErrorModel.hpp"

namespace wsf
{

//! @brief A set of absolute sensor errors to be used by the AbsoluteSensorErrorModel.
//! It is modeled as a NED offset from the truth target location.
using AbsoluteMeasurementError = UtVec3d;

//! A sensor error model that computes an error about the target that is either two or three dimensional,
//! based on a fixed radial distance from the target.
//! For 2D errors, applies position error to the track in Lat/Long (or target-centered North/East) dimensions
//! according to a Gaussian draw, resulting in 68% of track position measurements being within a
//! circle of one sigma radius centered on the target's truth position; target altitude will be
//! reported without error.  For 3D errors, applies positional error to the track in target-centered North, East,
//! and Down according to a Gaussian draw, resulting in 68% of track position measurements being
//! within a sphere of one sigma radius.
class WSF_EXPORT AbsoluteSensorErrorModel : public SensorErrorModel<AbsoluteMeasurementError>
{
public:
   //! Factory method for WsfSensorErrorModelTypes.
   //! This is called by WsfSensorErrorModelTypes::LoadType to determine if a pattern
   //! represented by this class is being requested.
   static std::unique_ptr<SensorErrorModelBase> ObjectFactory(const std::string& aTypeName);
   SensorErrorModelBase*                        Clone() const override;
   bool                                         ProcessInput(UtInput& aInput) override;

   void   SetSigma(double aSigma) { mSigma = aSigma; }
   double GetSigma() const { return mSigma; }

   std::unique_ptr<AbsoluteMeasurementError> ComputeMeasurementErrors(WsfSensorResult& aResult) const override;
   void ApplyMeasurementErrors(const AbsoluteMeasurementError& aErrors, WsfSensorResult& aResult) override;

private:
   bool   mIs3D{false};
   double mSigma{0.0};
};

} // namespace wsf
#endif
