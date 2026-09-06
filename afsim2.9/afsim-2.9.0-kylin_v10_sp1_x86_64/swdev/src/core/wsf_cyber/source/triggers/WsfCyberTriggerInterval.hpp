// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERTRIGGERINTERVAL_HPP
#define WSFCYBERTRIGGERINTERVAL_HPP

#include "wsf_cyber_export.h"

#include <limits>

#include "UtScriptAccessible.hpp"
#include "WsfRandomVariable.hpp"

namespace wsf
{
namespace cyber
{
namespace trigger
{

//! Object container for time interval definitions used by triggers.
class WSF_CYBER_EXPORT Interval : public UtScriptAccessible
{
public:
   //! Convenience definition for multiple interval usage.
   using IntervalVec = std::vector<Interval>;

   Interval() = default;
   const char* GetScriptClassName() const override { return "WsfCyberTriggerInterval"; }

   bool IsFromSet() const { return mFromSet; }
   bool IsToSet() const { return mToSet; }
   bool IsEverySet() const { return mEverySet; }

   //! @name Accessors for random time variables.
   //! @note All values are only drawn once. They then remain
   //! constant once drawn throughout the simulation. This may be
   //! expanded in the future for multiple methods of drawing
   //! if requested by users, much like how WsfCyber allows
   //! multiple draw methods for probabilities associated with
   //! cyber attacks.
   //@{
   double GetFromTime() const;
   double GetToTime() const;
   double GetEveryTime() const;
   //@}

   //! @name Mutators for random time variables.
   //! Provided for programmatic usage only.
   //@{
   bool SetFromTime(const WsfRandomVariable& aFrom);
   bool SetToTime(const WsfRandomVariable& aTo);
   bool SetEveryTime(const WsfRandomVariable& aEvery);
   //@}

   virtual void Initialize();
   void         Clear();

   //! @name NextEvaluationTime Method
   //! Given a current time, returns the next occurring
   //! time defined by this interval. The return value
   //! is a std::pair value, where the boolean indicates
   //! if a next occurring time occurs, and the value of the
   //! said time. Return values of false always return time 0.0.
   //! @note Incorrect interval definitions or choice of distributions
   //! with large ranges may result in a "every" time that will never
   //! occur within the range of the interval. It is advised that users
   //! ensure that any distribution for checks do not possibly exceed
   //! the interval, as this will result in this call always returning false.
   virtual std::pair<bool, double> NextEvaluationTime(double aSimTime) const;

   virtual bool ProcessInput(UtInput& aInput);

private:
   bool mFromSet{false};
   bool mToSet{false};
   bool mEverySet{false};

   //! Default constant, from time 0.0 (sim start)
   WsfRandomVariable mFromTime{WsfRandomVariable::cCONSTANT, 0.0, 0.0, WsfRandomVariable::cNON_NEGATIVE};

   //! Default constant, to time DBL_MAX (entire sim)
   WsfRandomVariable mToTime{WsfRandomVariable::cCONSTANT,
                             std::numeric_limits<double>::max(),
                             0.0,
                             WsfRandomVariable::cNON_NEGATIVE};

   //! Default constant, interval 1.0 s
   WsfRandomVariable mEveryTime{WsfRandomVariable::cCONSTANT, 1.0, 0.0, WsfRandomVariable::cNON_NEGATIVE};
};

class WSF_CYBER_EXPORT ScriptInterval : public UtScriptClass
{
public:
   ScriptInterval(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptInterval() override = default;

   static UtScriptRef* Create(const Interval& aInterval);
   void*               Create(const UtScriptContext& aInstance) override;
   void*               Clone(void* aObjectPtr) override;
   void                Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(SetEvery);
   UT_DECLARE_SCRIPT_METHOD(SetFromEvery);
   UT_DECLARE_SCRIPT_METHOD(SetToEvery);
   UT_DECLARE_SCRIPT_METHOD(SetFromToEvery);

   UT_DECLARE_SCRIPT_METHOD(IsFromSet);
   UT_DECLARE_SCRIPT_METHOD(IsToSet);
   UT_DECLARE_SCRIPT_METHOD(IsEverySet);

   UT_DECLARE_SCRIPT_METHOD(GetFrom);
   UT_DECLARE_SCRIPT_METHOD(GetTo);
   UT_DECLARE_SCRIPT_METHOD(GetEvery);

   UT_DECLARE_SCRIPT_METHOD(NextEvaluationTime_1);
   UT_DECLARE_SCRIPT_METHOD(NextEvaluationTime_2);
};

} // namespace trigger
} // namespace cyber
} // namespace wsf

#endif
