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

#ifndef WSFSCRIPTTARGETPOINT_HPP
#define WSFSCRIPTTARGETPOINT_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WsfScriptTargetPoint : public UtScriptClass
{
public:
   WsfScriptTargetPoint(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptTargetPoint() override = default;

   void Destroy(void* aObjectPtr) override;

   // static construction methods
   UT_DECLARE_SCRIPT_METHOD(ConstructFromTrack);
   UT_DECLARE_SCRIPT_METHOD(ConstructFromLibrationPoint);
   UT_DECLARE_SCRIPT_METHOD(ConstructFromKinematicStatePosition);
   UT_DECLARE_SCRIPT_METHOD(ConstructFromKinematicState);

   // Queries
   UT_DECLARE_SCRIPT_METHOD(HasTrackTarget);
   UT_DECLARE_SCRIPT_METHOD(HasLibrationPointTarget);
   UT_DECLARE_SCRIPT_METHOD(HasKinematicStateTarget);
   UT_DECLARE_SCRIPT_METHOD(HasOffset); // DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(HasPositionOffset);
   UT_DECLARE_SCRIPT_METHOD(HasVelocityOffset);
   UT_DECLARE_SCRIPT_METHOD(HasOffsetTime);
   UT_DECLARE_SCRIPT_METHOD(HasLagTime);

   UT_DECLARE_SCRIPT_METHOD(OffsetFrame); // DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(Offset);      // DEPRECATED

   UT_DECLARE_SCRIPT_METHOD(PositionOffsetFrame);
   UT_DECLARE_SCRIPT_METHOD(PositionOffset);

   UT_DECLARE_SCRIPT_METHOD(VelocityOffsetFrame);
   UT_DECLARE_SCRIPT_METHOD(VelocityOffset);

   UT_DECLARE_SCRIPT_METHOD(OffsetTime);
   UT_DECLARE_SCRIPT_METHOD(LagTime);

   UT_DECLARE_SCRIPT_METHOD(LibrationPointSystem);
   UT_DECLARE_SCRIPT_METHOD(LibrationPointSelection);
   UT_DECLARE_SCRIPT_METHOD(TrackId);

   UT_DECLARE_SCRIPT_METHOD(KinematicStateTargetPosition);
   UT_DECLARE_SCRIPT_METHOD(KinematicStateTargetVelocity);

   // Offset related
   UT_DECLARE_SCRIPT_METHOD(ClearOffset); // DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(SetOffset);   // DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(ClearPositionOffset);
   UT_DECLARE_SCRIPT_METHOD(SetPositionOffset);
   UT_DECLARE_SCRIPT_METHOD(ClearVelocityOffset);
   UT_DECLARE_SCRIPT_METHOD(SetVelocityOffset);
   UT_DECLARE_SCRIPT_METHOD(ClearOffsetTime);
   UT_DECLARE_SCRIPT_METHOD(SetOffsetTime);
   UT_DECLARE_SCRIPT_METHOD(ClearLagTime);
   UT_DECLARE_SCRIPT_METHOD(SetLagTime);
};

class WsfScriptTargetOffsetFrame : public UtScriptClass
{
public:
   WsfScriptTargetOffsetFrame(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptTargetOffsetFrame() override = default;

   void Destroy(void* aObjectPtr) override;
   bool EqualTo(void* aLHS, void* aRHS) override;

   // Static creation methods
   UT_DECLARE_SCRIPT_METHOD(RIC);
   UT_DECLARE_SCRIPT_METHOD(NTW);
};

#endif // WSFSCRIPTTARGETPOINT_HPP
