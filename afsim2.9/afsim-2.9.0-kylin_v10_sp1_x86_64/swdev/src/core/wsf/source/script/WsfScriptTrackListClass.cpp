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

#include "script/WsfScriptTrackListClass.hpp"

#include "UtMemory.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"

WsfScriptTrackListClass::WsfScriptTrackListClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfTrackList");

   // Set the key and data types of this container.
   SetContainerKeyTypeId(GetTypes()->GetTypeId("int"));
   SetContainerDataTypeId(GetTypes()->GetTypeId("WsfTrack"));

   mContainer = true;

   // Add each of the method objects to the class.

   AddMethod(ut::make_unique<Count>());
   AddMethod(ut::make_unique<Count>("Size"));       // Support Array<>-type interface
   AddMethod(ut::make_unique<Count>("TrackCount")); // For backward compatibility with very old files
   AddMethod(ut::make_unique<Empty>());             // Support Array<>-type interface
   AddMethod(ut::make_unique<Entry>());
   AddMethod(ut::make_unique<Entry>("Get"));        // Support Array<>-type interface
   AddMethod(ut::make_unique<Entry>("TrackEntry")); // For backward compatibility with very old files
   AddMethod(ut::make_unique<Find>());
   AddMethod(ut::make_unique<Find>("FindTrack")); // For backward compatibility with very old files
   AddMethod(ut::make_unique<GetIterator>());
}

WsfScriptTrackListClass::~WsfScriptTrackListClass() {}

// virtual
void* WsfScriptTrackListClass::Create(const UtScriptContext& /*aContext*/)
{
   return new WsfTrackList();
}

// virtual
void* WsfScriptTrackListClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfTrackList*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptTrackListClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfTrackList*>(aObjectPtr);
}

int WsfScriptTrackListClass::EnumerateSize(void* aObjectPtr)
{
   WsfTrackList* listPtr = (WsfTrackList*)aObjectPtr;
   return listPtr ? (int)listPtr->GetTrackCount() : 0;
}

UtScriptData WsfScriptTrackListClass::Enumerate(void* aObjectPtr, int aIndex)
{
   WsfTrackList* listPtr = (WsfTrackList*)aObjectPtr;
   if (listPtr && (int)listPtr->GetTrackCount() > aIndex && aIndex >= 0)
   {
      return UtScriptData(new UtScriptRef(listPtr->GetTrackEntry(aIndex), GetTypes()->GetClass("WsfTrack")));
   }
   return UtScriptData();
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptTrackListClass, WsfTrackList, Count, 0, "int", "")
{
   int count = 0;
   if (aObjectPtr != nullptr)
   {
      count = aObjectPtr->GetTrackCount();
   }
   aReturnVal.SetInt(count);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptTrackListClass, WsfTrackList, Empty, 0, "bool", "")
{
   bool empty(true);
   if (aObjectPtr != nullptr)
   {
      empty = (aObjectPtr->GetTrackCount() == 0);
   }
   aReturnVal.SetBool(empty);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackListClass, WsfTrackList, Entry, 1, "WsfTrack", "int")
{
   // arg1: track list index.

   int       index    = aVarArgs[0].GetInt();
   WsfTrack* trackPtr = nullptr;
   if ((index >= 0) && (index < static_cast<int>(aObjectPtr->GetTrackCount())))
   {
      trackPtr = aObjectPtr->GetTrackEntry(index);
   }
   aReturnVal.SetPointer(new UtScriptRef(trackPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackListClass, WsfTrackList, Find, 1, "WsfTrack", "WsfTrackId")
{
   // arg1: track id.

   WsfTrackId trackId(*((WsfTrackId*)aVarArgs[0].GetPointer()->GetAppObject()));
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->FindTrack(trackId), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackListClass, WsfTrackList, GetIterator, 0, "WsfTrackListIterator", "")
{
   // Create the iterator.
   void* iterPtr = new WsfScriptTrackListIteratorClass::Iterator(*aObjectPtr);

   // Create a return object that manages the iterator memory we just created.
   aReturnVal.SetPointer(new UtScriptRef(iterPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

WsfScriptTrackListIteratorClass::WsfScriptTrackListIteratorClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
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
WsfScriptTrackListIteratorClass::~WsfScriptTrackListIteratorClass() {}

// virtual
void WsfScriptTrackListIteratorClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfScriptTrackListIteratorClass::Iterator*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackListIteratorClass, WsfScriptTrackListIteratorClass::Iterator, HasNext, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasNext());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackListIteratorClass, WsfScriptTrackListIteratorClass::Iterator, Next, 0, "WsfTrack", "")
{
   WsfTrack* rVal = aObjectPtr->Next();
   if (!aObjectPtr->IteratorValidForAccess())
   {
      UT_SCRIPT_ABORT(aObjectPtr->IteratorErrorString());
   }
   aReturnVal.SetPointer(new UtScriptRef(rVal, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackListIteratorClass, WsfScriptTrackListIteratorClass::Iterator, HasPrev, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasPrev());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackListIteratorClass, WsfScriptTrackListIteratorClass::Iterator, Prev, 0, "WsfTrack", "")
{
   WsfTrack* rVal = aObjectPtr->Prev();
   if (!aObjectPtr->IteratorValidForAccess())
   {
      UT_SCRIPT_ABORT(aObjectPtr->IteratorErrorString());
   }
   aReturnVal.SetPointer(new UtScriptRef(rVal, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackListIteratorClass, WsfScriptTrackListIteratorClass::Iterator, Key, 0, "int", "")
{
   aReturnVal = aObjectPtr->Key();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackListIteratorClass, WsfScriptTrackListIteratorClass::Iterator, Data, 0, "WsfTrack", "")
{
   if (!aObjectPtr->IteratorValidForAccess())
   {
      UT_SCRIPT_ABORT(aObjectPtr->IteratorErrorString());
   }
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->Data(), aReturnClassPtr));
}

WsfScriptTrackListIteratorClass::Iterator::Iterator(const WsfTrackList& aTrackList)
   : mTrackListRef(aTrackList)
   , mCurrentKey(-1)
   , mVectorSize(aTrackList.GetTrackCount())
{
}

// virtual
WsfScriptTrackListIteratorClass::Iterator::~Iterator() {}

// virtual
bool WsfScriptTrackListIteratorClass::Iterator::HasNext()
{
   return (mCurrentKey + 1 < (int)mTrackListRef.GetTrackCount());
}

// virtual
WsfTrack* WsfScriptTrackListIteratorClass::Iterator::Next()
{
   if (mCurrentKey + 1 < static_cast<int>(mTrackListRef.GetTrackCount()))
   {
      ++mCurrentKey;
      return Data();
   }
   return nullptr;
}

// virtual
bool WsfScriptTrackListIteratorClass::Iterator::HasPrev()
{
   return (mCurrentKey > 0);
}

// virtual
WsfTrack* WsfScriptTrackListIteratorClass::Iterator::Prev()
{
   if (mCurrentKey > 0)
   {
      --mCurrentKey;
      return Data();
   }
   return nullptr;
}

// virtual
int WsfScriptTrackListIteratorClass::Iterator::Key() const
{
   return mCurrentKey;
}

// virtual
WsfTrack* WsfScriptTrackListIteratorClass::Iterator::Data() const
{
   return const_cast<WsfTrack*>(mTrackListRef.GetTrackEntry(mCurrentKey));
}


bool WsfScriptTrackListIteratorClass::Iterator::IteratorValidForAccess() const
{
   return (mVectorSize == static_cast<int>(mTrackListRef.GetTrackCount())) && (mCurrentKey >= 0) &&
          (mCurrentKey < (int)mVectorSize);
}

std::string WsfScriptTrackListIteratorClass::Iterator::IteratorErrorString() const
{
   if (mVectorSize != static_cast<int>(mTrackListRef.GetTrackCount()))
   {
      return "Iterator invalid -- List was modified";
   }
   return "Iterator invalid -- out of range";
}
