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

#include "WsfScriptTargetPoint.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptVec3.hpp"
#include "UtVec3.hpp"
#include "WsfLibrationPoint.hpp"
#include "WsfOrbitalReferenceFrame.hpp"
#include "WsfTargetPointOptions.hpp"
#include "WsfTrackId.hpp"

WsfScriptTargetPoint::WsfScriptTargetPoint(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTargetPoint");

   AddStaticMethod(ut::make_unique<ConstructFromTrack>());
   AddStaticMethod(ut::make_unique<ConstructFromLibrationPoint>());
   AddStaticMethod(ut::make_unique<ConstructFromKinematicStatePosition>("ConstructFromKinematicState"));
   AddStaticMethod(ut::make_unique<ConstructFromKinematicState>("ConstructFromKinematicState"));
   AddMethod(ut::make_unique<HasTrackTarget>());
   AddMethod(ut::make_unique<HasLibrationPointTarget>());
   AddMethod(ut::make_unique<HasKinematicStateTarget>());
   AddMethod(ut::make_unique<HasOffset>()); // DEPRECATED
   AddMethod(ut::make_unique<HasPositionOffset>());
   AddMethod(ut::make_unique<HasVelocityOffset>());
   AddMethod(ut::make_unique<HasOffsetTime>());
   AddMethod(ut::make_unique<HasLagTime>());
   AddMethod(ut::make_unique<OffsetFrame>()); // DEPRECATED
   AddMethod(ut::make_unique<Offset>());      // DEPRECATED
   AddMethod(ut::make_unique<PositionOffsetFrame>());
   AddMethod(ut::make_unique<PositionOffset>());
   AddMethod(ut::make_unique<VelocityOffsetFrame>());
   AddMethod(ut::make_unique<VelocityOffset>());
   AddMethod(ut::make_unique<OffsetTime>());
   AddMethod(ut::make_unique<LagTime>());
   AddMethod(ut::make_unique<LibrationPointSystem>());
   AddMethod(ut::make_unique<LibrationPointSelection>());
   AddMethod(ut::make_unique<TrackId>());
   AddMethod(ut::make_unique<KinematicStateTargetPosition>());
   AddMethod(ut::make_unique<KinematicStateTargetVelocity>());
   AddMethod(ut::make_unique<ClearOffset>()); // DEPRECATED
   AddMethod(ut::make_unique<SetOffset>());   // DEPRECATED
   AddMethod(ut::make_unique<ClearPositionOffset>());
   AddMethod(ut::make_unique<SetPositionOffset>());
   AddMethod(ut::make_unique<ClearVelocityOffset>());
   AddMethod(ut::make_unique<SetVelocityOffset>());
   AddMethod(ut::make_unique<ClearOffsetTime>());
   AddMethod(ut::make_unique<SetOffsetTime>());
   AddMethod(ut::make_unique<ClearLagTime>());
   AddMethod(ut::make_unique<SetLagTime>());
}

void WsfScriptTargetPoint::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::space::TargetPointOptions*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, ConstructFromTrack, 1, "WsfTargetPoint", "WsfTrackId")
{
   auto trackIdPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfTrackId>();
   if (trackIdPtr->IsNull())
   {
      UT_SCRIPT_ABORT("Provided track id is invalid.");
   }
   auto targetPointOptionsPtr = ut::make_unique<wsf::space::TargetPointOptions>(*trackIdPtr);
   aReturnVal.SetPointer(new UtScriptRef{targetPointOptionsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint,
                        wsf::space::TargetPointOptions,
                        ConstructFromLibrationPoint,
                        2,
                        "WsfTargetPoint",
                        "WsfLibrationPointSystem, WsfLibrationPointSelection")
{
   auto systemPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::LibrationPoint::System>();
   auto pointPtr  = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::LibrationPoint::Point>();
   if (!wsf::space::LibrationPoint::ValidLibrationPoint(*systemPtr, *pointPtr))
   {
      UT_SCRIPT_ABORT("Inconsistent libration point inputs to WsfTargetPointOptions.ConstructFromLibrationPoint");
   }
   auto targetPointOptionsPtr = ut::make_unique<wsf::space::TargetPointOptions>(*systemPtr, *pointPtr);
   aReturnVal.SetPointer(new UtScriptRef{targetPointOptionsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint,
                        wsf::space::TargetPointOptions,
                        ConstructFromKinematicStatePosition,
                        1,
                        "WsfTargetPoint",
                        "Vec3")
{
   auto positionPtr           = aVarArgs[0].GetPointer()->GetAppObject<UtVec3d>();
   auto targetPointOptionsPtr = ut::make_unique<wsf::space::TargetPointOptions>(*positionPtr, UtVec3d{});
   aReturnVal.SetPointer(new UtScriptRef{targetPointOptionsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint,
                        wsf::space::TargetPointOptions,
                        ConstructFromKinematicState,
                        2,
                        "WsfTargetPoint",
                        "Vec3, Vec3")
{
   auto positionPtr           = aVarArgs[0].GetPointer()->GetAppObject<UtVec3d>();
   auto velocityPtr           = aVarArgs[1].GetPointer()->GetAppObject<UtVec3d>();
   auto targetPointOptionsPtr = ut::make_unique<wsf::space::TargetPointOptions>(*positionPtr, *velocityPtr);
   aReturnVal.SetPointer(new UtScriptRef{targetPointOptionsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, HasTrackTarget, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasTrackTarget());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, HasLibrationPointTarget, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasLibrationPointTarget());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, HasKinematicStateTarget, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasKinematicStateTarget());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, HasOffset, 0, "bool", "") // DEPRECATED
{
   auto msg = ut::log::warning() << "Script method is deprecated.";
   msg.AddNote() << "Deprecated method: WsfTargetPoint.HasOffset";
   msg.AddNote() << "Please see: WsfTargetPoint.HasPositionOffset";
   aReturnVal.SetBool(aObjectPtr->HasPositionOffset());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, HasPositionOffset, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasPositionOffset());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, HasVelocityOffset, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasVelocityOffset());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, HasOffsetTime, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasOffsetTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, HasLagTime, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasLagTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, OffsetFrame, 0, "WsfTargetOffsetFrame", "") // DEPRECATED
{
   auto msg = ut::log::warning() << "Script method is deprecated.";
   msg.AddNote() << "Deprecated method: WsfTargetPoint.OffsetFrame";
   msg.AddNote() << "Please see: WsfTargetPoint.PositionOffsetFrame";
   auto objectPtr = ut::make_unique<wsf::space::OrbitalReferenceFrame>(aObjectPtr->GetPositionOffsetFrame());
   aReturnVal.SetPointer(new UtScriptRef{objectPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, Offset, 0, "Vec3", "") // DEPRECATED
{
   auto msg = ut::log::warning() << "Script method is deprecated.";
   msg.AddNote() << "Deprecated method: WsfTargetPoint.Offset";
   msg.AddNote() << "Please see: WsfTargetPoint.PositionOffset";
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetPositionOffset()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, PositionOffsetFrame, 0, "WsfOrbitalReferenceFrame", "")
{
   auto objectPtr = ut::make_unique<wsf::space::OrbitalReferenceFrame>(aObjectPtr->GetPositionOffsetFrame());
   aReturnVal.SetPointer(new UtScriptRef{objectPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, PositionOffset, 0, "Vec3", "")
{
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetPositionOffset()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, VelocityOffsetFrame, 0, "WsfOrbitalReferenceFrame", "")
{
   auto objectPtr = ut::make_unique<wsf::space::OrbitalReferenceFrame>(aObjectPtr->GetVelocityOffsetFrame());
   aReturnVal.SetPointer(new UtScriptRef{objectPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, VelocityOffset, 0, "Vec3", "")
{
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetVelocityOffset()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, OffsetTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOffsetTime().GetAsUnit(UtUnitTime::cSECONDS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, LagTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLagTime().GetAsUnit(UtUnitTime::cSECONDS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, LibrationPointSystem, 0, "WsfLibrationPointSystem", "")
{
   auto objectPtr = ut::make_unique<wsf::space::LibrationPoint::System>(aObjectPtr->GetLP_System());
   aReturnVal.SetPointer(new UtScriptRef{objectPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint,
                        wsf::space::TargetPointOptions,
                        LibrationPointSelection,
                        0,
                        "WsfLibrationPointSelection",
                        "")
{
   auto objectPtr = ut::make_unique<wsf::space::LibrationPoint::Point>(aObjectPtr->GetLP_Point());
   aReturnVal.SetPointer(new UtScriptRef{objectPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, TrackId, 0, "WsfTrackId", "")
{
   auto objectPtr = ut::make_unique<WsfTrackId>(aObjectPtr->GetLocalTrackId());
   aReturnVal.SetPointer(new UtScriptRef{objectPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, KinematicStateTargetPosition, 0, "Vec3", "")
{
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetKinematicStatePosition()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, KinematicStateTargetVelocity, 0, "Vec3", "")
{
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetKinematicStateVelocity()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, ClearOffset, 0, "void", "") // DEPRECATED
{
   auto msg = ut::log::warning() << "Script method is deprecated.";
   msg.AddNote() << "Deprecated method: WsfTargetPoint.ClearOffset";
   msg.AddNote() << "Please see: WsfTargetPoint.ClearPositionOffset";
   aObjectPtr->ClearPositionOffset();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, ClearPositionOffset, 0, "void", "")
{
   aObjectPtr->ClearPositionOffset();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, SetOffset, 2, "void", "Vec3, WsfTargetOffsetFrame") // DEPRECATED
{
   auto msg = ut::log::warning() << "Script method is deprecated.";
   msg.AddNote() << "Deprecated method: WsfTargetPoint.SetOffset";
   msg.AddNote() << "Please see: WsfTargetPoint.SetPositionOffset";
   auto offsetVec   = aVarArgs[0].GetPointer()->GetAppObject<UtVec3d>();
   auto offsetFrame = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::OrbitalReferenceFrame>();
   aObjectPtr->SetPositionOffset(*offsetVec, "m", *offsetFrame);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, SetPositionOffset, 2, "void", "Vec3, WsfOrbitalReferenceFrame")
{
   auto offsetVec   = aVarArgs[0].GetPointer()->GetAppObject<UtVec3d>();
   auto offsetFrame = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::OrbitalReferenceFrame>();
   aObjectPtr->SetPositionOffset(*offsetVec, "m", *offsetFrame);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, ClearVelocityOffset, 0, "void", "")
{
   aObjectPtr->ClearVelocityOffset();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, SetVelocityOffset, 2, "void", "Vec3, WsfOrbitalReferenceFrame")
{
   auto offsetVec   = aVarArgs[0].GetPointer()->GetAppObject<UtVec3d>();
   auto offsetFrame = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::OrbitalReferenceFrame>();
   aObjectPtr->SetVelocityOffset(*offsetVec, "m/s", *offsetFrame);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, ClearOffsetTime, 0, "void", "")
{
   aObjectPtr->ClearOffsetTime();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, SetOffsetTime, 1, "void", "double")
{
   aObjectPtr->SetOffsetTime(UtTimeValue{aVarArgs[0].GetDouble(), UtUnitTime::cSECONDS});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, ClearLagTime, 0, "void", "")
{
   aObjectPtr->ClearLagTime();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetPoint, wsf::space::TargetPointOptions, SetLagTime, 1, "void", "double")
{
   aObjectPtr->SetLagTime(UtTimeValue{aVarArgs[0].GetDouble(), UtUnitTime::cSECONDS});
}

WsfScriptTargetOffsetFrame::WsfScriptTargetOffsetFrame(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTargetOffsetFrame");
   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<RIC>());
   AddStaticMethod(ut::make_unique<NTW>());
}

void WsfScriptTargetOffsetFrame::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::space::OrbitalReferenceFrame*>(aObjectPtr);
}

bool WsfScriptTargetOffsetFrame::EqualTo(void* aLHS, void* aRHS)
{
   auto lhsPtr = static_cast<wsf::space::OrbitalReferenceFrame*>(aLHS);
   auto rhsPtr = static_cast<wsf::space::OrbitalReferenceFrame*>(aRHS);
   return *lhsPtr == *rhsPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetOffsetFrame, wsf::space::OrbitalReferenceFrame, RIC, 0, "WsfTargetOffsetFrame", "")
{
   auto objectPtr = ut::make_unique<wsf::space::OrbitalReferenceFrame>(wsf::space::OrbitalReferenceFrame::cRIC);
   aReturnVal.SetPointer(new UtScriptRef{objectPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTargetOffsetFrame, wsf::space::OrbitalReferenceFrame, NTW, 0, "WsfTargetOffsetFrame", "")
{
   auto objectPtr = ut::make_unique<wsf::space::OrbitalReferenceFrame>(wsf::space::OrbitalReferenceFrame::cNTW);
   aReturnVal.SetPointer(new UtScriptRef{objectPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}
