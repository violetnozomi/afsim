// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include "WsfScriptBMTrackMessageClass.hpp"
#include "WsfBMTrackMessage.hpp"
#include <UtInput.hpp>

#include <WsfCovariance.hpp>
#include <iadsLib/enum.hpp>
#include <WsfBMUtils.hpp>
#include <script/WsfScriptDefs.hpp>

const char* WsfScriptBMTrackMessageClass::SCRIPT_CLASS_NAME = "WsfBMTrackMessage";
const char* WsfScriptBMTrackMessageClass::BASE_CLASS_NAME = "WSF_BM_TRACK_MESSAGE";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Script Class /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WsfScriptBMTrackMessageClass::WsfScriptBMTrackMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName(SCRIPT_CLASS_NAME);

   mConstructible = true;
   mCloneable = true;

   AddMethod(ut::make_unique<SetUpdateInterval>());
   AddMethod(ut::make_unique<GetUpdateInterval>());
   AddMethod(ut::make_unique<SetTrackID>());
   AddMethod(ut::make_unique<GetTrackingSystemID>());
   AddMethod(ut::make_unique<GetTrackingSystemTrackID>());
   AddMethod(ut::make_unique<SetPosition>());
   AddMethod(ut::make_unique<GetPosition>());
   AddMethod(ut::make_unique<GetPositionTime>());
   AddMethod(ut::make_unique<SetVelocityECEF>());
   AddMethod(ut::make_unique<GetVelocityECEF>());
   AddMethod(ut::make_unique<SetCovarianceMatrix>());
   AddMethod(ut::make_unique<GetCovarianceMatrix>());
   AddMethod(ut::make_unique<SetHeadingENUDegs>());
   AddMethod(ut::make_unique<SetOrientationWCSDegs>());
   AddMethod(ut::make_unique<GetOrientationWCSPsiDegs>());
   AddMethod(ut::make_unique<GetOrientationWCSThetaDegs>());
   AddMethod(ut::make_unique<GetOrientationWCSPhiDegs>());
   AddMethod(ut::make_unique<SetTargetType>());
   AddMethod(ut::make_unique<GetTargetType>());
   AddMethod(ut::make_unique<GetTargetSubType>());
   AddMethod(ut::make_unique<SetTrackingStatusNormal>());
   AddMethod(ut::make_unique<SetTrackingStatusCoasting>());
   AddMethod(ut::make_unique<SetTrackingStatusTimedOut>());
   AddMethod(ut::make_unique<SetTrackingStatusDropping>());
   AddMethod(ut::make_unique<SetTrackingStatusDropped>());
   AddMethod(ut::make_unique<IsTrackingStatusNormal>());
   AddMethod(ut::make_unique<IsTrackingStatusCoasting>());
   AddMethod(ut::make_unique<IsTrackingStatusTimedOut>());
   AddMethod(ut::make_unique<IsTrackingStatusDropping>());
   AddMethod(ut::make_unique<IsTrackingStatusDropped>());
   AddMethod(ut::make_unique<SetAltitudeReliable>());
   AddMethod(ut::make_unique<GetAltitudeReliable>());
   AddMethod(ut::make_unique<SetIFFUnknown>());
   AddMethod(ut::make_unique<SetIFFFriendly>());
   AddMethod(ut::make_unique<SetIFFHostile>());
   AddMethod(ut::make_unique<SetIFFNeutral>());
   AddMethod(ut::make_unique<IsIFFUnknown>());
   AddMethod(ut::make_unique<IsIFFFriendly>());
   AddMethod(ut::make_unique<IsIFFHostile>());
   AddMethod(ut::make_unique<IsIFFNeutral>());
   AddMethod(ut::make_unique<SetManeuveringFlag>());
   AddMethod(ut::make_unique<GetManeuveringFlag>());
   AddMethod(ut::make_unique<SetQuantity>());
   AddMethod(ut::make_unique<GetQuantity>());
   AddMethod(ut::make_unique<SetJamming>());
   AddMethod(ut::make_unique<GetJamming>());
   AddMethod(ut::make_unique<SetJammingPower>());
   AddMethod(ut::make_unique<GetJammingPower>());
   AddMethod(ut::make_unique<SetTargetTruthInfo>());
   AddMethod(ut::make_unique<SetReportingSensorTypeEW>());
   AddMethod(ut::make_unique<SetReportingSensorTypeTAR>());
   AddMethod(ut::make_unique<SetReportingSensorTypeTTR>());
   AddMethod(ut::make_unique<IsReportingSensorTypeEW>());
   AddMethod(ut::make_unique<IsReportingSensorTypeTAR>());
   AddMethod(ut::make_unique<IsReportingSensorTypeTTR>());
}

WsfScriptBMTrackMessageClass::~WsfScriptBMTrackMessageClass()
{
}

void* WsfScriptBMTrackMessageClass::Create(const UtScriptContext& aContext)
{
   auto sim = WsfScriptContext::GetSIMULATION(aContext);
   WsfBMTrackMessage* msg_ptr = new WsfBMTrackMessage(sim);
   return msg_ptr;
}

void* WsfScriptBMTrackMessageClass::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfBMTrackMessage*>(aObjectPtr)->Clone();
}

void  WsfScriptBMTrackMessageClass::Destroy(void* aObjectPtr)
{
   WsfBMTrackMessage* objPtr = reinterpret_cast<WsfBMTrackMessage*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetUpdateInterval, 1, "void", "double")
{
   aObjectPtr->SetUpdateInterval(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetUpdateInterval, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetUpdateInterval());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetTrackID, 1, "void", "WsfTrackId")
{
   const auto track_id = reinterpret_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetTrackID(*track_id);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetTrackingSystemID, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetTrackingSystemID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetTrackingSystemTrackID, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetTrackingSystemTrackID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetPosition, 2, "void", "double, WsfGeoPoint")
{
   auto position_time = aVarArgs[0].GetDouble();
   const WsfGeoPoint* position_ptr = reinterpret_cast<WsfGeoPoint*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->SetPosition(position_time, *position_ptr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetPosition, 0, "WsfGeoPoint", "")
{
   WsfGeoPoint* position = new WsfGeoPoint(aObjectPtr->GetPosition());
   aReturnVal.SetPointer(new UtScriptRef(position, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetPositionTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPositionTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetVelocityECEF, 1, "void", "Vec3")
{
   const UtVec3d* vel = reinterpret_cast<UtVec3d*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetVelocityECEF(*vel);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetVelocityECEF, 0, "Vec3", "")
{
   UtVec3d* vel = new UtVec3d(aObjectPtr->GetVelocityECEF());
   aReturnVal.SetPointer(new UtScriptRef(vel, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetCovarianceMatrix, 1, "void", "WsfCovariance")
{
   const WsfCovariance* covariance = reinterpret_cast<WsfCovariance*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetCovarianceMatrix(*covariance);
}

// Note: you will need to call CopyTrackData() on this object once it's retreived to set up the object fully
UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetCovarianceMatrix, 0, "WsfCovariance", "")
{
   WsfCovariance* covariance_ptr = new WsfCovariance(aObjectPtr->GetCovarianceMatrix());
   aReturnVal.SetPointer(new UtScriptRef(covariance_ptr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetHeadingENUDegs, 1, "void", "double")
{
   aObjectPtr->SetHeadingENUDegs(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetOrientationWCSDegs, 3, "void", "double, double, double")
{
   aObjectPtr->SetOrientationWCSDegs(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetOrientationWCSPsiDegs, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrientationWCSPsiDegs());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetOrientationWCSThetaDegs, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrientationWCSThetaDegs());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetOrientationWCSPhiDegs, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrientationWCSPhiDegs());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetTargetType, 2, "void", "string, string")
{
   auto type_str = aVarArgs[0].GetString();
   auto subtype_str = aVarArgs[1].GetString();
   aObjectPtr->SetTargetType(type_str, subtype_str);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetTargetType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTargetType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetTargetSubType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTargetSubType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetTrackingStatusNormal, 0, "void", "")
{
   aObjectPtr->SetTrackingStatus(WsfBMTrackMessage::STATUS_NORMAL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetTrackingStatusCoasting, 0, "void", "")
{
   aObjectPtr->SetTrackingStatus(WsfBMTrackMessage::STATUS_COASTING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetTrackingStatusTimedOut, 0, "void", "")
{
   aObjectPtr->SetTrackingStatus(WsfBMTrackMessage::STATUS_TIMEDOUT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetTrackingStatusDropping, 0, "void", "")
{
   aObjectPtr->SetTrackingStatus(WsfBMTrackMessage::STATUS_DROPPING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetTrackingStatusDropped, 0, "void", "")
{
   aObjectPtr->SetTrackingStatus(WsfBMTrackMessage::STATUS_DROPPED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsTrackingStatusNormal, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetTrackingStatus() == WsfBMTrackMessage::STATUS_NORMAL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsTrackingStatusCoasting, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetTrackingStatus() == WsfBMTrackMessage::STATUS_COASTING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsTrackingStatusTimedOut, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetTrackingStatus() == WsfBMTrackMessage::STATUS_TIMEDOUT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsTrackingStatusDropping, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetTrackingStatus() == WsfBMTrackMessage::STATUS_DROPPING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsTrackingStatusDropped, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetTrackingStatus() == WsfBMTrackMessage::STATUS_DROPPED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetAltitudeReliable, 1, "void", "bool")
{
   aObjectPtr->SetAltitudeReliable(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetAltitudeReliable, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAltitudeReliable());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetIFFUnknown, 0, "void", "")
{
   aObjectPtr->SetIFF(WsfTrack::cIFF_UNKNOWN);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetIFFFriendly, 0, "void", "")
{
   aObjectPtr->SetIFF(WsfTrack::cIFF_FRIEND);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetIFFHostile, 0, "void", "")
{
   aObjectPtr->SetIFF(WsfTrack::cIFF_FOE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetIFFNeutral, 0, "void", "")
{
   aObjectPtr->SetIFF(WsfTrack::cIFF_NEUTRAL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsIFFUnknown, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIFF() == WsfTrack::cIFF_UNKNOWN || aObjectPtr->GetIFF() == WsfTrack::cIFF_AMBIGUOUS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsIFFFriendly, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIFF() == WsfTrack::cIFF_FRIEND);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsIFFHostile, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIFF() == WsfTrack::cIFF_FOE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsIFFNeutral, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIFF() == WsfTrack::cIFF_NEUTRAL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetManeuveringFlag, 1, "void", "bool")
{
   aObjectPtr->SetManeuveringFlag(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetManeuveringFlag, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetManeuveringFlag());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetQuantity, 1, "void", "int")
{
   aObjectPtr->SetQuantity(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetQuantity, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetQuantity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetJamming, 1, "void", "bool")
{
   aObjectPtr->SetJamming(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetJamming, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetJamming());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetJammingPower, 1, "void", "double")
{
   aObjectPtr->SetJammingPower(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, GetJammingPower, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetJammingPower());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetTargetTruthInfo, 1, "void", "WsfPlatform")
{
   const WsfPlatform* target = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());

   aObjectPtr->SetTargetTruthInfo(target);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetReportingSensorTypeEW, 0, "void", "")
{
   aObjectPtr->SetReportingSensorType(il::SENSOR_EW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetReportingSensorTypeTAR, 0, "void", "")
{
   aObjectPtr->SetReportingSensorType(il::SENSOR_TAR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, SetReportingSensorTypeTTR, 0, "void", "")
{
   aObjectPtr->SetReportingSensorType(il::SENSOR_TTR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsReportingSensorTypeEW, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetReportingSensorType() == il::SENSOR_EW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsReportingSensorTypeTAR, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetReportingSensorType() == il::SENSOR_TAR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMTrackMessageClass, WsfBMTrackMessage, IsReportingSensorTypeTTR, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetReportingSensorType() == il::SENSOR_TTR);
}
