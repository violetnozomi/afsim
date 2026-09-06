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

#ifndef WSFXIO_SCRIPTCLASS_HPP
#define WSFXIO_SCRIPTCLASS_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

class UtCallback;
#include "UtScriptBasicTypes.hpp"
class WsfSimulation;
#include "WsfStringId.hpp"
class WsfXIO_Interface;
class WsfXIO_PublishFilter;
class WsfXIO_PublishKey;

class WSF_EXPORT WsfXIO_ScriptClass : public UtScriptClass
{
public:
   WsfXIO_ScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfXIO_ScriptClass() override = default;

   bool AddStaticMethodP(std::unique_ptr<InterfaceMethod> aInterfaceMethod)
   {
      return UtScriptClass::AddStaticMethod(std::move(aInterfaceMethod));
   }

   static WsfXIO_ScriptClass* GetInstance() { return mInstancePtr; }

   UT_DECLARE_SCRIPT_METHOD(Publish);
   UT_DECLARE_SCRIPT_METHOD(Unpublish);
   UT_DECLARE_SCRIPT_METHOD(Subscribe_1);
   UT_DECLARE_SCRIPT_METHOD(Subscribe_2);
   UT_DECLARE_SCRIPT_METHOD(Connection_1);
   UT_DECLARE_SCRIPT_METHOD(Connection_2);

   struct Callee
   {
      Callee(WsfXIO_Interface* aInterfacePtr, WsfSimulation& aSimulation);

      void HandleCallback(const WsfXIO_PublishKey& aKey);

      WsfXIO_Interface*           mInterfacePtr;
      WsfSimulation&              mSimulation;
      int                         mPlatformIndex;
      WsfStringId                 mProcessorName;
      WsfStringId                 mScriptName;
      std::unique_ptr<UtCallback> mCallbackPtr;
   };

   void SubscribeScript(const WsfXIO_PublishFilter& aFilter, std::unique_ptr<Callee> aCalleePtr);

protected:
   std::vector<std::unique_ptr<Callee>> mCalleeList;
   static WsfXIO_ScriptClass*           mInstancePtr;
};
#endif
