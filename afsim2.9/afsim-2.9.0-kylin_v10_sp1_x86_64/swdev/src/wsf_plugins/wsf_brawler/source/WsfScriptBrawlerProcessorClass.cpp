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

#include "WsfScriptBrawlerProcessorClass.hpp"

#include <string>

#include "BrawlerCoordinateConversion.hpp"
#include "BrawlerUtil.hpp"
#include "UtScriptDataPack.hpp"
#include "UtScriptVec3.hpp"
#include "UtStringUtil.hpp"
#include "WsfBrawlerProcessor.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfTrack.hpp"
#include "script/WsfScriptContext.hpp"

namespace
{

// Helper method that converts string input to WsfBrawlerPlatform::ManeuverValueComponentType
// WsfBrawlerPlatform::ManeuverValueComponentType GetManeuverValueComponentType(std::string aType)
BRAWLER::ManeuverValueComponentType GetManeuverValueComponentType(std::string& aType)
{
   BRAWLER::ManeuverValueComponentType mvcType = BRAWLER::i_voffef;
   if (aType == "i_voffef" || aType == "1" || aType == "offensive")
   {
      mvcType = BRAWLER::i_voffef;
   }
   else if (aType == "i_vlospd" || aType == "2" || aType == "low speed recovery")
   {
      mvcType = BRAWLER::i_vlospd;
   }
   else if (aType == "i_vform" || aType == "3" || aType == "formation")
   {
      mvcType = BRAWLER::i_vform;
   }
   else if (aType == "i_compr" || aType == "4" || aType == "route-point")
   {
      mvcType = BRAWLER::i_compr;
   }
   else if (aType == "i_valdfx" || aType == "5" || aType == "defensive")
   {
      mvcType = BRAWLER::i_valdfx;
   }
   else if (aType == "i_gaval" || aType == "6" || aType == "ground avoidance")
   {
      mvcType = BRAWLER::i_gaval;
   }
   else if (aType == "i_vvec" || aType == "7" || aType == "vector following")
   {
      mvcType = BRAWLER::i_vvec;
   }
   else if (aType == "i_villum" || aType == "8" || aType == "illumination")
   {
      mvcType = BRAWLER::i_villum;
   }
   else if (aType == "i_vmusup" || aType == "9" || aType == "mutual support")
   {
      mvcType = BRAWLER::i_vmusup;
   }
   else if (aType == "i_valevd" || aType == "10" || aType == "missile evasion")
   {
      mvcType = BRAWLER::i_valevd;
   }
   else if (aType == "i_vmaim" || aType == "11" || aType == "missile aiming")
   {
      mvcType = BRAWLER::i_vmaim;
   }
   else if (aType == "i_vdisng" || aType == "12" || aType == "disengage")
   {
      mvcType = BRAWLER::i_vdisng;
   }
   else if (aType == "i_sepval" || aType == "13" || aType == "separation")
   {
      mvcType = BRAWLER::i_sepval;
   }
   else if (aType == "i_vescrt" || aType == "14" || aType == "escort bomber")
   {
      mvcType = BRAWLER::i_vescrt;
   }
   else if (aType == "i_vsamz" || aType == "15" || aType == "sam avoidance")
   {
      mvcType = BRAWLER::i_vsamz;
   }
   else if (aType == "i_vavbdd" || aType == "16" || aType == "bad aspect avoidance")
   {
      mvcType = BRAWLER::i_vavbdd;
   }
   else if (aType == "i_hddval" || aType == "17" || aType == "hard-deck")
   {
      mvcType = BRAWLER::i_hddval;
   }
   else if (aType == "i_terrval" || aType == "18" || aType == "terrain")
   {
      mvcType = BRAWLER::i_terrval;
   }
   else if (aType == "i_vdew" || aType == "19" || aType == "directed energy weapon")
   {
      mvcType = BRAWLER::i_vdew;
   }
   else if (aType == "i_vtpt" || aType == "20" || aType == "third party targeting")
   {
      mvcType = BRAWLER::i_vtpt;
   }
   return mvcType;
}

// Helper method that converts string input to WsfBrawlerPlatform::InherentBiasFaultTypes
BRAWLER::InherentBiasFaultTypes GetInherentBiasFaultTypes(std::string& aType)
{
   BRAWLER::InherentBiasFaultTypes ibfType = BRAWLER::i_ovroff;
   if (aType == "i_ovroff" || aType == "1" || aType == "aggressiveness")
   {
      ibfType = BRAWLER::i_ovroff;
   }
   else if (aType == "i_nmusup" || aType == "2" || aType == "mutual support")
   {
      ibfType = BRAWLER::i_nmusup;
   }
   else if (aType == "i_nospd" || aType == "3" || aType == "airspeed maintenance")
   {
      ibfType = BRAWLER::i_nospd;
   }
   return ibfType;
}
} // namespace

WsfScriptBrawlerProcessorClass::WsfScriptBrawlerProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptQuantumTaskerProcessor(aClassName, aScriptTypesPtr)
{
   AddClassName("WsfBrawlerProcessor");

   AddMethod(ut::make_unique<ThreatProcessor>());
   AddMethod(ut::make_unique<GetConsciousnessEventTime>());
   AddMethod(ut::make_unique<GetNextConsciousnessEventTime>());
   AddMethod(ut::make_unique<ScheduleConsciousnessEvent>());
   AddMethod(ut::make_unique<EvaluateRates>());
   AddMethod(ut::make_unique<EvaluateHeadingSpeedAltitude>());
   AddMethod(ut::make_unique<EvaluateTurnInPlaneWithSpeed>());
   AddMethod(ut::make_unique<EvaluateTurnInPlaneWithThrottle>());
   AddMethod(ut::make_unique<EvaluateVectorWithSpeed>());
   AddMethod(ut::make_unique<EvaluateVectorWithThrottle>());
   AddMethod(ut::make_unique<ConvertNEDtoWind>());
   AddMethod(ut::make_unique<ConvertWindtoNED>());
   AddMethod(ut::make_unique<ProjectedTimeDelta>());
   AddMethod(ut::make_unique<OffBoresightAngle>());
   AddMethod(ut::make_unique<MaxAvailableGs>());
   AddMethod(ut::make_unique<MaxSustainedGs>());
   AddMethod(ut::make_unique<MaxTacticalGs>());
   AddMethod(ut::make_unique<MaxForwardAccelWithGravity>());
   AddMethod(ut::make_unique<MinForwardAccelWithGravity>());
   AddMethod(ut::make_unique<RollOverTopValue>());
   AddMethod(ut::make_unique<SlowFlight>());
   AddMethod(ut::make_unique<RawManeuverValueComponent>());
   AddMethod(ut::make_unique<ManeuverValueComponent>());
   AddMethod(ut::make_unique<TunnelVisionMultiplier>());
   AddMethod(ut::make_unique<InherentBiasFaults>());
   AddMethod(ut::make_unique<OffensiveMultiplier>());
   AddMethod(ut::make_unique<DefensiveMultiplier>());
   AddMethod(ut::make_unique<LastEngagementValue>());
   AddMethod(ut::make_unique<DegreeOfThreat>());
   AddMethod(ut::make_unique<DegreeOfThreatCompliment>());
   AddMethod(ut::make_unique<EvasionDirection>());
   AddMethod(ut::make_unique<CorneringVelocity>());
   AddMethod(ut::make_unique<AimPoint>());
   AddMethod(ut::make_unique<Intercept>());
   AddMethod(ut::make_unique<SuggestVectoredFlight>());
   AddMethod(ut::make_unique<LocationNED_1>("LocationNED")); // LocationNED();
   AddMethod(ut::make_unique<LocationNED_2>("LocationNED")); // LocationNED(WsfPlatform);
   AddMethod(ut::make_unique<LocationNED_3>("LocationNED")); // LocationNED(WsfTrack);
   AddMethod(ut::make_unique<LocationNED_4>("LocationNED")); // LocationNED(WsfGeoPoint);

   AddMethod(ut::make_unique<Alpha>());
   AddMethod(ut::make_unique<AlphaCL0>());
   AddMethod(ut::make_unique<Area>());
   AddMethod(ut::make_unique<DynamicPressure>());
   AddMethod(ut::make_unique<Mach>());
   AddMethod(ut::make_unique<Throttle>());
   AddMethod(ut::make_unique<Weight>());
   AddMethod(ut::make_unique<EmptyWeight>());
   AddMethod(ut::make_unique<FuelWeight>());
   AddMethod(ut::make_unique<Lift_1>("Lift"));     // Lift();
   AddMethod(ut::make_unique<Lift_2>("Lift"));     // Lift(alpha, speed, altitude);
   AddMethod(ut::make_unique<Drag_1>("Drag"));     // Drag();
   AddMethod(ut::make_unique<Drag_2>("Drag"));     // Drag(alpha, speed, altitude);
   AddMethod(ut::make_unique<Thrust_1>("Thrust")); // Thrust();
   AddMethod(ut::make_unique<Thrust_2>("Thrust")); // Thrust(throttle, speed, altitude);

   AddMethod(ut::make_unique<SetFlightAggressiveness>());
   AddMethod(ut::make_unique<SetCombatEffectiveness>());
   AddMethod(ut::make_unique<SetForceRatio>());
}

// virtual
void* WsfScriptBrawlerProcessorClass::Create(const UtScriptContext& aContext)
{
   return static_cast<void*>(new WsfBrawlerProcessor(*WsfScriptContext::GetSCENARIO(aContext)));
}

// virtual
void* WsfScriptBrawlerProcessorClass::Clone(void* aObjectPtr)
{
   WsfBrawlerProcessor* objPtr = static_cast<WsfBrawlerProcessor*>(aObjectPtr);
   assert(objPtr != nullptr);
   return objPtr->Clone();
}

// virtual
void WsfScriptBrawlerProcessorClass::Destroy(void* aObjectPtr)
{
   WsfBrawlerProcessor* objPtr = static_cast<WsfBrawlerProcessor*>(aObjectPtr);
   delete objPtr;
}


// UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, QuantumTaskerProcessor, 0,
// "WsfQuantumTaskerProcessor", "")
//{
//    //WsfQuantumTaskerProcessor* procPtr = WsfBrawlerProcessor::Find(*aObjectPtr)->GetQuantumTaskerProcessor();
//    WsfQuantumTaskerProcessor* procPtr = aObjectPtr->GetQuantumTaskerProcessor();
//    aReturnVal.SetPointer(new UtScriptRef(procPtr, aReturnClassPtr));
// }

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, ThreatProcessor, 0, "WsfThreatProcessor", "")
{
   WsfThreatProcessor* procPtr = aObjectPtr->GetThreatProcessor();
   aReturnVal.SetPointer(new UtScriptRef(procPtr, aReturnClassPtr));
}

// Return the time between consciousness events, can be used to schedule when the next event should occur
UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, GetConsciousnessEventTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetConsciousnessEventTime());
}

// Return the time of the next scheduled consciousness event
UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, GetNextConsciousnessEventTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetNextConsciousnessEventTime());
}

// Takes one argument, the time in seconds relative to the start of the simulation
// that the consciousness event is to occur.
UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, ScheduleConsciousnessEvent, 1, "void", "double")
{
   double eventTime = aVarArgs[0].GetDouble();
   aObjectPtr->AddSelfCalledConsciousnessEvent(eventTime);
}

// Accepts Rates data for evaluation and alternative ID
UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, EvaluateRates, 6, "double", " Vec3, Vec3, int, int, int, int")
{
   double   score = 0;
   UtVec3d* angularRates;
   UtVec3d* longitudinalAccel;
   UtScriptDataUnpack(aVarArgs, angularRates, longitudinalAccel);
   int ilevel = aVarArgs[2].GetInt();
   int kalt   = aVarArgs[3].GetInt();
   int icall  = aVarArgs[4].GetInt();
   int lcall  = aVarArgs[5].GetInt();
   aObjectPtr->SetAlternative(ilevel, kalt, icall, lcall);

   score = aObjectPtr->EvaluateRates(*angularRates, *longitudinalAccel);
   aReturnVal.SetDouble(score);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass,
                        WsfBrawlerProcessor,
                        EvaluateHeadingSpeedAltitude,
                        9,
                        "double",
                        "double, double, double, double, double, int, int, int, int")
{
   double score = 0;
   double heading, speed, altitude, maxGs, maxClimb;
   UtScriptDataUnpack(aVarArgs, heading, speed, altitude, maxGs, maxClimb);
   int ilevel = aVarArgs[5].GetInt();
   int kalt   = aVarArgs[6].GetInt();
   int icall  = aVarArgs[7].GetInt();
   int lcall  = aVarArgs[8].GetInt();
   aObjectPtr->SetAlternative(ilevel, kalt, icall, lcall);
   score = aObjectPtr->EvaluateHeadingSpeedAltitude(heading, speed, altitude, maxGs, maxClimb);
   aReturnVal.SetDouble(score);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass,
                        WsfBrawlerProcessor,
                        EvaluateTurnInPlaneWithSpeed,
                        7,
                        "double",
                        "Vec3, double, double, int, int, int, int")
{
   double   score = 0;
   UtVec3d* plane;
   double   Gs, speed;
   UtScriptDataUnpack(aVarArgs, plane, Gs, speed);
   int ilevel = aVarArgs[3].GetInt();
   int kalt   = aVarArgs[4].GetInt();
   int icall  = aVarArgs[5].GetInt();
   int lcall  = aVarArgs[6].GetInt();
   aObjectPtr->SetAlternative(ilevel, kalt, icall, lcall);
   score = aObjectPtr->EvaluateTurnInPlaneWithSpeed(*plane, Gs, speed);
   aReturnVal.SetDouble(score);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass,
                        WsfBrawlerProcessor,
                        EvaluateTurnInPlaneWithThrottle,
                        7,
                        "double",
                        "Vec3, double, double, int, int, int, int")
{
   double   score = 0;
   UtVec3d* plane;
   double   Gs, throttle;
   UtScriptDataUnpack(aVarArgs, plane, Gs, throttle);
   int ilevel = aVarArgs[3].GetInt();
   int kalt   = aVarArgs[4].GetInt();
   int icall  = aVarArgs[5].GetInt();
   int lcall  = aVarArgs[6].GetInt();
   aObjectPtr->SetAlternative(ilevel, kalt, icall, lcall);
   score = aObjectPtr->EvaluateTurnInPlaneWithThrottle(*plane, Gs, throttle);
   aReturnVal.SetDouble(score);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass,
                        WsfBrawlerProcessor,
                        EvaluateVectorWithSpeed,
                        7,
                        "double",
                        "Vec3, double, double, int, int, int, int")
{
   double   score = 0;
   UtVec3d* velocity;
   double   Gs, speed;
   UtScriptDataUnpack(aVarArgs, velocity, Gs, speed);
   int ilevel = aVarArgs[3].GetInt();
   int kalt   = aVarArgs[4].GetInt();
   int icall  = aVarArgs[5].GetInt();
   int lcall  = aVarArgs[6].GetInt();
   aObjectPtr->SetAlternative(ilevel, kalt, icall, lcall);
   score = aObjectPtr->EvaluateVectorWithSpeed(*velocity, Gs, speed);
   aReturnVal.SetDouble(score);
}

// Used by a maneuver alternative to project the result of the maneuver and score the result
// Value returned in (0 ... 1)
UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass,
                        WsfBrawlerProcessor,
                        EvaluateVectorWithThrottle,
                        7,
                        "double",
                        "Vec3, double, double, int, int, int, int")
{
   double   score = 0;
   UtVec3d* velocity;
   double   Gs, throttle;
   UtScriptDataUnpack(aVarArgs, velocity, Gs, throttle);
   int ilevel = aVarArgs[3].GetInt();
   int kalt   = aVarArgs[4].GetInt();
   int icall  = aVarArgs[5].GetInt();
   int lcall  = aVarArgs[6].GetInt();
   aObjectPtr->SetAlternative(ilevel, kalt, icall, lcall);
   score = aObjectPtr->EvaluateVectorWithThrottle(*velocity, Gs, throttle);
   aReturnVal.SetDouble(score);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, ConvertNEDtoWind, 1, "Vec3", "Vec3")
{
   UtVec3d* ned;
   UtScriptDataUnpack(aVarArgs, ned);
   ned->Multiply(UtMath::cFT_PER_M);
   UtVec3d wind = aObjectPtr->ConvertNEDtoWind(*ned);
   wind.Multiply(UtMath::cM_PER_FT);
   aReturnVal.SetPointer(UtScriptVec3::Create(wind));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, ConvertWindtoNED, 1, "Vec3", "Vec3")
{
   UtVec3d* wind;
   UtScriptDataUnpack(aVarArgs, wind);
   wind->Multiply(UtMath::cFT_PER_M);
   UtVec3d ned = aObjectPtr->ConvertWindtoNED(*wind);
   ned.Multiply(UtMath::cM_PER_FT);
   aReturnVal.SetPointer(UtScriptVec3::Create(ned));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, ProjectedTimeDelta, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->ProjectedTimeDelta());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, OffBoresightAngle, 2, "double", "WsfPlatform, WsfPlatform")
{
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfPlatform* targetPtr   = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   aReturnVal.SetDouble(aObjectPtr->OffBoreSightAngle(*platformPtr, *targetPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, MaxAvailableGs, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->MaxInstantaneousGLoad_g());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, MaxSustainedGs, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->MaxSustainedGLoad_g());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, MaxTacticalGs, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->MaxTacticalGLoad_g());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, MaxForwardAccelWithGravity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->MaxForwardAccelerationWithGravity_fps2() * UtMath::cM_PER_FT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, MinForwardAccelWithGravity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->MinForwardAccelerationWithGravity_fps2() * UtMath::cM_PER_FT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, RollOverTopValue, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->RollOverTopValue());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, SlowFlight, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->SlowFlight());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, RawManeuverValueComponent, 1, "double", "string")
{
   double      rmvc = 0;
   std::string type = aVarArgs[0].GetString();
   UtStringUtil::ToLower(type);
   rmvc = aObjectPtr->RawManeuverValueComponent(GetManeuverValueComponentType(type));
   aReturnVal.SetDouble(rmvc);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, ManeuverValueComponent, 1, "double", "string")
{
   double      mvc  = 0;
   std::string type = aVarArgs[0].GetString();
   UtStringUtil::ToLower(type);
   mvc = aObjectPtr->ManeuverValueComponent(GetManeuverValueComponentType(type));
   aReturnVal.SetDouble(mvc);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, TunnelVisionMultiplier, 1, "double", "string")
{
   std::string type = aVarArgs[0].GetString();
   UtStringUtil::ToLower(type);
   double tvm = aObjectPtr->TunnelVisionMultiplier(GetManeuverValueComponentType(type));
   aReturnVal.SetDouble(tvm);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, InherentBiasFaults, 1, "double", "string")
{
   std::string type = aVarArgs[0].GetString();
   UtStringUtil::ToLower(type);
   double ibf = aObjectPtr->InherentBiasFaults(GetInherentBiasFaultTypes(type));
   aReturnVal.SetDouble(ibf);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, OffensiveMultiplier, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->OffensiveMultiplier());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, DefensiveMultiplier, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->DefensiveMultiplier());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, LastEngagementValue, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetDouble(aObjectPtr->LastComputedEngagementValue(*platformPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, DegreeOfThreat, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetDouble(aObjectPtr->DegreeOfThreat(*platformPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, DegreeOfThreatCompliment, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetDouble(aObjectPtr->DegreeOfThreatCompliment(*platformPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, EvasionDirection, 0, "Vec3", "")
{
   UtVec3d meters(aObjectPtr->EvasionDirection());
   meters.Multiply(UtMath::cM_PER_FT);
   aReturnVal.SetPointer(UtScriptVec3::Create(meters));
}


UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, CorneringVelocity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->CorneringVelocity_fps() * UtMath::cM_PER_FT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, AimPoint, 5, "Vec3", "Vec3, double, Vec3, Vec3, double")
{
   UtVec3d* xaPtr;
   double   sa;
   UtVec3d* xtPtr;
   UtVec3d* vtPtr;
   double   st;
   UtVec3d  aimp;
   UtScriptDataUnpack(aVarArgs, xaPtr, sa, xtPtr, vtPtr, st);

   // Convert metric to imperial units
   sa *= UtMath::cFT_PER_M;
   xtPtr->Multiply(UtMath::cFT_PER_M);
   vtPtr->Multiply(UtMath::cFT_PER_M);
   st *= UtMath::cFT_PER_M;

   if (BRAWLER::AimPoint(*xaPtr, sa, *xtPtr, *vtPtr, st, aimp))
   {
      // Convert back to metric units
      aimp.Multiply(UtMath::cM_PER_FT);
      aReturnVal.SetPointer(UtScriptVec3::Create(aimp));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptVec3::Create(UtVec3d())); // zeros
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass,
                        WsfBrawlerProcessor,
                        Intercept,
                        7,
                        "Vec3",
                        "Vec3, double, Vec3, Vec3, double, double, double")
{
   UtVec3d* xaPtr;
   double   sa;
   UtVec3d* xtPtr;
   UtVec3d* vtPtr;
   double   closmn;
   double   angint;
   double   hdes;
   UtVec3d  desdir;
   double   sdes;
   UtScriptDataUnpack(aVarArgs, xaPtr, sa, xtPtr, vtPtr, closmn, angint, hdes);

   // Convert metric to imperial units
   xaPtr->Multiply(UtMath::cFT_PER_M);
   sa *= UtMath::cFT_PER_M;
   xtPtr->Multiply(UtMath::cFT_PER_M);
   hdes *= UtMath::cFT_PER_M;

   BRAWLER::intcpt(*xaPtr, sa, *xtPtr, *vtPtr, closmn, angint, hdes, desdir, sdes);
   // combine direction & speed into one velocity vector return value
   desdir.Normalize();
   desdir.Multiply(sdes);

   // Convert back to metric units
   desdir.Multiply(UtMath::cM_PER_FT);

   aReturnVal.SetPointer(UtScriptVec3::Create(desdir));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, SuggestVectoredFlight, 3, "void", "Vec3, double, double")
{
   UtVec3d* vecPtr;
   double   val;
   double   spd;
   UtScriptDataUnpack(aVarArgs, vecPtr, val, spd);

   // Convert metric to imperial units
   vecPtr->Multiply(UtMath::cFT_PER_M);
   vecPtr->Normalize();
   spd *= UtMath::cFT_PER_M;
   aObjectPtr->SuggestVectoredFlight(*vecPtr, val, spd);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, LocationNED_1, 0, "Vec3", "")
{
   // UtVec3d *ned;
   // UtScriptDataUnpack(aVarArgs, ned);
   // UtVec3d wind = aObjectPtr->ConvertNEDtoWind(*ned);
   UtVec3d ned = aObjectPtr->PositionNED();
   ned.Multiply(UtMath::cM_PER_FT);
   aReturnVal.SetPointer(UtScriptVec3::Create(ned));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, LocationNED_2, 1, "Vec3", "WsfPlatform")
{
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   UtVec3d      lla;
   platformPtr->GetLocationLLA(lla[0], lla[1], lla[2]);
   UtVec3d ned = BrawlerCoordinateConversion::ConvertLLAtoNED(lla);
   ned.Multiply(UtMath::cM_PER_FT);
   aReturnVal.SetPointer(UtScriptVec3::Create(ned));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, LocationNED_3, 1, "Vec3", "WsfTrack")
{
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   UtVec3d   lla;
   trackPtr->GetLocationLLA(lla[0], lla[1], lla[2]);
   UtVec3d ned = BrawlerCoordinateConversion::ConvertLLAtoNED(lla);
   ned.Multiply(UtMath::cM_PER_FT);
   aReturnVal.SetPointer(UtScriptVec3::Create(ned));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, LocationNED_4, 1, "Vec3", "WsfGeoPoint")
{
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   UtVec3d      lla;
   pointPtr->GetLocationLLA(lla[0], lla[1], lla[2]);
   UtVec3d ned = BrawlerCoordinateConversion::ConvertLLAtoNED(lla);
   ned.Multiply(UtMath::cM_PER_FT);
   aReturnVal.SetPointer(UtScriptVec3::Create(ned));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, Alpha, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->Alpha());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, AlphaCL0, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->AlphaCL0());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, Area, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->Area() * UtMath::cM_PER_FT * UtMath::cM_PER_FT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, DynamicPressure, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->DynamicPressure_psf() * UtMath::cPASCAL_PER_PSF);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, Mach, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->Mach());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, Throttle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->Throttle());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, Weight, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->CurrentWeight_lbs() * UtMath::cKG_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, EmptyWeight, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->EmptyWeight_lbs() * UtMath::cKG_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, FuelWeight, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->CurrentFuelWeight_lbs() * UtMath::cKG_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, Lift_1, 0, "double", "")
{
   WsfBrawlerProcessor* aProcPtr = aObjectPtr;
   double               alpha    = aProcPtr->Alpha();      // Angle of attack, degrees
   double               speed    = aObjectPtr->GetSpeed(); // feet/s

   double altitude = aObjectPtr->GetPlatform()->GetAltitude(); // meters

   double lift = aProcPtr->Lift(alpha, speed * UtMath::cM_PER_FT, altitude);
   aReturnVal.SetDouble(lift * UtMath::cNT_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, Lift_2, 3, "double", "double, double, double")
{
   WsfBrawlerProcessor* aProcPtr = aObjectPtr;
   double               alpha    = aVarArgs[0].GetDouble();
   double               speed    = aVarArgs[1].GetDouble();
   double               altitude = aVarArgs[2].GetDouble();
   double               lift     = aProcPtr->Lift(alpha, speed, altitude);
   aReturnVal.SetDouble(lift * UtMath::cNT_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, Drag_1, 0, "double", "")
{
   WsfBrawlerProcessor* aProcPtr = aObjectPtr;
   double               alpha    = aProcPtr->Alpha();                        // Angle of attack, degrees
   double               speed    = aObjectPtr->GetSpeed();                   // feet/s
   double               altitude = aObjectPtr->GetPlatform()->GetAltitude(); // meters
   double               drag     = aProcPtr->Drag(alpha, speed * UtMath::cM_PER_FT, altitude);
   aReturnVal.SetDouble(drag * UtMath::cNT_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, Drag_2, 3, "double", "double, double, double")
{
   WsfBrawlerProcessor* aProcPtr = aObjectPtr;
   double               alpha    = aVarArgs[0].GetDouble();
   double               speed    = aVarArgs[1].GetDouble();
   double               altitude = aVarArgs[2].GetDouble();
   double               drag     = aProcPtr->Drag(alpha, speed, altitude);
   aReturnVal.SetDouble(drag * UtMath::cNT_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, Thrust_1, 0, "double", "")
{
   WsfBrawlerProcessor* aProcPtr = aObjectPtr;
   double               throttle = aProcPtr->Throttle();
   double               speed    = aObjectPtr->GetSpeed();                   // feet/s
   double               altitude = aObjectPtr->GetPlatform()->GetAltitude(); // meters
   double               thrust   = aProcPtr->Thrust(throttle, speed * UtMath::cM_PER_FT, altitude);
   aReturnVal.SetDouble(thrust * UtMath::cNT_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, Thrust_2, 3, "double", "double, double, double")
{
   WsfBrawlerProcessor* aProcPtr = aObjectPtr;
   double               throttle = aVarArgs[0].GetDouble();
   double               speed    = aVarArgs[1].GetDouble();
   double               altitude = aVarArgs[2].GetDouble();
   double               thrust   = aProcPtr->Thrust(throttle, speed, altitude);
   aReturnVal.SetDouble(thrust * UtMath::cNT_PER_LB);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, SetFlightAggressiveness, 1, "void", "double")
{
   aObjectPtr->SetFlightAggressiveness(aVarArgs[0].GetDouble());
}
UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, SetCombatEffectiveness, 1, "void", "double")
{
   aObjectPtr->SetCombatEffectiveness(aVarArgs[0].GetDouble());
}
UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerProcessorClass, WsfBrawlerProcessor, SetForceRatio, 1, "void", "double")
{
   aObjectPtr->SetForceRatio(aVarArgs[0].GetDouble());
}
// void WsfBrawlerExtendScriptClasses(UtScriptTypes& aTypes)
//{
//    // WsfPlatform
//    {
//       using namespace WsfBrawlerPlatformScriptMethods;
//       aTypes.AddClassMethod("WsfPlatform", new QuantumTaskerProcessor);
//       aTypes.AddClassMethod("WsfPlatform", new ThreatProcessor);
//       aTypes.AddClassMethod("WsfPlatform", new GetConsciousnessEventTime);
//       aTypes.AddClassMethod("WsfPlatform", new GetNextConsciousnessEventTime);
//       aTypes.AddClassMethod("WsfPlatform", new ScheduleConsciousnessEvent);
//       aTypes.AddClassMethod("WsfPlatform", new EvaluateRates);
//       aTypes.AddClassMethod("WsfPlatform", new EvaluateHeadingSpeedAltitude);
//       aTypes.AddClassMethod("WsfPlatform", new EvaluateTurnInPlaneWithSpeed);
//       aTypes.AddClassMethod("WsfPlatform", new EvaluateTurnInPlaneWithThrottle);
//       aTypes.AddClassMethod("WsfPlatform", new EvaluateVectorWithSpeed);
//       aTypes.AddClassMethod("WsfPlatform", new EvaluateVectorWithThrottle);
//       aTypes.AddClassMethod("WsfPlatform", new ConvertNEDtoWind);
//       aTypes.AddClassMethod("WsfPlatform", new ConvertWindtoNED);
//       aTypes.AddClassMethod("WsfPlatform", new ProjectedTimeDelta);
//       aTypes.AddClassMethod("WsfPlatform", new OffBoresightAngle);
//       aTypes.AddClassMethod("WsfPlatform", new MaxAvailableGs);
//       aTypes.AddClassMethod("WsfPlatform", new MaxSustainedGs);
//       aTypes.AddClassMethod("WsfPlatform", new MaxTacticalGs);
//       aTypes.AddClassMethod("WsfPlatform", new MaxForwardAccelWithGravity);
//       aTypes.AddClassMethod("WsfPlatform", new MinForwardAccelWithGravity);
//       aTypes.AddClassMethod("WsfPlatform", new RollOverTopValue);
//       aTypes.AddClassMethod("WsfPlatform", new SlowFlight);
//       aTypes.AddClassMethod("WsfPlatform", new RawManeuverValueComponent);
//       aTypes.AddClassMethod("WsfPlatform", new ManeuverValueComponent);
//       aTypes.AddClassMethod("WsfPlatform", new TunnelVisionMultiplier);
//       aTypes.AddClassMethod("WsfPlatform", new InherentBiasFaults);
//       aTypes.AddClassMethod("WsfPlatform", new OffensiveMultiplier);
//       aTypes.AddClassMethod("WsfPlatform", new DefensiveMultiplier);
//       aTypes.AddClassMethod("WsfPlatform", new LastEngagementValue);
//       aTypes.AddClassMethod("WsfPlatform", new DegreeOfThreat);
//       aTypes.AddClassMethod("WsfPlatform", new DegreeOfThreatCompliment);
//       aTypes.AddClassMethod("WsfPlatform", new EvasionDirection);
//       aTypes.AddClassMethod("WsfPlatform", new CorneringVelocity);
//       aTypes.AddClassMethod("WsfPlatform", new AimPoint);
//       aTypes.AddClassMethod("WsfPlatform", new Intercept);
//       aTypes.AddClassMethod("WsfPlatform", new SuggestVectoredFlight);
//       aTypes.AddClassMethod("WsfPlatform", new LocationNED_1("LocationNED"));    // LocationNED();
//       aTypes.AddClassMethod("WsfPlatform", new LocationNED_2("LocationNED"));    // LocationNED(WsfPlatform);
//       aTypes.AddClassMethod("WsfPlatform", new LocationNED_3("LocationNED"));    // LocationNED(WsfTrack);
//       aTypes.AddClassMethod("WsfPlatform", new LocationNED_4("LocationNED"));    // LocationNED(WsfGeoPoint);
//
//       aTypes.AddClassMethod("WsfPlatform", new Alpha);
//       aTypes.AddClassMethod("WsfPlatform", new AlphaCL0);
//       aTypes.AddClassMethod("WsfPlatform", new Area);
//       aTypes.AddClassMethod("WsfPlatform", new DynamicPressure);
//       aTypes.AddClassMethod("WsfPlatform", new Mach);
//       aTypes.AddClassMethod("WsfPlatform", new Throttle);
//       aTypes.AddClassMethod("WsfPlatform", new Weight);
//       aTypes.AddClassMethod("WsfPlatform", new Lift_1("Lift"));    // Lift();
//       aTypes.AddClassMethod("WsfPlatform", new Lift_2("Lift"));    // Lift(alpha, speed, altitude);
//       aTypes.AddClassMethod("WsfPlatform", new Drag_1("Drag"));    // Drag();
//       aTypes.AddClassMethod("WsfPlatform", new Drag_2("Drag"));    // Drag(alpha, speed, altitude);
//       aTypes.AddClassMethod("WsfPlatform", new Thrust_1("Thrust"));    // Thrust();
//       aTypes.AddClassMethod("WsfPlatform", new Thrust_2("Thrust"));    // Thrust(throttle, speed, altitude);
//    }
// }
