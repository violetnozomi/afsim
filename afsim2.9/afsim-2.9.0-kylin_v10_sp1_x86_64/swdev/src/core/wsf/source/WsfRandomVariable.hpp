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

#ifndef WSFRANDOMVARIABLE_HPP
#define WSFRANDOMVARIABLE_HPP

#include "wsf_export.h"

#include "UtInput.hpp"
#include "UtRandom.hpp"
#include "UtScriptAccessible.hpp"
#include "UtScriptClassDefine.hpp"
class WsfScriptContext;
class WsfSimulation;
#include "WsfVariable.hpp"

//! A class that defines a continuous random variable.
//! This class defines a continuous random variable whose distribution can
//! be defined from an input stream.
//! @see WsfRandom
class WSF_EXPORT WsfRandomVariable : public UtScriptAccessible
{
public:
   //! The distribution of the resulting values.
   enum Distribution
   {
      cCONSTANT,    //!< The random values will always have a constant value.
      cEXPONENTIAL, //!< The random values will have a exponential distribution.
      cLOG_NORMAL,  //!< The random values will have a log-normal distribution.
      cNORMAL,      //!< The random values will have a normal distribution.
      cUNIFORM      //!< The random values will have a uniform distribution.
   };

   //! The constraints on the resulting values.
   enum Constraints
   {
      cNO_CONSTRAINT, //!< The variable values can be negative, zero or positive (subject to the distribution).
      cPOSITIVE,      //!< Value must be greater than zero.
      cNON_NEGATIVE   //!< Value must be greater than or equal to zero.
   };

   // NOTE: The default (member-wise) copy constructor and assignment operator are used.
   //       Copying or assigning a 'Initialized variable is OK as long as the copy would also
   //       refer to the same script context.

   WsfRandomVariable();
   WsfRandomVariable(Constraints aConstraints);
   WsfRandomVariable(double aConstant, Constraints aConstraints);
   WsfRandomVariable(double aMinimum, double aMaximum, Constraints aConstraints);
   WsfRandomVariable(Distribution aDistribution, double aParameter1, double aParameter2, Constraints aConstraints);
   WsfRandomVariable(Distribution               aDistribution,
                     const WsfVariable<double>& aParameter1,
                     const WsfVariable<double>& aParameter2,
                     Constraints                aConstraints);

   bool Initialize(const std::string& aName, WsfComponent* aObjectPtr, WsfScriptContext* aContextPtr, WsfSimulation& aSimulation);

   void ProcessInput(UtInput& aInput, UtInput::ValueType aValueType);

   void DefineDistribution(Distribution               aDistribution,
                           const WsfVariable<double>& aParameter1,
                           const WsfVariable<double>& aParameter2,
                           Constraints                aConstraints);

   double Draw();

   //! Return the last drawn value.
   double LastDraw() const { return mValue; }

   //! @name WsfRandomVariable Accessors
   //@{
   double       GetParameterFirst() const { return mParameter1; }
   double       GetParameterSecond() const { return mParameter2; }
   Constraints  GetConstraints() const { return mConstraints; }
   Distribution GetDistribution() const { return mDistribution; }
   //@}

   //! @name Assignment operators.
   //@{
   //! We're using the default assignment operator.
   //! WsfRandomVariable& operator=(const WsfRandomVariable& aRhs)
   //@}

   //! @name Relational operators.
   //@{
   //! Is the last drawn value equal to the specified value?
   double operator==(double aRhs) const { return (mValue == aRhs); }
   //! Is the last drawn value not equal to the specified value.
   double operator!=(double aRhs) const { return (mValue != aRhs); }
   //! Is the last draw value less than the specified value.
   double operator<(double aRhs) const { return (mValue < aRhs); }
   //@}

   //! @name Conversion operators.
   //@{
   //! Returns the last drawn value.
   operator double() const { return mValue; }
   //@}

   const char* GetScriptClassName() const override { return "WsfRandomVariable"; }

   //! Set the random pointer for usage outside ProcessInput()
   void SetGenerator(WsfSimulation& aSim);

protected:
   //! Helper method to set initial values when 'Draw' is not called first.
   void SetDefaultValues();

   //! A pointer to the currently used generator.
   ut::Random* mRandomPtr;

   //! A random number generator instance, for script usage.
   ut::Random mRandom;

   //! The type of distribution.
   Distribution mDistribution;

   //! Constraints on the resulting values.
   Constraints mConstraints;

   //! The current (last drawn) value.
   double mValue;

   //! The first parameter of the distribution.
   //! - For a constant distribution it is the constant value.
   //! - For a uniform distribution this is the minimum value.
   //! - For normal or log normal it is the mean value.
   WsfVariable<double> mParameter1;

   //! The second parameter of the distribution.
   //! - For a constant distribution it is unused.
   //! - For a uniform distribution this is the maximum value.
   //! - For a normal or log normal it is the standard deviation.
   WsfVariable<double> mParameter2;
};

class WSF_EXPORT WsfScriptRandomVariableClass : public UtScriptClass
{
public:
   WsfScriptRandomVariableClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~WsfScriptRandomVariableClass() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Set);

   UT_DECLARE_SCRIPT_METHOD(Draw);
   UT_DECLARE_SCRIPT_METHOD(LastDraw);

   UT_DECLARE_SCRIPT_METHOD(GetFirstParameter);
   UT_DECLARE_SCRIPT_METHOD(GetSecondParameter);
   UT_DECLARE_SCRIPT_METHOD(GetConstraint);
   UT_DECLARE_SCRIPT_METHOD(GetDistribution);
};

#endif
