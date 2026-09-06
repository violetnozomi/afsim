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

#include "xio_sim/WsfXIO_ScriptClass.hpp"

#include "PakSerializeImpl.hpp"
#include "UtMemory.hpp"
#include "UtScriptDataPack.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptProcessor.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Data.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_Publisher.hpp"
#include "xio_sim/WsfXIO_DisExtension.hpp"
#include "xio_sim/WsfXIO_Extension.hpp"
#include "xio_sim/WsfXIO_ScriptSerialize.hpp"

WsfXIO_ScriptClass* WsfXIO_ScriptClass::mInstancePtr(nullptr);

WsfXIO_ScriptClass::WsfXIO_ScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mInstancePtr = this;
   SetClassName("WsfXIO");

   AddStaticMethod(ut::make_unique<Publish>());
   AddStaticMethod(ut::make_unique<Unpublish>());
   AddStaticMethod(ut::make_unique<Subscribe_1>("Subscribe"));
   AddStaticMethod(ut::make_unique<Subscribe_2>("Subscribe"));
   AddStaticMethod(ut::make_unique<Connection_1>("Connection"));
   AddStaticMethod(ut::make_unique<Connection_2>("Connection"));
}

void WsfXIO_ScriptClass::SubscribeScript(const WsfXIO_PublishFilter&                 aFilter,
                                         std::unique_ptr<WsfXIO_ScriptClass::Callee> aCalleePtr)
{
   aCalleePtr->mCallbackPtr =
      aCalleePtr->mInterfacePtr->GetPublisher().Subscribe(aFilter, &Callee::HandleCallback, aCalleePtr.get());
   mCalleeList.push_back(std::move(aCalleePtr));
}

#include "script/WsfScriptDefs.hpp"

#define XIO_INTERFACE WsfXIO_Extension::Find(*SIMULATION)

UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptClass, void, Publish, 2, "void", "WsfXIO_PublishKey, Object")
{
   WsfXIO_Interface* interfacePtr = XIO_INTERFACE;
   if (interfacePtr == nullptr)
   {
      return;
   }
   WsfXIO_Publisher&   publisher = interfacePtr->GetPublisher();
   WsfXIO_PublishKey*  keyPtr    = (WsfXIO_PublishKey*)aVarArgs[0].GetPointer()->GetAppObject();
   const UtScriptData& arg       = aVarArgs[1];
   publisher.Publish(*keyPtr, arg);
}

UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptClass, void, Unpublish, 1, "void", "WsfXIO_PublishKey")
{
   WsfXIO_Interface* interfacePtr = XIO_INTERFACE;
   if (interfacePtr == nullptr)
   {
      return;
   }
   WsfXIO_Publisher&  publisher = interfacePtr->GetPublisher();
   WsfXIO_PublishKey* keyPtr    = (WsfXIO_PublishKey*)aVarArgs[0].GetPointer()->GetAppObject();
   publisher.Unpublish(*keyPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptClass, void, Subscribe_1, 3, "void", "WsfXIO_PublishKey, WsfProcessor, string")
{
   WsfXIO_Interface* interfacePtr = XIO_INTERFACE;
   if (interfacePtr == nullptr)
   {
      return;
   }

   WsfXIO_PublishKey*   keyPtr = (WsfXIO_PublishKey*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfXIO_PublishFilter filter(*keyPtr);
   WsfProcessor&        processor = *(WsfProcessor*)aVarArgs[1].GetPointer()->GetAppObject();
   auto                 callee    = ut::make_unique<WsfXIO_ScriptClass::Callee>(interfacePtr, *SIMULATION);
   callee->mPlatformIndex         = static_cast<int>(processor.GetPlatform()->GetIndex());
   callee->mProcessorName         = processor.GetName();
   callee->mScriptName            = aVarArgs[2].GetString();
   WsfXIO_ScriptClass::GetInstance()->SubscribeScript(filter, std::move(callee));
}

UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptClass, void, Subscribe_2, 3, "void", "WsfXIO_PublishKey, WsfPlatform, string")
{
   WsfXIO_Interface* interfacePtr = XIO_INTERFACE;
   if (interfacePtr == nullptr)
   {
      return;
   }
   WsfXIO_PublishKey* keyPtr;
   WsfPlatform*       platformPtr;
   std::string        scriptName;
   UtScriptDataUnpack(aVarArgs, keyPtr, platformPtr, scriptName);
   // WsfXIO_PublishKey* keyPtr = (WsfXIO_PublishKey*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfXIO_PublishFilter filter(*keyPtr);
   // WsfPlatform& platform = *(WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();

   auto callee            = ut::make_unique<WsfXIO_ScriptClass::Callee>(interfacePtr, *SIMULATION);
   callee->mPlatformIndex = static_cast<int>(platformPtr->GetIndex());
   callee->mProcessorName = nullptr;
   callee->mScriptName    = scriptName;
   WsfXIO_ScriptClass::GetInstance()->SubscribeScript(filter, std::move(callee));
}

UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptClass, void, Connection_1, 1, "WsfXIO_Connection", "string")
{
   WsfXIO_Interface* interfacePtr = XIO_INTERFACE;
   if (interfacePtr == nullptr)
   {
      return;
   }
   WsfXIO_Connection* connectionPtr = interfacePtr->FindConnection(aVarArgs[0].GetString());
   if (connectionPtr != nullptr)
   {
      aReturnVal.SetPointer(new UtScriptRef(connectionPtr, aReturnClassPtr, connectionPtr->GetReferenceCount()));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptClass, void, Connection_2, 1, "WsfXIO_Connection", "int")
{
   WsfXIO_Interface* interfacePtr = XIO_INTERFACE;
   if (interfacePtr == nullptr)
   {
      return;
   }
   WsfXIO_Connection* connectionPtr = interfacePtr->FindConnection(aVarArgs[0].GetInt());
   if (connectionPtr != nullptr)
   {
      aReturnVal.SetPointer(new UtScriptRef(connectionPtr, aReturnClassPtr, connectionPtr->GetReferenceCount()));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}

void WsfXIO_ScriptClass::Callee::HandleCallback(const WsfXIO_PublishKey& aKey)
{
   WsfPlatform* platformPtr = mSimulation.GetPlatformByIndex(mPlatformIndex);
   if (platformPtr != nullptr)
   {
      UtScriptDataList args;

      args.push_back(UtScriptData());
      args.push_back(UtScriptData());
      args[0].SetPointer(
         new UtScriptRef(new WsfXIO_PublishKey(aKey), WsfXIO_PublishKey::GetScriptClass(), UtScriptRef::cMANAGE));
      if (mInterfacePtr->GetPublisher().GetData(aKey, args.back()))
      {
         UtScriptData retval;
         double       simTime = mSimulation.GetSimTime();
         if (mProcessorName != 0)
         {
            WsfProcessor*       processorPtr       = platformPtr->GetComponent<WsfProcessor>(mProcessorName);
            WsfScriptProcessor* scriptProcessorPtr = dynamic_cast<WsfScriptProcessor*>(processorPtr);
            if (scriptProcessorPtr != nullptr)
            {
               scriptProcessorPtr->ExecuteScript(simTime, mScriptName.GetString(), retval, args);
            }
         }
         else
         {
            platformPtr->ExecuteScript(simTime, mScriptName.GetString(), retval, args);
         }
      }
   }
}

WsfXIO_ScriptClass::Callee::Callee(WsfXIO_Interface* aInterfacePtr, WsfSimulation& aSimulation)
   : mInterfacePtr(aInterfacePtr)
   , mSimulation(aSimulation)
{
}
