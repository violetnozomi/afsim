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

#ifndef WSFSENSORERRORMODEL_HPP
#define WSFSENSORERRORMODEL_HPP

#include "wsf_export.h"

#include <memory>

#include "UtInput.hpp"
namespace ut
{
class Random;
}

#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"
class WsfSimulation;
class WsfSensorMode;
class WsfSensorResult;

namespace wsf
{
//! A base class for SensorErrorModels.
class WSF_EXPORT SensorErrorModelBase : public WsfObject
{
public:
   bool         ProcessInput(UtInput& aInput) override;
   virtual bool Initialize(WsfSensorMode* aSensorModePtr);

   //! Compute and apply measurement errors to the current detection attempt.
   //! @param aResult The results of a sensing chance to be modified by the error model.
   //! @note This method is defined in wsf::SensorErrorModel.
   virtual void ApplyMeasurementErrors(WsfSensorResult& aResult) = 0;

   using SensorModelTypes = WsfObjectTypeList<SensorErrorModelBase>;

   //! Create a clone of "this" object.
   SensorErrorModelBase* Clone() const override = 0;

   //! Is this a null model (i.e., ApplyMeasurementErrors has no effect).
   virtual bool IsNullModel() const { return false; }

   //! Is this a "standard" model that utilizes az/el/range/range rate
   //! standard deviations to compute normally distributed errors.
   virtual bool IsStandardModel() const { return false; }

   bool DebugEnabled() const { return mDebugEnabled; }

   WsfSensorMode* GetSensorMode() const { return mSensorModePtr; }
   void           SetSensorMode(WsfSensorMode* aSensorModePtr) { mSensorModePtr = aSensorModePtr; }

private:
   bool           mDebugEnabled{false};
   WsfSensorMode* mSensorModePtr{nullptr};
};

//! This class defines the interface to compute specific measurement errors.
template<typename ErrorType>
class SensorErrorModel : public SensorErrorModelBase
{
public:
   //! Compute the sensor errors associated with this model and return them.
   //! @param aResult The sensing chance result that is used as a basis for the error computation.
   //! @return The computed sensor errors for the given sensing chance.
   //! @note This method is used to implement the single parameter ApplyMeasurementErrors method,
   //! and it may also be used in the sensor mode when the errors are applied to more than one sensor result.
   virtual std::unique_ptr<ErrorType> ComputeMeasurementErrors(WsfSensorResult& aResult) const = 0;

   //! Apply measurement errors computed with ComputeMeasurementErrors to a sensor result.
   //! @param aErrors The sensor errors as returned from ComputeMeasurementErrors.
   //! @param aResult The sensing chance result to which the error is applied.
   virtual void ApplyMeasurementErrors(const ErrorType& aErrors, WsfSensorResult& aResult) = 0;

   void ApplyMeasurementErrors(WsfSensorResult& aResult) final
   {
      ApplyMeasurementErrors(*ComputeMeasurementErrors(aResult), aResult);
   }
};

} // namespace wsf

#endif
