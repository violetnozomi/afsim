// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFBISTATICERRORMODEL_HPP
#define WSFBISTATICERRORMODEL_HPP

#include "wsf_mil_export.h"

#include <memory>

#include "UtVec3.hpp"
#include "WsfStandardSensorErrorModel.hpp"

namespace wsf
{

class WSF_MIL_EXPORT BistaticErrorModel : public StandardSensorErrorModel
{
public:
   struct TransmitterKey
   {
      std::string mSide;
      std::string mDomain;
      bool        operator<(const TransmitterKey& aRhs) const
      {
         return this->mSide == aRhs.mSide ? this->mDomain > aRhs.mDomain : this->mSide > aRhs.mSide;
      }
   };

   struct BistaticMeasurement
   {
      // These AZ-EL are sort of unique because they should be blurred before the
      // range error calculation.
      double   mAz;
      double   mEl;
      double   mTs;
      double   mTx;
      double   mRng;
      UtVec3d  mTransmitterPositionPCS;
      UtVec3d* mTransmitterPositionSigma;
   };

   BistaticErrorModel();
   ~BistaticErrorModel() override = default;

   static std::unique_ptr<SensorErrorModelBase> ObjectFactory(const std::string& aTypeName);

   BistaticErrorModel* Clone() const override;
   bool                ProcessInput(UtInput& aInput) override;
   bool                Initialize(WsfSensorMode* aSensorModePtr) override;

   void LoadPositionSigmaTable(UtInput& aInput);

protected:
   BistaticErrorModel(const BistaticErrorModel& aSrc) = default;

   SphericalMeasurementErrors GetSphericalErrorStdDev(ut::Random&      aRandom,
                                                      WsfSensorResult& aResult,
                                                      const Sigmas&    aSigmas) const override;

   Sigmas GetBistaticSphericalErrorStdDev(BistaticMeasurement& mMeasurement) const;

   //! The reflected time measurement sigma.  The reflected time, or scatter time (ts), is the time of reception
   //! of a signal that travels from transmitter to target and then to receiver.
   double mTsErrorStdDev;

   //! The direct time measurement sigma.  The direct time is the time of reception
   //! of a signal that travels from transmitter directly to the receiver.
   double mTxErrorStdDev;

   //! The range can be blurred in 2 ways.  Realistically based on this model, but this
   //! causes multiplicative noise vs additive, and our current filters may not track the
   //! measurement outputs.  Conversely, we can blur it in an additive way so the resulting
   //! track will simulate a system that can process this type of noise.
   bool mRealBlurring;

   //! The initial spherical standard deviations provided by the sensor mode.
   Sigmas mInitialSigmas;

   //! A dual key map of transmitter platform to the sigmas associated with its location measurement
   //! sigmas.  Note that the transmitter in a bistatic transaction can be on the red/opposite team.
   //! The comparable key is a paring of the side and domain.
   std::map<TransmitterKey, UtVec3d> mPositionDeltaMap;
};

} // namespace wsf

#endif
