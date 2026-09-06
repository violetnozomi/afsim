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

#include "WsfCyberTriggerInterval.hpp"

#include <limits>

#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace cyber
{
namespace trigger
{

namespace
{
ScriptInterval* sClassPtr = nullptr;
} // namespace

// =================================================================================================
bool Interval::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   auto command   = aInput.GetCommand();

   if (command == "from")
   {
      mFromTime.ProcessInput(aInput, UtInput::ValueType::cTIME);
      mFromSet = true;
   }
   else if (command == "to")
   {
      mToTime.ProcessInput(aInput, UtInput::ValueType::cTIME);
      mToSet = true;
   }
   else if (command == "every")
   {
      mEveryTime.ProcessInput(aInput, UtInput::ValueType::cTIME);
      mEverySet = true;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
double Interval::GetFromTime() const
{
   if (mFromSet)
   {
      return mFromTime.LastDraw();
   }

   return 0.0;
}

// =================================================================================================
double Interval::GetToTime() const
{
   if (mToSet)
   {
      return mToTime.LastDraw();
   }

   return 0.0;
}

// =================================================================================================
double Interval::GetEveryTime() const
{
   if (mEverySet)
   {
      return mEveryTime.LastDraw();
   }

   return 0.0;
}

// =================================================================================================
bool Interval::SetFromTime(const WsfRandomVariable& aFrom)
{
   if (mFromTime.GetConstraints() == WsfRandomVariable::cNON_NEGATIVE)
   {
      mFromTime = aFrom;
      mFromSet  = true;
      return true;
   }

   return false;
}

// =================================================================================================
bool Interval::SetToTime(const WsfRandomVariable& aTo)
{
   if (mToTime.GetConstraints() == WsfRandomVariable::cNON_NEGATIVE)
   {
      mToTime = aTo;
      mToSet  = true;
      return true;
   }

   return false;
}

// =================================================================================================
bool Interval::SetEveryTime(const WsfRandomVariable& aEvery)
{
   if (mEveryTime.GetConstraints() == WsfRandomVariable::cNON_NEGATIVE)
   {
      mEveryTime = aEvery;
      mEverySet  = true;
      return true;
   }

   return false;
}

// =================================================================================================
void Interval::Initialize()
{
   //! Note: We currently only draw these values once, during initialization.
   if (mFromSet)
   {
      mFromTime.Draw();
   }

   if (mToSet)
   {
      mToTime.Draw();
   }

   if (mEverySet)
   {
      mEveryTime.Draw();
   }
}

// =================================================================================================
void Interval::Clear()
{
   (*this) = Interval();
}

// =================================================================================================
std::pair<bool, double> Interval::NextEvaluationTime(double aSimTime) const
{
   // Determine the relation of current time to the defined interval. Three mutually exclusive possibilities:
   // 1. Current time is before the interval
   // 2. Current time is within the interval
   // 3. Current time is after the interval (will immediately return false)

   // If "every" was not set, just return.
   if (!IsEverySet())
   {
      return std::make_pair(false, 0.0);
   }

   // To time is less than current time - interval has been exceeded. There will
   // never be a next interval time.
   constexpr double tol = std::numeric_limits<double>::epsilon();
   if (IsToSet() && (GetToTime() <= aSimTime - tol))
   {
      return std::make_pair(false, 0.0);
   }

   // From time exceeds current time - interval not yet reached.
   // Implicitly not within the interval.
   // The next interval time is when the interval begins.
   if (IsFromSet() && (GetFromTime() > aSimTime + tol))
   {
      return std::make_pair(true, GetFromTime());
   }

   // If we've reached this point, the current time is within the interval.
   // Determine the next multiple of the "every" time that exceeds the current time.
   auto elapsedIntervalTime = aSimTime - GetFromTime();
   auto intervalMultiplier  = std::ceil((elapsedIntervalTime / GetEveryTime()));
   auto initialIntervalTime = 0.0;
   if (IsFromSet())
   {
      initialIntervalTime = GetFromTime();
   }
   auto nextIntervalTime = ((GetEveryTime() * intervalMultiplier) + initialIntervalTime);

   // If the next interval time is the current time, increment by the every time
   if (nextIntervalTime == aSimTime)
   {
      nextIntervalTime += GetEveryTime();
   }

   // Ensure the next interval time does not exceed the "to" time, if applicable
   if (IsToSet() && (nextIntervalTime > GetToTime()))
   {
      return std::make_pair(false, 0.0);
   }
   else
   {
      return std::make_pair(true, nextIntervalTime);
   }
}

// =================================================================================================
ScriptInterval::ScriptInterval(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   sClassPtr = this;
   SetClassName("WsfCyberTriggerInterval");

   mIsScriptAccessible = true;
   mConstructible      = true;
   mCloneable          = true;

   AddMethod(ut::make_unique<SetEvery>());
   AddMethod(ut::make_unique<SetFromEvery>());
   AddMethod(ut::make_unique<SetToEvery>());
   AddMethod(ut::make_unique<SetFromToEvery>());

   AddMethod(ut::make_unique<IsFromSet>());
   AddMethod(ut::make_unique<IsToSet>());
   AddMethod(ut::make_unique<IsEverySet>());

   AddMethod(ut::make_unique<GetFrom>());
   AddMethod(ut::make_unique<GetTo>());
   AddMethod(ut::make_unique<GetEvery>());

   AddMethod(ut::make_unique<NextEvaluationTime_1>("NextEvaluationTime"));
   AddMethod(ut::make_unique<NextEvaluationTime_2>("NextEvaluationTime"));
}

// =================================================================================================
UtScriptRef* ScriptInterval::Create(const Interval& aInterval)
{
   auto intervalPtr = ut::make_unique<Interval>(aInterval);
   return UtScriptRef::Ref(intervalPtr.release(), sClassPtr, UtScriptRef::cMANAGE);
}

// =================================================================================================
void* ScriptInterval::Create(const UtScriptContext&)
{
   auto intervalPtr = ut::make_unique<Interval>();
   return intervalPtr.release();
}

// =================================================================================================
void* ScriptInterval::Clone(void* aObjectPtr)
{
   auto intervalPtr = ut::make_unique<Interval>(*static_cast<Interval*>(aObjectPtr));
   return intervalPtr.release();
}

// =================================================================================================
void ScriptInterval::Destroy(void* aObjectPtr)
{
   delete static_cast<Interval*>(aObjectPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, SetEvery, 1, "bool", "WsfRandomVariable")
{
   aObjectPtr->Clear();
   auto everyPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfRandomVariable>();

   auto ok = aObjectPtr->SetEveryTime(*everyPtr);
   aObjectPtr->Initialize();

   aReturnVal.SetBool(ok);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, SetFromEvery, 2, "bool", "WsfRandomVariable, WsfRandomVariable")
{
   aObjectPtr->Clear();
   auto fromPtr  = aVarArgs[0].GetPointer()->GetAppObject<WsfRandomVariable>();
   auto everyPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfRandomVariable>();

   auto ok = aObjectPtr->SetFromTime(*fromPtr);
   ok &= aObjectPtr->SetEveryTime(*everyPtr);
   aObjectPtr->Initialize();

   aReturnVal.SetBool(ok);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, SetToEvery, 2, "bool", "WsfRandomVariable, WsfRandomVariable")
{
   aObjectPtr->Clear();
   auto toPtr    = aVarArgs[0].GetPointer()->GetAppObject<WsfRandomVariable>();
   auto everyPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfRandomVariable>();

   auto ok = aObjectPtr->SetToTime(*toPtr);
   ok &= aObjectPtr->SetEveryTime(*everyPtr);
   aObjectPtr->Initialize();

   aReturnVal.SetBool(ok);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, SetFromToEvery, 3, "bool", "WsfRandomVariable, WsfRandomVariable, WsfRandomVariable")
{
   aObjectPtr->Clear();
   auto fromPtr  = aVarArgs[0].GetPointer()->GetAppObject<WsfRandomVariable>();
   auto toPtr    = aVarArgs[1].GetPointer()->GetAppObject<WsfRandomVariable>();
   auto everyPtr = aVarArgs[2].GetPointer()->GetAppObject<WsfRandomVariable>();

   auto ok = aObjectPtr->SetFromTime(*fromPtr);
   ok &= aObjectPtr->SetToTime(*toPtr);
   ok &= aObjectPtr->SetEveryTime(*everyPtr);
   aObjectPtr->Initialize();

   aReturnVal.SetBool(ok);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, IsFromSet, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsFromSet());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, IsToSet, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsToSet());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, IsEverySet, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsEverySet());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, GetFrom, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetFromTime());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, GetTo, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetToTime());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, GetEvery, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetEveryTime());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, NextEvaluationTime_1, 0, "double", "")
{
   auto result = aObjectPtr->NextEvaluationTime(TIME_NOW);
   auto time   = -1.0;

   if (result.first)
   {
      time = result.second;
   }

   aReturnVal.SetDouble(time);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptInterval, Interval, NextEvaluationTime_2, 1, "double", "double")
{
   auto userTime = aVarArgs[0].GetDouble();
   auto result   = aObjectPtr->NextEvaluationTime(userTime);
   auto time     = -1.0;

   if (result.first)
   {
      time = result.second;
   }

   aReturnVal.SetDouble(time);
}

} // namespace trigger
} // namespace cyber
} // namespace wsf
