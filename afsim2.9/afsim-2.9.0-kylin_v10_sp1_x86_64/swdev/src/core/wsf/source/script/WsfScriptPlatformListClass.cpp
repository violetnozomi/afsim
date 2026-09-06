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

#include "script/WsfScriptPlatformListClass.hpp"

#include <sstream>

#include "UtMemory.hpp"
#include "WsfCommandChain.hpp"

WsfScriptPlatformListClass::WsfScriptPlatformListClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfPlatformList");

   // Set the key and data types of this container.
   SetContainerKeyTypeId(GetTypes()->GetTypeId("int"));
   SetContainerDataTypeId(GetTypes()->GetTypeId("WsfPlatform"));

   mContainer = true;

   AddMethod(ut::make_unique<Count>());
   AddMethod(ut::make_unique<Count>("Size")); // Support Array<>-type interface
   AddMethod(ut::make_unique<Empty>());       // Support Array<>-type interface
   AddMethod(ut::make_unique<Entry>());
   AddMethod(ut::make_unique<Entry>("Get")); // Support Array<>-type interface
   AddMethod(ut::make_unique<GetIterator>());
}

// virtual
void WsfScriptPlatformListClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfCommandChain::PlatformList*>(aObjectPtr);
}

// virtual
std::string WsfScriptPlatformListClass::ToString(void* aObjectPtr) const
{
   WsfCommandChain::PlatformList* platforms = static_cast<WsfCommandChain::PlatformList*>(aObjectPtr);
   std::stringstream              ss;
   UtScriptClass*                 platformClassPtr = GetTypes()->GetClass("WsfPlatform");
   if (platformClassPtr != nullptr)
   {
      ss << "{ ";
      for (size_t i = 0; i < platforms->size(); ++i)
      {
         if (i != 0)
         {
            ss << ", ";
         }
         ss << platformClassPtr->ToString((*platforms)[i]);
      }
      ss << " }";
   }
   else
   {
      ss << "WsfPlatformList(null)";
   }
   return ss.str();
}

int WsfScriptPlatformListClass::EnumerateSize(void* aObjectPtr)
{
   WsfCommandChain::PlatformList* listPtr = (WsfCommandChain::PlatformList*)aObjectPtr;
   return listPtr ? (int)listPtr->size() : 0;
}

UtScriptData WsfScriptPlatformListClass::Enumerate(void* aObjectPtr, int aIndex)
{
   WsfCommandChain::PlatformList* listPtr = (WsfCommandChain::PlatformList*)aObjectPtr;
   if (listPtr && (int)listPtr->size() > aIndex && aIndex >= 0)
   {
      return UtScriptData(new UtScriptRef((*listPtr)[aIndex], GetTypes()->GetClass("WsfPlatform")));
   }
   return UtScriptData();
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformListClass, WsfCommandChain::PlatformList, Count, 0, "int", "")
{
   int count = 0;
   if (aObjectPtr != nullptr)
   {
      count = static_cast<int>(aObjectPtr->size());
   }
   aReturnVal.SetInt(count);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformListClass, WsfCommandChain::PlatformList, Empty, 0, "bool", "")
{
   bool empty(true);
   if (aObjectPtr != nullptr)
   {
      empty = aObjectPtr->empty();
   }
   aReturnVal.SetBool(empty);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformListClass, WsfCommandChain::PlatformList, Entry, 1, "WsfPlatform", "int")
{
   // arg1: platform index.

   WsfPlatform* platformPtr = nullptr;
   if (aObjectPtr != nullptr)
   {
      int index = aVarArgs[0].GetInt();
      if ((index >= 0) && (index < static_cast<int>(aObjectPtr->size())))
      {
         platformPtr = (*aObjectPtr)[index];
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformListClass,
                                WsfCommandChain::PlatformList,
                                GetIterator,
                                0,
                                "WsfPlatformListIterator",
                                "")
{
   // Create the iterator.
   void* iterPtr = new WsfScriptPlatformListIteratorClass::Iterator(aObjectPtr);

   // Create a return object that manages the iterator memory we just created.
   aReturnVal.SetPointer(new UtScriptRef(iterPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

WsfScriptPlatformListIteratorClass::WsfScriptPlatformListIteratorClass(const std::string& aClassName,
                                                                       UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<HasNext>());
   AddMethod(ut::make_unique<Next>());
   AddMethod(ut::make_unique<HasPrev>());
   AddMethod(ut::make_unique<Prev>());
   AddMethod(ut::make_unique<Key>());
   AddMethod(ut::make_unique<Data>());
}

// virtual
WsfScriptPlatformListIteratorClass::~WsfScriptPlatformListIteratorClass() {}

// virtual
void WsfScriptPlatformListIteratorClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfScriptPlatformListIteratorClass::Iterator*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformListIteratorClass, WsfScriptPlatformListIteratorClass::Iterator, HasNext, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasNext());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformListIteratorClass, WsfScriptPlatformListIteratorClass::Iterator, Next, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->Next(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformListIteratorClass, WsfScriptPlatformListIteratorClass::Iterator, HasPrev, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasPrev());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformListIteratorClass, WsfScriptPlatformListIteratorClass::Iterator, Prev, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->Prev(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformListIteratorClass, WsfScriptPlatformListIteratorClass::Iterator, Key, 0, "int", "")
{
   aReturnVal = aObjectPtr->Key();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformListIteratorClass, WsfScriptPlatformListIteratorClass::Iterator, Data, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->Data(), aReturnClassPtr));
}

WsfScriptPlatformListIteratorClass::Iterator::Iterator(const WsfCommandChain::PlatformList* aPlatformListPtr)
   : mPlatformListPtr(aPlatformListPtr)
   , mCurrentKey(-1)
   , mCurrentDataPtr(nullptr)
{
}

// virtual
WsfScriptPlatformListIteratorClass::Iterator::~Iterator() {}

// virtual
bool WsfScriptPlatformListIteratorClass::Iterator::HasNext()
{
   bool hasNext = false;
   if ((mPlatformListPtr != nullptr) && (mCurrentKey < static_cast<int>(mPlatformListPtr->size() - 1)))
   {
      hasNext = true;
   }
   return hasNext;
}

// virtual
WsfPlatform* WsfScriptPlatformListIteratorClass::Iterator::Next()
{
   if ((mPlatformListPtr != nullptr) && (mCurrentKey < static_cast<int>(mPlatformListPtr->size() - 1)))
   {
      mCurrentDataPtr = (*mPlatformListPtr)[++mCurrentKey];
      return mCurrentDataPtr;
   }
   return nullptr;
}

// virtual
bool WsfScriptPlatformListIteratorClass::Iterator::HasPrev()
{
   return (mCurrentKey > 0);
}

// virtual
WsfPlatform* WsfScriptPlatformListIteratorClass::Iterator::Prev()
{
   if (mCurrentKey > 0)
   {
      mCurrentDataPtr = (*mPlatformListPtr)[--mCurrentKey];
      return mCurrentDataPtr;
   }
   return nullptr;
}

// virtual
int WsfScriptPlatformListIteratorClass::Iterator::Key() const
{
   return mCurrentKey;
}

// virtual
WsfPlatform* WsfScriptPlatformListIteratorClass::Iterator::Data() const
{
   return mCurrentDataPtr;
}
