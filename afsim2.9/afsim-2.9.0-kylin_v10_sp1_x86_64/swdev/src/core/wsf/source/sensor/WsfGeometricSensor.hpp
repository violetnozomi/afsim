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

#ifndef WSFGEOMETRICSENSOR_HPP
#define WSFGEOMETRICSENSOR_HPP

#include "wsf_export.h"

#include <map>
#include <memory>
#include <vector>

#include "TblLookup.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"

//! A specialization of WsfSensor that implements a simple sensor based strictly on geometry.

class WSF_EXPORT WsfGeometricSensor : public WsfSensor
{
public:
   WsfGeometricSensor(WsfScenario& aScenario);
   WsfGeometricSensor(const WsfGeometricSensor& aSrc);
   WsfGeometricSensor& operator=(const WsfGeometricSensor&) = delete;
   ~WsfGeometricSensor() override                           = default;

   WsfSensor* Clone() const override;
   bool       Initialize(double aSimTime) override;
   bool       ProcessInput(UtInput& aInput) override;
   void       Update(double aSimTime) override;

   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;

   //! A mode of the sensor.
   class WSF_EXPORT GeometricMode : public WsfSensorMode
   {
   public:
      GeometricMode();
      GeometricMode(const GeometricMode& aSrc);
      GeometricMode& operator=(const GeometricMode& aRhs);
      WsfMode*       Clone() const override;
      bool           Initialize(double aSimTime) override;
      bool           ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      double GetRangeRate(WsfPlatform* aTargetPtr);

      //! A Probability vs Range table
      struct PD_RangeTable
      {
         TblIndVarU<double> mRange;
         TblDepVar1<double> mProbability;
      };
      using PD_RangeTablePtr = std::shared_ptr<PD_RangeTable>;

      //! Detection range and PD range table for a target type
      struct TypeDetectionCriteria
      {
         TypeDetectionCriteria()
            : mMaximumRange(1.0E30)
         {
         }
         double           mMaximumRange;
         PD_RangeTablePtr mPD_TablePtr;
      };
      typedef std::map<WsfStringId, TypeDetectionCriteria> TypeCriteriaMap;

      TypeDetectionCriteria& GetDetectionCriteria(WsfPlatform* aTargetPtr);

      WsfEM_Antenna mAntenna;
      WsfEM_Rcvr    mRcvr;
      double        mMinRangeRate;
      double        mMaxRangeRate;
      //! A map of type vs detection criteria
      TypeCriteriaMap mTypeCriteria;
   };

protected:
   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<GeometricMode*> mGeometricModeList;
};

#endif
