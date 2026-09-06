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

// =================================================================================================
// =================================================================================================
// PROGRAMMING NOTE: About thread-safety.
//
//         DO NOT MODIFY THIS CLASS IF YOU DO NOT UNDERSTAND THE FOLLOWING INFORMATION!
//
// Every instance of this class contains a pointer to a SharedTypeData object that contains the
// type data for instance (it's type, base type and complete type list). In order to save space and
// time, all instances of of the same type point to the same shared type data.
//
// Thread-safety is a concern only for OBJECTS THAT ARE ACTIVE IN A SIMULATION (that is, one that
// has directly or indirectly been initialized and added to the simulation). Thread-safety is not
// a consideration during scenario loading as it is not considered thread-safe and must be run in a
// single thread. However, cloning of instances from a scenario to create the simulation instances
// must be thread-safe enough to ensure multiple simulations running in parallel do not cause issues.
// The thread-safety of UtSharedPtr ensures that level of thread-safety.
//
// The main concern is the use of SetType ON AN OBJECT ACTIVE IN A SIMULATION (as defined above).
// In general, calling SetType ON AN ACTIVE OBJECT MUST NOT BE DONE because it can cause reallocation
// of the shared type data. In a multi-thread environment other threads using GetType, GetTypeList or
// IsA_TypeOf could reference deleted memory if SetType is called at just the wrong time. The window
// is very, very narrow, but in theory it could happen.
//
//     +-----------------------------------------------------------------------------------+
//     | The general rule is then: DO NOT USE SetType ON AN OBJECT ACTIVE IN A SIMULATION! |
//     +-----------------------------------------------------------------------------------+
//
// It is conceivable that code could call SetType, but it would be easy to search for such occurrences.
// But for this reason, SetType is NOT exposed to script because it would be very difficult to find
// all of the uses.
//
// A mutex could be used to allow for complete thread-safety, but the performance losses caused by
// locking all calls to GetType, GetTypeList and IsA_TypeOf shouldn't be incurred because of something
// that shouldn't be allowed anyhow!
//
// ANOTHER NOTE: The type list is stored as a vector for code compatibility and performance reasons.
// In the early implementations it was a vector and stored in reverse order from the current form.
// It is now most-specific -> least-specific (It used to be the other way around, but in practice
// most callers really wanted it in the most-specific -> least-specific order.) It was then converted
// to a UtImmutableList. Callers had to be modified not only call 'toVector' to get a vector from the
// list, but also to reverse the direction in which they processed the vector. The current code once
// again uses a vector, but in order to avoid having to again change the order in which callers
// traversed the list, the code now inserts at the front of the vector when adding new types to the
// type list. This is generally a no-no on vectors, but these vectors are very small. A <list> would
// have been better from this stand point, but the callers would have been forced to convert to use
// iterators rather than vector indices as was commonly done. The old code looked like:
//
//    WsfObject::TypeList typeList = objectPtr->GetTypeList().toVector();
//    for (size_t i = 0; i < typeList.size(); ++i)
//
// The new syntax is:
//
//    const WsfObject::TypeList& typeList = objectPtr->GetTypeList();
//    for (size_t i = 0; i < typeList.size(); ++i)
//
// The first statement is exactly what the code looked like prior to the use of UtImmutableList. The
// caller can simply eliminate the '.toVector()' and leave the rest of the statement, but it would not
// be as efficient because it would cause a copy of the vector.
//
// The performance hit from inserting elements at the front of the vector should be small because
// The number of elements is small and is done only when reading the scenario.
//
// =================================================================================================
// =================================================================================================

#include "WsfObject.hpp"

#include <algorithm>

#include "UtInput.hpp"
#include "WsfException.hpp"
#include "WsfScenario.hpp"
#include "WsfSystemLog.hpp"

// =================================================================================================
//! This is the constructor for the WsfObject class.
WsfObject::WsfObject()
   : mName()
   , mTypeDataPtr(new SharedTypeData)
{
}

// =================================================================================================
//! Copy constructor (for Clone())
// protected
WsfObject::WsfObject(const WsfObject& aSrc)
   : UtReferenceTracked(aSrc)
   , mName(aSrc.mName)
   , mTypeDataPtr(aSrc.mTypeDataPtr)
{
}

// =================================================================================================
//! This is the destructor for the WsfObject class.
// virtual
WsfObject::~WsfObject()
{
   UtReferenceTracked::FreeReference();
}

// =================================================================================================
//! @return The most specific script class name for this class
// virtual
const char* WsfObject::GetScriptClassName() const
{
   return "WsfObject";
}

// =================================================================================================
WsfObject* WsfUncloneableObject::Clone() const
{
   throw WsfUncloneableException();
}

// =================================================================================================
//! Process input from a generic source.
//!
//! Examine the current input command.  If it is NOT one of the commands recognized by this class
//! then it simply returns 'false'.  If it is one of the commands recognized by this class then it
//! processes the command and returns 'true'.
//! @param aInput a reference to a UtInput object.
//! @returns 'true' if the command was one recognized by the class or 'false' if not.
//! @throws UtInput::ExceptionBase (or an object derived from it) if the command was recognized
//!         but contains some sort of error.
//! @note If a derived class does not recognize the command (i.e.: the return value is false)
//!       then it should call the base class ProcessInput method.  This chain should continue
//!       until either the command is recognized or the top-level base class is encountered.
// virtual
bool WsfObject::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "is_a_type_of") || (command == "is_type_of"))
   {
      std::string typeNameStr;
      aInput.ReadValue(typeNameStr);
      WsfStringId typeName(typeNameStr);
      if (!IsA_TypeOf(typeName))
      {
         SharedTypeData* typeDataPtr = new SharedTypeData(*mTypeDataPtr);
         typeDataPtr->AddToTypeList(typeName);
         mTypeDataPtr.reset(typeDataPtr);
      }
   }
   // This allows a comment/version directive to be inserted in the system log.
   else if (WsfScenario::FromInput(aInput).GetSystemLog().ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Set the 'type' of the object.
//! @param aType The type to be assigned to the object.
//! @note The type of an object MUST NOT be changed once the object is actively part of a simulation
//! as there are thread-safety issues.
void WsfObject::SetType(WsfStringId aType)
{
   // Perform trival check(s) to minimize unnecessary deletion and (re)creation of shared type data.
   // There are more involved checks that can potentially be done, but we err on the side of safefy.

   if (aType == mTypeDataPtr->mType)
   {
      // The type is not changing. This would occur when:
      //
      // - A new derived object type is being created (e.g.: Wsf<X>ObjectTypeList::LoadType.)
      // - A new instance object is being created (e.g.: Wsf<X>ObjectTypeList::LoadInstance.)
      //
      // In each case, CloneP is called with the type of the source object. CloneP calls FindP with
      // the source object type, calls the objects Clone method and then calls SetType with the same
      // type. Because the current type and input type are the same, it ends up being a no-op.
      //
      // Note that in the first case, SetTypeAndBaseType is eventually called by AddP.
   }
   else
   {
      // Changing the type. A new type data object must be created.
      //
      // The new shared type data is constructed and populated before setting it as the new data.
      // This minimizes any exposure to others who might be accessing the data (See notes at top of file)
      SharedTypeData* typeDataPtr = new SharedTypeData(*mTypeDataPtr);
      typeDataPtr->mType          = aType;
      typeDataPtr->AddToTypeList(aType);
      mTypeDataPtr.reset(typeDataPtr);
   }
}

// =================================================================================================
//! Set the type and base type of the object.
//! This is called only from WsfObjectTypeListBase::AddP (which is called by WsfObjectTypeList::Add
//! and WsfObjectTypeList::AddCoreType when adding a new type object.)
//! @param aType     The type of the object.
//! @param aBaseType The base type of the object.
// private
void WsfObject::SetTypeAndBaseType(WsfStringId aType, WsfStringId aBaseType)
{
   // Perform trival check(s) to minimize unnecessary deletion and (re)creation of shared type data.
   // There are more involved checks that can potentially be done, but we err on the side of safefy.

   if ((aType == mTypeDataPtr->mType) && (aBaseType == mTypeDataPtr->mBaseType))
   {
      // No change in the type/base type. This typically won't occur, but if it does it will be ignored.
   }
   else
   {
      // Changing the type or subtype. A new type data object must be created.
      // NOTE: Thread-safety should not be a concern here as this method is only called during the cloning
      //       process. The object is not yet part of the simulation.
      SharedTypeData* typeDataPtr = new SharedTypeData(*mTypeDataPtr);
      typeDataPtr->mType          = aType;
      typeDataPtr->mBaseType      = aBaseType;
      typeDataPtr->AddToTypeList(aType);
      mTypeDataPtr.reset(typeDataPtr);
   }
}

// =================================================================================================
//! Get the name of the object.
//! @returns A string containing the name of the object.
const std::string& WsfObject::GetName() const
{
   return mName.GetString();
}

// =================================================================================================
//! Get the 'type' of the object.
//! @returns A string containing the 'type' of the object.
const std::string& WsfObject::GetType() const
{
   return mTypeDataPtr->mType.GetString();
}

// =================================================================================================
//! Get the 'base type' of the object (the type of the object from which this object derives.)
//! @returns a std::string  containing the 'base type' of the object.
const std::string& WsfObject::GetBaseType() const
{
   return mTypeDataPtr->mBaseType.GetString();
}

// =================================================================================================
//! Assignment operator.
WsfObject& WsfObject::operator=(const WsfObject& aRhs)
{
   if (this != &aRhs)
   {
      mName        = aRhs.mName;
      mTypeDataPtr = aRhs.mTypeDataPtr;
   }
   return *this;
}

// =================================================================================================
bool WsfObject::SharedTypeData::IsA_TypeOf(WsfStringId aType) const
{
   return std::find(mTypeList.begin(), mTypeList.end(), aType) != mTypeList.end();
}

// =================================================================================================
void WsfObject::SharedTypeData::AddToTypeList(WsfStringId aType)
{
   if (!IsA_TypeOf(aType))
   {
      // Not very efficient, but it doesn't happen much, the lists are small, and it keeps the list in
      // the desired order (most-specific -> least-specific).
      mTypeList.insert(mTypeList.begin(), aType);
   }
}
