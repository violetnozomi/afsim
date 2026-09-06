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

#include "WsfScriptEM_XmtrRcvrClass.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Attenuation.hpp"
#include "WsfEM_Propagation.hpp"
#include "WsfEM_Types.hpp"
#include "WsfEM_Util.hpp"
#include "WsfEM_XmtrRcvr.hpp"
#include "WsfScenario.hpp"
#include "WsfScriptDefs.hpp"

// =================================================================================================
WsfScriptEM_XmtrRcvrClass::WsfScriptEM_XmtrRcvrClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfEM_XmtrRcvr");
   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<Antenna>());

   // This can be removed in AFSIM 3.0. This is a wrapper
   // for data now available via the Antenna in script.
   // It is still provided at this time for legacy support.
   AddMethod(ut::make_unique<ArticulatedPart>());

   AddMethod(ut::make_unique<AttenuationModel>());
   AddMethod(ut::make_unique<PropagationModel>());

   AddMethod(ut::make_unique<Bandwidth>());
   AddMethod(ut::make_unique<SetBandwidth>());
   AddMethod(ut::make_unique<Frequency>());
   AddMethod(ut::make_unique<Wavelength>());
   AddMethod(ut::make_unique<SetFrequency>());

   AddMethod(ut::make_unique<PolarizationTypes>());
   AddMethod(ut::make_unique<AntennaPattern_1>("AntennaPattern"));
   AddMethod(ut::make_unique<AntennaPattern_2>("AntennaPattern"));
   AddMethod(ut::make_unique<Polarization>());
   AddMethod(ut::make_unique<SetPolarization>());
   AddMethod(ut::make_unique<BeamTilt>());
   AddMethod(ut::make_unique<Index>());
   AddMethod(ut::make_unique<EarthRadiusMultiplier>());
   AddMethod(ut::make_unique<CheckMasking>());
   AddMethod(ut::make_unique<InternalLoss>());
   AddMethod(ut::make_unique<SetInternalLoss>());
   AddMethod(ut::make_unique<SetBeamTilt>());
   AddMethod(ut::make_unique<SetCheckMasking>());
   AddMethod(ut::make_unique<SetEarthRadiusMultiplier>());
   AddMethod(ut::make_unique<MaskingMode>());
   AddMethod(ut::make_unique<SetMaskingMode>());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, Antenna, 0, "WsfEM_Antenna", "")
{
   aReturnVal.SetPointer(UtScriptRef::Ref(aObjectPtr->GetAntenna(), aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, ArticulatedPart, 0, "WsfArticulatedPart", "")
{
   aReturnVal.SetPointer(UtScriptRef::Ref(aObjectPtr->GetArticulatedPart(), aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, AttenuationModel, 0, "WsfEM_Attenuation", "")
{
   aReturnVal.SetPointer(UtScriptRef::Ref(aObjectPtr->GetAttenuationModel(), aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, PropagationModel, 0, "WsfEM_Propagation", "")
{
   aReturnVal.SetPointer(UtScriptRef::Ref(aObjectPtr->GetPropagationModel(), aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, Bandwidth, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBandwidth());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, SetBandwidth, 1, "void", "double")
{
   aObjectPtr->SetBandwidth(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, Frequency, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetFrequency());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, Wavelength, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetWavelength());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, SetFrequency, 1, "void", "double")
{
   aObjectPtr->SetFrequency(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, PolarizationTypes, 0, "Array<string>", "")
{
   auto data = ut::make_unique<std::vector<UtScriptData>>();
   for (auto i = 0; i < WsfEM_Types::Polarization::cPOL_COUNT; ++i)
   {
      auto polEnum    = static_cast<WsfEM_Types::Polarization>(i);
      auto patternPtr = aObjectPtr->GetAntennaPatternList(polEnum);
      if (patternPtr)
      {
         data->emplace_back(WsfEM_Util::EnumToString(polEnum));
      }
   }

   auto scriptRefPtr = ut::make_unique<UtScriptRef>(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(scriptRefPtr.release());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, Polarization, 0, "string", "")
{
   aReturnVal.SetString(WsfEM_Util::EnumToString(aObjectPtr->GetPolarization()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, SetPolarization, 1, "void", "string")
{
   auto polString = aVarArgs[0].GetString();
   UtStringUtil::ToLower(polString);
   auto pol = WsfEM_Types::Polarization::cPOL_DEFAULT;
   if (!WsfEM_Util::StringToEnum(pol, polString))
   {
      UT_SCRIPT_ABORT("Invalid polarization type provided");
   }

   aObjectPtr->SetPolarization(pol);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, AntennaPattern_1, 1, "WsfAntennaPattern", "double")
{
   auto patternPtr = aObjectPtr->GetAntennaPattern(WsfEM_Types::Polarization::cPOL_DEFAULT, aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(UtScriptRef::Ref(patternPtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, AntennaPattern_2, 2, "WsfAntennaPattern", "double, string")
{
   auto polString = aVarArgs[1].GetString();
   UtStringUtil::ToLower(polString);
   auto pol = WsfEM_Types::Polarization::cPOL_DEFAULT;
   if (!WsfEM_Util::StringToEnum(pol, polString))
   {
      UT_SCRIPT_ABORT("Invalid polarization type provided");
   }

   auto patternPtr = aObjectPtr->GetAntennaPattern(pol, aVarArgs[0].GetDouble());
   aReturnVal.SetPointer(UtScriptRef::Ref(patternPtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, BeamTilt, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBeamTilt() * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, Index, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->GetIndex()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, EarthRadiusMultiplier, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetEarthRadiusMultiplier());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, CheckMasking, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->CheckMasking());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, InternalLoss, 0, "double", "")
{
   aReturnVal.SetDouble(UtMath::LinearToDB(aObjectPtr->GetInternalLoss()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, SetInternalLoss, 1, "void", "double")
{
   aObjectPtr->SetInternalLoss(UtMath::DB_ToLinear(aVarArgs[0].GetDouble()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, SetBeamTilt, 1, "void", "double")
{
   aObjectPtr->SetBeamTilt(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, SetCheckMasking, 1, "void", "bool")
{
   aObjectPtr->SetCheckMasking(aVarArgs[0].GetBool());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, SetEarthRadiusMultiplier, 1, "void", "double")
{
   aObjectPtr->SetEarthRadiusMultiplier(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, MaskingMode, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->MaskingMode());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_XmtrRcvrClass, WsfEM_XmtrRcvr, SetMaskingMode, 1, "void", "string")
{
   aObjectPtr->SetMaskingMode(aVarArgs[0].GetString());
}
