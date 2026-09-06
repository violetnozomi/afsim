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

#ifndef WSFEM_SIMPLEATTENUATION_HPP
#define WSFEM_SIMPLEATTENUATION_HPP

#include "wsf_export.h"

#include "WsfEM_Attenuation.hpp"

//! A trivial attenuation model that is uses a fixed dB/length.
class WSF_EXPORT WsfEM_SimpleAttenuation : public WsfEM_Attenuation
{
public:
   WsfEM_SimpleAttenuation();
   WsfEM_SimpleAttenuation(const WsfEM_SimpleAttenuation& aSrc);
   WsfEM_SimpleAttenuation& operator=(const WsfEM_SimpleAttenuation& aRhs) = delete;
   WsfEM_Attenuation*       Clone() const override;
   ~WsfEM_SimpleAttenuation() override = default;

   static WsfEM_Attenuation* ObjectFactory(const std::string& aTypeName);

   bool ProcessInput(UtInput& aInput) override;

   bool AcceptsInlineBlockInput() const override { return true; }

private:
   double ComputeAttenuationFactorP(double aRange, double aElevation, double aAltitude, double aFrequency) override;

   //! @name Attenuation values.
   //! The attenuation factor will be used if it is non-zero, otherwise the specific
   //! attenuation will be used.
   //@{
   double mAttenuationFactor;   //!< absolute
   double mSpecificAttenuation; //!< dB/m
                                //@}
};

#endif
