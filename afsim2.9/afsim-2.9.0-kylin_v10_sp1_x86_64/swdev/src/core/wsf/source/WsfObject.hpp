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

#ifndef WSFOBJECT_HPP
#define WSFOBJECT_HPP

#include <memory>
#include <vector>

class UtInput;
#include "wsf_export.h"

#include "UtReferenceTracked.hpp"
#include "UtScriptAccessible.hpp"
#include "WsfStringId.hpp"

// =================================================================================================
// PROGRAMMING NOTE: See the source code file for information about thread-safety.
//                   DO NOT MODIFY THIS CLASS IF YOU DON'T UNDERSTAND THE INFORMATION.
// =================================================================================================

//! A base class for objects that have a name and a type.
//!
//! This class provides the methods needed by objects that have a 'name' and a
//! 'type' (this is pretty much every major user level object in WSF).  In
//! addition it defines other methods that such an object would typically
//! required (such as ProcessInput).
//!
//! Objects that derive from WsfObject can be stored the classes that
//! used WsfObjectTypeList.  This forms the basis for the WSF 'type' system.
class WSF_EXPORT WsfObject : public UtScriptAccessible, public UtReferenceTracked
{
public:
   friend class WsfObjectTypeListBase;
   friend class WsfPluginManager;

   //! The list of types returned by GetTypeList.
   // See comments in code why this is a vector and not a list.
   typedef std::vector<WsfStringId> TypeList;

   WsfObject();
   ~WsfObject() override;

   const char* GetScriptClassName() const override;

   //! Return a copy of this object.  Must be implemented by all derived types.
   //! If cloning is impossible, a WsfUncloneableException should be thrown.
   virtual WsfObject* Clone() const = 0;

   virtual bool ProcessInput(UtInput& aInput);

   //! @name Name methods.
   //@{
   const std::string& GetName() const;

   //! Get the string ID of the name of the object.
   //! @returns an integer containing the string ID of the name of the object.
   WsfStringId GetNameId() const { return mName; }

   //! Set the name of the object.
   //! @param aName a const reference to a WsfStringId containing the name of the object.
   void SetName(WsfStringId aName) { mName = aName; }
   //@}

   //! @name Type methods.
   //@{
   const std::string& GetType() const;

   //! Get the string ID of the 'type' of the object.
   WsfStringId GetTypeId() const { return mTypeDataPtr->mType; }

   void SetType(WsfStringId aType);

   const std::string& GetBaseType() const;

   //! Get the string ID of the object from which this object derives.
   //! This will be the type Id of the type object from which this instance derives.
   WsfStringId GetBaseTypeId() const { return mTypeDataPtr->mBaseType; }

   //! Return the type hierarchy.
   //! The first entry is the most specific type, followed by increasing basic types.
   //! (Basically, in order from last to first of calls to SetType).
   //! @note The list is guaranteed to be exist only as long as the owning WsfObject exists.
   const TypeList& GetTypeList() const { return mTypeDataPtr->mTypeList; }

   //! Is the object a 'type-of' the indicated type?
   //! This method searches the type list to see if the object is-a-type-of the indicated type.
   //! @returns true if the object is-a-type-of the indicated type.
   bool IsA_TypeOf(WsfStringId aType) const { return mTypeDataPtr->IsA_TypeOf(aType); }
   //@}

   WsfObject& operator=(const WsfObject& aRhs);

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mName & mTypeDataPtr->mType & mTypeDataPtr->mBaseType & mTypeDataPtr->mTypeList;
   }

protected:
   WsfObject(const WsfObject& aSrc);

private:
   void SetTypeAndBaseType(WsfStringId aType, WsfStringId aBaseType);

   //! The name of the object.
   WsfStringId mName;

   //! Type data is shared among all instances of a particular type.
   class SharedTypeData
   {
   public:
      void AddToTypeList(WsfStringId aType);

      bool IsA_TypeOf(WsfStringId aType) const;

      //! The type of the object (as defined by the most recent call to SetName).
      WsfStringId mType;

      //! The base type from which this type defined.
      //! @note If the input command 'is_a_type_of' (or 'is_type_of') did not exist then this could
      //! be derived directly from mTypeList using the next-to-last-entry. Because this commands can
      //! add other names to the end of the list, and it is derived to return the true base type,
      //! this must be maintained separately.
      WsfStringId mBaseType;

      //! The type hierarchy of the object, including all 'is_a_type_of' commands.
      //! The most specific entry is the first entry in the list, followed by less specific entries.
      TypeList mTypeList;
   };

   std::shared_ptr<SharedTypeData> mTypeDataPtr;
};

//! An object that does not support Clone.
class WSF_EXPORT WsfUncloneableObject : public WsfObject
{
public:
   WsfUncloneableObject()
      : WsfObject()
   {
   }
   WsfObject* Clone() const override;
};

#endif
