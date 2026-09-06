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

#ifndef WSFWEAPONTASK_HPP
#define WSFWEAPONTASK_HPP

#include "wsf_mil_export.h"

#include "UtScriptAccessible.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptTypes.hpp"
#include "WsfAttributeContainer.hpp"
class WsfPlatform;
class WsfProcessor;
#include "WsfTask.hpp"
#include "WsfTaskData.hpp"
#include "WsfWeaponTaskResource.hpp"

//! A class that defines a task to be performed.
class WSF_MIL_EXPORT WsfWeaponTask : public WsfTask
{
public:
   WsfWeaponTask() = default;
   WsfWeaponTask(const WsfTaskResource& aTaskResource)
      : WsfTask(aTaskResource)
   {
   }
   WsfWeaponTask(const WsfTask& aSrc)
      : WsfTask(aSrc)
   {
   }

   WsfWeaponTask& operator=(const WsfTask& aRhs)
   {
      WsfTaskData::operator=(aRhs);
      return *this;
   }

   ~WsfWeaponTask() override = default;

   const char* GetScriptClassName() const override { return "WsfWeaponTask"; };
   static void RegisterScriptTypes(UtScriptTypes* aScriptTypesPtr);
   static void RegisterScriptMethods(UtScriptTypes& aScriptTypes);

   // static UtScriptClass* CreateScriptClass(const std::string& aClassName,
   //                                         UtScriptTypes*     aScriptTypesPtr);

   // void Initialize(double        aSimTime,
   //                 unsigned long aTaskId,
   //                 WsfProcessor* aAssignerPtr);
};

class WsfWeaponTaskScriptExtensions : public UtScriptExtension
{
public:
   ~WsfWeaponTaskScriptExtensions() override = default;
   bool AddExtClassMethods(const std::string& aClassName, const std::string& aBaseName, UtScriptTypes* aScriptTypesPtr) override;
};

#endif
