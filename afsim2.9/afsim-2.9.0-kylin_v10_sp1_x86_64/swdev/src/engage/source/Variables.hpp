// ****************************************************************************
// CUI//REL TO USA ONLY
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

#ifndef VARIABLES_HPP
#define VARIABLES_HPP

#include <string>

#include "UtInput.hpp"

namespace engage
{

class Simulation;

//! This represents all of the variables the user can select for writing.
class Variables
{
public:
   Variables();

   static size_t FindRealVariable(const std::string& aName);
   static size_t FindIntegerVariable(const std::string& aName);
   static size_t FindStringVariable(const std::string& aName);

   static double GetScaleFactor(size_t aIndex, const std::string& aUnits);

   static double GetRealValue(size_t aVarIndex, Simulation& aSimulation, size_t aArrayIndex);

   static int GetIntegerValue(size_t aVarIndex, Simulation& aSimulation, size_t aArrayIndex);

   static std::string GetStringValue(size_t aVarIndex, Simulation& aSimulation, size_t aArrayIndex);

private:
   typedef double (*RealAccessorPtr)(Simulation&, size_t aArrayIndex);
   typedef int (*IntegerAccessorPtr)(Simulation&, size_t aArrayIndex);
   typedef std::string (*StringAccessorPtr)(Simulation&, size_t aArrayIndex);

   class Variable
   {
   public:
      std::string mFullName;
      std::string mESAMS_Name;
   };

   class RealVariable : public Variable
   {
   public:
      UtInput::ValueType mUnitType;
      RealAccessorPtr    mAccessorPtr;
   };

   class IntegerVariable : public Variable
   {
   public:
      IntegerAccessorPtr mAccessorPtr;
   };

   class StringVariable : public Variable
   {
   public:
      StringAccessorPtr mAccessorPtr;
   };

   void DefineRealVariable(const std::string& aName,
                           const std::string& aESAMS_Name,
                           UtInput::ValueType aUnitType,
                           RealAccessorPtr    aAccessorPtr);

   void DefineIntegerVariable(const std::string& aName, const std::string& aESAMS_Name, IntegerAccessorPtr aAccessorPtr);

   void DefineStringVariable(const std::string& aName, const std::string& aESAMS_Name, StringAccessorPtr aAccessorPtr);

   static std::vector<RealVariable>    sRealVariables;
   static std::vector<IntegerVariable> sIntegerVariables;
   static std::vector<StringVariable>  sStringVariables;
};

} // namespace engage

#endif
