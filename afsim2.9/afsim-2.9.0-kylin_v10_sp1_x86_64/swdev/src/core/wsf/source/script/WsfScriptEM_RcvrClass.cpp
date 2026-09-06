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

#include "WsfScriptEM_RcvrClass.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtStringUtil.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Util.hpp"
#include "WsfScenario.hpp"
#include "WsfScriptDefs.hpp"

// =================================================================================================
WsfScriptEM_RcvrClass::WsfScriptEM_RcvrClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptEM_XmtrRcvrClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfEM_Rcvr");

   AddMethod(ut::make_unique<Activate>());
   AddMethod(ut::make_unique<Deactivate>());
   AddMethod(ut::make_unique<BandwidthEffect>());
   AddMethod(ut::make_unique<CanInteractWith>());

   AddMethod(ut::make_unique<DetectionThreshold>());
   AddMethod(ut::make_unique<Function>());
   AddMethod(ut::make_unique<InstantaneousBandwidth>());
   AddMethod(ut::make_unique<NoisePower>());
   AddMethod(ut::make_unique<NoiseFigure>());
   AddMethod(ut::make_unique<NoiseMultiplier>());
   AddMethod(ut::make_unique<PolarizationEffect>());
   AddMethod(ut::make_unique<AntennaOhmicLoss>());
   AddMethod(ut::make_unique<ReceiveLineLoss>());

   AddMethod(ut::make_unique<SetDetectionThreshold>());
   AddMethod(ut::make_unique<SetInstantaneousBandwidth>());
   AddMethod(ut::make_unique<SetNoisePower>());
   AddMethod(ut::make_unique<SetNoiseFigure>());
   AddMethod(ut::make_unique<SetNoiseMultiplier>());
   AddMethod(ut::make_unique<SetPolarizationEffect>());
   AddMethod(ut::make_unique<SetAntennaOhmicLoss>());
   AddMethod(ut::make_unique<SetReceiveLineLoss>());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, Activate, 0, "void", "")
{
   aObjectPtr->Activate();
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, Deactivate, 0, "void", "")
{
   aObjectPtr->Deactivate();
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, BandwidthEffect, 2, "double", "double, double")
{
   aReturnVal.SetDouble(aObjectPtr->GetBandwidthEffect(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, CanInteractWith, 1, "bool", "WsfEM_Xmtr")
{
   auto xmtrPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfEM_Xmtr>();
   if (xmtrPtr)
   {
      aReturnVal.SetBool(aObjectPtr->CanInteractWith(xmtrPtr));
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, DetectionThreshold, 0, "double", "")
{
   aReturnVal.SetDouble(UtMath::LinearToDB(aObjectPtr->GetDetectionThreshold()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, Function, 0, "string", "")
{
   auto function = aObjectPtr->GetFunction();

   switch (function)
   {
   case WsfEM_Rcvr::cRF_COMM:
   {
      aReturnVal.SetString("rf_comm");
      break;
   }
   case WsfEM_Rcvr::cRF_SENSOR:
   {
      aReturnVal.SetString("rf_sensor");
      break;
   }
   case WsfEM_Rcvr::cRF_PASSIVE_SENSOR:
   {
      aReturnVal.SetString("rf_passive_sensor");
      break;
   }
   case WsfEM_Rcvr::cRF_INTERFERER:
   {
      aReturnVal.SetString("rf_interferer");
      break;
   }
   default:
   {
      aReturnVal.SetString("rf_undefined");
      break;
   }
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, InstantaneousBandwidth, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInstantaneousBandwidth());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, NoisePower, 0, "double", "")
{
   aReturnVal.SetDouble(UtMath::LinearToDB(aObjectPtr->GetNoisePower()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, NoiseFigure, 0, "double", "")
{
   aReturnVal.SetDouble(UtMath::LinearToDB(aObjectPtr->GetNoiseFigure()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, NoiseMultiplier, 0, "double", "")
{
   aReturnVal.SetDouble(UtMath::LinearToDB(aObjectPtr->GetNoiseMultiplier()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, PolarizationEffect, 1, "double", "string")
{
   auto polString = aVarArgs[0].GetString();
   UtStringUtil::ToLower(polString);
   auto pol = WsfEM_Types::Polarization::cPOL_DEFAULT;
   WsfEM_Util::StringToEnum(pol, polString);

   aReturnVal.SetDouble(aObjectPtr->GetPolarizationEffect(pol));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, AntennaOhmicLoss, 0, "double", "")
{
   aReturnVal.SetDouble(UtMath::LinearToDB(aObjectPtr->GetAntennaOhmicLoss()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, ReceiveLineLoss, 0, "double", "")
{
   aReturnVal.SetDouble(UtMath::LinearToDB(aObjectPtr->GetReceiveLineLoss()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, SetDetectionThreshold, 1, "void", "double")
{
   aObjectPtr->SetDetectionThreshold(UtMath::DB_ToLinear(aVarArgs[0].GetDouble()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, SetInstantaneousBandwidth, 1, "void", "double")
{
   aObjectPtr->SetInstantaneousBandwidth(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, SetNoisePower, 1, "void", "double")
{
   aObjectPtr->SetNoisePower(UtMath::DB_ToLinear(aVarArgs[0].GetDouble()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, SetNoiseFigure, 1, "void", "double")
{
   aObjectPtr->SetNoiseFigure(UtMath::DB_ToLinear(aVarArgs[0].GetDouble()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, SetNoiseMultiplier, 1, "void", "double")
{
   aObjectPtr->SetNoiseMultiplier(UtMath::DB_ToLinear(aVarArgs[0].GetDouble()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, SetPolarizationEffect, 2, "void", "string, double")
{
   auto polString = aVarArgs[0].GetString();
   UtStringUtil::ToLower(polString);
   auto pol = WsfEM_Types::Polarization::cPOL_DEFAULT;
   if (!WsfEM_Util::StringToEnum(pol, polString))
   {
      UT_SCRIPT_ABORT("Invalid polarization type provided");
   }
   auto fraction = aVarArgs[1].GetDouble();
   aObjectPtr->SetPolarizationEffect(pol, fraction);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, SetAntennaOhmicLoss, 1, "void", "double")
{
   aObjectPtr->SetAntennaOhmicLoss(UtMath::DB_ToLinear(aVarArgs[0].GetDouble()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_RcvrClass, WsfEM_Rcvr, SetReceiveLineLoss, 1, "void", "double")
{
   aObjectPtr->SetReceiveLineLoss(UtMath::DB_ToLinear(aVarArgs[0].GetDouble()));
}
