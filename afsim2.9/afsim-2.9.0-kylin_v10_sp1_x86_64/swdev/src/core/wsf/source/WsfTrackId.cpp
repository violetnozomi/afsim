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

#include "WsfTrackId.hpp"

#include <iostream>
#include <sstream>

#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtStringUtil.hpp"
#include "WsfStringId.hpp"

// ***************************************************************************

//! The script interface 'class'
class WSF_EXPORT WsfScriptTrackIdClass : public UtScriptClass
{
public:
   WsfScriptTrackIdClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void*       Create(const UtScriptContext& aInstance) override;
   void*       Clone(void* aObjectPtr) override;
   void        Destroy(void* aObjectPtr) override;
   std::string ToString(void* aObjectPtr) const override;
   bool        LessThan(void* aLHS, void* aRHS) override;
   bool        EqualTo(void* aLHS, void* aRHS) override;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(Name);
   UT_DECLARE_SCRIPT_METHOD(SetName);
   UT_DECLARE_SCRIPT_METHOD(NameId); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(Number);
   UT_DECLARE_SCRIPT_METHOD(SetNumber);
   UT_DECLARE_SCRIPT_METHOD(IsNull);
   UT_DECLARE_SCRIPT_METHOD(Null);
   UT_DECLARE_SCRIPT_METHOD(ToString);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfTrackId::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptTrackIdClass>(aClassName, aScriptTypesPtr);
}

WsfScriptTrackIdClass::WsfScriptTrackIdClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfTrackId");

   mConstructible      = true;
   mCloneable          = true;
   mEqualityComparable = true;
   mLessThanComparable = true;
   mIsScriptAccessible = true;

   // Add each of the method objects to the class.

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<Name>());
   AddMethod(ut::make_unique<SetName>());
   AddMethod(ut::make_unique<NameId>()); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<Number>());
   AddMethod(ut::make_unique<SetNumber>());
   AddMethod(ut::make_unique<IsNull>());
   AddMethod(ut::make_unique<Null>());
   AddMethod(ut::make_unique<Name>("OwningPlatform"));     // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<NameId>("OwningPlatformId")); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<Number>("TrackNumber"));      // NO_DOC | DEPRECATED
}

// virtual
void* WsfScriptTrackIdClass::Create(const UtScriptContext& /*aContext*/)
{
   return new WsfTrackId();
}

// virtual
void* WsfScriptTrackIdClass::Clone(void* aObjectPtr)
{
   return new WsfTrackId(*static_cast<WsfTrackId*>(aObjectPtr));
}

// virtual
void WsfScriptTrackIdClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfTrackId*>(aObjectPtr);
}

// virtual
std::string WsfScriptTrackIdClass::ToString(void* aObjectPtr) const
{
   std::stringstream ss;
   if (aObjectPtr != nullptr)
   {
      WsfTrackId* trackIdPtr = static_cast<WsfTrackId*>(aObjectPtr);
      ss << *trackIdPtr;
   }
   else
   {
      ss << "null";
   }
   return ss.str();
}

// virtual
bool WsfScriptTrackIdClass::LessThan(void* aLhsPtr, void* aRhsPtr)
{
   bool lessThan = false;
   if ((aLhsPtr != nullptr) && (aRhsPtr != nullptr))
   {
      WsfTrackId* lhsPtr = static_cast<WsfTrackId*>(aLhsPtr);
      WsfTrackId* rhsPtr = static_cast<WsfTrackId*>(aRhsPtr);
      lessThan           = *lhsPtr < *rhsPtr;
   }
   return lessThan;
}

// virtual
bool WsfScriptTrackIdClass::EqualTo(void* aLhsPtr, void* aRhsPtr)
{
   bool equalTo = false;
   if ((aLhsPtr != nullptr) && (aRhsPtr != nullptr))
   {
      WsfTrackId* lhsPtr = static_cast<WsfTrackId*>(aLhsPtr);
      WsfTrackId* rhsPtr = static_cast<WsfTrackId*>(aRhsPtr);
      equalTo            = *lhsPtr == *rhsPtr;
   }
   return equalTo;
}

// friend
std::ostream& operator<<(std::ostream& aOut, const WsfTrackId& aTrackId)
{
   aOut << aTrackId.mOwningPlatformId << '.' << aTrackId.mLocalTrackNumber;
   return aOut;
}

// friend
std::istream& operator>>(std::istream& aIn, WsfTrackId& aTrackId)
{
   std::string trackId;
   aIn >> trackId;
   size_t dotPoint = trackId.find_last_of('.');
   if (dotPoint != std::string::npos)
   {
      aTrackId.SetOwningPlatformId(WsfStringId(trackId.substr(0, dotPoint)));
      aTrackId.SetLocalTrackNumber(UtStringUtil::ToInt(trackId.substr(dotPoint + 1)));
   }
   return aIn;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackIdClass, WsfTrackId, Construct, 2, "WsfTrackId", "string, int")
{
   WsfTrackId* trackIdPtr = new WsfTrackId((WsfStringId)aVarArgs[0].GetString(), aVarArgs[1].GetInt());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackIdClass, WsfTrackId, Name, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetOwningPlatformId().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackIdClass, WsfTrackId, SetName, 1, "void", "string")
{
   aObjectPtr->SetOwningPlatformId(aVarArgs[0].GetString());
}

// Deprecated
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackIdClass, WsfTrackId, NameId, 0, "int", "") // NO_DOC | DEPRECATED
{
   // aReturnVal.SetInt(aObjectPtr->GetOwningPlatformId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackIdClass, WsfTrackId, Number, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetLocalTrackNumber());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackIdClass, WsfTrackId, SetNumber, 1, "void", "int")
{
   aObjectPtr->SetLocalTrackNumber(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackIdClass, WsfTrackId, IsNull, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsNull());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackIdClass, WsfTrackId, Null, 0, "void", "")
{
   aObjectPtr->Null();
}
