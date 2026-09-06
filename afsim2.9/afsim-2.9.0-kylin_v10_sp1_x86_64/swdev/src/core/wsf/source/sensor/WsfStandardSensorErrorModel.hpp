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

#ifndef WSFSTANDARDSENSORERRORMODEL_HPP
#define WSFSTANDARDSENSORERRORMODEL_HPP

#include "wsf_export.h"

#include <memory>

#include "WsfSensorErrorModel.hpp"

namespace wsf
{

//! A standard set of errors associated with single sensor mono-static detections.
struct SphericalMeasurementErrors
{
   SphericalMeasurementErrors() = default;

   SphericalMeasurementErrors(double aAzError, double aElError, double aRangeError, double aRangeRateError)
      : mAzError(aAzError)
      , mElError(aElError)
      , mRangeError(aRangeError)
      , mRangeRateError(aRangeRateError)
   {
   }

   double mAzError{0.0};
   double mElError{0.0};
   double mRangeError{0.0};
   double mRangeRateError{0.0};
};

//! A sensor error model for the default/standard sensor error model functionality.
//! This model explicitly uses spherical input parameters (Sigmas)
//! to produce normally-distributed az, el, range and range-rate errors.
class WSF_EXPORT StandardSensorErrorModel : public SensorErrorModel<SphericalMeasurementErrors>
{
public:
   //! A set of standard deviations of the mean associated with single sensor monostatic detections.
   struct Sigmas
   {
      Sigmas() = default;
      Sigmas(double aAzErrorStdDev, double aElErrorStdDev, double aRangeErrorStdDev, double aRangeRateErrorStdDev);

      double mAzErrorStdDev{0.0};
      double mElErrorStdDev{0.0};
      double mRangeErrorStdDev{0.0};
      double mRangeRateErrorStdDev{0.0};
   };

   static std::unique_ptr<SensorErrorModelBase> ObjectFactory(const std::string& aTypeName);
   StandardSensorErrorModel*                    Clone() const override;
   bool                                         ProcessInput(UtInput& aInput) override;

   bool IsStandardModel() const final { return true; }

   //! @name Sensor error access methods.
   //@{
   void          SetSigmas(const Sigmas& aSigmas) { mSigmas = aSigmas; }
   const Sigmas& GetSigmas() const { return mSigmas; }

   void SetAzErrorStdDev(double aAzErrorStdDev) { mSigmas.mAzErrorStdDev = aAzErrorStdDev; }
   void SetElErrorStdDev(double aElErrorStdDev) { mSigmas.mElErrorStdDev = aElErrorStdDev; }
   void SetRangeErrorStdDev(double aRangeErrorStdDev) { mSigmas.mRangeErrorStdDev = aRangeErrorStdDev; }
   void SetRangeRateErrorStdDev(double aRangeRateErrorStdDev) { mSigmas.mRangeRateErrorStdDev = aRangeRateErrorStdDev; }
   //@}

   std::unique_ptr<SphericalMeasurementErrors> ComputeMeasurementErrors(WsfSensorResult& aResult) const override;
   void ApplyMeasurementErrors(const SphericalMeasurementErrors& aErrors, WsfSensorResult& aResult) override;

   virtual SphericalMeasurementErrors GetSphericalErrorStdDev(ut::Random&      aRandom,
                                                              WsfSensorResult& aResult,
                                                              const Sigmas&    aSigmas) const;

private:
   Sigmas mSigmas;
};

} // namespace wsf

#endif
