// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VaObject_hpp
#define VaObject_hpp

#include "vespatk_export.h"

#include <map>
#include <set>

#include "UtVariant.hpp"
#include "VaSystemDefines.hpp"
#include "VaUniqueId.hpp"

/** A base class for objects that have a name and a type.

    This class provides the methods needed by objects that have a 'name' and a
    'type'.
*/
namespace vespa
{
class VESPATK_EXPORT VaObject : public VaUniqueId
{
public:
   //! The list of types returned by GetTypeList.
   using TypeList = std::set<std::string>;

   VaObject();
   virtual ~VaObject() = default;

   //! @name Name methods.
   //@{

   //! Get the name of the object.
   //! @returns a std::string containing the name of the object.
   std::string GetName() const { return mName; }

   //! Set the name of the object.
   //! @param aName the name of the object.
   virtual void SetName(const std::string& aName) { mName = aName; }

   //@}

   //! @name Type methods.
   //@{

   //! Get the 'type' of the object.
   //! @returns a std::string  containing the 'type' of the object.
   std::string GetType() const { return mType; }

   template<class T>
   void SetType();

   template<class T>
   bool IsA_TypeOf() const;

   //! Return the type hierarchy.
   //! The first entry is the most basic type, followed by increasing specific types.
   //! (Basically, in order from first to last of calls to SetType).
   const TypeList& GetTypeList() const { return mTypeList; }

   //@}

   VaObject& operator=(const VaObject& aRhs);

   template<class T>
   void Serialize(T& aBuff)
   {
      aBuff& mName& mType& mTypeList;
   }

   void      SetData(const std::string& aTag, const UtVariant& aData) { mData[aTag] = aData; }
   UtVariant GetData(const std::string& aTag) const;
   bool      DataExists(const std::string& aTag) const { return mData.count(aTag) > 0; }

protected:
   VaObject(const VaObject& aSrc, bool aCreateNewUniqueId = true);

private:
   //! The string name.
   std::string mName;

   //! The string type.
   //! mType represent the most specific type of the object.
   std::string mType;

   //! The type hierarchy
   TypeList mTypeList;

   std::map<std::string, UtVariant> mData;
};
} // end namespace vespa.


//! Struct template representing a unique string identifier for VaObject types, which can be used
//! to find VaObject by type without having to explicitly specify the string literal.
//! It is compile-time error to instantiate this template. VaObject types that define identifiers
//! must specialize the template using the helper macro below.
template<class T>
struct VaObjectType
{
   template<class U>
   struct always_false : std::false_type
   {
   };
   static_assert(
      always_false<T>::value,
      "Type has no VaObject identifier registered. Use the VA_DECLARE_OBJECT_TYPE macro to register the type.");
};

//! Helper macro to declare a string identifier for an VaObject type by specializing VaObjectType template.
//! If TYPE is in a namespace, the macro has to be outside the namespace:
//! namespace vespa
//! {
//!    class VaAttachmentModel { ... };
//!    ...
//! }
//! VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentModel)
#define VA_DECLARE_OBJECT_TYPE(TYPE)                                                           \
   template<>                                                                                  \
   struct VaObjectType<TYPE>                                                                   \
   {                                                                                           \
      static_assert(std::is_base_of<vespa::VaObject, TYPE>::value,                             \
                    "Cannot register identifier for type that does not derive from VaObject"); \
      static constexpr const char* name = #TYPE;                                               \
   };

//! Helper variable template can be used when the code base is moved to c++14 or further.
//! Until then, struct template below must be used.
// template <class T>
// constexpr const char* cVAOBJECT_TYPE = VaObjectType<T>::name;

template<class T>
struct cVAOBJECT_TYPE
{
   operator std::string() const noexcept { return VaObjectType<T>::name; }
};

template<class T>
void vespa::VaObject::SetType()
{
   if (!IsA_TypeOf<T>())
   {
      mType = cVAOBJECT_TYPE<T>();
      mTypeList.insert(cVAOBJECT_TYPE<T>());
   }
}

//! Is the object a 'type-of' the indicated type.
//! This method searches the type list to see if the object is-a-type-of the indicated type.
//! @returns true if the object is-a-type-of the indicated type.
template<class T>
bool vespa::VaObject::IsA_TypeOf() const
{
   return (mTypeList.find(cVAOBJECT_TYPE<T>()) != mTypeList.end());
}

#endif
