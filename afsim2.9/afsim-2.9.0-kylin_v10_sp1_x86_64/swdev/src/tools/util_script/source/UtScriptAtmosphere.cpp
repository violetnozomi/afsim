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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtScriptAtmosphere.hpp"

#include "UtMemory.hpp"
#include "UtScriptMethodDefine.hpp"
namespace
{
UtScriptAtmosphere* sScriptAtmosphereClassPtr = nullptr;
}

UtScriptAtmosphere::UtScriptAtmosphere(UtScriptTypes* aScriptTypesPtr, const UtAtmosphere& aAtmosphere)
   : UtScriptClass("Atmosphere", aScriptTypesPtr)
   , mAtmosphere(aAtmosphere)
{
   UT_SCRIPT_WRAP_CLASS(UtAtmosphere)
   sScriptAtmosphereClassPtr = this;
   mConstructible            = true;
   mCloneable                = true;

   // Methods
   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtAtmosphere>, Construct, (const std::string& aType))
   {
      UtAtmosphere::AtmosphereType type = UtAtmosphere::cDEFAULT;
      UtAtmosphere::StringToAtmosphere(aType, type);
      return ut::make_unique<UtAtmosphere>(sScriptAtmosphereClassPtr->mAtmosphere, type);
   };

   UT_SCRIPT_WRAP_MEMBER("Density", Density);
   UT_SCRIPT_WRAP_MEMBER("Pressure", Pressure);
   UT_SCRIPT_WRAP_MEMBER("SonicVelocity", SonicVelocity);
   UT_SCRIPT_WRAP_MEMBER("Temperature", Temperature);
   UT_SCRIPT_WRAP_MEMBER("CanProduceContrails", IsWithinContrailAltitudeBand);
   UT_SCRIPT_WRAP_MEMBER("ContrailingFloor", GetContrailFloor);
   UT_SCRIPT_WRAP_MEMBER("ContrailingCeiling", GetContrailCeiling);
}

//! Creates a new script reference of a new UtAtmosphere object
UtScriptRef* UtScriptAtmosphere::Create(const UtAtmosphere& aAtmosphere)
{
   return new UtScriptRef(new UtAtmosphere(aAtmosphere, aAtmosphere.GetAtmosphereType()),
                          sScriptAtmosphereClassPtr,
                          UtScriptRef::cMANAGE);
}

void* UtScriptAtmosphere::Create(const UtScriptContext& aContext)
{
   return new UtAtmosphere(mAtmosphere);
}

void UtScriptAtmosphere::Destroy(void* aObject)
{
   delete static_cast<UtAtmosphere*>(aObject);
}

void* UtScriptAtmosphere::Clone(void* aObject)
{
   return new UtAtmosphere(*static_cast<UtAtmosphere*>(aObject));
}
