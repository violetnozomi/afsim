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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

// ================================================================================================
// This contains the underlying code for a UtTable::Table.
// ================================================================================================

#include "UtTable.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtTable_IntervalTable.hpp"
#include "UtTable_IrregularTable.hpp"
#include "UtTable_RegularTable.hpp"

// ================================================================================================
// Class UtTable::ValueCheck
// ================================================================================================

bool UtTable::ValueCheck::IsValid(double aValue) const
{
   bool isValid = true;
   switch (mRule)
   {
   case cVCR_GT:
      isValid = (aValue > mArg1);
      break;
   case cVCR_GE:
      isValid = (aValue >= mArg1);
      break;
   case cVCR_LT:
      isValid = (aValue < mArg1);
      break;
   case cVCR_LE:
      isValid = (aValue <= mArg1);
      break;
   case cVCR_GT_LT:
      isValid = ((aValue > mArg1) && (aValue < mArg2));
      break;
   case cVCR_GE_LT:
      isValid = ((aValue >= mArg1) && (aValue < mArg2));
      break;
   case cVCR_GT_LE:
      isValid = ((aValue > mArg1) && (aValue <= mArg2));
      break;
   case cVCR_GE_LE:
      isValid = ((aValue >= mArg1) && (aValue <= mArg2));
      break;
   default:
      break;
   }
   return isValid;
}

// ================================================================================================
// Class UtTable::Parameters
// ================================================================================================

// ================================================================================================
void UtTable::Parameters::AddRealParameter(const std::string& aName,
                                           UtInput::ValueType aUnitType,
                                           const ValueCheck&  aValueCheck,
                                           size_t             aDataIndex)
{
   mParameters[aName] = Parameter(cDT_REAL, aUnitType, aValueCheck, aDataIndex);
}

// ================================================================================================
bool UtTable::Parameters::GetTypeAndIndex(const std::string& aName, DataType& aDataType, size_t& aDataIndex) const
{
   bool found = false;
   auto vdi   = mParameters.find(aName);
   if (vdi != mParameters.end())
   {
      found      = true;
      aDataType  = (*vdi).second.mDataType;
      aDataIndex = (*vdi).second.mDataIndex;
   }
   return found;
}

// ================================================================================================
bool UtTable::Parameters::GetAttributes(const std::string&  aName,
                                        DataType&           aDataType,
                                        UtInput::ValueType& aUnitType,
                                        ValueCheck&         aValueCheck,
                                        size_t&             aDataIndex) const
{
   bool found = false;
   auto vdi   = mParameters.find(aName);
   if (vdi != mParameters.end())
   {
      found       = true;
      aDataType   = (*vdi).second.mDataType;
      aUnitType   = (*vdi).second.mUnitType;
      aValueCheck = (*vdi).second.mValueCheck;
      aDataIndex  = (*vdi).second.mDataIndex;
   }
   return found;
}

// ================================================================================================
// Class UtTable::Parameters::Parameter
// ================================================================================================

UtTable::Parameters::Parameter::Parameter(DataType           aDataType,
                                          UtInput::ValueType aUnitType,
                                          const ValueCheck&  aValueCheck,
                                          size_t             aDataIndex)
   : mDataIndex(aDataIndex)
   , mDataType(aDataType)
   , mUnitType(aUnitType)
   , mValueCheck(aValueCheck)
{
}

// ================================================================================================
// The 'Curve' class
// ================================================================================================

// ================================================================================================
UtTable::Curve::Curve(std::unique_ptr<Table> aTablePtr)
   : mTablePtr(std::move(aTablePtr))
   , mIV_Name()
{
   UpdateVariableName();
}

// ================================================================================================
UtTable::Curve::Curve(const Curve& aSrc)
   : mTablePtr(aSrc.mTablePtr)
   , mIV_Name()
{
   UpdateVariableName();
}

// ================================================================================================
UtTable::Curve& UtTable::Curve::operator=(const Curve& aRhs)
{
   if (this != &aRhs)
   {
      mTablePtr = aRhs.mTablePtr;
      UpdateVariableName();
   }
   return *this;
}

// ================================================================================================
UtTable::Curve* UtTable::Curve::Clone() const
{
   return new Curve(*this);
}

// ================================================================================================
double UtTable::Curve::Lookup(double aIV_Value)
{
   return mTablePtr->Lookup(&aIV_Value);
}

// ================================================================================================
void UtTable::Curve::ProcessInput(UtInput&           aInput,
                                  const std::string& aIV_Name,
                                  UtInput::ValueType aIV_UnitType,
                                  const std::string& aIV_Units,
                                  const ValueCheck&  aIV_ValueCheck,
                                  UtInput::ValueType aDV_UnitType,
                                  const std::string& aDV_Units,
                                  const ValueCheck&  aDV_ValueCheck)
{
   mTablePtr =
      RegularTable::LoadCurve(aInput, aIV_Name, aIV_UnitType, aIV_Units, aIV_ValueCheck, aDV_UnitType, aDV_Units, aDV_ValueCheck);
   UpdateVariableName();
}

// ================================================================================================
void UtTable::Curve::ProcessInput(UtInput&           aInput,
                                  UtInput::ValueType aIV_UnitType,
                                  const std::string& aIV_Units,
                                  const ValueCheck&  aIV_ValueCheck,
                                  UtInput::ValueType aDV_UnitType,
                                  const std::string& aDV_Units,
                                  const ValueCheck&  aDV_ValueCheck)
{
   mTablePtr =
      RegularTable::LoadCurve(aInput, "<undefined>", aIV_UnitType, aIV_Units, aIV_ValueCheck, aDV_UnitType, aDV_Units, aDV_ValueCheck);
   UpdateVariableName();
}

// ================================================================================================
bool UtTable::Curve::GetVariable(const std::string& aName, std::vector<double>& aValues) const
{
   bool ok = false;
   if (mTablePtr != nullptr)
   {
      ok = mTablePtr->GetVariable(aName, aValues);
   }
   return ok;
}

// ================================================================================================
void UtTable::Curve::GetVariableNames(std::vector<std::string>& aNames) const
{
   if (mTablePtr != nullptr)
   {
      mTablePtr->GetVariableNames(aNames);
   }
   else
   {
      aNames.clear();
   }
}

// ================================================================================================
bool UtTable::Curve::GetVariableLimits(const std::string& aName, double& aMinValue, double& aMaxValue) const
{
   bool ok(false);
   if (mTablePtr != nullptr)
   {
      ok = mTablePtr->GetVariableLimits(aName, aMinValue, aMaxValue);
   }
   return ok;
}

// ================================================================================================
bool UtTable::Curve::VariableExists(const std::string& aName) const
{
   bool ok(false);
   if (mTablePtr != nullptr)
   {
      ok = mTablePtr->VariableExists(aName);
   }
   return ok;
}

// ================================================================================================
bool UtTable::Curve::GetVariableName(std::string& aName) const
{
   aName = mIV_Name;
   return (mTablePtr != nullptr);
}

// ================================================================================================
bool UtTable::Curve::GetVariableLimits(double& aMinValue, double& aMaxValue) const
{
   bool ok(false);
   if (mTablePtr != nullptr)
   {
      ok = mTablePtr->GetVariableLimits(mIV_Name, aMinValue, aMaxValue);
   }
   return ok;
}

// ================================================================================================
void UtTable::Curve::UpdateVariableName()
{
   mIV_Name.clear();
   if (mTablePtr != nullptr)
   {
      std::vector<std::string> names;
      mTablePtr->GetVariableNames(names);
      if (!names.empty())
      {
         mIV_Name = names[0];
      }
   }
}

// ================================================================================================
// 'Global' methods within the UtTable namespace
// ================================================================================================

// ================================================================================================
//! Create a table object with the definition from an input stream.
//!
//! This form is the most one most typically used. This is typically used when the programmer
//! has determined that the current variable is defined to accept table input. The current
//! command is typically the 'variable name'.  What follows in the input stream MUST be a table
//! definition, or an input error will be throw. The typical use is:
//!
//! \code
//! UtCloneablePtr<UtTable::Table>    mCdTablePtr;
//! UtTable::Parameters               mParameters;
//! ...
//! else if (command == "cd")
//! {
//!    mCdTablePtr = UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL,
//!                                        UtTable::ValueGT(0.0), mParameters);
//! }
//! \endcode
//!
//! @param [input] aInput  The input stream from which to load the table definition.
//! @param [input] aDV_UnitType The type of units for the dependent variable.
//! @param [input] aDV_ValueCheck The numeric limits of dependent variable values.
//! @param [input] aParameters The list of possible variables and their attributes.
//! @returns A pointer to the constructed table. If the next command was not the start of
//! a table definition then an exception will be thrown.
std::unique_ptr<UtTable::Table> UtTable::LoadInstance(UtInput&           aInput,
                                                      UtInput::ValueType aDV_UnitType,
                                                      const ValueCheck&  aDV_ValueCheck,
                                                      const Parameters&  aParameters)
{
   std::unique_ptr<Table> tablePtr = nullptr;
   std::string            command;
   if (aInput.ReadCommand(command))
   {
      if (!ProcessInput(aInput, aDV_UnitType, aDV_ValueCheck, aParameters, tablePtr))
      {
         // The command was not a standard table command.
         // Accept a raw value of the dependent variable type.
         aInput.PushBack(command);
         double value;
         if (aDV_UnitType == UtInput::cNON_DIMENSIONAL)
         {
            aInput.ReadValue(value);
         }
         else
         {
            aInput.ReadValueOfType(value, aDV_UnitType);
         }
         if (!aDV_ValueCheck.IsValid(value))
         {
            throw UtInput::BadValue(aInput);
         }
         tablePtr = ut::make_unique<ConstantTable>(value);
      }
   }
   else
   {
      throw; // Shouldn't happen
   }
   return tablePtr;
}

// ================================================================================================
//! Load a curve.
std::unique_ptr<UtTable::Curve> UtTable::LoadCurve(UtInput&           aInput,
                                                   const std::string& aIV_Name,
                                                   UtInput::ValueType aIV_UnitType,
                                                   const std::string& aIV_Units,
                                                   const ValueCheck&  aIV_ValueCheck,
                                                   UtInput::ValueType aDV_UnitType,
                                                   const std::string& aDV_Units,
                                                   const ValueCheck&  aDV_ValueCheck)
{
   auto tablePtr =
      RegularTable::LoadCurve(aInput, aIV_Name, aIV_UnitType, aIV_Units, aIV_ValueCheck, aDV_UnitType, aDV_Units, aDV_ValueCheck);
   return ut::make_unique<Curve>(std::move(tablePtr));
}

// ================================================================================================
//! Load a curve.
std::unique_ptr<UtTable::Curve> UtTable::LoadCurve(UtInput&           aInput,
                                                   UtInput::ValueType aIV_UnitType,
                                                   const std::string& aIV_Units,
                                                   const ValueCheck&  aIV_ValueCheck,
                                                   UtInput::ValueType aDV_UnitType,
                                                   const std::string& aDV_Units,
                                                   const ValueCheck&  aDV_ValueCheck)
{
   auto tablePtr =
      RegularTable::LoadCurve(aInput, "<undefined>", aIV_UnitType, aIV_Units, aIV_ValueCheck, aDV_UnitType, aDV_Units, aDV_ValueCheck);
   return ut::make_unique<Curve>(std::move(tablePtr));
}

// ================================================================================================
//! Attempt to create a table object with the definition from an input stream.
//!
//! This form is used when it is not known if the current command points at a table. If the current
//! command one that starts a table then a new table object is created and populated from the input
//! stream.
//!
//! @param aInput     The input stream from which to possibly load the table definition.
//! @param aDV_UnitType  The type of units for the dependent variable.
//! @param aDV_ValueCheck The numeric limits of dependent variable values.
//! @param aParameters The list of possible variables and their attributes.
//! @param aTablePtr  [output] The location in which to store the pointer to the created table
//!                            (only set if the return value is true).
//! @returns true if a table was created or false if not. If a table was created then the pointer
//! is returned in aTablePtr.
bool UtTable::ProcessInput(UtInput&                aInput,
                           UtInput::ValueType      aDV_UnitType,
                           const ValueCheck&       aDV_ValueCheck,
                           const Parameters&       aParameters,
                           std::unique_ptr<Table>& aTablePtr)
{
   bool        myCommand = true;
   std::string command;
   aInput.GetCommand(command);
   if (command == "constant")
   {
      double value;
      if (aDV_UnitType == UtInput::cNON_DIMENSIONAL)
      {
         aInput.ReadValue(value);
      }
      else
      {
         aInput.ReadValueOfType(value, aDV_UnitType);
      }
      if (!aDV_ValueCheck.IsValid(value))
      {
         throw UtInput::BadValue(aInput);
      }
      aTablePtr = ut::make_unique<ConstantTable>(value);
   }
   else if (command == "interval_table")
   {
      aTablePtr = IntervalTable::LoadInstance(aInput, aDV_UnitType, aDV_ValueCheck, aParameters);
   }
   else if (command == "irregular_table")
   {
      aTablePtr = IrregularTable::LoadInstance(aInput, aDV_UnitType, aDV_ValueCheck, aParameters);
   }
   else if (command == "regular_table")
   {
      aTablePtr = RegularTable::LoadInstance(aInput, aDV_UnitType, aDV_ValueCheck, aParameters);
   }
   else if (command == "curve")
   {
      aTablePtr = RegularTable::LoadCurve(aInput, aDV_UnitType, aDV_ValueCheck, aParameters);
   }
   else if (command == "file")
   {
      static bool fileCommandActive = false; // Used to prevent possible recursion.

      if (fileCommandActive)
      {
         throw UtInput::BadValue(aInput, "'file' commands cannot be nested");
      }
      fileCommandActive = true;

      std::string fileName;
      aInput.ReadValueQuoted(fileName);
      fileName = aInput.LocateFile(fileName);
      try
      {
         UtInput input;
         input.PushInput(ut::make_unique<UtInputFile>(fileName));
         aInput.DoFileOpenCallback(fileName);
         aTablePtr         = UtTable::LoadInstance(input, aDV_UnitType, aDV_ValueCheck, aParameters);
         fileCommandActive = false;
      }
      catch (UtInputFile::OpenError&)
      {
         throw UtInput::BadValue(aInput, "Unable to open file " + fileName);
      }
   }
   else
   {
      myCommand = false;
   }
   if (myCommand && (aTablePtr != nullptr) && (!aTablePtr->ResolveReferences(aParameters)))
   {
      throw UtInput::BadValue(aInput, "Unexpected error resolving parameters");
   }
   return myCommand;
}

// ================================================================================================
//! A convenience method to resolve parameter references for a table.
//! @params aTablePtr Pointer to a table, which may be zero.
//! @params aParameters The parameters for resolving independent variables.
//! @returns true if successful or if the pointer was 0. false if one or more independent
//! variables could not be resolved.
bool UtTable::ResolveReferences(Table* aTablePtr, const Parameters& aParameters)
{
   return (aTablePtr != nullptr) ? (aTablePtr->ResolveReferences(aParameters)) : true;
}

// ================================================================================================
//! Process query input for testing.
//! This is used as part of the daily test process.
void UtTable::TestQuery(UtInput&           aInput,
                        Table*             aTablePtr,
                        UtInput::ValueType aDV_UnitType,
                        const ValueCheck&  aDV_ValueCheck,
                        const Parameters&  aParameters)
{
   if (aTablePtr == nullptr)
   {
      return;
   }

   UtInputBlock inputBlock(aInput);

   // Initialize references using the supplied parameters
   if (!aTablePtr->ResolveReferences(aParameters))
   {
      throw UtInput::BadValue(aInput, "Unable to resolve parameter references");
   }

   std::vector<std::string> ivNames;
   aTablePtr->GetVariableNames(ivNames);
   std::vector<double> arguments(1);

   bool        haveExpectedValue = false;
   double      expectedValue     = 0.0;
   std::string testName;

   std::string command;
   while (inputBlock.ReadCommand(command))
   {
      if (command == "variable")
      {
         std::string ivName;
         aInput.ReadValue(ivName);

         DataType           dataType;
         UtInput::ValueType unitType;
         ValueCheck         valueCheck;
         size_t             dataIndex;
         if (!aParameters.GetAttributes(ivName, dataType, unitType, valueCheck, dataIndex))
         {
            throw UtInput::BadValue(aInput, "Unknown variable name: " + ivName);
         }

         double value;
         if (unitType == UtInput::cNON_DIMENSIONAL)
         {
            aInput.ReadValue(value);
         }
         else
         {
            aInput.ReadValueOfType(value, unitType);
         }

         if (!valueCheck.IsValid(value))
         {
            throw UtInput::BadValue(aInput);
         }

         // Add the value to the arguments vector

         if (arguments.size() < (dataIndex + 1))
         {
            arguments.resize(dataIndex + 1);
         }
         arguments[dataIndex] = value;

         // If the variable exists in the list of independent variables in the table, remove it from
         // the list to indicate it has been processed.

         auto nli = std::find(ivNames.begin(), ivNames.end(), ivName);
         if (nli != ivNames.end())
         {
            ivNames.erase(nli);
         }
      }
      else if (command == "expect")
      {
         haveExpectedValue = true;
         if (aDV_UnitType == UtInput::cNON_DIMENSIONAL)
         {
            aInput.ReadValue(expectedValue);
         }
         else
         {
            aInput.ReadValueOfType(expectedValue, aDV_UnitType);
         }
         if (!aDV_ValueCheck.IsValid(expectedValue))
         {
            throw UtInput::BadValue(aInput);
         }
      }
      else if (command == "name")
      {
         aInput.ReadValueQuoted(testName);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   // Issue a warning if values not provided for all table variables.
   if (!ivNames.empty())
   {
      auto out = ut::log::warning() << "No definition for independent variables:";
      for (auto& ivName : ivNames)
      {
         out.AddNote() << ivName;
      }
   }

   double value = aTablePtr->Lookup(&arguments[0]);
   if (haveExpectedValue)
   {
      // Immediately-invoked lambda
      auto out = [](bool aFailed) -> ut::log::MessageStream
      {
         if (aFailed)
         {
            return ut::log::info("test_fail") << "-FAIL- Actual value is not the same as expected value.";
         }
         return ut::log::info() << "-PASS- Actual value matches expected value.";
      }(fabs(value - expectedValue) > (1.0E-5 * expectedValue));

      if (!testName.empty())
      {
         out.AddNote() << "Test Name: " << testName;
      }
      out.AddNote() << "Actual Value: " << value;
      out.AddNote() << "Expected Value: " << expectedValue;
   }
   else
   {
      auto out = ut::log::info() << "Value does not have an expected value.";
      if (!testName.empty())
      {
         out.AddNote() << "Test Name: " << testName;
      }
      out.AddNote() << "Actual Value: " << value;
   }
}

// ================================================================================================
//! Read and check the indicator that indicates if a variable is a double or a float.
void UtTable::ReadPrecision(UtInput& aInput, bool& aIsDouble)
{
   std::string str;
   aInput.ReadValue(str);
   if (str == "double")
   {
      aIsDouble = true;
   }
   else if (str == "float")
   {
      aIsDouble = false;
   }
   else
   {
      throw UtInput::BadValue(aInput);
   }
}

// ================================================================================================
//! Read and the value for a 'units' keyword.
void UtTable::ReadUnits(UtInput& aInput, UtInput::ValueType aUnitType, std::string& aUnits)
{
   if (aUnitType == UtInput::cNON_DIMENSIONAL)
   {
      throw UtInput::BadValue(aInput, "The 'units' command is invalid for a non-dimensional value");
   }
   aInput.ReadValue(aUnits);
   if (!UtInput::ValidateUnits(aUnits, aUnitType))
   {
      throw UtInput::BadValue(aInput);
   }
}

// ================================================================================================
double UtTable::ReadValue(UtInput& aInput, UtInput::ValueType aUnitType, const std::string& aUnits, const ValueCheck& aValueCheck)
{
   double value;
   if (aUnitType == UtInput::cNON_DIMENSIONAL)
   {
      // Non-dimensional value
      aInput.ReadValue(value);
   }
   else if (aUnits.empty())
   {
      // Dimensional value, but units were not provided in the argument list.
      // The user must supply the units with the value.
      aInput.ReadValueOfType(value, aUnitType);
   }
   else
   {
      // Dimensional value and units were provided in the argument list.
      // A raw value is read and is converted according to the value in the argument list.
      aInput.ReadValue(value);
      value = aInput.ConvertValue(value, aUnits, aUnitType);
   }

   // Verify the value is within limits.
   if (!aValueCheck.IsValid(value))
   {
      throw UtInput::BadValue(aInput);
   }
   return value;
}
