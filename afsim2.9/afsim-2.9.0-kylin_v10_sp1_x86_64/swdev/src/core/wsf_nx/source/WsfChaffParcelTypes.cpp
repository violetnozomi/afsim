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
#include "WsfChaffParcelTypes.hpp"

#include "UtMemory.hpp"
#include "WsfScenario.hpp"

template<typename SCENARIO, typename RT>
RT& GetHelper(SCENARIO& aScenario)
{
   auto* typesPtr = dynamic_cast<WsfChaffParcelTypes*>(aScenario.GetTypes("chaff_parcel"));
   if (typesPtr)
   {
      return *typesPtr;
   }
   throw UtException("WsfChaffParcelTypes type list does not exist in scenario");
}

WsfChaffParcelTypes& WsfChaffParcelTypes::Get(WsfScenario& aScenario)
{
   return GetHelper<WsfScenario&, WsfChaffParcelTypes&>(aScenario);
}

const WsfChaffParcelTypes& WsfChaffParcelTypes::Get(const WsfScenario& aScenario)
{
   return GetHelper<const WsfScenario&, const WsfChaffParcelTypes&>(aScenario);
}

WsfChaffParcelTypes::WsfChaffParcelTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfChaffParcel>(aScenario, cREDEFINITION_ALLOWED, "chaff_parcel")
{
   AddCoreType("WSF_CHAFF_PARCEL", ut::make_unique<WsfChaffParcel>());
}

WsfChaffParcelTypes::~WsfChaffParcelTypes()
{
   WsfChaffParcel::ClearMaxRcsCache();
}

bool WsfChaffParcelTypes::InitializeType(WsfChaffParcel* aTypePtr)
{
   return aTypePtr->InitializeType();
}
