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

#ifndef WSFVARIABLE_HPP
#define WSFVARIABLE_HPP

#include "wsf_export.h"

#include <iosfwd>
#include <string>

#include "UtInput.hpp"
class UtScriptContext;
#include "UtScriptData.hpp"
class WsfComponent;
class WsfScriptContext;
class WsfScriptManager;
#include "WsfStringId.hpp"

//! A class which supports defining a variable that can either be a fixed value or a reference to a script variable.
//!
//! This class is defined so that only minimal changes need to be made to the code to replace a simple scalar
//! variable with one that can get its value from a script variable.

//-------------------------------------------------------------------------------------------------
//! The base class for a variable.
//! This contains:
//! - attributes that are not dependent on the type of the variable.
//! - type-specific methods to read, initialize and access the variable.
class WSF_EXPORT WsfVariableBase
{
public:
   // NOTE: The default (member-wise) copy constructor and assignment operator are used.

   WsfVariableBase();

   //! Has a default value been defined for a reference.
   bool HasDefaultValue() const { return mHasDefaultValue; }

   //! Is the variable a reference to a script variable?
   bool IsReference() const { return !mRefVarNameId.IsNull(); }

protected:
   bool DefaultOptionPresent(UtInput& aInput);

   std::string ObjectName(WsfComponent* aObjectPtr);

   bool ReadReference(UtInput& aInput);

   bool ResolveReference(const std::string&   aVariableName,
                         WsfComponent*        aObjectPtr,
                         WsfScriptContext*    aContextPtr,
                         const UtScriptData*& aRefValuePtr,
                         WsfScriptContext&    aGlobalContext);

   //! @name Routines for type 'double'
   //@{
   bool Initialize(const std::string& aName,
                   WsfComponent*      aObjectPtr,
                   WsfScriptContext*  aContextPtr,
                   double&            aValue,
                   WsfScriptContext&  aGlobalContext);

   void ReadValue(UtInput& aInput, double& aValue);

   void ReadValueOfType(UtInput& aInput, UtInput::ValueType aType, double& aValue);
   //@}

   //! @name Routines for type 'float'
   //@{
   bool Initialize(const std::string& aName,
                   WsfComponent*      aObjectPtr,
                   WsfScriptContext*  aContextPtr,
                   float&             aValue,
                   WsfScriptContext&  aGlobalContext);

   void ReadValue(UtInput& aInput, float& aValue);

   void ReadValueOfType(UtInput& aInput, UtInput::ValueType aType, float& aValue);
   //@}

   //! @name Routines for type 'int'
   //@{
   bool Initialize(const std::string& aName,
                   WsfComponent*      aObjectPtr,
                   WsfScriptContext*  aContextPtr,
                   int&               aValue,
                   WsfScriptContext&  aGlobalContext);

   void ReadValue(UtInput& aInput, int& aValue);

   void ReadValueOfType(UtInput& aInput, UtInput::ValueType aType, int& aValue);
   //@}

   //! @name Routines for type 'bool'
   //@{
   bool Initialize(const std::string& aName,
                   WsfComponent*      aObjectPtr,
                   WsfScriptContext*  aContextPtr,
                   bool&              aValue,
                   WsfScriptContext&  aGlobalContext);

   void ReadValue(UtInput& aInput, bool& aValue);
   //@}

   //! @name Routines for type 'std::string'
   //@{
   bool Initialize(const std::string& aName,
                   WsfComponent*      aObjectPtr,
                   WsfScriptContext*  aContextPtr,
                   std::string&       aValue,
                   WsfScriptContext&  aGlobalContext);

   void ReadValue(UtInput& aInput, std::string& aValue);
   //@}

   //! @name Routines for type 'WsfStringId'
   //@{
   bool Initialize(const std::string& aName,
                   WsfComponent*      aObjectPtr,
                   WsfScriptContext*  aContextPtr,
                   WsfStringId&       aValue,
                   WsfScriptContext&  aGlobalContext);

   void ReadValue(UtInput& aInput, WsfStringId& aValue);
   //@}

   //! The name (ID) of the referenced variable. It is less than zero if the variable is not a reference.
   WsfStringId mRefVarNameId;

   //! 'true' if the '/default' option was provided
   bool mHasDefaultValue;
};

//-------------------------------------------------------------------------------------------------
//! A template that defines a variable of a specific type.
//!
//! This template is primarily for numeric types.
template<typename T>
class WsfVariable : public WsfVariableBase
{
public:
   // NOTE: The default (member-wise) copy constructor and assignment operator are used.
   //       Copying or assigning a 'Initialize'd variable is OK as long as the copy would also
   //       refer to the same script context.

   WsfVariable() = default;

   WsfVariable(const T& aInitialValue)
      : WsfVariableBase()
      , mValue(aInitialValue)
   {
   }

   WsfVariable(const WsfVariable<T>&) = default;

   //! Return the fixed or resolved value of the variable.
   //! @note: This should not be called until after Initialize is called.
   operator T() const { return mValue; }

   T GetValue() const { return mValue; }

   WsfVariable<T>& operator=(const T& aRhs)
   {
      mRefVarNameId.Clear();
      mHasDefaultValue = false;
      mValue           = aRhs;
      return *this;
   }

   WsfVariable<T>& operator=(const WsfVariable<T>& aRhs)
   {
      mRefVarNameId    = aRhs.mRefVarNameId;
      mHasDefaultValue = aRhs.mHasDefaultValue;
      mValue           = aRhs.mValue;
      return *this;
   }

   T operator+(const T& aRhs) const { return operator T() + aRhs; }

   T operator-(const T& aRhs) const { return operator T() - aRhs; }

   T& operator+=(const T& aRhs)
   {
      mRefVarNameId.Clear();
      mHasDefaultValue = false;
      mValue           = operator T() + aRhs;
      return mValue;
   }

   T& operator-=(const T& aRhs)
   {
      mRefVarNameId.Clear();
      mHasDefaultValue = false;
      mValue           = operator T() - aRhs;
      return mValue;
   }

   //! Initialize the variable.
   //! @returns 'true' if successful, 'false' if not.
   bool Initialize(const std::string& aName,
                   WsfComponent*      aObjectPtr,
                   WsfScriptContext*  aContextPtr,
                   WsfScriptContext&  aGlobalContext)
   {
      return WsfVariableBase::Initialize(aName, aObjectPtr, aContextPtr, mValue, aGlobalContext);
   }

   //! Read a non-unitary value from an input stream.
   void ReadValue(UtInput& aInput) { return WsfVariableBase::ReadValue(aInput, mValue); }

   //! Read a unitary value from an input stream.
   void ReadValueOfType(UtInput& aInput, UtInput::ValueType aType)
   {
      return WsfVariableBase::ReadValueOfType(aInput, aType, mValue);
   }

   //! Interface to UtInput::ValueInClosedRange.
   //! This checks a static or default value to see if it is valid. Nothing is done for a pure reference.
   void ValueInClosedRange(UtInput& aInput, T aMinValue, T aMaxValue)
   {
      if ((!IsReference()) || HasDefaultValue())
      {
         aInput.ValueInClosedRange(mValue, aMinValue, aMaxValue);
      }
   }

   //! Interface to UtInput::ValueGreater.
   //! This checks a static or default value to see if it is valid. Nothing is done for a pure reference.
   void ValueGreater(UtInput& aInput, T aMinValue)
   {
      if ((!IsReference()) || HasDefaultValue())
      {
         aInput.ValueGreater(mValue, aMinValue);
      }
   }

   //! Interface to UtInput::ValueGreaterOrEqual.
   //! This checks a static or default value to see if it is valid. Nothing is done for a pure reference.
   void ValueGreaterOrEqual(UtInput& aInput, T aMinValue)
   {
      if ((!IsReference()) || HasDefaultValue())
      {
         aInput.ValueGreaterOrEqual(mValue, aMinValue);
      }
   }

   //! Interface to UtInput::ValueLess.
   //! This checks a static or default value to see if it is valid. Nothing is done for a pure reference.
   void ValueLess(UtInput& aInput, T aMaxValue)
   {
      if ((!IsReference()) || HasDefaultValue())
      {
         aInput.ValueLess(mValue, aMaxValue);
      }
   }

   //! Interface to UtInput::ValueLessOrEqual.
   //! This checks a static or default value to see if it is valid. Nothing is done for a pure reference.
   void ValueLessOrEqual(UtInput& aInput, T aMaxValue)
   {
      if ((!IsReference()) || HasDefaultValue())
      {
         aInput.ValueLessOrEqual(mValue, aMaxValue);
      }
   }

private:
   T mValue;
};

// ================================================================================================
// Explicit specialization of WsfVariable for WsfStringId.

template<>
class WsfVariable<WsfStringId> : public WsfVariableBase
{
public:
   WsfVariable() = default;

   WsfVariable(const std::string& aInitialValue)
      : WsfVariableBase()
      , mValue(aInitialValue)
   {
   }

   WsfVariable(WsfStringId aInitialValue)
      : WsfVariableBase()
      , mValue(aInitialValue)
   {
   }

   WsfStringId operator=(const std::string& aValue)
   {
      mValue = aValue;
      return mValue;
   }

   WsfStringId operator=(WsfStringId aValue)
   {
      mValue = aValue;
      return mValue;
   }

   operator WsfStringId() const { return mValue; }

   operator const std::string&() const { return mValue.GetString(); }

   WsfStringId GetId() const { return mValue; }

   const std::string& GetString() const { return mValue.GetString(); }

   WsfStringId GetConstReference() const { return mValue; }

   //! Initialize the variable.
   //! @returns 'true' if successful, 'false' if not.
   bool Initialize(const std::string& aName,
                   WsfComponent*      aObjectPtr,
                   WsfScriptContext*  aContextPtr,
                   WsfScriptContext&  aGlobalContext)
   {
      return WsfVariableBase::Initialize(aName, aObjectPtr, aContextPtr, mValue, aGlobalContext);
   }

   //! Read a value from an input stream.
   void ReadValue(UtInput& aInput) { WsfVariableBase::ReadValue(aInput, mValue); }

private:
   WsfStringId mValue;
};

// ================================================================================================
// A stream inserter is defined for a WsfVariable<std::string> or WsfVariable<WsfStringId> because
// the normal casting operator did not work (it worked for POD's, but not class types).

std::ostream& operator<<(std::ostream& aOut, const WsfVariable<std::string>& aVariable);

std::ostream& operator<<(std::ostream& aOut, const WsfVariable<WsfStringId>& aVariable);

#endif
