// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptConjunctionReport.hpp"

#include "UtScriptClass.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptData.hpp"
#include "WsfOrbitalConjunctionAssessment.hpp"

namespace wsf
{

namespace space
{

namespace script
{

Conjunction::Conjunction(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   AddClassName("WsfOrbitalConjunctionReport");

   AddMethod(ut::make_unique<MissDistance>());
   AddMethod(ut::make_unique<RelativeVelocity>());
   AddMethod(ut::make_unique<StartTime>());
   AddMethod(ut::make_unique<EndTime>());
   AddMethod(ut::make_unique<MinimumTime>());
   AddMethod(ut::make_unique<MaximumProbability>());
   AddMethod(ut::make_unique<Primary>());
   AddMethod(ut::make_unique<Secondary>());
}

//! Return the miss distance of the encounter in meters.
UT_DEFINE_SCRIPT_METHOD(Conjunction, WsfOrbitalConjunctionRecord, MissDistance, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mMissDistance);
}

//! Return the relative velocity at the point of closest approach in meters / second.
UT_DEFINE_SCRIPT_METHOD(Conjunction, WsfOrbitalConjunctionRecord, RelativeVelocity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mRelativeVelocity);
}

//! Return the start time of the conjunction in seconds.
//! This is measured from the starting time of the simulation. The starting time of the
//! conjunction is defined to be the predicted moment when the objects enter the combined
//! error ellipsoid of the two tracks.
UT_DEFINE_SCRIPT_METHOD(Conjunction, WsfOrbitalConjunctionRecord, StartTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mStartTime);
}

//! Return the end time of the conjunction in seconds.
//! This is measured from the starting time of the simulation. The ending time of the
//! conjunction is defined to be the predicted moment when the objects leave the combined
//! error ellipsoid of the two tracks.
UT_DEFINE_SCRIPT_METHOD(Conjunction, WsfOrbitalConjunctionRecord, EndTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mEndTime);
}

//! Return the time of closest approach.
//! This is measured from the starting time of the simulation. This time is the moment when
//! the predicted positions of the objects are closest together.
UT_DEFINE_SCRIPT_METHOD(Conjunction, WsfOrbitalConjunctionRecord, MinimumTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mMinTime);
}

//! Return a worst case probability estimate of the conjunction.
//! This gives a worst case probability, maximizing over all possible error ellipsoids.
UT_DEFINE_SCRIPT_METHOD(Conjunction, WsfOrbitalConjunctionRecord, MaximumProbability, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mMaxProbability);
}

//! Return an identifier for the primary object in the conjunction.
UT_DEFINE_SCRIPT_METHOD(Conjunction, WsfOrbitalConjunctionRecord, Primary, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->mPrimary);
}

//! Return an identifier for the secondary object in the conjunction.
UT_DEFINE_SCRIPT_METHOD(Conjunction, WsfOrbitalConjunctionRecord, Secondary, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->mSecondary);
}


} // namespace script

} // namespace space

} // namespace wsf
