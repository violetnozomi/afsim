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

#include "WsfSixDOF_ThrustProducerObject.hpp"

#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "WsfSixDOF_JetEngine.hpp"
#include "WsfSixDOF_MassProperties.hpp"
#include "WsfSixDOF_RamjetEngine.hpp"
#include "WsfSixDOF_RocketLiquidPropellantEngine.hpp"
#include "WsfSixDOF_RocketSolidPropellantEngine.hpp"
#include "WsfSixDOF_TypeManager.hpp"

wsf::six_dof::ThrustProducerObject::ThrustProducerObject(const ThrustProducerObject& aSrc)
   : Object(aSrc)
   , mParentPropulsionSystemPtr(aSrc.mParentPropulsionSystemPtr)
   , mEnginePtr(aSrc.mEnginePtr)
   , mNominalThrustVectorDirection(aSrc.mNominalThrustVectorDirection)
   , mThrustInstalledAnglesYPR_rad(aSrc.mThrustInstalledAnglesYPR_rad)
   , mReferenceLocation_ft(aSrc.mReferenceLocation_ft)
   , mThrustLocation_ft(aSrc.mThrustLocation_ft)
   , mThrustOffset_ft(aSrc.mThrustOffset_ft)
   , mInOpRefArea_ft2(aSrc.mInOpRefArea_ft2)
   , mThrustVectoringEnabled(aSrc.mThrustVectoringEnabled)
{
   mEnginePtr->SetParentThrustProducer(this);
}

void wsf::six_dof::ThrustProducerObject::DeriveFrom(const ThrustProducerObject* aSrc)
{
   if (aSrc == nullptr)
   {
      return;
   }

   mEnginePtr = aSrc->mEnginePtr;
   mEnginePtr->SetParentThrustProducer(this);

   mNominalThrustVectorDirection = aSrc->mNominalThrustVectorDirection;
   mThrustInstalledAnglesYPR_rad = aSrc->mThrustInstalledAnglesYPR_rad;
   mReferenceLocation_ft         = aSrc->mReferenceLocation_ft;
   mThrustLocation_ft            = aSrc->mThrustLocation_ft;
   mThrustOffset_ft              = aSrc->mThrustOffset_ft;
   mInOpRefArea_ft2              = aSrc->mInOpRefArea_ft2;
   mThrustVectoringEnabled       = aSrc->mThrustVectoringEnabled;
}

bool wsf::six_dof::ThrustProducerObject::ProcessInputType(UtInput& aInput, TypeManager* aTypeManager)
{
   aInput.ReadCommand(mObjName);
   aInput.ReadCommand(mBaseName);

   // We have a known type from which to derive
   // Determine if the name is unique
   if (aTypeManager->ThrustProducerObjectTypeExists(mObjName))
   {
      // Non-unique type name -- error since we can't have duplicate names
      auto out = ut::log::error() << "ThrustProducerObject is using a duplicate name.";
      out.AddNote() << "Name: " << mObjName;
      out.AddNote() << "Location: " << aInput.GetLocation();
      throw UtInput::UnknownCommand(aInput);
   }

   if (mBaseName != "BASE_TYPE")
   {
      // This is not derived from the base type
      // Determine if this base type exists
      if (!aTypeManager->ThrustProducerObjectTypeExists(mBaseName))
      {
         // Unknown type -- error since we can't derive from an unknown type
         auto out = ut::log::error() << "ThrustProducerObject is trying to derive from an unknown type.";
         out.AddNote() << "Base Type: " << mBaseName;
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }

      DeriveFrom(aTypeManager->GetThrustProducerObjectType(mBaseName));
   }

   return true;
}

bool wsf::six_dof::ThrustProducerObject::ProcessInput(UtInput& aInput, TypeManager* aTypeManager)
{
   const std::string& command = aInput.GetCommand();

   if (command == "thrust_offset")
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      value *= UtMath::cFT_PER_M;
      mThrustOffset_ft = value;
   }
   else if (command == "inop_ref_area")
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cAREA);
      value *= (UtMath::cFT_PER_M * UtMath::cFT_PER_M);
      mInOpRefArea_ft2 = value;
   }
   else if (command == "jet")
   {
      auto engine = ut::make_unique<JetEngine>(this);
      engine->ProcessInput(aInput, aTypeManager);
      SetEngine(std::move(engine));
   }
   else if (command == "ramjet")
   {
      auto engine = ut::make_unique<RamjetEngine>(this);
      engine->ProcessInput(aInput, aTypeManager);
      SetEngine(std::move(engine));
   }
   else if (command == "liquid_propellant_rocket")
   {
      auto engine = ut::make_unique<RocketLiquidPropellantEngine>(this);
      engine->ProcessInput(aInput, aTypeManager);
      SetEngine(std::move(engine));
   }
   else if (command == "solid_propellant_rocket")
   {
      auto engine = ut::make_unique<RocketSolidPropellantEngine>(this);
      engine->ProcessInput(aInput, aTypeManager);
      SetEngine(std::move(engine));
   }
   else
   {
      return false;
   }

   return true;
}

double wsf::six_dof::ThrustProducerObject::GetMaximumPotentialThrust_lbs(double aAlt_ft,
                                                                         double aDynPress_lbsqft,
                                                                         double aStatPress_lbssqft,
                                                                         double aSpeed_fps,
                                                                         double aMach,
                                                                         double aAlpha_rad,
                                                                         double aBeta_rad)
{
   return mEnginePtr->GetMaximumPotentialThrust_lbs(aAlt_ft,
                                                    aDynPress_lbsqft,
                                                    aStatPress_lbssqft,
                                                    aSpeed_fps,
                                                    aMach,
                                                    aAlpha_rad,
                                                    aBeta_rad);
}

double wsf::six_dof::ThrustProducerObject::GetMinimumPotentialThrust_lbs(double aAlt_ft,
                                                                         double aDynPress_lbsqft,
                                                                         double aStatPress_lbssqft,
                                                                         double aSpeed_fps,
                                                                         double aMach,
                                                                         double aAlpha_rad,
                                                                         double aBeta_rad)
{
   return mEnginePtr->GetMinimumPotentialThrust_lbs(aAlt_ft,
                                                    aDynPress_lbsqft,
                                                    aStatPress_lbssqft,
                                                    aSpeed_fps,
                                                    aMach,
                                                    aAlpha_rad,
                                                    aBeta_rad);
}

double wsf::six_dof::ThrustProducerObject::GetInoperatingDragArea_ft2()
{
   return mInOpRefArea_ft2;
}

double wsf::six_dof::ThrustProducerObject::GetForwardThrust_lbs() const
{
   UtVec3dX thrustDirection;
   GetCurrentThrustVectorDirection(thrustDirection);
   return GetThrust_lbs() * thrustDirection.X();
}

bool wsf::six_dof::ThrustProducerObject::SetFuelTank(const std::string& aFuelTankName)
{
   if (mEnginePtr)
   {
      return mEnginePtr->SetFuelTank(aFuelTankName);
   }
   return false;
}
