// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFELEMENTESA_ANTENNAPATTERN_HPP
#define WSFELEMENTESA_ANTENNAPATTERN_HPP

#include <vector>

class UtInput;
#include "UtVec2.hpp"
#include "WsfESA_AntennaPattern.hpp"
#include "WsfObject.hpp"

//! The implementation of an 'Element-Wise (user-defined elements) Electronically Scanned Array (ESA)' antenna patterns.
//!
//! This ESA antenna pattern extends the base ESA pattern by allowing the elements to be located by the user via an
//! input table. Although not perfect for non-uniformed arrays it does allow for more precisely modeling other than
//! rectangular ESA patterns, as limited by the base ESA pattern class.
//!
//! Adaption from Skolnik 2nd edition adding in third dimension making it non-exportable.

class WsfElementESA_AntennaPattern : public WsfESA_AntennaPattern
{
public:
   using ElementLocXY = std::vector<UtVec2d>;
   using WeightVec    = std::vector<double>;

   class EESA_Data : public WsfESA_AntennaPattern::ESA_Data
   {
   public:
      EESA_Data();
      ~EESA_Data() override = default;

      bool ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput) override;
   };

   WsfElementESA_AntennaPattern();
   ~WsfElementESA_AntennaPattern() override = default;

   static WsfAntennaPattern* ObjectFactory(const std::string& aTypeName);

   WsfAntennaPattern* Clone() const override;

   bool Initialize(WsfSimulation* aSimulationPtr) override;

   bool ProcessInput(UtInput& aInput) override;


protected:
   WsfElementESA_AntennaPattern(const WsfElementESA_AntennaPattern& aSrc);

   //! Assignment is not supported.
   WsfElementESA_AntennaPattern& operator=(const WsfElementESA_AntennaPattern& aRhs) = delete;

   double ComputeElementGain(double aFrequency, double aAzAngle, double aElAngle) const override;

   double ComputeDirectivityGain(double aFrequency, double aEBS_Az, double aEBS_El) const override;

   EESA_Data* GetEESA_Data() const { return dynamic_cast<EESA_Data*>(GetData()); }

private:
   double ComputeApertureEfficiency() override;

   bool mExplicitApertureEff;
   bool mExplicitApertureEffXY;
};

#endif
