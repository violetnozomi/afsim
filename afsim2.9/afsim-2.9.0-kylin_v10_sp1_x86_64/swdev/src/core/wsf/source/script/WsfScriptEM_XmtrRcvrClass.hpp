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

#ifndef WSFSCRIPTEM_XMTRRCVRCLASS_HPP
#define WSFSCRIPTEM_XMTRRCVRCLASS_HPP

#include "wsf_export.h"

#include <string>

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

//! This is a common base script class for shared capabilities of the
//! EM based transmitters and receivers.
class WSF_EXPORT WsfScriptEM_XmtrRcvrClass : public UtScriptClass
{
public:
   WsfScriptEM_XmtrRcvrClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Antenna);
   UT_DECLARE_SCRIPT_METHOD(ArticulatedPart);
   UT_DECLARE_SCRIPT_METHOD(AttenuationModel);
   UT_DECLARE_SCRIPT_METHOD(PropagationModel);

   UT_DECLARE_SCRIPT_METHOD(Bandwidth);
   UT_DECLARE_SCRIPT_METHOD(SetBandwidth);
   UT_DECLARE_SCRIPT_METHOD(Frequency);
   UT_DECLARE_SCRIPT_METHOD(Wavelength);
   UT_DECLARE_SCRIPT_METHOD(SetFrequency);

   UT_DECLARE_SCRIPT_METHOD(PolarizationTypes);
   UT_DECLARE_SCRIPT_METHOD(Polarization);
   UT_DECLARE_SCRIPT_METHOD(SetPolarization);
   UT_DECLARE_SCRIPT_METHOD(AntennaPattern_1);
   UT_DECLARE_SCRIPT_METHOD(AntennaPattern_2);
   UT_DECLARE_SCRIPT_METHOD(BeamTilt);
   UT_DECLARE_SCRIPT_METHOD(Index);
   UT_DECLARE_SCRIPT_METHOD(EarthRadiusMultiplier);
   UT_DECLARE_SCRIPT_METHOD(CheckMasking);
   UT_DECLARE_SCRIPT_METHOD(InternalLoss);
   UT_DECLARE_SCRIPT_METHOD(SetInternalLoss);
   UT_DECLARE_SCRIPT_METHOD(SetBeamTilt);
   UT_DECLARE_SCRIPT_METHOD(SetCheckMasking);
   UT_DECLARE_SCRIPT_METHOD(SetEarthRadiusMultiplier);
   UT_DECLARE_SCRIPT_METHOD(MaskingMode);
   UT_DECLARE_SCRIPT_METHOD(SetMaskingMode);
};

#endif
