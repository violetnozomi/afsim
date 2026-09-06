// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTANTENNAPATTERNCLASS_HPP
#define WSFSCRIPTANTENNAPATTERNCLASS_HPP

#include "wsf_export.h"

#include <string>

#include "WsfScriptObjectClass.hpp"

class WSF_EXPORT WsfScriptAntennaPatternClass : public WsfScriptObjectClass
{
public:
   WsfScriptAntennaPatternClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~WsfScriptAntennaPatternClass() override = default;

   UT_DECLARE_SCRIPT_METHOD(AzimuthBeamwidth_1);   // AzimuthBeamwidth(freq)
   UT_DECLARE_SCRIPT_METHOD(AzimuthBeamwidth_2);   // AzimuthBeamwidth(freq, azEBS, elEBS)
   UT_DECLARE_SCRIPT_METHOD(ElevationBeamwidth_1); // ElevationBeamwidth(freq)
   UT_DECLARE_SCRIPT_METHOD(ElevationBeamwidth_2); // ElevationBeamwidth(freq, azEBS, elEBS)
   UT_DECLARE_SCRIPT_METHOD(MinimumGain);
   UT_DECLARE_SCRIPT_METHOD(PeakGain);
   UT_DECLARE_SCRIPT_METHOD(GainAdjustment);
};

#endif
