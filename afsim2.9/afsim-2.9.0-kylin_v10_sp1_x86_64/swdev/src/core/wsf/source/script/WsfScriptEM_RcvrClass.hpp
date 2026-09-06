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

#ifndef WSFSCRIPTEM_RCVRCLASS_HPP
#define WSFSCRIPTEM_RCVRCLASS_HPP

#include "wsf_export.h"

#include "WsfScriptEM_XmtrRcvrClass.hpp"

//! Define script methods for the WsfEM_Rcvr.
class WSF_EXPORT WsfScriptEM_RcvrClass : public WsfScriptEM_XmtrRcvrClass
{
public:
   WsfScriptEM_RcvrClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Activate);
   UT_DECLARE_SCRIPT_METHOD(Deactivate);
   UT_DECLARE_SCRIPT_METHOD(BandwidthEffect);
   UT_DECLARE_SCRIPT_METHOD(CanInteractWith);

   UT_DECLARE_SCRIPT_METHOD(DetectionThreshold);
   UT_DECLARE_SCRIPT_METHOD(Function);
   UT_DECLARE_SCRIPT_METHOD(InstantaneousBandwidth);
   UT_DECLARE_SCRIPT_METHOD(NoisePower);
   UT_DECLARE_SCRIPT_METHOD(NoiseFigure);
   UT_DECLARE_SCRIPT_METHOD(NoiseMultiplier);
   UT_DECLARE_SCRIPT_METHOD(PolarizationEffect);
   UT_DECLARE_SCRIPT_METHOD(AntennaOhmicLoss);
   UT_DECLARE_SCRIPT_METHOD(ReceiveLineLoss);

   UT_DECLARE_SCRIPT_METHOD(SetDetectionThreshold);
   UT_DECLARE_SCRIPT_METHOD(SetInstantaneousBandwidth);
   UT_DECLARE_SCRIPT_METHOD(SetNoisePower);
   UT_DECLARE_SCRIPT_METHOD(SetNoiseFigure);
   UT_DECLARE_SCRIPT_METHOD(SetNoiseMultiplier);
   UT_DECLARE_SCRIPT_METHOD(SetPolarizationEffect);
   UT_DECLARE_SCRIPT_METHOD(SetAntennaOhmicLoss);
   UT_DECLARE_SCRIPT_METHOD(SetReceiveLineLoss);
};

#endif
