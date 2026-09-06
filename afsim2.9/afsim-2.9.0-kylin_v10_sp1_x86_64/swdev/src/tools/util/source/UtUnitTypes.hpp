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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTUNITTYPES_HPP
#define UTUNITTYPES_HPP

#include <istream>
#include <map>
#include <ostream>

#include "UtMath.hpp"
#include "UtStringUtil.hpp"
#include "UtUnits.hpp"

/////////////////////////////////////////////////////////////////////////
// This file provides a number of unit-related classes.
// The approach taken is to provide an integer-value representing
// each unit, such as 'meters' = 1, 'feet' = 2 .
// This allows unit information to be stored as a single integer.
// Compound units can be made by combining two other units, packing
// the unit information into an integer as follows:
//   subject_bits  predicate_bits  other_bits
// For instance, UtUnitSpeed would have
//   (length_unit << 4) | (time_unit << 0)
// ----------------------------------------------------
// Some code in this file is generated with a script (UtUnits.py)
// Please update UtUnits.py if a change to script generated code is required.
//
// Note: this file is separated from UtUnits.hpp only to reduce compilation time
/////////////////////////////////////////////////////////////////////////

// Contains functions shared by all unit classes
namespace UtUnitsDetail
{
bool UT_EXPORT SplitUnit(const std::string& aUnits, std::string& aSubject, std::string& aPredicate, char aOperator);

//! Contains shared functions, parameterized by the type of unit being represented
//! @note These definitions are used by "Simple" units like length
//! The "Compound" and "Multi-dimensional" units use these as fallbacks
template<typename UNIT_TYPE, bool IS_COMPOUND, bool IS_MULTIDIM>
struct UnitFunctionsT
{
   //! Convert a value in a non-standard unit to the standard unit
   //! @param aValue is the value in the non-standard unit
   //! @param aUnit is the enumeration of the non-standard unit
   //! @return the value in the standard unit
   static inline double ConvertToStandard(double aValue, int aUnit)
   {
      return aValue * UNIT_TYPE::mBaseUnitMultiplier[aUnit];
   }

   //! Convert a value in a standard unit to the non-standard unit
   //! @param aValue is the value in the standard unit
   //! @param aUnit is the enumeration of the non-standard unit
   //! @return the value in the non-standard unit
   static double ConvertFromStandard(double aValue, int aUnit)
   {
      return aValue / UNIT_TYPE::mBaseUnitMultiplier[aUnit];
   }

   //! Find the unit name corresponding to the unit enumeration
   //! @param aUnit is the enumeration of the unit
   //! @return the name of the unit
   static std::string FindUnitName(int aUnit)
   {
      auto i = UNIT_TYPE::mUnitToString.find(aUnit);
      if (i != UNIT_TYPE::mUnitToString.end())
      {
         return i->second;
      }
      return std::string();
   }

   //! Find the unit enumeration corresponding to the unit name
   //! @param aUnit is the name of the unit
   //! @return the enumeration of the unit
   static int ReadUnit(const std::string& aName)
   {
      auto i = UNIT_TYPE::mUnitStrings.find(aName);
      if (i != UNIT_TYPE::mUnitStrings.end())
      {
         return i->second;
      }
      return -1;
   }

   //! Determines whether the unit enumeration is valid
   //! @param aUnit is the enumeration of the unit
   //! @return whether or not the unit enumeration is valid
   static bool IsUnitValid(int aUnit) { return !FindUnitName(aUnit).empty(); }

   //! Determines whether the unit name is valid
   //! @param aUnit is the name of the unit
   //! @return whether or not the unit name is valid
   static bool IsUnitValid(const std::string& aName) { return ReadUnit(aName) != -1; }
};

//! Unit functions for "Compound" unit types like speed (length/time)
template<typename UNIT_TYPE>
struct UnitFunctionsT<UNIT_TYPE, true, false>
{
   static_assert(UNIT_TYPE::cCOMPOUND_OPERATOR == '/' || UNIT_TYPE::cCOMPOUND_OPERATOR == '*',
                 "Compound operator must be '/' or '*'");

   //! A compound unit is constructed from a 'subject' and a 'predicate'
   //@{
   using Subject   = typename UNIT_TYPE::SubjectType;
   using Predicate = typename UNIT_TYPE::PredicateType;
   //@}

   //! number of least significant bits before the compound unit information.
   static const int cSHIFT = UNIT_TYPE::cUSED_BITS - (Subject::cUSED_BITS + Predicate::cUSED_BITS);
   //! this is where non-compound unit type bits are stored.
   static const int cSIMPLE_MASK = ~(0xffffffff << cSHIFT);

   //! Get the subject name unit from the compound unit enumeration
   //! @param aCompoundUnit is the compound unit enumeration
   //! @return the subject unit name
   static std::string GetSubjectName(int aCompoundUnit)
   {
      const int SUBJECT_MASK = (1 << Subject::cUSED_BITS) - 1;
      const int BIT_SHIFT    = UNIT_TYPE::cUSED_BITS - Subject::cUSED_BITS;
      const int UNIT_MASK    = (1 << UNIT_TYPE::cUSED_BITS) - 1;
      if ((aCompoundUnit & UNIT_MASK) != aCompoundUnit)
      {
         return std::string();
      }
      else
      {
         return Subject::FindUnitName((aCompoundUnit >> BIT_SHIFT) & SUBJECT_MASK);
      }
   }

   //! Get the subject unit name from the compound unit name
   //! @param aCompoundUnit is the compound unit name
   //! @return the subject unit name
   static std::string GetSubjectName(const std::string& aCompoundName)
   {
      std::string subjectName;
      std::string predicateName;
      SplitUnit(aCompoundName, subjectName, predicateName, UNIT_TYPE::cCOMPOUND_OPERATOR);
      return subjectName;
   }

   //! Get the subject unit enumeration from the compound unit enumeration
   //! @param aCompoundUnit is the compound unit enumeration
   //! @return the subject unit enumeration
   static int GetSubjectUnit(int aCompoundUnit)
   {
      const int SUBJECT_MASK = (1 << Subject::cUSED_BITS) - 1;
      const int BIT_SHIFT    = UNIT_TYPE::cUSED_BITS - Subject::cUSED_BITS;
      const int UNIT_MASK    = (1 << UNIT_TYPE::cUSED_BITS) - 1;
      if ((aCompoundUnit & UNIT_MASK) != aCompoundUnit)
      {
         return -1;
      }
      else
      {
         return (aCompoundUnit >> BIT_SHIFT) & SUBJECT_MASK;
      }
   }

   //! Get the subject unit enumeration from the compound unit name
   //! @param aCompoundUnit is the compound unit name
   //! @return the subject unit enumeration
   static int GetSubjectUnit(const std::string& aCompoundName)
   {
      const std::string subjectName = GetSubjectName(aCompoundName);
      return Subject::ReadUnit(subjectName);
   }

   //! Get the predicate name unit from the compound unit enumeration
   //! @param aCompoundUnit is the compound unit enumeration
   //! @return the predicate unit name
   static std::string GetPredicateName(int aCompoundUnit)
   {
      const int PREDICATE_MASK = (1 << Predicate::cUSED_BITS) - 1;
      const int BIT_SHIFT      = UNIT_TYPE::cUSED_BITS - Subject::cUSED_BITS - Predicate::cUSED_BITS;
      const int UNIT_MASK      = (1 << UNIT_TYPE::cUSED_BITS) - 1;
      if ((aCompoundUnit & UNIT_MASK) != aCompoundUnit)
      {
         return std::string();
      }
      else
      {
         return Predicate::FindUnitName((aCompoundUnit >> BIT_SHIFT) & PREDICATE_MASK);
      }
   }

   //! Get the predicate unit name from the compound unit name
   //! @param aCompoundUnit is the compound unit name
   //! @return the predicate unit name
   static std::string GetPredicateName(const std::string& aCompoundName)
   {
      std::string subjectName;
      std::string predicateName;
      SplitUnit(aCompoundName, subjectName, predicateName, UNIT_TYPE::cCOMPOUND_OPERATOR);
      return predicateName;
   }

   //! Get the predicate unit enumeration from the compound unit enumeration
   //! @param aCompoundUnit is the compound unit enumeration
   //! @return the predicate unit enumeration
   static int GetPredicateUnit(int aCompoundUnit)
   {
      const int PREDICATE_MASK = (1 << Predicate::cUSED_BITS) - 1;
      const int BIT_SHIFT      = UNIT_TYPE::cUSED_BITS - Subject::cUSED_BITS - Predicate::cUSED_BITS;
      const int UNIT_MASK      = (1 << UNIT_TYPE::cUSED_BITS) - 1;
      if ((aCompoundUnit & UNIT_MASK) != aCompoundUnit)
      {
         return -1;
      }
      else
      {
         return (aCompoundUnit >> BIT_SHIFT) & PREDICATE_MASK;
      }
   }

   //! Get the predicate unit enumeration from the compound unit name
   //! @param aCompoundUnit is the compound unit name
   //! @return the predicate unit enumeration
   static int GetPredicateUnit(const std::string& aCompoundName)
   {
      const std::string predicateName = GetPredicateName(aCompoundName);
      return Predicate::ReadUnit(predicateName);
   }

   //! Combine the subject and predicate unit enumerations into the compound unit enumeration
   //! @param aSubjectUnit is the subject unit enumeration
   //! @param aPredicateUnit is the predicate unit enumeration
   //! @return the compound unit enumeration
   static int FromSubjectPredicate(int aSubjectUnit, int aPredicateUnit)
   {
      if (aSubjectUnit != -1 && aPredicateUnit != -1)
      {
         return (aSubjectUnit << (cSHIFT + Predicate::cUSED_BITS)) | (aPredicateUnit << (cSHIFT));
      }
      else
      {
         return -1;
      }
   }

   //! Combine the subject and predicate unit names into the compound unit name
   //! @param aSubjectUnit is the subject unit name
   //! @param aPredicateUnit is the predicate unit name
   //! @return the compound unit name
   static std::string FromSubjectPredicate(const std::string& aSubjectName, const std::string& aPredicateName)
   {
      if (!aSubjectName.empty() && !aPredicateName.empty())
      {
         return aSubjectName + UNIT_TYPE::cCOMPOUND_OPERATOR + aPredicateName;
      }
      else
      {
         return std::string();
      }
   }

   //! Specialization of ConvertToStandard for "Compound" units
   static double ConvertToStandard(double aValue, int aCompoundUnit)
   {
      if ((aCompoundUnit & cSIMPLE_MASK) == 0)
      {
         const int subjectUnit   = GetSubjectUnit(aCompoundUnit);
         const int predicateUnit = GetPredicateUnit(aCompoundUnit);
         double    subjStd       = Subject::ConvertToStandard(aValue, subjectUnit);
         double    predStd       = Predicate::ConvertToStandard(1.0, predicateUnit);
         if (UNIT_TYPE::cCOMPOUND_OPERATOR == '/')
         {
            return subjStd / predStd;
         }
         else
         {
            return subjStd * predStd;
         }
      }
      else
      {
         return UnitFunctionsT<UNIT_TYPE, false, false>::ConvertToStandard(aValue, aCompoundUnit);
      }
   }

   //! Specialization of ConvertFromStandard for "Compound" units
   static double ConvertFromStandard(double aValue, int aCompoundUnit)
   {
      if ((aCompoundUnit & cSIMPLE_MASK) == 0)
      {
         const int subjectUnit   = GetSubjectUnit(aCompoundUnit);
         const int predicateUnit = GetPredicateUnit(aCompoundUnit);
         double    subjStd       = Subject::ConvertFromStandard(aValue, subjectUnit);
         double    predStd       = Predicate::ConvertFromStandard(1.0, predicateUnit);
         if (UNIT_TYPE::cCOMPOUND_OPERATOR == '/')
         {
            return subjStd / predStd;
         }
         else
         {
            return subjStd * predStd;
         }
      }
      else
      {
         return UnitFunctionsT<UNIT_TYPE, false, false>::ConvertFromStandard(aValue, aCompoundUnit);
      }
   }

   //! Specialization of FindUnitName for "Compound" units
   static std::string FindUnitName(int aCompoundUnit)
   {
      if ((aCompoundUnit & cSIMPLE_MASK) == 0)
      {
         // Prefer pre-defined units like 'mph' to the compound representation 'miles/hour'
         std::string unitName = UnitFunctionsT<UNIT_TYPE, false, false>::FindUnitName(aCompoundUnit);
         if (unitName.empty())
         {
            // This represents a compound unit, construct the name now
            const std::string subjectName   = GetSubjectName(aCompoundUnit);
            const std::string predicateName = GetPredicateName(aCompoundUnit);
            unitName                        = FromSubjectPredicate(subjectName, predicateName);
         }
         return unitName;
      }
      else
      {
         // This is a singular unit
         return UnitFunctionsT<UNIT_TYPE, false, false>::FindUnitName(aCompoundUnit);
      }
   }

   //! Specialization of ReadUnit for "Compound" units
   static int ReadUnit(const std::string& aCompoundName)
   {
      if (cSHIFT != 0)
      {
         auto i = UNIT_TYPE::mUnitStrings.find(aCompoundName);
         if (i != UNIT_TYPE::mUnitStrings.end())
         {
            return i->second;
         }
      }
      std::string subjectName;
      std::string predicateName;
      if (SplitUnit(aCompoundName, subjectName, predicateName, UNIT_TYPE::cCOMPOUND_OPERATOR))
      {
         int subjectUnit   = Subject::ReadUnit(subjectName);
         int predicateUnit = Predicate::ReadUnit(predicateName);
         if ((subjectUnit | predicateUnit) != -1)
         {
            return FromSubjectPredicate(subjectUnit, predicateUnit);
         }
      }
      return -1;
   }

   //! Specialization of IsUnitValid for "Compound" units
   static bool IsUnitValid(int aCompoundUnit)
   {
      if ((aCompoundUnit & cSIMPLE_MASK) == 0)
      {
         // Prefer pre-defined units like 'mph' to the compound representation 'miles/hour'
         bool valid = UnitFunctionsT<UNIT_TYPE, false, false>::IsUnitValid(aCompoundUnit);
         if (!valid)
         {
            int subjectUnit   = GetSubjectUnit(aCompoundUnit);
            int predicateUnit = GetPredicateUnit(aCompoundUnit);
            return Subject::IsUnitValid(subjectUnit) && Predicate::IsUnitValid(predicateUnit);
         }
         return true;
      }
      else
      {
         // This is a singular unit
         return UnitFunctionsT<UNIT_TYPE, false, false>::IsUnitValid(aCompoundUnit);
      }
   }

   //! Specialization of IsUnitValid for "Compound" units
   static bool IsUnitValid(const std::string& aCompoundName)
   {
      if (cSHIFT != 0)
      {
         // Prefer pre-defined units like 'mph' to the compound representation 'miles/hour'
         bool valid = UnitFunctionsT<UNIT_TYPE, false, false>::IsUnitValid(aCompoundName);
         if (!valid)
         {
            const std::string subjectName   = GetSubjectName(aCompoundName);
            const std::string predicateName = GetPredicateName(aCompoundName);
            return Subject::IsUnitValid(subjectName) && Predicate::IsUnitValid(predicateName);
         }
         return true;
      }
      else
      {
         // This is a singular unit
         return UnitFunctionsT<UNIT_TYPE, false, false>::IsUnitValid(aCompoundName);
      }
   }
};

//! Unit functions for "Multi-dimensional" unit types like area (length^2)
//! @note at the moment, we don't handle things like 'liters', only 'cm^3'
template<typename UNIT_TYPE>
struct UnitFunctionsT<UNIT_TYPE, false, true>
{
   //! a multi-dimensional unit is a base unit and an exponent
   using BaseUnit = typename UNIT_TYPE::OneDimensionType;

   //! Specialization of ConvertToStandard for "Multi-dimensional" units
   static double ConvertToStandard(double aValue, int aMultiDimUnit)
   {
      int    baseUnit = GetBaseUnit(aMultiDimUnit);
      double v        = pow(aValue, 1.0 / UNIT_TYPE::cDIM);
      double v2       = BaseUnit::ConvertToStandard(v, baseUnit);
      return pow(v2, UNIT_TYPE::cDIM);
   }

   //! Specialization of ConvertFromStandard for "Multi-dimensional" units
   static double ConvertFromStandard(double aValue, int aMultiDimUnit)
   {
      int    baseUnit = GetBaseUnit(aMultiDimUnit);
      double v        = pow(aValue, 1.0 / UNIT_TYPE::cDIM);
      double v2       = BaseUnit::ConvertFromStandard(v, baseUnit);
      return pow(v2, UNIT_TYPE::cDIM);
   }

   //! Get the base unit enumeration from the multi-dimensional unit enumeration
   //! @param aMultiDimUnit is the multi-dimensional unit enumeration
   //! @return the base unit enumeration
   static int GetBaseUnit(int aMultiDimUnit)
   {
      const std::string multiDimName = FindUnitName(aMultiDimUnit);
      return ReadBaseUnitP(multiDimName);
   }

   //! Specialization of FindUnitName for "Multi-dimensional" units
   static std::string FindUnitName(int aMultiDimUnit)
   {
      auto i = UNIT_TYPE::mUnitToString.find(aMultiDimUnit);
      if (i != UNIT_TYPE::mUnitToString.end())
      {
         return i->second;
      }
      std::string unitName = UnitFunctionsT<UNIT_TYPE, false, false>::FindUnitName(aMultiDimUnit);
      if (unitName.length() == 0)
      {
         unitName = BaseUnit::FindUnitName(aMultiDimUnit);
         if (unitName.length() != 0)
         {
            unitName += '^';
            unitName += '0' + UNIT_TYPE::cDIM;
         }
      }
      return unitName;
   }

   //! Specialization of ReadUnit for "Multi-dimensional" units
   static int ReadUnit(const std::string& aMultiDimName)
   {
      if (aMultiDimName.empty())
      {
         return -1;
      }
      auto i = UNIT_TYPE::mUnitStrings.find(aMultiDimName);
      if (i != UNIT_TYPE::mUnitStrings.end())
      {
         return i->second;
      }
      return ReadBaseUnitP(aMultiDimName);
   }

   //! Specialization of IsUnitValid for "Multi-dimensional" units
   static bool IsUnitValid(int aMultiDimUnit) { return !FindUnitName(aMultiDimUnit).empty(); }

   //! Specialization of IsUnitValid for "Multi-dimensional" units
   static bool IsUnitValid(const std::string& aMultiDimName) { return ReadUnit(aMultiDimName) != -1; }

protected:
   //! Find the base unit enumeration from the multi-dimensional unit name
   //! @param aMultiDimUnit is the multi-dimensional unit name
   //! @return the base unit enumeration
   static int ReadBaseUnitP(const std::string& aMultiDimName)
   {
      if (aMultiDimName[aMultiDimName.length() - 1] == '0' + UNIT_TYPE::cDIM)
      {
         if (aMultiDimName.length() > 2)
         {
            if (aMultiDimName[aMultiDimName.length() - 2] == '^')
            {
               // this assumes a power less than 10
               // chop off the caret and exponent before calling BaseUnit::ReadUnit
               return BaseUnit::ReadUnit(aMultiDimName.substr(0, aMultiDimName.length() - 2));
            }
         }
         // chop off the exponent before calling BaseUnit::ReadUnit
         return BaseUnit::ReadUnit(aMultiDimName.substr(0, aMultiDimName.length() - 1));
      }
      return -1;
   }
};
} // namespace UtUnitsDetail

//! Stores a value and associated unit.
//! @note The value is converted to a 'standard' unit for storage, and converted back to the input unit when written to a stream.
template<typename UNIT_CLASS>
class UtUnitaryValue
{
public:
   using UnitType = UNIT_CLASS;

   UtUnitaryValue()
      : mValue(0)
      , mUnitId(UnitType::cSTANDARD_UNIT_ID)
      , mPadding(0)
   {
   }

   UtUnitaryValue(double aValue)
      : mValue(aValue)
      , mUnitId(UnitType::cSTANDARD_UNIT_ID)
      , mPadding(0)
   {
   }

   operator double() const { return mValue; }

   void Set(double aValue, int aUnit)
   {
      mValue = UnitType::ConvertToStandard(aValue, aUnit);
      if (aUnit != -1)
      {
         mUnitId = aUnit;
      }
      else
      {
         mUnitId = UnitType::cSTANDARD_UNIT_ID;
      }
   }

   //! Set to the provided value with units specified by name
   //! @param  aValue    The value in units of @p aUnitName
   //! @param  aUnitName The name of the units, e.g. "meters"
   //! @throws std::invalid_argument if an invalid unit name is provided
   void Set(double aValue, const std::string& aUnitName)
   {
      int unit = UnitType::ReadUnit(aUnitName);
      if (unit == -1)
      {
         throw std::invalid_argument("Invalid unit name");
      }
      Set(aValue, unit);
   }

   UtUnitaryValue& operator=(double aValue)
   {
      Set(aValue, UnitType::cSTANDARD_UNIT_ID);
      return *this;
   }

   std::string GetUnitName() const { return UnitType::FindUnitName(mUnitId); }

   double GetAsUnit(int aUnitId) const { return UnitType::ConvertFromStandard(mValue, aUnitId); }

   double GetAsUnit() const { return GetAsUnit(mUnitId); }

   double GetAsUnit(const std::string& aUnitName) const
   {
      int unit = UnitType::ReadUnit(aUnitName);
      if (unit != -1)
      {
         return GetAsUnit(unit);
      }
      else
      {
         return mValue; // unknown unit value
      }
   }

   bool ConvertToUnit(const std::string& aUnitName)
   {
      int unit = UnitType::ReadUnit(aUnitName);
      if (unit != -1)
      {
         mUnitId = unit;
         return true;
      }
      else
      {
         return false; // unknown unit value
      }
   }

   //! Returns the unit used when specifying this value.
   //! @note The value is always stored in the standard unit
   int GetUnit() const { return mUnitId; }

   void ConvertToUnit(int aUnitId) { mUnitId = aUnitId; }

protected:
   double mValue;
   int    mUnitId;
   //! @note Most compilers will add 4 bytes of padding.
   //! So, add it explicitly and set it to zero (needed for IDE)
   int mPadding;
};

template<typename UNIT_CLASS>
std::istream& operator>>(std::istream& aIn, UtUnitaryValue<UNIT_CLASS>& aUnitary)
{
   std::string unit;
   double      value;
   aIn >> value;
   if (aIn >> unit)
   {
      UtStringUtil::ToLower(unit);
      int id = UNIT_CLASS::ReadUnit(unit);
      if (id != -1)
      {
         aUnitary.Set(value, id);
      }
      else
      {
         // TODO: Right now, we just accept the input as the standard unit
         aUnitary.Set(value, UNIT_CLASS::cSTANDARD_UNIT_ID);
      }
   }
   return aIn;
}

template<typename UNIT_CLASS>
std::ostream& operator<<(std::ostream& aOut, const UtUnitaryValue<UNIT_CLASS>& aUnitary)
{
   aOut << aUnitary.GetAsUnit(aUnitary.GetUnit()) << " " << aUnitary.GetUnitName();
   return aOut;
}

///////////////////////////////////////////////////////////////////////////
// BEGIN GENERATED CODE
//    This code is generated by UtUnits.py.  Do not modify directly.
///////////////////////////////////////////////////////////////////////////
// UtUnitHeader{

struct UT_EXPORT UtUnitLength
{
   enum BaseUnit
   {
      cMETERS            = 0,
      cKILOMETERS        = 1,
      cMEGAMETERS        = 2,
      cFEET              = 3,
      cKILOFEET          = 4,
      cMILES             = 5,
      cNAUTICAL_MILES    = 6,
      cCENTIMETERS       = 7,
      cMILLIMETERS       = 8,
      cMICROMETERS       = 9,
      cNANOMETERS        = 10,
      cANGSTROMS         = 11,
      cINCHES            = 12,
      cASTRONOMICAL_UNIT = 13
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 14;
   static constexpr int  cUSED_BITS        = 4;
   static constexpr int  cUNIT_TYPE_ID     = 0;
   static constexpr int  cLAST_SIMPLE_UNIT = 13;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitLength, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitTime
{
   enum BaseUnit
   {
      cSECONDS      = 0,
      cMINUTES      = 1,
      cHOURS        = 2,
      cMILLISECONDS = 3,
      cMICROSECONDS = 4,
      cNANOSECONDS  = 5,
      cDAYS         = 6
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 7;
   static constexpr int  cUSED_BITS        = 3;
   static constexpr int  cUNIT_TYPE_ID     = 1;
   static constexpr int  cLAST_SIMPLE_UNIT = 6;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions                     = UtUnitsDetail::UnitFunctionsT<UtUnitTime, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitSpeed
{
   enum BaseUnit
   {
      cMETERS_PER_SECOND   = 0,
      cKILOMETERS_PER_HOUR = 20,
      cFEET_PER_SECOND     = 48,
      cFEET_PER_MINUTE     = 50,
      cMILES_PER_HOUR      = 84,
      cKNOTS               = 100
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 8;
   static constexpr int  cUNIT_TYPE_ID     = 2;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitSpeed, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitLength;
   using PredicateType                      = UtUnitTime;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitDataSize
{
   enum BaseUnit
   {
      cBITS   = 0,
      cKBITS  = 1,
      cMBITS  = 2,
      cGBITS  = 3,
      cBYTES  = 4,
      cKBYTES = 5,
      cMBYTES = 6,
      cGBYTES = 7
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 8;
   static constexpr int  cUSED_BITS        = 4;
   static constexpr int  cUNIT_TYPE_ID     = 3;
   static constexpr int  cLAST_SIMPLE_UNIT = 7;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitDataSize, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitPower
{
   enum BaseUnit
   {
      cWATTS      = 0,
      cKILOWATTS  = 1,
      cMEGAWATTS  = 2,
      cGIGAWATTS  = 3,
      cMILLIWATTS = 4,
      cMICROWATTS = 5
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 6;
   static constexpr int  cUSED_BITS        = 3;
   static constexpr int  cUNIT_TYPE_ID     = 4;
   static constexpr int  cLAST_SIMPLE_UNIT = 5;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitPower, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitPowerDB
{
   enum BaseUnit
   {
      cWATTS      = 0,
      cKILOWATTS  = 1,
      cMEGAWATTS  = 2,
      cGIGAWATTS  = 3,
      cMILLIWATTS = 4,
      cMICROWATTS = 5,
      cDBW        = 6,
      cDBM        = 7
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 8;
   static constexpr int  cUSED_BITS        = 4;
   static constexpr int  cUNIT_TYPE_ID     = 5;
   static constexpr int  cLAST_SIMPLE_UNIT = 7;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitPowerDB, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double       ConvertToStandard(const double aValue, const int aUnit);
   static double       ConvertFromStandard(const double aValue, const int aUnit);
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitSolidAngle
{
   enum BaseUnit
   {
      cSTERADIANS = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 1;
   static constexpr int  cUSED_BITS        = 1;
   static constexpr int  cUNIT_TYPE_ID     = 6;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitSolidAngle, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitMass
{
   enum BaseUnit
   {
      cKILOGRAMS = 0,
      cGRAMS     = 1,
      cPOUNDS    = 2,
      cKLB       = 3,
      cTONS      = 4,
      cTONNES    = 5,
      cSLUGS     = 6
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 7;
   static constexpr int  cUSED_BITS        = 3;
   static constexpr int  cUNIT_TYPE_ID     = 7;
   static constexpr int  cLAST_SIMPLE_UNIT = 6;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions                     = UtUnitsDetail::UnitFunctionsT<UtUnitMass, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitForce
{
   enum BaseUnit
   {
      cNEWTONS = 0,
      cKGF     = 1,
      cLBF     = 2
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 3;
   static constexpr int  cUSED_BITS        = 2;
   static constexpr int  cUNIT_TYPE_ID     = 8;
   static constexpr int  cLAST_SIMPLE_UNIT = 2;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitForce, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitTorque
{
   enum BaseUnit
   {
      cNEWTON_METERS = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 7;
   static constexpr int  cUNIT_TYPE_ID     = 9;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitTorque, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitForce;
   using PredicateType                      = UtUnitLength;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '*';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitArea
{
   enum BaseUnit
   {
      cMETERS2 = 0,
      cFEET2   = 3,
      cINCHES2 = 12
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 4;
   static constexpr int  cUNIT_TYPE_ID     = 10;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = true;
   using UnitFunctions                     = UtUnitsDetail::UnitFunctionsT<UtUnitArea, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;
   using OneDimensionType                  = UtUnitLength;
   static constexpr int cDIM               = 2;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitAreaDB
{
   enum BaseUnit
   {
      cMETERS2 = 0,
      cDBSM    = -2
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 1;
   static constexpr int  cUSED_BITS        = 5;
   static constexpr int  cUNIT_TYPE_ID     = 11;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = true;
   using UnitFunctions       = UtUnitsDetail::UnitFunctionsT<UtUnitAreaDB, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;
   using OneDimensionType    = UtUnitLength;
   static constexpr int cDIM = 2;

   static double       ConvertToStandard(const double aValue, const int aUnit);
   static double       ConvertFromStandard(const double aValue, const int aUnit);
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitVolume
{
   enum BaseUnit
   {
      cMETERS3 = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 4;
   static constexpr int  cUNIT_TYPE_ID     = 12;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = true;
   using UnitFunctions       = UtUnitsDetail::UnitFunctionsT<UtUnitVolume, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;
   using OneDimensionType    = UtUnitLength;
   static constexpr int cDIM = 3;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitTime2
{
   enum BaseUnit
   {
      cSECONDS2 = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 3;
   static constexpr int  cUNIT_TYPE_ID     = 13;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = true;
   using UnitFunctions       = UtUnitsDetail::UnitFunctionsT<UtUnitTime2, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;
   using OneDimensionType    = UtUnitTime;
   static constexpr int cDIM = 2;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitAcceleration
{
   enum BaseUnit
   {
      cMETERS_PER_SECOND2 = 0,
      cFEET_PER_SECOND2   = 96,
      cG                  = 1
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 1;
   static constexpr int  cUSED_BITS        = 9;
   static constexpr int  cUNIT_TYPE_ID     = 14;
   static constexpr int  cLAST_SIMPLE_UNIT = 1;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitAcceleration, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitLength;
   using PredicateType                      = UtUnitTime2;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitFrequency
{
   enum BaseUnit
   {
      cHZ  = 0,
      cKHZ = 1,
      cMHZ = 2,
      cGHZ = 3
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 4;
   static constexpr int  cUSED_BITS        = 3;
   static constexpr int  cUNIT_TYPE_ID     = 15;
   static constexpr int  cLAST_SIMPLE_UNIT = 3;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitFrequency, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitAngle
{
   enum BaseUnit
   {
      cRADIANS    = 0,
      cDEGREES    = 1,
      cMILS       = 2,
      cARCSECONDS = 3
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 4;
   static constexpr int  cUSED_BITS        = 3;
   static constexpr int  cUNIT_TYPE_ID     = 16;
   static constexpr int  cLAST_SIMPLE_UNIT = 3;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitAngle, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitAngularRate
{
   enum BaseUnit
   {
      cRADIANS_PER_SECOND = 0,
      cDEGREES_PER_SECOND = 32,
      cRPM                = 1
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 1;
   static constexpr int  cUSED_BITS        = 8;
   static constexpr int  cUNIT_TYPE_ID     = 17;
   static constexpr int  cLAST_SIMPLE_UNIT = 1;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitAngularRate, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitAngle;
   using PredicateType                      = UtUnitTime;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitAngularAcceleration
{
   enum BaseUnit
   {
      cRADIANS_PER_SECOND2 = 0,
      cDEGREES_PER_SECOND2 = 16
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 7;
   static constexpr int  cUNIT_TYPE_ID     = 18;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitAngularAcceleration, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitAngle;
   using PredicateType                      = UtUnitTime2;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitDataRate
{
   enum BaseUnit
   {
      cBIT_PER_SECOND = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 8;
   static constexpr int  cUNIT_TYPE_ID     = 19;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitDataRate, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitDataSize;
   using PredicateType                      = UtUnitTime;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitMassDensity
{
   enum BaseUnit
   {
      cKILOGRAMS_PER_METER3 = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 8;
   static constexpr int  cUNIT_TYPE_ID     = 20;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitMassDensity, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitMass;
   using PredicateType                      = UtUnitVolume;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitMassTransfer
{
   enum BaseUnit
   {
      cKILOGRAMS_PER_SECOND = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 7;
   static constexpr int  cUNIT_TYPE_ID     = 21;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitMassTransfer, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitMass;
   using PredicateType                      = UtUnitTime;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitEnergy
{
   enum BaseUnit
   {
      cJOULES     = 0,
      cKILOJOULES = 1
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 2;
   static constexpr int  cUSED_BITS        = 2;
   static constexpr int  cUNIT_TYPE_ID     = 22;
   static constexpr int  cLAST_SIMPLE_UNIT = 1;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitEnergy, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitFluence
{
   enum BaseUnit
   {
      cJOULES_PER_METER2 = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 7;
   static constexpr int  cUNIT_TYPE_ID     = 23;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitFluence, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitEnergy;
   using PredicateType                      = UtUnitArea;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitIrradiance
{
   enum BaseUnit
   {
      cWATTS_PER_METER2 = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 8;
   static constexpr int  cUNIT_TYPE_ID     = 24;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitIrradiance, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitPower;
   using PredicateType                      = UtUnitArea;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitSpectralIrradiance
{
   enum BaseUnit
   {
      cWATTS_PER_METER2_PER_METER = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 13;
   static constexpr int  cUNIT_TYPE_ID     = 25;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitSpectralIrradiance, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitIrradiance;
   using PredicateType                      = UtUnitLength;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitRadiance
{
   enum BaseUnit
   {
      cWATTS_PER_METER2_PER_STERADIAN = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 10;
   static constexpr int  cUNIT_TYPE_ID     = 26;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitRadiance, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitIrradiance;
   using PredicateType                      = UtUnitSolidAngle;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitSpectralRadiance
{
   enum BaseUnit
   {
      cWATTS_PER_METER2_PER_STERADIAN_PER_METER = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 15;
   static constexpr int  cUNIT_TYPE_ID     = 27;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitSpectralRadiance, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitRadiance;
   using PredicateType                      = UtUnitLength;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitRatio
{
   enum BaseUnit
   {
      cABSOLUTE = 0,
      cDB       = 1
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 2;
   static constexpr int  cUSED_BITS        = 2;
   static constexpr int  cUNIT_TYPE_ID     = 28;
   static constexpr int  cLAST_SIMPLE_UNIT = 1;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitRatio, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double       ConvertToStandard(const double aValue, const int aUnit);
   static double       ConvertFromStandard(const double aValue, const int aUnit);
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitNoisePressure
{
   enum BaseUnit
   {
      cABSOLUTE = 0,
      cDB_20UPA = 1
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 2;
   static constexpr int  cUSED_BITS        = 2;
   static constexpr int  cUNIT_TYPE_ID     = 29;
   static constexpr int  cLAST_SIMPLE_UNIT = 1;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitNoisePressure, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double       ConvertToStandard(const double aValue, const int aUnit);
   static double       ConvertFromStandard(const double aValue, const int aUnit);
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitPressure
{
   enum BaseUnit
   {
      cPASCAL       = 0,
      cKILOPASCALS  = 1,
      cMICROPASCALS = 2,
      cNANOPASCALS  = 3,
      cPSI          = 4,
      cPSF          = 5,
      cDBPA         = 6,
      cDBUPA        = 7
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 8;
   static constexpr int  cUSED_BITS        = 4;
   static constexpr int  cUNIT_TYPE_ID     = 30;
   static constexpr int  cLAST_SIMPLE_UNIT = 7;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitPressure, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double       ConvertToStandard(const double aValue, const int aUnit);
   static double       ConvertFromStandard(const double aValue, const int aUnit);
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitTemperature
{
   enum BaseUnit
   {
      cKELVIN     = 0,
      cCELSIUS    = 1,
      cFAHRENHEIT = 2
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 3;
   static constexpr int  cUSED_BITS        = 2;
   static constexpr int  cUNIT_TYPE_ID     = 31;
   static constexpr int  cLAST_SIMPLE_UNIT = 2;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitTemperature, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double       ConvertToStandard(const double aValue, const int aUnit);
   static double       ConvertFromStandard(const double aValue, const int aUnit);
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitSpecificRange
{
   enum BaseUnit
   {
      cMETERS_PER_KILOGRAM          = 0,
      cMILES_PER_POUND              = 1,
      cMILES_PER_KILOPOUND          = 2,
      cNAUTICAL_MILES_PER_POUND     = 3,
      cNAUTICAL_MILES_PER_KILOPOUND = 4
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 5;
   static constexpr int  cUSED_BITS        = 3;
   static constexpr int  cUNIT_TYPE_ID     = 32;
   static constexpr int  cLAST_SIMPLE_UNIT = 4;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitSpecificRange, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitAngularInertia
{
   enum BaseUnit
   {
      cKILOGRAMS_METERS2 = 1,
      cSLUG_FEET2        = 2,
      cLBM_INCHES2       = 3
   };
   static constexpr int  cSTANDARD_UNIT_ID = 1;
   static constexpr int  cBASE_UNIT_COUNT  = 3;
   static constexpr int  cUSED_BITS        = 10;
   static constexpr int  cUNIT_TYPE_ID     = 33;
   static constexpr int  cLAST_SIMPLE_UNIT = 3;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitAngularInertia, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitMass;
   using PredicateType                      = UtUnitArea;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '*';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitVoltage
{
   enum BaseUnit
   {
      cVOLTS      = 0,
      cMILLIVOLTS = 1,
      cMICROVOLTS = 2,
      cKILOVOLTS  = 3,
      cMEGAVOLTS  = 4
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 5;
   static constexpr int  cUSED_BITS        = 3;
   static constexpr int  cUNIT_TYPE_ID     = 34;
   static constexpr int  cLAST_SIMPLE_UNIT = 4;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitVoltage, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitCurrent
{
   enum BaseUnit
   {
      cAMPS      = 0,
      cMILLIAMPS = 1,
      cMICROAMPS = 2,
      cNANOAMPS  = 3
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 4;
   static constexpr int  cUSED_BITS        = 3;
   static constexpr int  cUNIT_TYPE_ID     = 35;
   static constexpr int  cLAST_SIMPLE_UNIT = 3;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitCurrent, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitCurrentDensity
{
   enum BaseUnit
   {
      cAMPS_PER_METER2 = 0
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 0;
   static constexpr int  cUSED_BITS        = 8;
   static constexpr int  cUNIT_TYPE_ID     = 36;
   static constexpr int  cLAST_SIMPLE_UNIT = 0;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitCurrentDensity, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitCurrent;
   using PredicateType                      = UtUnitArea;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitResistance
{
   enum BaseUnit
   {
      cOHMS      = 0,
      cMICROOHMS = 1,
      cMILLIOHMS = 2,
      cKILOOHMS  = 3,
      cMEGAOHMS  = 4,
      cGIGAOHMS  = 5
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 6;
   static constexpr int  cUSED_BITS        = 3;
   static constexpr int  cUNIT_TYPE_ID     = 37;
   static constexpr int  cLAST_SIMPLE_UNIT = 5;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitResistance, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitCapacitance
{
   enum BaseUnit
   {
      cFARADS      = 0,
      cMILLIFARADS = 1,
      cMICROFARADS = 2,
      cNANOFARADS  = 3,
      cPICOFARADS  = 4,
      cFEMTOFARADS = 5
   };
   static constexpr int  cSTANDARD_UNIT_ID = 0;
   static constexpr int  cBASE_UNIT_COUNT  = 6;
   static constexpr int  cUSED_BITS        = 3;
   static constexpr int  cUNIT_TYPE_ID     = 38;
   static constexpr int  cLAST_SIMPLE_UNIT = 5;
   static constexpr bool cIS_COMPOUND_UNIT = false;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitCapacitance, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitImpulse
{
   enum BaseUnit
   {
      cNEWTON_SECONDS = 1
   };
   static constexpr int  cSTANDARD_UNIT_ID = 1;
   static constexpr int  cBASE_UNIT_COUNT  = 1;
   static constexpr int  cUSED_BITS        = 7;
   static constexpr int  cUNIT_TYPE_ID     = 39;
   static constexpr int  cLAST_SIMPLE_UNIT = 1;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitImpulse, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitForce;
   using PredicateType                      = UtUnitTime;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '*';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

struct UT_EXPORT UtUnitResponsivity
{
   enum BaseUnit
   {
      cAMPS_PER_WATT = 1
   };
   static constexpr int  cSTANDARD_UNIT_ID = 1;
   static constexpr int  cBASE_UNIT_COUNT  = 1;
   static constexpr int  cUSED_BITS        = 8;
   static constexpr int  cUNIT_TYPE_ID     = 40;
   static constexpr int  cLAST_SIMPLE_UNIT = 1;
   static constexpr bool cIS_COMPOUND_UNIT = true;
   static constexpr bool cIS_MULTIDIM      = false;
   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<UtUnitResponsivity, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;

   using SubjectType                        = UtUnitCurrent;
   using PredicateType                      = UtUnitPower;
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '/';

   static double ConvertToStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertToStandard(aValue, aUnit);
   }
   static double ConvertFromStandard(const double aValue, const int aUnit)
   {
      return UnitFunctions::ConvertFromStandard(aValue, aUnit);
   }
   static std::string  FindUnitName(int aUnitId) { return UnitFunctions::FindUnitName(aUnitId); }
   static int          ReadUnit(const std::string& aUnitName) { return UnitFunctions::ReadUnit(aUnitName); }
   static bool         IsUnitValid(int aUnitId) { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool         IsUnitValid(const std::string& aName) { return UnitFunctions::IsUnitValid(aName); }
   static const double mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int> mUnitStrings;
   static const std::map<int, std::string> mUnitToString;
};

// UtUnitHeader}

///////////////////////////////////////////////////////////////////////
// Unit value classes:
//   These are convenience classes to hold a value and associated unit.
///////////////////////////////////////////////////////////////////////
// UnitValueClasses{

class UT_EXPORT UtLengthValue : public UtUnitaryValue<UtUnitLength>
{
public:
   UtLengthValue()
      : UtUnitaryValue<UtUnitLength>()
   {
   }
   UtLengthValue(double aValue)
      : UtUnitaryValue<UtUnitLength>(aValue)
   {
   }
   UtLengthValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitLength>()
   {
      Set(aValue, aUnit);
   }
   UtLengthValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitLength>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtTimeValue : public UtUnitaryValue<UtUnitTime>
{
public:
   UtTimeValue()
      : UtUnitaryValue<UtUnitTime>()
   {
   }
   UtTimeValue(double aValue)
      : UtUnitaryValue<UtUnitTime>(aValue)
   {
   }
   UtTimeValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitTime>()
   {
      Set(aValue, aUnit);
   }
   UtTimeValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitTime>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtSpeedValue : public UtUnitaryValue<UtUnitSpeed>
{
public:
   UtSpeedValue()
      : UtUnitaryValue<UtUnitSpeed>()
   {
   }
   UtSpeedValue(double aValue)
      : UtUnitaryValue<UtUnitSpeed>(aValue)
   {
   }
   UtSpeedValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitSpeed>()
   {
      Set(aValue, aUnit);
   }
   UtSpeedValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitSpeed>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtDataSizeValue : public UtUnitaryValue<UtUnitDataSize>
{
public:
   UtDataSizeValue()
      : UtUnitaryValue<UtUnitDataSize>()
   {
   }
   UtDataSizeValue(double aValue)
      : UtUnitaryValue<UtUnitDataSize>(aValue)
   {
   }
   UtDataSizeValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitDataSize>()
   {
      Set(aValue, aUnit);
   }
   UtDataSizeValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitDataSize>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtPowerValue : public UtUnitaryValue<UtUnitPower>
{
public:
   UtPowerValue()
      : UtUnitaryValue<UtUnitPower>()
   {
   }
   UtPowerValue(double aValue)
      : UtUnitaryValue<UtUnitPower>(aValue)
   {
   }
   UtPowerValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitPower>()
   {
      Set(aValue, aUnit);
   }
   UtPowerValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitPower>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtPowerDBValue : public UtUnitaryValue<UtUnitPowerDB>
{
public:
   UtPowerDBValue()
      : UtUnitaryValue<UtUnitPowerDB>()
   {
   }
   UtPowerDBValue(double aValue)
      : UtUnitaryValue<UtUnitPowerDB>(aValue)
   {
   }
   UtPowerDBValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitPowerDB>()
   {
      Set(aValue, aUnit);
   }
   UtPowerDBValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitPowerDB>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtSolidAngleValue : public UtUnitaryValue<UtUnitSolidAngle>
{
public:
   UtSolidAngleValue()
      : UtUnitaryValue<UtUnitSolidAngle>()
   {
   }
   UtSolidAngleValue(double aValue)
      : UtUnitaryValue<UtUnitSolidAngle>(aValue)
   {
   }
   UtSolidAngleValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitSolidAngle>()
   {
      Set(aValue, aUnit);
   }
   UtSolidAngleValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitSolidAngle>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtMassValue : public UtUnitaryValue<UtUnitMass>
{
public:
   UtMassValue()
      : UtUnitaryValue<UtUnitMass>()
   {
   }
   UtMassValue(double aValue)
      : UtUnitaryValue<UtUnitMass>(aValue)
   {
   }
   UtMassValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitMass>()
   {
      Set(aValue, aUnit);
   }
   UtMassValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitMass>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtForceValue : public UtUnitaryValue<UtUnitForce>
{
public:
   UtForceValue()
      : UtUnitaryValue<UtUnitForce>()
   {
   }
   UtForceValue(double aValue)
      : UtUnitaryValue<UtUnitForce>(aValue)
   {
   }
   UtForceValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitForce>()
   {
      Set(aValue, aUnit);
   }
   UtForceValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitForce>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtTorqueValue : public UtUnitaryValue<UtUnitTorque>
{
public:
   UtTorqueValue()
      : UtUnitaryValue<UtUnitTorque>()
   {
   }
   UtTorqueValue(double aValue)
      : UtUnitaryValue<UtUnitTorque>(aValue)
   {
   }
   UtTorqueValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitTorque>()
   {
      Set(aValue, aUnit);
   }
   UtTorqueValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitTorque>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtAreaValue : public UtUnitaryValue<UtUnitArea>
{
public:
   UtAreaValue()
      : UtUnitaryValue<UtUnitArea>()
   {
   }
   UtAreaValue(double aValue)
      : UtUnitaryValue<UtUnitArea>(aValue)
   {
   }
   UtAreaValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitArea>()
   {
      Set(aValue, aUnit);
   }
   UtAreaValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitArea>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtAreaDBValue : public UtUnitaryValue<UtUnitAreaDB>
{
public:
   UtAreaDBValue()
      : UtUnitaryValue<UtUnitAreaDB>()
   {
   }
   UtAreaDBValue(double aValue)
      : UtUnitaryValue<UtUnitAreaDB>(aValue)
   {
   }
   UtAreaDBValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitAreaDB>()
   {
      Set(aValue, aUnit);
   }
   UtAreaDBValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitAreaDB>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtVolumeValue : public UtUnitaryValue<UtUnitVolume>
{
public:
   UtVolumeValue()
      : UtUnitaryValue<UtUnitVolume>()
   {
   }
   UtVolumeValue(double aValue)
      : UtUnitaryValue<UtUnitVolume>(aValue)
   {
   }
   UtVolumeValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitVolume>()
   {
      Set(aValue, aUnit);
   }
   UtVolumeValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitVolume>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtTime2Value : public UtUnitaryValue<UtUnitTime2>
{
public:
   UtTime2Value()
      : UtUnitaryValue<UtUnitTime2>()
   {
   }
   UtTime2Value(double aValue)
      : UtUnitaryValue<UtUnitTime2>(aValue)
   {
   }
   UtTime2Value(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitTime2>()
   {
      Set(aValue, aUnit);
   }
   UtTime2Value(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitTime2>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtAccelerationValue : public UtUnitaryValue<UtUnitAcceleration>
{
public:
   UtAccelerationValue()
      : UtUnitaryValue<UtUnitAcceleration>()
   {
   }
   UtAccelerationValue(double aValue)
      : UtUnitaryValue<UtUnitAcceleration>(aValue)
   {
   }
   UtAccelerationValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitAcceleration>()
   {
      Set(aValue, aUnit);
   }
   UtAccelerationValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitAcceleration>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtFrequencyValue : public UtUnitaryValue<UtUnitFrequency>
{
public:
   UtFrequencyValue()
      : UtUnitaryValue<UtUnitFrequency>()
   {
   }
   UtFrequencyValue(double aValue)
      : UtUnitaryValue<UtUnitFrequency>(aValue)
   {
   }
   UtFrequencyValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitFrequency>()
   {
      Set(aValue, aUnit);
   }
   UtFrequencyValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitFrequency>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtAngleValue : public UtUnitaryValue<UtUnitAngle>
{
public:
   UtAngleValue()
      : UtUnitaryValue<UtUnitAngle>()
   {
   }
   UtAngleValue(double aValue)
      : UtUnitaryValue<UtUnitAngle>(aValue)
   {
   }
   UtAngleValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitAngle>()
   {
      Set(aValue, aUnit);
   }
   UtAngleValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitAngle>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtAngularRateValue : public UtUnitaryValue<UtUnitAngularRate>
{
public:
   UtAngularRateValue()
      : UtUnitaryValue<UtUnitAngularRate>()
   {
   }
   UtAngularRateValue(double aValue)
      : UtUnitaryValue<UtUnitAngularRate>(aValue)
   {
   }
   UtAngularRateValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitAngularRate>()
   {
      Set(aValue, aUnit);
   }
   UtAngularRateValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitAngularRate>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtAngularAccelerationValue : public UtUnitaryValue<UtUnitAngularAcceleration>
{
public:
   UtAngularAccelerationValue()
      : UtUnitaryValue<UtUnitAngularAcceleration>()
   {
   }
   UtAngularAccelerationValue(double aValue)
      : UtUnitaryValue<UtUnitAngularAcceleration>(aValue)
   {
   }
   UtAngularAccelerationValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitAngularAcceleration>()
   {
      Set(aValue, aUnit);
   }
   UtAngularAccelerationValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitAngularAcceleration>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtDataRateValue : public UtUnitaryValue<UtUnitDataRate>
{
public:
   UtDataRateValue()
      : UtUnitaryValue<UtUnitDataRate>()
   {
   }
   UtDataRateValue(double aValue)
      : UtUnitaryValue<UtUnitDataRate>(aValue)
   {
   }
   UtDataRateValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitDataRate>()
   {
      Set(aValue, aUnit);
   }
   UtDataRateValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitDataRate>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtMassDensityValue : public UtUnitaryValue<UtUnitMassDensity>
{
public:
   UtMassDensityValue()
      : UtUnitaryValue<UtUnitMassDensity>()
   {
   }
   UtMassDensityValue(double aValue)
      : UtUnitaryValue<UtUnitMassDensity>(aValue)
   {
   }
   UtMassDensityValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitMassDensity>()
   {
      Set(aValue, aUnit);
   }
   UtMassDensityValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitMassDensity>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtMassTransferValue : public UtUnitaryValue<UtUnitMassTransfer>
{
public:
   UtMassTransferValue()
      : UtUnitaryValue<UtUnitMassTransfer>()
   {
   }
   UtMassTransferValue(double aValue)
      : UtUnitaryValue<UtUnitMassTransfer>(aValue)
   {
   }
   UtMassTransferValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitMassTransfer>()
   {
      Set(aValue, aUnit);
   }
   UtMassTransferValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitMassTransfer>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtEnergyValue : public UtUnitaryValue<UtUnitEnergy>
{
public:
   UtEnergyValue()
      : UtUnitaryValue<UtUnitEnergy>()
   {
   }
   UtEnergyValue(double aValue)
      : UtUnitaryValue<UtUnitEnergy>(aValue)
   {
   }
   UtEnergyValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitEnergy>()
   {
      Set(aValue, aUnit);
   }
   UtEnergyValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitEnergy>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtFluenceValue : public UtUnitaryValue<UtUnitFluence>
{
public:
   UtFluenceValue()
      : UtUnitaryValue<UtUnitFluence>()
   {
   }
   UtFluenceValue(double aValue)
      : UtUnitaryValue<UtUnitFluence>(aValue)
   {
   }
   UtFluenceValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitFluence>()
   {
      Set(aValue, aUnit);
   }
   UtFluenceValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitFluence>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtIrradianceValue : public UtUnitaryValue<UtUnitIrradiance>
{
public:
   UtIrradianceValue()
      : UtUnitaryValue<UtUnitIrradiance>()
   {
   }
   UtIrradianceValue(double aValue)
      : UtUnitaryValue<UtUnitIrradiance>(aValue)
   {
   }
   UtIrradianceValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitIrradiance>()
   {
      Set(aValue, aUnit);
   }
   UtIrradianceValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitIrradiance>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtSpectralIrradianceValue : public UtUnitaryValue<UtUnitSpectralIrradiance>
{
public:
   UtSpectralIrradianceValue()
      : UtUnitaryValue<UtUnitSpectralIrradiance>()
   {
   }
   UtSpectralIrradianceValue(double aValue)
      : UtUnitaryValue<UtUnitSpectralIrradiance>(aValue)
   {
   }
   UtSpectralIrradianceValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitSpectralIrradiance>()
   {
      Set(aValue, aUnit);
   }
   UtSpectralIrradianceValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitSpectralIrradiance>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtRadianceValue : public UtUnitaryValue<UtUnitRadiance>
{
public:
   UtRadianceValue()
      : UtUnitaryValue<UtUnitRadiance>()
   {
   }
   UtRadianceValue(double aValue)
      : UtUnitaryValue<UtUnitRadiance>(aValue)
   {
   }
   UtRadianceValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitRadiance>()
   {
      Set(aValue, aUnit);
   }
   UtRadianceValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitRadiance>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtSpectralRadianceValue : public UtUnitaryValue<UtUnitSpectralRadiance>
{
public:
   UtSpectralRadianceValue()
      : UtUnitaryValue<UtUnitSpectralRadiance>()
   {
   }
   UtSpectralRadianceValue(double aValue)
      : UtUnitaryValue<UtUnitSpectralRadiance>(aValue)
   {
   }
   UtSpectralRadianceValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitSpectralRadiance>()
   {
      Set(aValue, aUnit);
   }
   UtSpectralRadianceValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitSpectralRadiance>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtRatioValue : public UtUnitaryValue<UtUnitRatio>
{
public:
   UtRatioValue()
      : UtUnitaryValue<UtUnitRatio>()
   {
   }
   UtRatioValue(double aValue)
      : UtUnitaryValue<UtUnitRatio>(aValue)
   {
   }
   UtRatioValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitRatio>()
   {
      Set(aValue, aUnit);
   }
   UtRatioValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitRatio>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtNoisePressureValue : public UtUnitaryValue<UtUnitNoisePressure>
{
public:
   UtNoisePressureValue()
      : UtUnitaryValue<UtUnitNoisePressure>()
   {
   }
   UtNoisePressureValue(double aValue)
      : UtUnitaryValue<UtUnitNoisePressure>(aValue)
   {
   }
   UtNoisePressureValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitNoisePressure>()
   {
      Set(aValue, aUnit);
   }
   UtNoisePressureValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitNoisePressure>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtPressureValue : public UtUnitaryValue<UtUnitPressure>
{
public:
   UtPressureValue()
      : UtUnitaryValue<UtUnitPressure>()
   {
   }
   UtPressureValue(double aValue)
      : UtUnitaryValue<UtUnitPressure>(aValue)
   {
   }
   UtPressureValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitPressure>()
   {
      Set(aValue, aUnit);
   }
   UtPressureValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitPressure>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtTemperatureValue : public UtUnitaryValue<UtUnitTemperature>
{
public:
   UtTemperatureValue()
      : UtUnitaryValue<UtUnitTemperature>()
   {
   }
   UtTemperatureValue(double aValue)
      : UtUnitaryValue<UtUnitTemperature>(aValue)
   {
   }
   UtTemperatureValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitTemperature>()
   {
      Set(aValue, aUnit);
   }
   UtTemperatureValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitTemperature>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtSpecificRangeValue : public UtUnitaryValue<UtUnitSpecificRange>
{
public:
   UtSpecificRangeValue()
      : UtUnitaryValue<UtUnitSpecificRange>()
   {
   }
   UtSpecificRangeValue(double aValue)
      : UtUnitaryValue<UtUnitSpecificRange>(aValue)
   {
   }
   UtSpecificRangeValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitSpecificRange>()
   {
      Set(aValue, aUnit);
   }
   UtSpecificRangeValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitSpecificRange>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtAngularInertiaValue : public UtUnitaryValue<UtUnitAngularInertia>
{
public:
   UtAngularInertiaValue()
      : UtUnitaryValue<UtUnitAngularInertia>()
   {
   }
   UtAngularInertiaValue(double aValue)
      : UtUnitaryValue<UtUnitAngularInertia>(aValue)
   {
   }
   UtAngularInertiaValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitAngularInertia>()
   {
      Set(aValue, aUnit);
   }
   UtAngularInertiaValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitAngularInertia>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtVoltageValue : public UtUnitaryValue<UtUnitVoltage>
{
public:
   UtVoltageValue()
      : UtUnitaryValue<UtUnitVoltage>()
   {
   }
   UtVoltageValue(double aValue)
      : UtUnitaryValue<UtUnitVoltage>(aValue)
   {
   }
   UtVoltageValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitVoltage>()
   {
      Set(aValue, aUnit);
   }
   UtVoltageValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitVoltage>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtCurrentValue : public UtUnitaryValue<UtUnitCurrent>
{
public:
   UtCurrentValue()
      : UtUnitaryValue<UtUnitCurrent>()
   {
   }
   UtCurrentValue(double aValue)
      : UtUnitaryValue<UtUnitCurrent>(aValue)
   {
   }
   UtCurrentValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitCurrent>()
   {
      Set(aValue, aUnit);
   }
   UtCurrentValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitCurrent>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtCurrentDensityValue : public UtUnitaryValue<UtUnitCurrentDensity>
{
public:
   UtCurrentDensityValue()
      : UtUnitaryValue<UtUnitCurrentDensity>()
   {
   }
   UtCurrentDensityValue(double aValue)
      : UtUnitaryValue<UtUnitCurrentDensity>(aValue)
   {
   }
   UtCurrentDensityValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitCurrentDensity>()
   {
      Set(aValue, aUnit);
   }
   UtCurrentDensityValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitCurrentDensity>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtResistanceValue : public UtUnitaryValue<UtUnitResistance>
{
public:
   UtResistanceValue()
      : UtUnitaryValue<UtUnitResistance>()
   {
   }
   UtResistanceValue(double aValue)
      : UtUnitaryValue<UtUnitResistance>(aValue)
   {
   }
   UtResistanceValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitResistance>()
   {
      Set(aValue, aUnit);
   }
   UtResistanceValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitResistance>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtCapacitanceValue : public UtUnitaryValue<UtUnitCapacitance>
{
public:
   UtCapacitanceValue()
      : UtUnitaryValue<UtUnitCapacitance>()
   {
   }
   UtCapacitanceValue(double aValue)
      : UtUnitaryValue<UtUnitCapacitance>(aValue)
   {
   }
   UtCapacitanceValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitCapacitance>()
   {
      Set(aValue, aUnit);
   }
   UtCapacitanceValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitCapacitance>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtImpulseValue : public UtUnitaryValue<UtUnitImpulse>
{
public:
   UtImpulseValue()
      : UtUnitaryValue<UtUnitImpulse>()
   {
   }
   UtImpulseValue(double aValue)
      : UtUnitaryValue<UtUnitImpulse>(aValue)
   {
   }
   UtImpulseValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitImpulse>()
   {
      Set(aValue, aUnit);
   }
   UtImpulseValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitImpulse>()
   {
      Set(aValue, aUnit);
   }
};

class UT_EXPORT UtResponsivityValue : public UtUnitaryValue<UtUnitResponsivity>
{
public:
   UtResponsivityValue()
      : UtUnitaryValue<UtUnitResponsivity>()
   {
   }
   UtResponsivityValue(double aValue)
      : UtUnitaryValue<UtUnitResponsivity>(aValue)
   {
   }
   UtResponsivityValue(double aValue, int aUnit)
      : UtUnitaryValue<UtUnitResponsivity>()
   {
      Set(aValue, aUnit);
   }
   UtResponsivityValue(double aValue, const std::string& aUnit)
      : UtUnitaryValue<UtUnitResponsivity>()
   {
      Set(aValue, aUnit);
   }
};

// UnitValueClasses}

///////////////////////////////////////////////////////////////////////////
// END GENERATED CODE
///////////////////////////////////////////////////////////////////////////


#endif
