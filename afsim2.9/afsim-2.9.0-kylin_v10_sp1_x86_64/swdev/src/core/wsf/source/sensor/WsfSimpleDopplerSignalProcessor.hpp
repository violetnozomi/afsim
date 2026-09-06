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

#ifndef WSFSIMPLEDOPPLERPROCESSOR_HPP
#define WSFSIMPLEDOPPLERPROCESSOR_HPP

#include "wsf_export.h"

#include <string>

#include "UtMath.hpp"
#include "UtTable.hpp"
#include "WsfMTI_TableManager.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorSignalProcessor.hpp"

class RadarMode;
class Result;

namespace wsf
{

//! A class derived from WsfSensorSignalProcessor that defines simple Doppler
//! processing techniques for a radar sensor.

class WSF_EXPORT SimpleDopplerSignalProcessor : public WsfSensorSignalProcessor
{
public:
   static WsfSensorSignalProcessor* ObjectFactory(const std::string& aTypeName);

   WsfSensorSignalProcessor* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   void Execute(double aSimTime, WsfSensorResult& aResult) override;

   bool GetFilterOwnshipFromDoppler() const { return mFilterOwnshipFromDoppler; }
   void SetFilterOwnshipFromDoppler(bool aFilterOwnshipFromDoppler)
   {
      mFilterOwnshipFromDoppler = aFilterOwnshipFromDoppler;
   }

   double GetMinDopplerSpeed() const { return mMinDopplerSpeed; }
   void   SetMinDopplerSpeed(double aMinDopplerSpeed) { mMinDopplerSpeed = aMinDopplerSpeed; }

   double GetMaxDopplerSpeed() const { return mMaxDopplerSpeed; }
   void   SetMaxDopplerSpeed(double aMaxDopplerSpeed) { mMaxDopplerSpeed = aMaxDopplerSpeed; }

private:
   //! If 'true' the ownship velocity will be filtered in the Doppler computations.
   bool mFilterOwnshipFromDoppler{true};

   //! Minimum Doppler speed required for detection.
   double mMinDopplerSpeed{0.0}; // meters/second

   //! Maximum Doppler speed required for detection.
   double mMaxDopplerSpeed{UtMath::cLIGHT_SPEED + 1.0}; // meters/second
};

} // namespace wsf

#endif
