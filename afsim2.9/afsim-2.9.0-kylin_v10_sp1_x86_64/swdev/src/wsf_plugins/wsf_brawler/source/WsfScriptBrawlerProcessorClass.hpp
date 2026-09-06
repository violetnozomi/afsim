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

#ifndef WSFSCRIPTBRAWLERPROCESSORCLASS_HPP
#define WSFSCRIPTBRAWLERPROCESSORCLASS_HPP

#include "wsf_brawler_export.h"

//#include "UtScriptClassDefine.hpp"
#include "WsfBrawlerProcessor.hpp"
#include "script/WsfScriptQuantumTaskerProcessor.hpp"

// void WSF_BRAWLER_EXPORT WsfScriptBrawlerProcessorClass(UtScriptTypes& aTypes);

class WSF_BRAWLER_EXPORT WsfScriptBrawlerProcessorClass : public WsfScriptQuantumTaskerProcessor
{
public:
   WsfScriptBrawlerProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   // See the base class for a description of Create, Clone, and Destroy.

   void* Create(const UtScriptContext& aContext) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   // UT_DECLARE_SCRIPT_METHOD(QuantumTaskerProcessor);
   UT_DECLARE_SCRIPT_METHOD(ThreatProcessor);
   UT_DECLARE_SCRIPT_METHOD(GetConsciousnessEventTime);
   UT_DECLARE_SCRIPT_METHOD(GetNextConsciousnessEventTime);
   UT_DECLARE_SCRIPT_METHOD(ScheduleConsciousnessEvent);
   UT_DECLARE_SCRIPT_METHOD(EvaluateRates);
   UT_DECLARE_SCRIPT_METHOD(EvaluateHeadingSpeedAltitude);
   UT_DECLARE_SCRIPT_METHOD(EvaluateTurnInPlaneWithSpeed);
   UT_DECLARE_SCRIPT_METHOD(EvaluateTurnInPlaneWithThrottle);
   UT_DECLARE_SCRIPT_METHOD(EvaluateVectorWithSpeed);
   UT_DECLARE_SCRIPT_METHOD(EvaluateVectorWithThrottle);
   UT_DECLARE_SCRIPT_METHOD(ConvertNEDtoWind);
   UT_DECLARE_SCRIPT_METHOD(ConvertWindtoNED);
   UT_DECLARE_SCRIPT_METHOD(ProjectedTimeDelta);
   UT_DECLARE_SCRIPT_METHOD(OffBoresightAngle);
   UT_DECLARE_SCRIPT_METHOD(MaxAvailableGs);
   UT_DECLARE_SCRIPT_METHOD(MaxSustainedGs);
   UT_DECLARE_SCRIPT_METHOD(MaxTacticalGs);
   UT_DECLARE_SCRIPT_METHOD(MaxForwardAccelWithGravity);
   UT_DECLARE_SCRIPT_METHOD(MinForwardAccelWithGravity);
   UT_DECLARE_SCRIPT_METHOD(RollOverTopValue);
   UT_DECLARE_SCRIPT_METHOD(SlowFlight);
   UT_DECLARE_SCRIPT_METHOD(RawManeuverValueComponent);
   UT_DECLARE_SCRIPT_METHOD(ManeuverValueComponent);
   UT_DECLARE_SCRIPT_METHOD(TunnelVisionMultiplier);
   UT_DECLARE_SCRIPT_METHOD(InherentBiasFaults);
   UT_DECLARE_SCRIPT_METHOD(OffensiveMultiplier);
   UT_DECLARE_SCRIPT_METHOD(DefensiveMultiplier);
   UT_DECLARE_SCRIPT_METHOD(LastEngagementValue);
   UT_DECLARE_SCRIPT_METHOD(DegreeOfThreat);
   UT_DECLARE_SCRIPT_METHOD(DegreeOfThreatCompliment);
   UT_DECLARE_SCRIPT_METHOD(EvasionDirection);
   UT_DECLARE_SCRIPT_METHOD(CorneringVelocity);
   UT_DECLARE_SCRIPT_METHOD(AimPoint);
   UT_DECLARE_SCRIPT_METHOD(Intercept);
   UT_DECLARE_SCRIPT_METHOD(SuggestVectoredFlight);

   UT_DECLARE_SCRIPT_METHOD(LocationNED_1); // LocationNED();
   UT_DECLARE_SCRIPT_METHOD(LocationNED_2); // LocationNED(WsfPlatform);
   UT_DECLARE_SCRIPT_METHOD(LocationNED_3); // LocationNED(WsfTrack);
   UT_DECLARE_SCRIPT_METHOD(LocationNED_4); // LocationNED(WsfGeoPoint);

   UT_DECLARE_SCRIPT_METHOD(Alpha);
   UT_DECLARE_SCRIPT_METHOD(AlphaCL0);
   UT_DECLARE_SCRIPT_METHOD(Area);
   UT_DECLARE_SCRIPT_METHOD(DynamicPressure);
   UT_DECLARE_SCRIPT_METHOD(Mach);
   UT_DECLARE_SCRIPT_METHOD(Throttle);
   UT_DECLARE_SCRIPT_METHOD(Weight);
   UT_DECLARE_SCRIPT_METHOD(EmptyWeight);
   UT_DECLARE_SCRIPT_METHOD(FuelWeight);
   UT_DECLARE_SCRIPT_METHOD(Lift_1);   // Lift();
   UT_DECLARE_SCRIPT_METHOD(Lift_2);   // Lift(alpha, speed, altitude);
   UT_DECLARE_SCRIPT_METHOD(Drag_1);   // Drag();
   UT_DECLARE_SCRIPT_METHOD(Drag_2);   // Drag(alpha, speed, altitude);
   UT_DECLARE_SCRIPT_METHOD(Thrust_1); // Thrust();
   UT_DECLARE_SCRIPT_METHOD(Thrust_2); // Thrust(throttle, speed, altitude);

   UT_DECLARE_SCRIPT_METHOD(SetFlightAggressiveness);
   UT_DECLARE_SCRIPT_METHOD(SetCombatEffectiveness);
   UT_DECLARE_SCRIPT_METHOD(SetForceRatio);
};

#endif
