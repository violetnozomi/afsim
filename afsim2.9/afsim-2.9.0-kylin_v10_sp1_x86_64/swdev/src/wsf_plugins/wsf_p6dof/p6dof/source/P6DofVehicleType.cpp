// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "P6DofVehicleType.hpp"

#include "P6DofAeroCoreObject.hpp"
#include "P6DofAeroMovableObject.hpp"
#include "P6DofFlightControlSystem.hpp"
#include "P6DofJetEngineObject.hpp"
#include "P6DofLandingGear.hpp"
#include "P6DofPilotManager.hpp"
#include "P6DofPilotObject.hpp"
#include "P6DofPropulsionSystem.hpp"
#include "P6DofRamjetEngineObject.hpp"
#include "P6DofRocketLiquidPropellantObject.hpp"
#include "P6DofRocketSolidPropellantObject.hpp"
#include "P6DofScenario.hpp"
#include "P6DofSequencer.hpp"
#include "P6DofTypeManager.hpp"
#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringUtil.hpp"
#include "UtTable.hpp"

P6DofVehicleType::P6DofVehicleType(P6DofScenario* aScenario)
   : P6DofObject(aScenario)
   , mMassProperties()
   , mKinematicState()
   , mIntegratorTypeName(aScenario->GetTypeManager()->GetDefaultIntegratorName())
   , mAeroCoreObjPtr(nullptr)
   , mPropulsionObjPtr(nullptr)
   , mPilotManagerPtr(nullptr)
   , mFlightControlsPtr(nullptr)
   , mLandingGearPtr(nullptr)
   , mPosRelToParent_m(0., 0., 0.)
   , mAngRelToParentYPR_rad(0., 0., 0.)
   , mSeparationVel_fps(0., 0., 0.)
   , mSeparationOmega_rps(0., 0., 0.)
   , mGraphicalSupport()
   , mSizeFactor()
   , mFixedObject(false)
   , mMaxMach(100.0)
   , mMaxAlpha_deg(180.0)
   , mMinAlpha_deg(-180.0)
   , mMaxBeta_deg(90.0)
{
   mPropulsionObjPtr = new P6DofPropulsionSystem(aScenario);
}

P6DofVehicleType::~P6DofVehicleType()
{
   // TODO -- Unregister can not happen in the destructor here, it must occur prior to the destructor.
   // The type manager might be iterating over all P6DofObjectTypes and deleting them,
   // Unregistering an object type in middle of that loop corrupts the iterator.

   // Unregister the object
   // mObjectTypeManagerPtr->UnregisterObjectType(mName);

   if (mAeroCoreObjPtr)
   {
      delete mAeroCoreObjPtr;
   }

   auto aeroPartIter = mAeroPartList.begin();
   while (aeroPartIter != mAeroPartList.end())
   {
      P6DofAeroMovableObject* tempObj = *aeroPartIter;
      ++aeroPartIter;
      delete tempObj;
   }
   mAeroPartList.clear();

   if (mPropulsionObjPtr != nullptr)
   {
      delete mPropulsionObjPtr;
   }
   if (mPilotManagerPtr != nullptr)
   {
      delete mPilotManagerPtr;
   }
   if (mFlightControlsPtr != nullptr)
   {
      delete mFlightControlsPtr;
   }
   if (mLandingGearPtr != nullptr)
   {
      delete mLandingGearPtr;
   }

   auto seqIter = mSequencerList.begin();
   while (seqIter != mSequencerList.end())
   {
      P6DofSequencer* tempObj = *seqIter;
      ++seqIter;
      delete tempObj;
   }
   mSequencerList.clear();

   auto subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      P6DofVehicleType* tempObj = *subObjIter;
      ++subObjIter;
      delete tempObj;
   }
   mSubObjectList.clear();
}

P6DofVehicleType::P6DofVehicleType(const P6DofVehicleType& aSrc)
   : P6DofObject(aSrc)
   , mSubobjectBaseName(aSrc.mSubobjectBaseName)
   , mDerivedFromType(aSrc.mDerivedFromType)
   , mMassProperties(aSrc.mMassProperties)
   , mKinematicState(aSrc.mKinematicState)
   , mIntegratorTypeName(aSrc.mIntegratorTypeName)
   , mAeroCoreObjPtr(nullptr)
   , mPropulsionObjPtr(nullptr)
   , mPilotManagerPtr(nullptr)
   , mFlightControlsPtr(nullptr)
   , mLandingGearPtr(nullptr)
   , mPosRelToParent_m(aSrc.mPosRelToParent_m)
   , mAngRelToParentYPR_rad(aSrc.mAngRelToParentYPR_rad)
   , mSeparationVel_fps(aSrc.mSeparationVel_fps)
   , mSeparationOmega_rps(aSrc.mSeparationOmega_rps)
   , mGraphicalSupport(aSrc.mGraphicalSupport)
   , mSizeFactor(aSrc.mSizeFactor)
   , mFixedObject(aSrc.mFixedObject)
   , mMaxMach(aSrc.mMaxMach)
   , mMaxAlpha_deg(aSrc.mMaxAlpha_deg)
   , mMinAlpha_deg(aSrc.mMinAlpha_deg)
   , mMaxBeta_deg(aSrc.mMaxBeta_deg)
{
   if (aSrc.mAeroCoreObjPtr)
   {
      mAeroCoreObjPtr = aSrc.mAeroCoreObjPtr->Clone();
   }

   if (!aSrc.mAeroPartList.empty())
   {
      auto aeroPartIter = aSrc.mAeroPartList.begin();
      while (aeroPartIter != aSrc.mAeroPartList.end())
      {
         P6DofAeroMovableObject* tempAeroPart = (*aeroPartIter)->Clone();
         mAeroPartList.push_back(tempAeroPart);
         ++aeroPartIter;
      }
   }

   if (aSrc.mPropulsionObjPtr)
   {
      mPropulsionObjPtr = aSrc.mPropulsionObjPtr->Clone();
   }

   if (aSrc.mPilotManagerPtr)
   {
      mPilotManagerPtr = aSrc.mPilotManagerPtr->Clone();
   }

   if (aSrc.mFlightControlsPtr)
   {
      mFlightControlsPtr = aSrc.mFlightControlsPtr->Clone();
   }

   if (aSrc.mLandingGearPtr)
   {
      mLandingGearPtr = aSrc.mLandingGearPtr->Clone();
   }

   auto sequencerIter = aSrc.mSequencerList.begin();
   while (sequencerIter != aSrc.mSequencerList.end())
   {
      P6DofSequencer* sequencer = (*sequencerIter)->Clone();
      mSequencerList.push_back(sequencer);
      ++sequencerIter;
   }

   auto objIter = aSrc.mSubObjectList.begin();
   while (objIter != aSrc.mSubObjectList.end())
   {
      P6DofVehicleType* obj = (*objIter)->Clone();
      mSubObjectList.push_back(obj);
      ++objIter;
   }
}

P6DofVehicleType* P6DofVehicleType::Clone() const
{
   return new P6DofVehicleType(*this);
}

void P6DofVehicleType::DeriveFrom(const P6DofVehicleType& aSrc)
{
   mMassProperties     = aSrc.mMassProperties;
   mKinematicState     = aSrc.mKinematicState;
   mIntegratorTypeName = aSrc.mIntegratorTypeName;

   if (aSrc.mAeroCoreObjPtr)
   {
      delete mAeroCoreObjPtr;

      mAeroCoreObjPtr = aSrc.mAeroCoreObjPtr->Clone();
   }

   if (!aSrc.mAeroPartList.empty())
   {
      auto aeroPartIter = aSrc.mAeroPartList.begin();
      while (aeroPartIter != aSrc.mAeroPartList.end())
      {
         P6DofAeroMovableObject* tempAeroPart = (*aeroPartIter)->Clone();
         mAeroPartList.push_back(tempAeroPart);
         ++aeroPartIter;
      }
   }

   if (aSrc.mPropulsionObjPtr)
   {
      delete mPropulsionObjPtr;
      mPropulsionObjPtr = aSrc.mPropulsionObjPtr->Clone();
   }

   if (aSrc.mPilotManagerPtr)
   {
      delete mPilotManagerPtr;
      mPilotManagerPtr = aSrc.mPilotManagerPtr->Clone();
   }

   if (aSrc.mFlightControlsPtr)
   {
      delete mFlightControlsPtr;
      mFlightControlsPtr = aSrc.mFlightControlsPtr->Clone();
   }

   if (aSrc.mLandingGearPtr)
   {
      delete mLandingGearPtr;
      mLandingGearPtr = aSrc.mLandingGearPtr->Clone();
   }

   auto sequencerIter = aSrc.mSequencerList.begin();
   while (sequencerIter != aSrc.mSequencerList.end())
   {
      P6DofSequencer* sequencer = (*sequencerIter)->Clone();
      mSequencerList.push_back(sequencer);
      ++sequencerIter;
   }

   auto objIter = aSrc.mSubObjectList.begin();
   while (objIter != aSrc.mSubObjectList.end())
   {
      P6DofVehicleType* obj = (*objIter)->Clone();
      mSubObjectList.push_back(obj);
      ++objIter;
   }

   mPosRelToParent_m      = aSrc.mPosRelToParent_m;
   mAngRelToParentYPR_rad = aSrc.mAngRelToParentYPR_rad;
   mSeparationVel_fps     = aSrc.mSeparationVel_fps;
   mSeparationOmega_rps   = aSrc.mSeparationOmega_rps;
   mGraphicalSupport      = aSrc.mGraphicalSupport;
   mSizeFactor            = aSrc.mSizeFactor;
   mFixedObject           = aSrc.mFixedObject;
   mMaxMach               = aSrc.mMaxMach;
   mMaxAlpha_deg          = aSrc.mMaxAlpha_deg;
   mMinAlpha_deg          = aSrc.mMinAlpha_deg;
   mMaxBeta_deg           = aSrc.mMaxBeta_deg;
}

P6DofPilotObject* P6DofVehicleType::PilotObject() const
{
   if (mPilotManagerPtr == nullptr)
   {
      return nullptr;
   }
   return mPilotManagerPtr->GetActivePilot();
}

bool P6DofVehicleType::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "p6dof_object_type")
   {
      std::string typeName;
      std::string baseType;
      aInput.ReadValue(typeName);
      aInput.ReadValue(baseType);
      SetName(typeName);

      bool derivedFromBase = true;
      if (baseType != "P6DOF_BASE_TYPE")
      {
         derivedFromBase = false;
         // This is not derived from the base type
         // Determine if this base type exists
         if (!GetScenario()->GetTypeManager()->ObjectTypeExists(baseType))
         {
            // Unknown type -- error since we can't derive from an unknown type
            auto out =
               ut::log::error()
               << "P6DofVehicleType is trying to derive from an unknown type in P6DofVehicleType::ProcessInput.";
            out.AddNote() << "Base Type: " << baseType;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // We have a known type from which to derive
      // Determine if the name is unique
      if (!GetScenario()->GetTypeManager()->ObjectTypeNameIsUnique(typeName))
      {
         // Non-unique type name -- error since we can't have duplicate names
         auto out = ut::log::error() << "P6DofVehicleType is using a duplicate name in P6DofVehicleType::ProcessInput.";
         out.AddNote() << "Name: " << typeName;
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }

      // Set data
      mDerivedFromType = baseType;

      // If derived, clone the data
      if (!derivedFromBase)
      {
         const P6DofVehicleType* derivedFromObj = GetScenario()->GetTypeManager()->GetObjectTypeByName(baseType);
         if (derivedFromObj)
         {
            DeriveFrom(*derivedFromObj);
         }
         else
         {
            auto out =
               ut::log::error()
               << "P6DofVehicleType is trying to derive from an unknown type in P6DofVehicleType::ProcessInput.";
            out.AddNote() << "Base Type: " << baseType;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Previously, we registered the object with the manager here, but now handle this
      // outside of P6DofVehicleType::ProcessInput
      // mObjectTypeManagerPtr->RegisterObjectType(this);

      UtInputBlock block(aInput, "end_p6dof_object_type");
      while (block.ReadCommand())
      {
         if (command == "object_graphical_type_num")
         {
            int typeNum = 0;
            aInput.ReadValue(typeNum);
            mGraphicalSupport.SetObjectGraphicalType(typeNum);
         }
         else if (command == "object_graphical_alt_type1_num")
         {
            int typeNum = 0;
            aInput.ReadValue(typeNum);
            mGraphicalSupport.SetObjectGraphicalAltType1(typeNum);
         }
         else if (command == "object_graphical_alt_type2_num")
         {
            int typeNum = 0;
            aInput.ReadValue(typeNum);
            mGraphicalSupport.SetObjectGraphicalAltType2(typeNum);
         }
         else if (command == "object_graphical_alt_type3_num")
         {
            int typeNum = 0;
            aInput.ReadValue(typeNum);
            mGraphicalSupport.SetObjectGraphicalAltType3(typeNum);
         }
         else if (command == "mass_properties")
         {
            mMassProperties.ProcessInput(aInput);
         }
         else if (command == "sequencer")
         {
            P6DofSequencer* tempSequencer = new P6DofSequencer(GetScenario());
            tempSequencer->ProcessInput(aInput);

            std::string sequencerName = tempSequencer->GetName();
            for (auto& iter : mSequencerList)
            {
               if (iter->GetName() == sequencerName)
               {
                  throw UtInput::BadValue(aInput, "Duplicate sequencer name (" + sequencerName + ")");
               }
            }

            mSequencerList.push_back(tempSequencer);
         }
         else if (command == "use_spherical_earth")
         {
            bool value;
            aInput.ReadValue(value);
            mKinematicState.SetUseSphericalEarth(value);
         }
         else if (command == "use_rotating_earth")
         {
            bool value;
            aInput.ReadValue(value);
            mKinematicState.SetUseRotatingEarth(value);
         }
         else if (command == "ignore_jettisoned_objects")
         {
            bool value;
            aInput.ReadValue(value);
            mKinematicState.SetIgnoreJettisonObjects(value);
         }
         else if (command == "aero_data")
         {
            if (mAeroCoreObjPtr == nullptr)
            {
               mAeroCoreObjPtr = new P6DofAeroCoreObject();
            }
            mAeroCoreObjPtr->ProcessInput(aInput);
         }
         else if (command == "aero_component")
         {
            P6DofAeroMovableObject* tempObj = new P6DofAeroMovableObject();
            tempObj->ProcessInput(aInput);

            // Add this object to the list
            mAeroPartList.push_back(tempObj);
         }
         else if (command == "flight_controls")
         {
            if (mFlightControlsPtr == nullptr)
            {
               mFlightControlsPtr = new P6DofFlightControlSystem(GetScenario());
            }
            mFlightControlsPtr->ProcessInput(aInput);
         }
         else if (command == "pilot_manager")
         {
            if (mPilotManagerPtr == nullptr)
            {
               mPilotManagerPtr = new P6DofPilotManager(GetScenario());
            }
            mPilotManagerPtr->ProcessInput(aInput);
         }
         else if (command == "landing_gear")
         {
            if (mLandingGearPtr == nullptr)
            {
               mLandingGearPtr = new P6DofLandingGear(GetScenario());
            }
            mLandingGearPtr->ProcessInput(aInput);
         }
         else if (command == "parent_rel_x")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cLENGTH);
            double x, y, z;
            mPosRelToParent_m.Get(x, y, z);
            mPosRelToParent_m.Set(num, y, z);
         }
         else if (command == "parent_rel_y")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cLENGTH);
            double x, y, z;
            mPosRelToParent_m.Get(x, y, z);
            mPosRelToParent_m.Set(x, num, z);
         }
         else if (command == "parent_rel_z")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cLENGTH);
            double x, y, z;
            mPosRelToParent_m.Get(x, y, z);
            mPosRelToParent_m.Set(x, y, num);
         }
         else if (command == "parent_rel_yaw")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGLE);
            double y, p, r;
            mAngRelToParentYPR_rad.Get(y, p, r);
            mAngRelToParentYPR_rad.Set(num, p, r);
         }
         else if (command == "parent_rel_pitch")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGLE);
            double y, p, r;
            mAngRelToParentYPR_rad.Get(y, p, r);
            mAngRelToParentYPR_rad.Set(y, num, r);
         }
         else if (command == "parent_rel_roll")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGLE);
            double y, p, r;
            mAngRelToParentYPR_rad.Get(y, p, r);
            mAngRelToParentYPR_rad.Set(y, p, num);
         }
         else if (command == "separation_vx")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cSPEED);
            num *= UtMath::cFT_PER_M;
            double x, y, z;
            mSeparationVel_fps.Get(x, y, z);
            mSeparationVel_fps.Set(num, y, z);
         }
         else if (command == "separation_vy")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cSPEED);
            num *= UtMath::cFT_PER_M;
            double x, y, z;
            mSeparationVel_fps.Get(x, y, z);
            mSeparationVel_fps.Set(x, num, z);
         }
         else if (command == "separation_vz")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cSPEED);
            num *= UtMath::cFT_PER_M;
            double x, y, z;
            mSeparationVel_fps.Get(x, y, z);
            mSeparationVel_fps.Set(x, y, num);
         }
         else if (command == "separation_omega_x")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
            double x, y, z;
            mSeparationOmega_rps.Get(x, y, z);
            mSeparationOmega_rps.Set(num, y, z);
         }
         else if (command == "separation_omega_y")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
            double x, y, z;
            mSeparationOmega_rps.Get(x, y, z);
            mSeparationOmega_rps.Set(x, num, z);
         }
         else if (command == "separation_omega_z")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
            double x, y, z;
            mSeparationOmega_rps.Get(x, y, z);
            mSeparationOmega_rps.Set(x, y, num);
         }
         else if (command == "size_factor_radius")
         {
            double num;
            aInput.ReadValueOfType(num, UtInput::cLENGTH);
            mSizeFactor.SetRefSizeFactorRadius_m(num);
         }
         else if (command == "size_factor_min")
         {
            double num;
            aInput.ReadValue(num);
            mSizeFactor.SetSizeFactorMin(num);
         }
         else if (command == "size_factor_max")
         {
            double num;
            aInput.ReadValue(num);
            mSizeFactor.SetSizeFactorMax(num);
         }
         else if (command == "size_factor_volume_rate_m3_per_sec")
         {
            double num;
            aInput.ReadValue(num);
            mSizeFactor.SetSizeFactorVolumeRate_m3_perSec(num);
         }
         else if (command == "size_factor_area_rate_m2_per_sec")
         {
            double num;
            aInput.ReadValue(num);
            mSizeFactor.SetSizeFactorAreaRate_m2_perSec(num);
         }
         else if (command == "size_factor_radius_rate_m_per_sec")
         {
            double num;
            aInput.ReadValue(num);
            mSizeFactor.SetSizeFactorRadiusRate_m_perSec(num);
         }
         else if (command == "fixed_object")
         {
            bool value;
            aInput.ReadValue(value);
            mFixedObject = value;
         }
         else if (command == "nominal_max_mach")
         {
            double value;
            aInput.ReadValue(value);
            mMaxMach = value;
         }
         else if (command == "nominal_max_alpha")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            mMaxAlpha_deg = value * UtMath::cDEG_PER_RAD;
         }
         else if (command == "nominal_min_alpha")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            mMinAlpha_deg = value * UtMath::cDEG_PER_RAD;
         }
         else if (command == "nominal_max_beta")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            mMaxBeta_deg = value * UtMath::cDEG_PER_RAD;
         }
         else if (command == "p6dof_integrator")
         {
            std::string value;
            aInput.ReadValue(value);
            mIntegratorTypeName = value;
         }
         else if (command == "subobject")
         {
            std::string subName;
            std::string subBaseType;
            aInput.ReadValue(subName);
            aInput.ReadValue(subBaseType);

            for (auto& iter : mSubObjectList)
            {
               if (iter->GetSubobjectBaseName() == subName)
               {
                  throw UtInput::BadValue(aInput, "Duplicate sub-object name (" + subName + ")");
               }
            }

            P6DofVehicleType* subobject = new P6DofVehicleType(GetScenario());
            subobject->SetName(subBaseType);
            subobject->mSubobjectBaseName = subName;
            subobject->mDerivedFromType   = subBaseType;

            const P6DofVehicleType* derivedFromObject = GetScenario()->GetTypeManager()->GetObjectTypeByName(subBaseType);
            if (derivedFromObject != nullptr)
            {
               subobject->DeriveFrom(*derivedFromObject);
            }
            else
            {
               auto out = ut::log::error() << "P6DofVehicleType is trying to create a subobject with an unknown type "
                                              "in P6DofVehicleType::ProcessInput.";
               out.AddNote() << "Subobject Type: " << subBaseType;
               out.AddNote() << "Location: " << aInput.GetLocation();
               throw UtInput::UnknownCommand(aInput);
            }

            mSubObjectList.push_back(subobject);

            UtInputBlock subBlock(aInput, "end_subobject");

            while (subBlock.ReadCommand())
            {
               if (command == "parent_rel_x")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cLENGTH);
                  double x, y, z;
                  subobject->mPosRelToParent_m.Get(x, y, z);
                  subobject->mPosRelToParent_m.Set(num, y, z);
               }
               else if (command == "parent_rel_y")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cLENGTH);
                  double x, y, z;
                  subobject->mPosRelToParent_m.Get(x, y, z);
                  subobject->mPosRelToParent_m.Set(x, num, z);
               }
               else if (command == "parent_rel_z")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cLENGTH);
                  double x, y, z;
                  subobject->mPosRelToParent_m.Get(x, y, z);
                  subobject->mPosRelToParent_m.Set(x, y, num);
               }
               else if (command == "parent_rel_yaw")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cANGLE);
                  double y, p, r;
                  subobject->mAngRelToParentYPR_rad.Get(y, p, r);
                  subobject->mAngRelToParentYPR_rad.Set(num, p, r);
               }
               else if (command == "parent_rel_pitch")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cANGLE);
                  double y, p, r;
                  subobject->mAngRelToParentYPR_rad.Get(y, p, r);
                  subobject->mAngRelToParentYPR_rad.Set(y, num, r);
               }
               else if (command == "parent_rel_roll")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cANGLE);
                  double y, p, r;
                  subobject->mAngRelToParentYPR_rad.Get(y, p, r);
                  subobject->mAngRelToParentYPR_rad.Set(y, p, num);
               }
               else if (command == "separation_vx")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cSPEED);
                  num *= UtMath::cFT_PER_M;
                  double x, y, z;
                  subobject->mSeparationVel_fps.Get(x, y, z);
                  subobject->mSeparationVel_fps.Set(num, y, z);
               }
               else if (command == "separation_vy")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cSPEED);
                  num *= UtMath::cFT_PER_M;
                  double x, y, z;
                  subobject->mSeparationVel_fps.Get(x, y, z);
                  subobject->mSeparationVel_fps.Set(x, num, z);
               }
               else if (command == "separation_vz")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cSPEED);
                  num *= UtMath::cFT_PER_M;
                  double x, y, z;
                  subobject->mSeparationVel_fps.Get(x, y, z);
                  subobject->mSeparationVel_fps.Set(x, y, num);
               }
               else if (command == "separation_omega_x")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
                  double x, y, z;
                  subobject->mSeparationOmega_rps.Get(x, y, z);
                  subobject->mSeparationOmega_rps.Set(num, y, z);
               }
               else if (command == "separation_omega_y")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
                  double x, y, z;
                  subobject->mSeparationOmega_rps.Get(x, y, z);
                  subobject->mSeparationOmega_rps.Set(x, num, z);
               }
               else if (command == "separation_omega_z")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cANGULAR_RATE);
                  double x, y, z;
                  subobject->mSeparationOmega_rps.Get(x, y, z);
                  subobject->mSeparationOmega_rps.Set(x, y, num);
               }
               else if (command == "size_factor_radius")
               {
                  double num;
                  aInput.ReadValueOfType(num, UtInput::cLENGTH);
                  subobject->mSizeFactor.SetRefSizeFactorRadius_m(num);
               }
               else if (command == "size_factor_min")
               {
                  double num;
                  aInput.ReadValue(num);
                  subobject->mSizeFactor.SetSizeFactorMin(num);
               }
               else if (command == "size_factor_max")
               {
                  double num;
                  aInput.ReadValue(num);
                  subobject->mSizeFactor.SetSizeFactorMax(num);
               }
               else if (command == "size_factor_volume_rate_m3_per_sec")
               {
                  double num;
                  aInput.ReadValue(num);
                  subobject->mSizeFactor.SetSizeFactorVolumeRate_m3_perSec(num);
               }
               else if (command == "size_factor_area_rate_m2_per_sec")
               {
                  double num;
                  aInput.ReadValue(num);
                  subobject->mSizeFactor.SetSizeFactorAreaRate_m2_perSec(num);
               }
               else if (command == "size_factor_radius_rate_m_per_sec")
               {
                  double num;
                  aInput.ReadValue(num);
                  subobject->mSizeFactor.SetSizeFactorRadiusRate_m_perSec(num);
               }
               else if (command == "fixed_object")
               {
                  bool value;
                  aInput.ReadValue(value);
                  subobject->mFixedObject = value;
               }
               else if (command == "nominal_max_mach")
               {
                  double value;
                  aInput.ReadValue(value);
                  subobject->mMaxMach = value;
               }
               else if (command == "nominal_max_alpha")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cANGLE);
                  subobject->mMaxAlpha_deg = value * UtMath::cDEG_PER_RAD;
               }
               else if (command == "nominal_min_alpha")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cANGLE);
                  subobject->mMinAlpha_deg = value * UtMath::cDEG_PER_RAD;
               }
               else if (command == "nominal_max_beta")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cANGLE);
                  subobject->mMaxBeta_deg = value * UtMath::cDEG_PER_RAD;
               }
               else if (command == "p6dof_integrator")
               {
                  std::string value;
                  aInput.ReadValue(value);
                  subobject->mIntegratorTypeName = value;
               }
               else
               {
                  auto out = ut::log::error()
                             << "Unrecognized command in subobject block within P6DofVehicleType::ProcessInput().";
                  out.AddNote() << "Command: " << command;
                  out.AddNote() << "Subobject: " << subName;
                  out.AddNote() << "Location: " << aInput.GetLocation();
                  throw UtInput::UnknownCommand(aInput);
               }
            }
         }
         else if (command == "propulsion_data")
         {
            mPropulsionObjPtr->ProcessInput(aInput);
         }
         else if (command == "fuel_transfer")
         {
            if (mPropulsionObjPtr == nullptr)
            {
               ut::log::error() << "P6DofVehicleType is trying setup a fuel transfer without a P6DofPropulsionSystem "
                                   "in P6DofVehicleType::ProcessInput.";
               throw UtInput::UnknownCommand(aInput);
            }

            std::string xferName;
            aInput.ReadValue(xferName);

            if (!mPropulsionObjPtr->FuelTransferNameUnique(xferName))
            {
               auto out = ut::log::error() << "P6DofVehicleType::ProcessInput() is trying to setup a fuel transfer "
                                              "which is not uniquely named in P6DofVehicleType::ProcessInput.";
               out.AddNote() << "Transfer: " << xferName;
               throw UtInput::UnknownCommand(aInput);
            }

            std::string sourceTankName;
            std::string targetTankName;

            UtInputBlock subBlock(aInput, "end_fuel_transfer");

            while (subBlock.ReadCommand())
            {
               std::string localCommand;
               localCommand = subBlock.GetCommand();

               if (localCommand == "source_tank")
               {
                  aInput.ReadValue(sourceTankName);
               }
               else if (localCommand == "target_tank")
               {
                  aInput.ReadValue(targetTankName);
               }
            }

            if (!sourceTankName.empty() && !targetTankName.empty())
            {
               mPropulsionObjPtr->AddFuelTransfer(xferName, sourceTankName, targetTankName);
            }
            else
            {
               if (sourceTankName.empty())
               {
                  auto out = ut::log::error()
                             << "Unable to find source fuel tank for transfer in P6DofVehicleType::ProcessInput().";
                  out.AddNote() << "Fuel Tank: " << sourceTankName;
                  out.AddNote() << "Transfer: " << xferName;
               }
               if (targetTankName.empty())
               {
                  auto out = ut::log::error()
                             << "Unable to find target fuel tank for transfer in P6DofVehicleType::ProcessInput().";
                  out.AddNote() << "Fuel Tank: " << targetTankName;
                  out.AddNote() << "Transfer: " << xferName;
               }
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (command == "remove_fuel_transfer")
         {
            if (mPropulsionObjPtr == nullptr)
            {
               ut::log::error() << "P6DofVehicleType is trying remove a fuel transfer without a P6DofPropulsionSystem "
                                   "in P6DofVehicleType::ProcessInput.";
               throw UtInput::UnknownCommand(aInput);
            }

            std::string xferName;
            aInput.ReadValue(xferName);

            if (!mPropulsionObjPtr->RemoveFuelTransfer(xferName))
            {
               auto out = ut::log::error() << "Cannot remove a fuel transfer in P6DofVehicleType::ProcessInput().";
               out.AddNote() << "Transfer: " << xferName;
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (command == "remove_fuel_tank")
         {
            if (mPropulsionObjPtr == nullptr)
            {
               ut::log::error() << "P6DofVehicleType is trying remove a fuel transfer without a P6DofPropulsionSystem "
                                   "in P6DofVehicleType::ProcessInput.";
               throw UtInput::UnknownCommand(aInput);
            }

            std::string tankName;
            aInput.ReadValue(tankName);

            if (!mPropulsionObjPtr->RemoveFuelTankByName(tankName))
            {
               auto out = ut::log::error() << "Cannot remove a fuel transfer in P6DofVehicleType::ProcessInput().";
               out.AddNote() << "Fuel Tank: " << tankName;
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (command == "modify_fuel_quantity")
         {
            if (mPropulsionObjPtr == nullptr)
            {
               ut::log::error() << "P6DofVehicleType is trying modify a fuel quantity without a P6DofPropulsionSystem "
                                   "in P6DofVehicleType::ProcessInput.";
               throw UtInput::UnknownCommand(aInput);
            }

            std::string tankName;
            aInput.ReadValue(tankName);

            UtInputBlock subBlock(aInput, "end_modify_fuel_quantity");

            while (subBlock.ReadCommand())
            {
               std::string localCommand;
               localCommand = subBlock.GetCommand();

               if (localCommand == "fuel_quantity")
               {
                  double fuelQty_lbs = 0.0;
                  aInput.ReadValueOfType(fuelQty_lbs, UtInput::cMASS);
                  fuelQty_lbs *= UtMath::cLB_PER_KG;
                  mPropulsionObjPtr->ModifyFuelQuantity(tankName, fuelQty_lbs);
               }
            }
         }
         else if (command == "remove_sequencer")
         {
            std::string seqname;
            aInput.ReadValue(seqname);

            if (!RemoveSequencer(seqname))
            {
               auto out = ut::log::error() << "Cannot remove sequencer in P6DofVehicleType::ProcessInput().";
               out.AddNote() << "Sequencer: " << seqname;
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofVehicleType::ProcessInput().";
            out.AddNote() << "Command: " << command;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

std::string P6DofVehicleType::GetSubobjectBaseName() const
{
   return mSubobjectBaseName;
}

std::string P6DofVehicleType::GetDerivedFromName() const
{
   return mDerivedFromType;
}

bool P6DofVehicleType::RemoveSequencer(const std::string& aName)
{
   auto iter = mSequencerList.begin();
   while (iter != mSequencerList.end())
   {
      if ((*iter)->GetName() == aName)
      {
         P6DofSequencer* sequencer = (*iter);
         mSequencerList.erase(iter);
         delete sequencer;

         return true;
      }
   }

   return false;
}
