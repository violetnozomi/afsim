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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFATMOSPHERICTURBULENCE_HPP
#define WSFATMOSPHERICTURBULENCE_HPP

#include "wsf_mil_export.h"

#include <map>

#include "UtInput.hpp"
#include "UtIntegrand.hpp"
#include "WsfObjectTypeList.hpp"
class WsfScenario;
#include "WsfStringId.hpp"

namespace wsf
{

//! An encapsulation of a turbulent CN2 model.
class CN2 : public WsfObject
{
public:
   using CN2_Ftn = std::function<double(double)>;

   CN2(const CN2_Ftn& aCN2_Ftn)
      : mCN2_Ftn(aCN2_Ftn)
   {
   }

   WsfObject* Clone() const override { return new CN2(*this); }

   double operator()(double aHeight) const { return mCN2_Ftn(aHeight); }

private:
   CN2_Ftn mCN2_Ftn;
};


//! A class encapsulating an atmospheric turbulence profile.
//! Based on a configurable CN2 model, the turbulent beam spread
//! angle is calculated in the BeamSpreadAngle method.
class WSF_MIL_EXPORT AtmosphericTurbulence
{
public:
   AtmosphericTurbulence(const WsfScenario& aScenario);
   virtual ~AtmosphericTurbulence() = default;

   virtual bool ProcessInput(UtInput& aInput);

   const std::string& GetCN2Form();
   void               SetCN2Form(const std::string& aCN2Form);

   const CN2& GetCN2();

   double BeamSpreadAngle(double aSlantRange, double aTargetHeight, double aPlatformHeight, double aWavelength);

protected:
private:
   class TurbulentBeamSpreadIntegrand : public UtIntegrand
   {
   public:
      TurbulentBeamSpreadIntegrand(double          aSlantRange,
                                   double          aTargetHeight,
                                   double          aPlatformHeight,
                                   double          aWavelength,
                                   const wsf::CN2& aCN2form);
      double Function(double aVal) override;

   private:
      TurbulentBeamSpreadIntegrand() = delete;

      double   mSlantRange;
      double   mTargetHeight;
      double   mPlatformHeight;
      double   mWavelength;
      wsf::CN2 mCN2form;
   };

   const WsfScenario& mScenario;
   const CN2*         mCN2_Ptr;
};

//! The types factory for CN2 models (owned by the scenario).
class CN2_Types : public WsfObjectTypeList<CN2>
{
public:
   static WSF_MIL_EXPORT CN2_Types&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const CN2_Types& Get(const WsfScenario& aScenario);

   CN2_Types(WsfScenario& aScenario);

private:
   static double hv57(double aHeight);
};

}; // namespace wsf

#endif
