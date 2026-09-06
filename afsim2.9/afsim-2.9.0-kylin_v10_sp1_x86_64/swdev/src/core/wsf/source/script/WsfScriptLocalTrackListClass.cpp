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

#include "script/WsfScriptLocalTrackListClass.hpp"

#include "UtMemory.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfTrackList.hpp"

WsfScriptLocalTrackListClass::WsfScriptLocalTrackListClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfLocalTrackList");

   // Set the key and data types of this container.
   SetContainerKeyTypeId(GetTypes()->GetTypeId("int"));
   SetContainerDataTypeId(GetTypes()->GetTypeId("WsfLocalTrack"));

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

WsfScriptLocalTrackListClass::~WsfScriptLocalTrackListClass() {}

// virtual
void* WsfScriptLocalTrackListClass::Create(const UtScriptContext& /*aContext*/)
{
   return static_cast<void*>(new WsfLocalTrackList());
}

// virtual
void* WsfScriptLocalTrackListClass::Clone(void* aObjectPtr)
{
   WsfLocalTrackList* objPtr = static_cast<WsfLocalTrackList*>(aObjectPtr);
   assert(objPtr != nullptr);
   return objPtr->Clone();
}

// virtual
void WsfScriptLocalTrackListClass::Destroy(void* aObjectPtr)
{
   WsfLocalTrackList* objPtr = static_cast<WsfLocalTrackList*>(aObjectPtr);
   delete objPtr;
}

int WsfScriptLocalTrackListClass::EnumerateSize(void* aObjectPtr)
{
   WsfLocalTrackList* listPtr = (WsfLocalTrackList*)aObjectPtr;
   return listPtr ? (int)listPtr->GetTrackCount() : 0;
}

UtScriptData WsfScriptLocalTrackListClass::Enumerate(void* aObjectPtr, int aIndex)
{
   WsfLocalTrackList* listPtr = (WsfLocalTrackList*)aObjectPtr;
   if (listPtr && (int)listPtr->GetTrackCount() > aIndex && aIndex >= 0)
   {
      return UtScriptData(new UtScriptRef(listPtr->GetTrackEntry(aIndex), GetTypes()->GetClass("WsfLocalTrack")));
   }
   return UtScriptData();
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptLocalTrackListClass, WsfLocalTrackList, Count, 0, "int", "")
{
   int count = 0;
   if (aObjectPtr != nullptr)
   {
      count = aObjectPtr->GetTrackCount();
   }
   aReturnVal.SetInt(count);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptLocalTrackListClass, WsfLocalTrackList, Empty, 0, "bool", "")
{
   bool empty(true);
   if (aObjectPtr != nullptr)
   {
      empty = (aObjectPtr->GetTrackCount() == 0);
   }
   aReturnVal.SetBool(empty);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackListClass, WsfLocalTrackList, Entry, 1, "WsfLocalTrack", "int")
{
   // arg1: track list index.

   int            index    = aVarArgs[0].GetInt();
   WsfLocalTrack* trackPtr = nullptr;
   if ((index >= 0) && (index < static_cast<int>(aObjectPtr->GetTrackCount())))
   {
      trackPtr = aObjectPtr->GetTrackEntry(index);
   }
   aReturnVal.SetPointer(new UtScriptRef(trackPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackListClass, WsfLocalTrackList, Find, 1, "WsfLocalTrack", "WsfTrackId")
{
   // arg1: track id.

   WsfTrackId trackId(*static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject()));
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->FindTrack(trackId), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackListClass, WsfLocalTrackList, GetIterator, 0, "WsfLocalTrackListIterator", "")
{
   // Create the iterator.
   void* iterPtr = new WsfScriptLocalTrackListIteratorClass::Iterator(*aObjectPtr);

   // Create a return object that manages the iterator memory we just created.
   aReturnVal.SetPointer(new UtScriptRef(iterPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

WsfScriptLocalTrackListIteratorClass::WsfScriptLocalTrackListIteratorClass(const std::string& aClassName,
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
WsfScriptLocalTrackListIteratorClass::~WsfScriptLocalTrackListIteratorClass() {}

// virtual
void WsfScriptLocalTrackListIteratorClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfScriptLocalTrackListIteratorClass::Iterator*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackListIteratorClass, WsfScriptLocalTrackListIteratorClass::Iterator, HasNext, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasNext());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackListIteratorClass,
                        WsfScriptLocalTrackListIteratorClass::Iterator,
                        Next,
                        0,
                        "WsfLocalTrack",
                        "")
{
   WsfTrack* rVal = aObjectPtr->Next();
   if (!aObjectPtr->IteratorValidForAccess())
   {
      UT_SCRIPT_ABORT(aObjectPtr->IteratorErrorString());
   }
   aReturnVal.SetPointer(new UtScriptRef(rVal, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackListIteratorClass, WsfScriptLocalTrackListIteratorClass::Iterator, HasPrev, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasPrev());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackListIteratorClass,
                        WsfScriptLocalTrackListIteratorClass::Iterator,
                        Prev,
                        0,
                        "WsfLocalTrack",
                        "")
{
   WsfTrack* rVal = aObjectPtr->Prev();
   if (!aObjectPtr->IteratorValidForAccess())
   {
      UT_SCRIPT_ABORT(aObjectPtr->IteratorErrorString());
   }
   aReturnVal.SetPointer(new UtScriptRef(rVal, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackListIteratorClass, WsfScriptLocalTrackListIteratorClass::Iterator, Key, 0, "int", "")
{
   aReturnVal = aObjectPtr->Key();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackListIteratorClass,
                        WsfScriptLocalTrackListIteratorClass::Iterator,
                        Data,
                        0,
                        "WsfLocalTrack",
                        "")
{
   if (!aObjectPtr->IteratorValidForAccess())
   {
      UT_SCRIPT_ABORT(aObjectPtr->IteratorErrorString());
   }
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->Data(), aReturnClassPtr));
}

WsfScriptLocalTrackListIteratorClass::Iterator::Iterator(const WsfLocalTrackList& aLocalTrackList)
   : mLocalTrackListRef(aLocalTrackList)
   , mCurrentKey(-1)
   , mVectorSize(aLocalTrackList.GetTrackCount())
{
}

// virtual
WsfScriptLocalTrackListIteratorClass::Iterator::~Iterator() {}

// virtual
bool WsfScriptLocalTrackListIteratorClass::Iterator::HasNext()
{
   return (mCurrentKey + 1 < (int)mLocalTrackListRef.GetTrackCount());
}

// virtual
WsfTrack* WsfScriptLocalTrackListIteratorClass::Iterator::Next()
{
   if (mCurrentKey + 1 < (int)mLocalTrackListRef.GetTrackCount())
   {
      ++mCurrentKey;
      return Data();
   }
   return nullptr;
}

// virtual
bool WsfScriptLocalTrackListIteratorClass::Iterator::HasPrev()
{
   return (mCurrentKey > 0);
}

// virtual
WsfTrack* WsfScriptLocalTrackListIteratorClass::Iterator::Prev()
{
   if (mCurrentKey > 0)
   {
      --mCurrentKey;
      return Data();
   }
   return nullptr;
}

// virtual
int WsfScriptLocalTrackListIteratorClass::Iterator::Key() const
{
   return mCurrentKey;
}

// virtual
WsfTrack* WsfScriptLocalTrackListIteratorClass::Iterator::Data() const
{
   if (mCurrentKey >= 0 && mCurrentKey < static_cast<int>(mLocalTrackListRef.GetTrackCount()))
   {
      return const_cast<WsfLocalTrack*>(mLocalTrackListRef.GetTrackEntry(mCurrentKey));
   }
   return nullptr;
}

bool WsfScriptLocalTrackListIteratorClass::Iterator::IteratorValidForAccess() const
{
   return (mVectorSize == static_cast<int>(mLocalTrackListRef.GetTrackCount())) && (mCurrentKey >= 0) &&
          (mCurrentKey < (int)mVectorSize);
}

std::string WsfScriptLocalTrackListIteratorClass::Iterator::IteratorErrorString() const
{
   if (mVectorSize != static_cast<int>(mLocalTrackListRef.GetTrackCount()))
   {
      return "Iterator invalid -- List was modified";
   }
   return "Iterator invalid -- out of range";
}
