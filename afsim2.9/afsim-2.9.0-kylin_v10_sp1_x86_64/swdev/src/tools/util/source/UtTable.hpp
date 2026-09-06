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

#ifndef UTTABLE_HPP
#define UTTABLE_HPP

#include "ut_export.h"

#include <map>
#include <string>
#include <vector>

#include "UtCloneablePtr.hpp"
#include "UtInput.hpp"

//! A template class that defines an 'table'.
//!
//! A templated class that defines a function using a table as follows:
//!
//!   y = c
//!   y = f(x1)
//!   y = f(x1,x2);
//!   y = f(x1,x2,x3);
//!   y = f(x1,x2,x3,x4);
//!
//! This code attempts to make it as simple as possible for programs to utilize tables while still
//! providing good performance. An instance of a table variable can be copy-constructed or assigned
//! without fear of copying large amounts of data. (All references to common data are maintained
//! with shared pointers.

namespace UtTable
{
class UT_EXPORT Parameters;
class UT_EXPORT Table;
class UT_EXPORT ConstantTable;
class UT_EXPORT Curve;

//! Limit rule for the class ValueCheck.
enum ValueCheckRule
{
   cVCR_NONE,  //!< no limit
   cVCR_GT,    //!< value >  limit1
   cVCR_GE,    //!< value >= limit1
   cVCR_LT,    //!< value <  limit1
   cVCR_LE,    //!< value <= limit1
   cVCR_GT_LT, //!< limit1 <  value <  limit2
   cVCR_GE_LT, //!< limit1 <= value <  limit2
   cVCR_GT_LE, //!< limit1 <  value <= limit2
   cVCR_GE_LE  //!< limit1 <= value <= limit2
};

//! A class used to define limits on independent and dependent values.
class UT_EXPORT ValueCheck
{
public:
   ValueCheck(ValueCheckRule aRule = cVCR_NONE, double aArg1 = 0.0, double aArg2 = 0.0)
      : mArg1(aArg1)
      , mArg2(aArg2)
      , mRule(aRule)
   {
   }

   bool IsValid(double aValue) const;

private:
   double         mArg1;
   double         mArg2;
   ValueCheckRule mRule;
};

//! @name Convenience functions for creating a ValueCheck object. (cuts down on typing).
//@{
inline UT_EXPORT ValueCheck NoCheck()
{
   return ValueCheck();
}
inline UT_EXPORT ValueCheck ValueGT(double aLimit1)
{
   return ValueCheck(cVCR_GT, aLimit1);
}
inline UT_EXPORT ValueCheck ValueGE(double aLimit1)
{
   return ValueCheck(cVCR_GE, aLimit1);
}
inline UT_EXPORT ValueCheck ValueLT(double aLimit1)
{
   return ValueCheck(cVCR_LT, aLimit1);
}
inline UT_EXPORT ValueCheck ValueLE(double aLimit1)
{
   return ValueCheck(cVCR_LE, aLimit1);
}
inline UT_EXPORT ValueCheck ValueGT_LT(double aLimit1, double aLimit2)
{
   return ValueCheck(cVCR_GT_LT, aLimit1, aLimit2);
}
inline UT_EXPORT ValueCheck ValueGE_LT(double aLimit1, double aLimit2)
{
   return ValueCheck(cVCR_GE_LT, aLimit1, aLimit2);
}
inline UT_EXPORT ValueCheck ValueGT_LE(double aLimit1, double aLimit2)
{
   return ValueCheck(cVCR_GT_LE, aLimit1, aLimit2);
}
inline UT_EXPORT ValueCheck ValueGE_LE(double aLimit1, double aLimit2)
{
   return ValueCheck(cVCR_GE_LE, aLimit1, aLimit2);
}
//@}

//! The data types of arguments.
enum DataType
{
   cDT_UNKNOWN, //!< An argument of an unknown data type
   cDT_REAL     //!< A argument of type 'double' or float
};

UT_EXPORT std::unique_ptr<Table> LoadInstance(UtInput&           aInput,
                                              UtInput::ValueType aDV_UnitType,
                                              const ValueCheck&  aDV_ValueCheck,
                                              const Parameters&  aParameters);

// New preferred form which includes a name for the independent variable.
UT_EXPORT std::unique_ptr<Curve> LoadCurve(UtInput&           aInput,
                                           const std::string& aIV_Name,
                                           UtInput::ValueType aIV_UnitType,
                                           const std::string& aIV_Units,
                                           const ValueCheck&  aIV_ValueCheck,
                                           UtInput::ValueType aDV_UnitType,
                                           const std::string& aDV_Units,
                                           const ValueCheck&  aDV_ValueCheck);

// Original form that did not include a name for the independent variable.
UT_EXPORT std::unique_ptr<Curve> LoadCurve(UtInput&           aInput,
                                           UtInput::ValueType aIV_UnitType,
                                           const std::string& aIV_Units,
                                           const ValueCheck&  aIV_ValueCheck,
                                           UtInput::ValueType aDV_UnitType,
                                           const std::string& aDV_Units,
                                           const ValueCheck&  aDV_ValueCheck);

UT_EXPORT bool ProcessInput(UtInput&                aInput,
                            UtInput::ValueType      aDV_UnitType,
                            const ValueCheck&       aDV_ValueCheck,
                            const Parameters&       aParameters,
                            std::unique_ptr<Table>& aTablePtr);

UT_EXPORT bool ResolveReferences(Table* aTablePtr, const Parameters& aParameters);

UT_EXPORT void TestQuery(UtInput&           aInput,
                         Table*             aTablePtr,
                         UtInput::ValueType aDV_UnitType,
                         const ValueCheck&  aDV_ValueCheck,
                         const Parameters&  aParameters);

UT_EXPORT void ReadPrecision(UtInput& aInput, bool& aIsDouble);

//! @name The remainder of the methods are used internally.
//@{
void ReadUnits(UtInput& aInput, UtInput::ValueType aUnitType, std::string& aUnits);

double ReadValue(UtInput& aInput, UtInput::ValueType aUnitType, const std::string& aUnits, const ValueCheck& aValueCheck);
//@}
} // namespace UtTable

// ================================================================================================
//! The abstract base class for the table implementations.
class UtTable::Table
{
public:
   Table()          = default;
   virtual ~Table() = default;

   virtual Table* Clone() const = 0;

   virtual bool ResolveReferences(const Parameters& aParameters) = 0;

   //! Perform a table lookup.
   //! @param aArguments An array of type 'double' containing the independent values for which the function value is desired.
   //! @returns The function value for the specified independent values.
   virtual double Lookup(const double* aArguments) const = 0;

   //! @name Variable information methods.
   //! The methods allow one to determine more detailed information about the table, without exposing too
   //! much of the underlying framework.) Not that some methods return boolean values that indicate if
   //! a variable is not found or if the table does not support the method (at this time the irregular
   //! and interval tables do not support the limit or value retrieval methods.)
   //@{

   //! Returns 'true' if the table is a 'constant table' - i.e. one that has no independent variables.
   //! Only tables of type UtTable::ConstantTable should return true.
   virtual bool IsConstant() const { return false; }

   //! Returns true if the table is a function of the specified independent variable.
   virtual bool VariableExists(const std::string& aName) const = 0;

   //! Returns the list of independent variables which the table is dependent upon.
   virtual void GetVariableNames(std::vector<std::string>& aNames) const = 0;

   //! Returns the limits of the specified variable.
   //! @param aName    One of the independent variable names or "" to select the dependent variable.
   //! @param aMinValue The minimum value of the variable.
   //! @param aMaxValue The maximum value of the variable.
   //! @returns 'true' if the output values are valid. 'false' if the variable does not exist or the
   //! table type does not support the function.
   virtual bool GetVariableLimits(const std::string& aName, double& aMinValue, double& aMaxValue) const = 0;

   //! Return the values for the specified variable.
   //! @param aName    One of the independent variable names or "" to select the dependent variable.
   //! @param aValues  The requested values.
   //! @returns 'true' if the output values are valid. 'false' if the variable does not exist or the
   //! table type does not support the function.
   //! @note This method can return a lot of data if the table is large.
   virtual bool GetVariable(const std::string& aName, std::vector<double>& aValues) const = 0;
   //@}
};

// ================================================================================================
//! A table that is a constant value (i.e.: y = c.)
class UtTable::ConstantTable : public UtTable::Table
{
public:
   ConstantTable(double aValue = 0.0)
      : mValue(aValue)
   {
   }

   Table* Clone() const override { return new ConstantTable(*this); }

   bool ResolveReferences(const Parameters& /*aParameters*/) override { return true; }

   double Lookup(const double* /*aArguments*/) const override { return mValue; }

   bool IsConstant() const override { return true; }

   bool VariableExists(const std::string& /*aName*/) const override { return false; }

   void GetVariableNames(std::vector<std::string>& aNames) const override { aNames.clear(); }

   bool GetVariableLimits(const std::string& aName, double& aMinValue, double& aMaxValue) const override
   {
      bool ok(false);
      if (aName.empty())
      {
         ok        = true;
         aMinValue = aMaxValue = mValue;
      }
      return ok;
   }

   bool GetVariable(const std::string& aName, std::vector<double>& aValues) const override
   {
      bool ok(false);
      if (aName.empty())
      {
         ok = true;
         aValues.resize(1);
         aValues[0] = mValue;
      }
      return ok;
   }

private:
   double mValue;
};

// ================================================================================================
//! Convenience class to represent a curve.
//!
//! The 'Curve' class is a special type of 1D table where the calling application KNOWS the attributes
//! of the independent and dependent variables. This allows the programmer to use a simple API that
//! doesn't require the use of 'Parameters' and allows the application user to specify points as
//! x/y pairs.
class UtTable::Curve
{
public:
   Curve() = default;
   Curve(std::unique_ptr<Table> aTablePtr);
   Curve(const Curve& aSrc);
   virtual ~Curve() = default;

   Curve& operator=(const Curve& aRhs);

   Curve* Clone() const;
   double Lookup(double aIV_Value);

   // New preferred form which includes a name for the independent variable.
   void ProcessInput(UtInput&           aInput,
                     const std::string& aIV_Name,
                     UtInput::ValueType aIV_UnitType,
                     const std::string& aIV_Units,
                     const ValueCheck&  aIV_ValueCheck,
                     UtInput::ValueType aDV_UnitType,
                     const std::string& aDV_Units,
                     const ValueCheck&  aDV_ValueCheck);

   // Original form that did not include a name for the independent variable.
   void ProcessInput(UtInput&           aInput,
                     UtInput::ValueType aIV_UnitType,
                     const std::string& aIV_Units,
                     const ValueCheck&  aIV_ValueCheck,
                     UtInput::ValueType aDV_UnitType,
                     const std::string& aDV_Units,
                     const ValueCheck&  aDV_ValueCheck);

   // Same form as the Table class
   bool VariableExists(const std::string& aName) const;

   // Same form as the table class
   void GetVariableNames(std::vector<std::string>& aNames) const;

   // Same form as the Table class
   bool GetVariableLimits(const std::string& aName, double& aMinValue, double& aMaxValue) const;

   // Same form as the table class
   bool GetVariable(const std::string& aName, std::vector<double>& aValues) const;

   // Returns only the single independent variable name
   bool GetVariableName(std::string& aName) const;

   // Returns only the limits of the independent variable
   bool GetVariableLimits(double& aMinValue, double& aMaxValue) const;

   //! Returns true if a curve has been defined.
   bool IsDefined() const { return (mTablePtr != nullptr); }

private:
   void UpdateVariableName();

   UtCloneablePtr<Table> mTablePtr{nullptr};
   std::string           mIV_Name;
};

// ================================================================================================
//! A class used to define the independent variable arguments used for performing table lookups.
class UtTable::Parameters
{
public:
   void AddRealParameter(const std::string& aName,
                         UtInput::ValueType aUnitType,
                         const ValueCheck&  aValueCheck,
                         size_t             aDataIndex);

   bool GetTypeAndIndex(const std::string& aName, DataType& aDataType, size_t& aDataIndex) const;

   bool GetAttributes(const std::string&  aName,
                      DataType&           aDataType,
                      UtInput::ValueType& aUnitType,
                      ValueCheck&         aValueCheck,
                      size_t&             aDataIndex) const;

   bool   Empty() const { return mParameters.empty(); }
   size_t Size() const { return mParameters.size(); }
   void   Clear() { mParameters.clear(); }

private:
   class Parameter
   {
   public:
      Parameter() = default;
      Parameter(DataType aDataType, UtInput::ValueType aUnitType, const ValueCheck& aValueCheck, size_t aDataIndex);

      size_t             mDataIndex{0};
      DataType           mDataType{cDT_UNKNOWN};
      UtInput::ValueType mUnitType{UtInput::cNON_DIMENSIONAL};
      ValueCheck         mValueCheck;
   };

   std::map<std::string, Parameter> mParameters;
};

#endif
