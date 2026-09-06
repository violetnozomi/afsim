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

#ifndef WSFSCRIPTEM_XMTRCLASS_HPP
#define WSFSCRIPTEM_XMTRCLASS_HPP

#include "wsf_export.h"

#include "WsfScriptEM_XmtrRcvrClass.hpp"

//! Define script methods for WsfEM_Xmtr.
class WSF_EXPORT WsfScriptEM_XmtrClass : public WsfScriptEM_XmtrRcvrClass
{
public:
   WsfScriptEM_XmtrClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Power_1);
   UT_DECLARE_SCRIPT_METHOD(Power_2);
   UT_DECLARE_SCRIPT_METHOD(SetPower_1);
   UT_DECLARE_SCRIPT_METHOD(SetPower_2);

   UT_DECLARE_SCRIPT_METHOD(PeakPower_1);
   UT_DECLARE_SCRIPT_METHOD(PeakPower_2);

   UT_DECLARE_SCRIPT_METHOD(DutyCycle);
   UT_DECLARE_SCRIPT_METHOD(SetDutyCycle);

   UT_DECLARE_SCRIPT_METHOD(AlternateFrequency);
   UT_DECLARE_SCRIPT_METHOD(AlternateFrequencyCount);
   UT_DECLARE_SCRIPT_METHOD(AlternateFrequencyMap);
   UT_DECLARE_SCRIPT_METHOD(AlternateFrequencyIndex);

   UT_DECLARE_SCRIPT_METHOD(PulseRepetitionInterval_1);
   UT_DECLARE_SCRIPT_METHOD(PulseRepetitionInterval_2);
   UT_DECLARE_SCRIPT_METHOD(SetPulseRepetitionInterval_1);
   UT_DECLARE_SCRIPT_METHOD(SetPulseRepetitionInterval_2);
   UT_DECLARE_SCRIPT_METHOD(PulseRepetitionIntervalCount);
   UT_DECLARE_SCRIPT_METHOD(PulseRepetitionIntervals);

   UT_DECLARE_SCRIPT_METHOD(PulseRepetitionFrequency_1);
   UT_DECLARE_SCRIPT_METHOD(PulseRepetitionFrequency_2);
   UT_DECLARE_SCRIPT_METHOD(SetPulseRepetitionFrequency_1);
   UT_DECLARE_SCRIPT_METHOD(SetPulseRepetitionFrequency_2);
   UT_DECLARE_SCRIPT_METHOD(PulseRepetitionFrequencyCount);
   UT_DECLARE_SCRIPT_METHOD(PulseRepetitionFrequencies);

   UT_DECLARE_SCRIPT_METHOD(PulseWidth);
   UT_DECLARE_SCRIPT_METHOD(SetPulseWidth);

   UT_DECLARE_SCRIPT_METHOD(PulseCompressionRatio);
   UT_DECLARE_SCRIPT_METHOD(SetPulseCompressionRatio);
};

#endif
