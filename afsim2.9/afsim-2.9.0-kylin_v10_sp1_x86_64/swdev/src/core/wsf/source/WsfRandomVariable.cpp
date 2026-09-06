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

#include "WsfRandomVariable.hpp"

#include <algorithm>

#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtStringUtil.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"

namespace
{
// =================================================================================================
bool ValidateInput(WsfRandomVariable::Distribution aDistribution,
                   double                          aParameter1,
                   double                          aParameter2,
                   WsfRandomVariable::Constraints  aConstraints)
{
   switch (aDistribution)
   {
   case WsfRandomVariable::cCONSTANT:
   {
      switch (aConstraints)
      {
      case WsfRandomVariable::cPOSITIVE:
      {
         if (aParameter1 <= 0.0)
         {
            return false;
         }
      }
      case WsfRandomVariable::cNON_NEGATIVE:
      {
         if (aParameter1 < 0.0)
         {
            return false;
         }
      }
      default:
      {
         return true;
      }
      }
      break;
   }
   case WsfRandomVariable::cUNIFORM:
   {
      switch (aConstraints)
      {
      case WsfRandomVariable::cPOSITIVE:
      {
         if (aParameter1 <= 0.0 || aParameter2 <= 0.0)
         {
            return false;
         }
      }
      case WsfRandomVariable::cNON_NEGATIVE:
      {
         if (aParameter1 < 0.0 || aParameter2 < 0.0)
         {
            return false;
         }
      }
      default:
      {
         return true;
      }
      }
      break;
   }
   case WsfRandomVariable::cNORMAL:
   case WsfRandomVariable::cLOG_NORMAL:
   {
      if (aParameter2 <= 0.0)
      {
         return false;
      }

      switch (aConstraints)
      {
      case WsfRandomVariable::cPOSITIVE:
      {
         if (aParameter1 <= 0.0)
         {
            return false;
         }
      }
      case WsfRandomVariable::cNON_NEGATIVE:
      {
         if (aParameter1 < 0.0)
         {
            return false;
         }
      }
      default:
      {
         return true;
      }
      }
      break;
   }
   case WsfRandomVariable::cEXPONENTIAL:
   {
      if (aParameter1 <= 0.0)
      {
         return false;
      }
      break;
   }
   }
   return true;
}
} // namespace

// =================================================================================================
//! Construct a default random variable with a uniform distribution in the range (0..1)
WsfRandomVariable::WsfRandomVariable()
   : mRandomPtr(nullptr)
{
   DefineDistribution(cUNIFORM, 0.0, 1.0, cPOSITIVE);
}

// =================================================================================================
//! Construct a default random variable with a uniform distribution in the range (0..1)
WsfRandomVariable::WsfRandomVariable(Constraints aConstraints)
   : mRandomPtr(nullptr)
{
   DefineDistribution(cUNIFORM, 0.0, 1.0, aConstraints);
}

// =================================================================================================
//! Construct a random variable with a constant value.
//! @param aConstant     The constant value.
//! @param aConstraints  Additional constraints on the resulting values.
WsfRandomVariable::WsfRandomVariable(double aConstant, Constraints aConstraints)
   : mRandomPtr(nullptr)
{
   DefineDistribution(cCONSTANT, aConstant, aConstant, aConstraints);
}

// =================================================================================================
//! Construct a random variable with a uniform distribution of (minimum, maximum).
//! @param aMinimum      The minimum value of the distribution.
//! @param aMaximum      The maximum value of the distribution.
//! @param aConstraints  Additional constraints on the resulting values.
WsfRandomVariable::WsfRandomVariable(double aMinimum, double aMaximum, Constraints aConstraints)
   : mRandomPtr(nullptr)
{
   DefineDistribution(cUNIFORM, aMinimum, aMaximum, aConstraints);
}

// =================================================================================================
//! Construct a random variable with a specific distribution.
//! @param aDistribution The type of distribution.
//! @param aParameter1   The first parameter for the distribution.
//! - For a constant distribution this is the constant value.
//! - For a uniform distribution this is the minimum value.
//! - For a normal or log-normal distribution this is the mean value.
//! - For an exponential distribution this is the lambda.
//! @param aParameter2   The second parameter for the distribution.
//! - For a constant distribution this is unused.
//! - For a uniform distribution this is the maximum value.
//! - For a normal or log-normal distribution this is the standard deviation.
//! - For an exponential distribution this is unused.
//! @param aConstraints  Additional constraints on the resulting values.
WsfRandomVariable::WsfRandomVariable(Distribution aDistribution, double aParameter1, double aParameter2, Constraints aConstraints)
   : mRandomPtr(nullptr)
{
   DefineDistribution(aDistribution, WsfVariable<double>(aParameter1), WsfVariable<double>(aParameter2), aConstraints);
}

// =================================================================================================
//! Construct a random variable with a specific distribution.
//! @param aDistribution The type of distribution.
//! @param aParameter1   The first parameter for the distribution.
//! - For a constant distribution this is the constant value.
//! - For a uniform distribution this is the minimum value.
//! - For a normal or log-normal distribution this is the mean value.
//! - For an exponential distribution this is the lambda.
//! @param aParameter2   The second parameter for the distribution.
//! - For a constant distribution this is unused.
//! - For a uniform distribution this is the maximum value.
//! - For a normal or log-normal distribution this is the standard deviation.
//! - For an exponential distribution this is unused.
//! @param aConstraints  Additional constraints on the resulting values.
WsfRandomVariable::WsfRandomVariable(Distribution               aDistribution,
                                     const WsfVariable<double>& aParameter1,
                                     const WsfVariable<double>& aParameter2,
                                     Constraints                aConstraints)
   : mRandomPtr(nullptr)
{
   DefineDistribution(aDistribution, aParameter1, aParameter2, aConstraints);
}

// =================================================================================================
bool WsfRandomVariable::Initialize(const std::string& aName,
                                   WsfComponent*      aObjectPtr,
                                   WsfScriptContext*  aContextPtr,
                                   WsfSimulation&     aSimulation)
{
   bool ok    = true;
   mRandomPtr = &aSimulation.GetRandom();
   std::string name1;
   std::string name2;
   switch (mDistribution)
   {
   case cCONSTANT:
      name1 = aName;
      name2 = name1;
      break;
   case cUNIFORM:
      name1 = aName + ".minimum";
      name2 = aName + ".maximum";
      break;
   case cNORMAL:
   case cLOG_NORMAL:
      name1 = aName + ".mean";
      name2 = aName + ".sigma";
      break;
   case cEXPONENTIAL:
      name1 = aName + ".lambda";
      name2 = name1;
      break;
   default: // Should never happen, but just in case...
      name1 = aName;
      name2 = name1;
      break;
   }

   ok &= mParameter1.Initialize(name1, aObjectPtr, aContextPtr, aSimulation.GetScriptContext());
   ok &= mParameter2.Initialize(name2, aObjectPtr, aContextPtr, aSimulation.GetScriptContext());

   // Set the default value in case 'Draw' is not called first.
   SetDefaultValues();

   return ok;
}

// =================================================================================================
//! Draw the next random value from the distribution.
double WsfRandomVariable::Draw()
{
   switch (mDistribution)
   {
   case cCONSTANT:
      mValue = mParameter1;
      break;
   case cEXPONENTIAL:
      mValue = mRandomPtr->Exponential<double>(mParameter1);
      break;
   case cLOG_NORMAL:
      mValue = mRandomPtr->LogNormal<double>(mParameter1, mParameter2);
      break;
   case cNORMAL:
      switch (mConstraints)
      {
      case cPOSITIVE:
         while ((mValue = mRandomPtr->Normal<double>(mParameter1, mParameter2)) <= 0.0)
         {
         }
         break;
      case cNON_NEGATIVE:
         while ((mValue = mRandomPtr->Normal<double>(mParameter1, mParameter2)) < 0.0)
         {
         }
         break;
      default:
         mValue = mRandomPtr->Normal<double>(mParameter1, mParameter2);
         break;
      }
      break;
   case cUNIFORM:
      mValue = mRandomPtr->Uniform<double>(mParameter1, mParameter2);
      break;
   default:
      break;
   }
   return mValue;
}

// =================================================================================================
//! Define a distribution from an input stream.
//! This method reads a distribution definition from the input stream.
//! The input can be of the form:
//! - constant \<value\>
//! - \<value\> (a synonym for a constant value)
//! - exponential lambda \<value\>
//! - exponential (a synonym for exponential w\ lambda = 1.0)
//! - log_normal mean \<value\> sigma \<value\>
//! - normal mean \<value\> sigma \<value\>
//! - uniform minimum \<value\> maximum \<value\>
//! @param aInput     The input stream.
//! @param aValueType The units of the values.
void WsfRandomVariable::ProcessInput(UtInput& aInput, UtInput::ValueType aValueType)
{
   // NOTE: This routine also accepts the syntax used by the 'time_distribution' command of the deprecated
   //       WSF_DELAY_PROCESSOR. This enables eliminating WSF_DELAY_PROCESSOR with the more general
   //       WSF_MESSAGE_PROCESSOR.

   std::string command;
   aInput.ReadValue(command);
   if (command == "uniform")
   {
      bool                haveMinimum = false;
      bool                haveMaximum = false;
      WsfVariable<double> minimum(0.0);
      WsfVariable<double> maximum(0.0);
      while (true)
      {
         aInput.ReadCommand(command);
         if ((command == "minimum") ||
             ((aValueType == UtInput::cTIME) && (command == "minimum_time"))) // See NOTE at top of routine
         {
            minimum.ReadValueOfType(aInput, aValueType);
            // POSITIVE and NON-NEGATIVE are treated the same because the lower bound is never generated.
            if ((mConstraints == cPOSITIVE) || (mConstraints == cNON_NEGATIVE))
            {
               minimum.ValueGreaterOrEqual(aInput, 0.0);
            }
            haveMinimum = true;
         }
         else if ((command == "maximum") ||
                  ((aValueType == UtInput::cTIME) && (command == "maximum_time"))) // See NOTE at top of routine
         {
            maximum.ReadValueOfType(aInput, aValueType);
            haveMaximum = true;
         }
         else
         {
            aInput.PushBack(command);
            break;
         }
      }
      if ((!haveMinimum) || (!haveMaximum))
      {
         throw UtInput::BadValue(aInput, "must provide a minimum and a maximum value");
      }
      maximum.ValueGreater(aInput, minimum);
      DefineDistribution(cUNIFORM, minimum, maximum, mConstraints);
   }
   else if ((command == "normal") || (command == "log_normal"))
   {
      Distribution distribution = cNORMAL;
      if (command == "log_normal")
      {
         distribution = cLOG_NORMAL;
      }

      bool                haveMean  = false;
      bool                haveSigma = false;
      WsfVariable<double> mean(0.0);
      WsfVariable<double> sigma(0.0);
      while (true)
      {
         aInput.ReadCommand(command);
         if ((command == "mean") ||
             ((aValueType == UtInput::cTIME) && (command == "mean_time"))) // See NOTE at top of routine
         {
            mean.ReadValueOfType(aInput, aValueType);
            if (mConstraints == cPOSITIVE)
            {
               mean.ValueGreater(aInput, 0.0);
            }
            else if (mConstraints == cNON_NEGATIVE)
            {
               mean.ValueGreaterOrEqual(aInput, 0.0);
            }
            haveMean = true;
         }
         else if ((command == "sigma") || (command == "standard_deviation") || (command == "std_dev") ||
                  ((aValueType == UtInput::cTIME) && (command == "sigma_time"))) // See NOTE at top of routine
         {
            sigma.ReadValueOfType(aInput, aValueType);
            sigma.ValueGreater(aInput, 0.0);
            haveSigma = true;
         }
         else
         {
            aInput.PushBack(command);
            break;
         }
      }
      if ((!haveMean) || (!haveSigma))
      {
         throw UtInput::BadValue(aInput, "mean and sigma must both be provided");
      }
      DefineDistribution(distribution, mean, sigma, mConstraints);
   }
   else if (command == "exponential")
   {
      double lambda(1.0);
      aInput.ReadCommand(command);

      if (command == "lambda")
      {
         aInput.ReadValue(lambda);
         aInput.ValueGreater(lambda, 0.0);
      }
      else
      {
         aInput.PushBack(command);
      }
      DefineDistribution(cEXPONENTIAL, lambda, 0.0, mConstraints);
   }
   else
   {
      // 'constant' distribution.  Accept either 'constant <value>' or '<value>'.
      if (command != "constant")
      {
         aInput.PushBack(command);
      }
      else if (aValueType == UtInput::cTIME)
      {
         // See NOTE at top of routine
         aInput.ReadValue(command);
         if (command != "time")
         {
            aInput.PushBack(command);
         }
      }

      WsfVariable<double> value(0.0);
      value.ReadValueOfType(aInput, aValueType);
      if (mConstraints == cPOSITIVE)
      {
         value.ValueGreater(aInput, 0.0);
      }
      else if (mConstraints == cNON_NEGATIVE)
      {
         value.ValueGreaterOrEqual(aInput, 0.0);
      }
      DefineDistribution(cCONSTANT, value, 0.0, mConstraints);
   }
}

// =================================================================================================
//! Set the defining parameters of the distribution.
//! @param aDistribution The type of distribution.
//! @param aParameter1   The first parameter for the distribution.
//! - For a constant distribution this is the constant value.
//! - For a uniform distribution this is the minimum value.
//! - For a normal or log-normal distribution this is the mean value.
//! @param aParameter2   The second parameter for the distribution.
//! - For a constant distribution this is unused.
//! - For a uniform distribution this is the maximum value.
//! - For a normal or log-normal distribution this is the standard deviation.
//! @param aConstraints  Additional constraints on the resulting values.
void WsfRandomVariable::DefineDistribution(Distribution               aDistribution,
                                           const WsfVariable<double>& aParameter1,
                                           const WsfVariable<double>& aParameter2,
                                           Constraints                aConstraints)
{
   mDistribution = aDistribution;
   mConstraints  = aConstraints;
   mParameter1   = aParameter1;
   mParameter2   = aParameter2;

   // Set the default value when 'Draw' is not called first.
   SetDefaultValues();
}

// =================================================================================================
void WsfRandomVariable::SetGenerator(WsfSimulation& aSim)
{
   mRandom.SetSeed(aSim.GetRandom().GetSeed());
   mRandomPtr = &mRandom;
}

// =================================================================================================
void WsfRandomVariable::SetDefaultValues()
{
   if ((!mParameter1.IsReference()) && (!mParameter2.IsReference()))
   {
      switch (mDistribution)
      {
      case cCONSTANT:
         mValue = mParameter1; // The constant value
         break;
      case cUNIFORM:
         mValue = 0.5 * (mParameter1 + mParameter2); // The mid point
         break;
      case cNORMAL:
      case cLOG_NORMAL:
         mValue = mParameter1; // The mean
         break;
      case cEXPONENTIAL:
         mValue = 1.0 / mParameter1; // The mean
         break;
      default:
         mValue = mParameter1; // should never happen... but just in case.
         break;
      }
   }
}

// =================================================================================================
WsfScriptRandomVariableClass::WsfScriptRandomVariableClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfRandomVariable");
   mIsScriptAccessible = true;
   mConstructible      = true;
   mCloneable          = true;

   AddMethod(ut::make_unique<Set>());

   AddMethod(ut::make_unique<Draw>());
   AddMethod(ut::make_unique<LastDraw>());

   AddMethod(ut::make_unique<GetFirstParameter>());
   AddMethod(ut::make_unique<GetSecondParameter>());
   AddMethod(ut::make_unique<GetConstraint>());
   AddMethod(ut::make_unique<GetDistribution>());
}

// =================================================================================================
void* WsfScriptRandomVariableClass::Create(const UtScriptContext& aInstance)
{
   return new WsfRandomVariable();
}

// =================================================================================================
void* WsfScriptRandomVariableClass::Clone(void* aObjectPtr)
{
   return new WsfRandomVariable(*static_cast<WsfRandomVariable*>(aObjectPtr));
}

// =================================================================================================
void WsfScriptRandomVariableClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfRandomVariable*>(aObjectPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRandomVariableClass, WsfRandomVariable, Set, 4, "void", "string, double, double, string")
{
   aObjectPtr->SetGenerator(*SIMULATION);

   auto distribution    = aVarArgs[0].GetString();
   auto firstParameter  = aVarArgs[1].GetDouble();
   auto secondParameter = aVarArgs[2].GetDouble();
   auto constraint      = aVarArgs[3].GetString();

   UtStringUtil::ToLower(distribution);
   UtStringUtil::ToLower(constraint);

   WsfRandomVariable::Distribution distributionEnum = WsfRandomVariable::Distribution::cCONSTANT;
   WsfRandomVariable::Constraints  constraintEnum   = WsfRandomVariable::Constraints::cNO_CONSTRAINT;

   if (distribution == "exponential")
   {
      distributionEnum = WsfRandomVariable::Distribution::cEXPONENTIAL;
   }
   else if (distribution == "log_normal")
   {
      distributionEnum = WsfRandomVariable::Distribution::cLOG_NORMAL;
   }
   else if (distribution == "normal")
   {
      distributionEnum = WsfRandomVariable::Distribution::cNORMAL;
   }
   else if (distribution == "uniform")
   {
      distributionEnum = WsfRandomVariable::Distribution::cUNIFORM;
   }

   if (constraint == "positive")
   {
      constraintEnum = WsfRandomVariable::Constraints::cPOSITIVE;
   }
   else if (constraint == "non_negative")
   {
      constraintEnum = WsfRandomVariable::Constraints::cNON_NEGATIVE;
   }

   WsfVariable<double> param1(firstParameter);
   WsfVariable<double> param2(secondParameter);

   UT_SCRIPT_ASSERT(ValidateInput(distributionEnum, firstParameter, secondParameter, constraintEnum))
   aObjectPtr->DefineDistribution(distributionEnum, param1, param2, constraintEnum);
}


// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRandomVariableClass, WsfRandomVariable, Draw, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->Draw());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRandomVariableClass, WsfRandomVariable, LastDraw, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->LastDraw());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRandomVariableClass, WsfRandomVariable, GetFirstParameter, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetParameterFirst());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRandomVariableClass, WsfRandomVariable, GetSecondParameter, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetParameterSecond());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRandomVariableClass, WsfRandomVariable, GetConstraint, 0, "string", "")
{
   auto constraint = aObjectPtr->GetConstraints();

   switch (constraint)
   {
   case WsfRandomVariable::Constraints::cPOSITIVE:
   {
      aReturnVal.SetString("positive");
      break;
   }
   case WsfRandomVariable::Constraints::cNON_NEGATIVE:
   {
      aReturnVal.SetString("non_negative");
      break;
   }
   default:
   {
      aReturnVal.SetString("no_constraint");
      break;
   }
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRandomVariableClass, WsfRandomVariable, GetDistribution, 0, "string", "")
{
   auto distribution = aObjectPtr->GetDistribution();

   switch (distribution)
   {
   case WsfRandomVariable::Distribution::cEXPONENTIAL:
   {
      aReturnVal.SetString("exponential");
      break;
   }
   case WsfRandomVariable::Distribution::cLOG_NORMAL:
   {
      aReturnVal.SetString("log_normal");
      break;
   }
   case WsfRandomVariable::Distribution::cNORMAL:
   {
      aReturnVal.SetString("normal");
      break;
   }
   case WsfRandomVariable::Distribution::cUNIFORM:
   {
      aReturnVal.SetString("uniform");
      break;
   }
   default:
   {
      aReturnVal.SetString("constant");
      break;
   }
   }
}
