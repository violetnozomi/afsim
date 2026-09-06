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
#include "WsfScriptChaffWeaponClass.hpp"

#include "UtMemory.hpp"
#include "WsfChaffWeapon.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptChaffWeaponClass::WsfScriptChaffWeaponClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptWeaponClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfChaffWeapon");
   AddMethod(ut::make_unique<DropChaffCloud>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptChaffWeaponClass, WsfChaffWeapon, DropChaffCloud, 3, "bool", "int, double, Array<string>")
{
   auto ejectorListPtr = (std::vector<UtScriptData>*)aVarArgs[2].GetPointer()->GetAppObject();

   WsfChaffWeapon::ChaffOptions::Ejectors ejectors(ejectorListPtr->size());
   std::transform(ejectorListPtr->begin(),
                  ejectorListPtr->end(),
                  ejectors.begin(),
                  [](const UtScriptData& data) { return data.GetString(); });

   WsfChaffWeapon::ChaffOptions options(aVarArgs[0].GetInt(), aVarArgs[1].GetDouble(), ejectors);
   aReturnVal.SetBool(aObjectPtr->DropChaffCloud(TIME_NOW, options));
}
