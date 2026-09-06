// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSPACEORBITALPROPAGATORCONDITION_HPP
#define WSFSPACEORBITALPROPAGATORCONDITION_HPP

#include "wsf_space_export.h"

#include <utility>

#include "UtCalendar.hpp"
class UtInput;
class UtOrbitalPropagatorBase;
#include "UtUnitTypes.hpp"
#include "UtVec3.hpp"
#include "WsfObject.hpp"

namespace wsf
{
namespace space
{
class OrbitalPropagatorConditionVisitor;

//! A condition that might be satisfied during orbital propagation.
//! This class represents a condition that might obtain during orbital propagation
//! and is used to find execution times of orbital mission events. All conditions are
//! able to set an orbit number, which specifies how many full orbits should execute
//! before considering if the condition is satisfied.
class WSF_SPACE_EXPORT OrbitalPropagatorCondition : public WsfObject
{
public:
   OrbitalPropagatorCondition* Clone() const override = 0;

   bool ProcessInput(UtInput& aInput) override;

   //! Return the orbit number.
   unsigned GetOrbitNumber() const { return mOrbitNumber; }

   //! Set the orbit number.
   void SetOrbitNumber(unsigned aOrbitNumber) { mOrbitNumber = aOrbitNumber; }

   //! Validate any input parameters for the conditions.
   //! This method should be overridden for conditions that take parameters for which
   //! there are some restrictions on their values (e.g., the parameter must be positive).
   //! If the parameter is outside the allowed range, this method should print an error
   //! and return false.
   //! \returns true if all parameters are valid; false otherwise.
   virtual bool ValidateParameterRanges() const { return true; }

   //! Get the time at which the condition is satisfied.
   //! This method computes the time in seconds from the current time of the @p aPropagator
   //! at which the condition is satisfied. If the method is able to compute such a time, the
   //! @p aTimeToCondition is set to the appropriate value and this method returns true. If
   //! the method is unable to determine when the condition obtains, then the method returns
   //! false.
   //! \param aPropagator - The propagator for which the time to the condition being satisfied is desired.
   //! \param aTimeToCondition [out] - The time in seconds from the current time of the provided @p aPropagator
   //!                                 at which the condition is satisfied.
   //! \returns true if the time was able to be determined; false otherwise.
   virtual bool GetTimeToCondition(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const = 0;

   //! Accept a visitor.
   virtual void Accept(OrbitalPropagatorConditionVisitor& aVisitor) = 0;

   //! Return the string identifying the condition.
   virtual std::string GetConditionString() const = 0;

protected:
   unsigned mOrbitNumber{0};
};

//! A condition that performs an optimization.
//! Subclasses of this condition use a optimization procedure to determine when the condition is
//! satisfied. The sorts of conditions that can be computed as a subclass are any for which the
//! condition is expressible as a function with a root representing the time at which the condition
//! obtains. Most conditions are derived from this to allow for non-classical (i.e., not two-body)
//! orbital propagators the opportunity to use their full dynamics to discover the time at which the
//! condition is satisfied.
class WSF_SPACE_EXPORT OrbitalPropagatorOptimizingCondition : public OrbitalPropagatorCondition
{
public:
   //! This function is the objective function whose root gives the time of the condition.
   //! The provided propagator will be advanced to @p aOffsetTime seconds after the provided
   //! @p aBaseEpoch and the resulting state will be used to evaluate the condition. The
   //! condition needs to be formulated so that a boolean is returned, where the value of
   //! the boolean indicates if @p aOffsetTime is in a region before or after the root.
   //! The condition can return either true or false for values before the root, and the
   //! choice is up to the convenience of the implementer.
   //! \param aPropagator - The propagator for which the objective is being examined.
   //! \param aBaseEpoch - A base epoch from which the offset will be specified.
   //! \param aOffsetTime - The offset from the base epoch at which the objective will be computed.
   //! \returns If the given @p aOffsetTime is before the root.
   virtual bool Objective(UtOrbitalPropagatorBase& aPropagator, const UtCalendar& aBaseEpoch, double aOffsetTime) const = 0;

   //! Produce an initial guess for when the condition might obtain.
   //! The initial guess produced by this method is used to set the initial search area for the root.
   //! In some rare cases, it may be impossible to form an initial guess.
   //! \param aPropagator - The propagator for which an initial guess is to be returned.
   //! \returns - a pair, with a first member set to true if an initial guess was able to be computed,
   //!            and a second member set to the time of the initial guess.
   virtual std::pair<bool, double> InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const = 0;

   //! This template pattern method performs the search for the time at which the condition obtains.
   bool GetTimeToCondition(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const final;

   //! Handle special cases of initial or final orbits for a condition.
   //! If there are special initial or final orbit classes that need to be handled separately, this
   //! method can be overridden to provide the requisite values. If the @p aPropagator or the condition
   //! instance is such that special handling is required, this value should set the time at which the
   //! condition obtains in @p aTimeToCondition, and return true. Otherwise, this method should return
   //! false. Typically such special cases are called for in equatorial, or circular orbits.
   //! \param aPropagator - The propagator being examined for special cases.
   //! \param aTimeToCondition [out] - The time at which the condition obtains.
   //! \returns true if a special case was detected; false otherwise.
   virtual bool HandleSpecialCases(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const
   {
      return false;
   }
};

//! A condition that means as soon as possible.
class WSF_SPACE_EXPORT NoneCondition : public OrbitalPropagatorCondition
{
public:
   NoneCondition() { SetType(cTYPE); }

   NoneCondition* Clone() const override { return new NoneCondition{*this}; }

   bool GetTimeToCondition(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "NONE"; }

   constexpr static const char* cTYPE{"none"};
};

//! A condition that obtains after a given time has elapsed.
class WSF_SPACE_EXPORT RelativeTimeCondition : public OrbitalPropagatorCondition
{
public:
   RelativeTimeCondition() { SetType(cTYPE); }

   RelativeTimeCondition* Clone() const override { return new RelativeTimeCondition{*this}; }

   bool ProcessInput(UtInput& aInput) override;

   UtTimeValue GetOffsetTime() const { return mOffsetTime; }

   void SetOffsetTime(const UtTimeValue& aOffsetTime) { mOffsetTime = aOffsetTime; }

   bool ValidateParameterRanges() const override;

   bool GetTimeToCondition(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "AT_RELATIVE_TIME"; }

   constexpr static const char* cTYPE{"relative_time"};

private:
   UtTimeValue mOffsetTime{};
};

//! A condition indicating the point at which the propagator reaches periapsis.
class WSF_SPACE_EXPORT PeriapsisCondition : public OrbitalPropagatorOptimizingCondition
{
public:
   PeriapsisCondition() { SetType(cTYPE); }

   PeriapsisCondition* Clone() const override { return new PeriapsisCondition{*this}; }

   std::pair<bool, double> InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const override;

   bool Objective(UtOrbitalPropagatorBase& aPropagator, const UtCalendar& aBaseEpoch, double aOffsetTime) const override;

   bool HandleSpecialCases(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "AT_PERIAPSIS"; }

   constexpr static const char* cTYPE{"periapsis"};
};

//! A condition indicating the point at which the propagator reaches apoapsis.
class WSF_SPACE_EXPORT ApoapsisCondition : public OrbitalPropagatorOptimizingCondition
{
public:
   ApoapsisCondition() { SetType(cTYPE); }

   ApoapsisCondition* Clone() const override { return new ApoapsisCondition{*this}; }

   std::pair<bool, double> InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const override;

   bool Objective(UtOrbitalPropagatorBase& aPropagator, const UtCalendar& aBaseEpoch, double aOffsetTime) const override;

   bool HandleSpecialCases(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "AT_APOAPSIS"; }

   constexpr static const char* cTYPE{"apoapsis"};
};

//! A condition indicating the point at which the propagator passes the equatorial plane traveling northward.
class WSF_SPACE_EXPORT AscendingNodeCondition : public OrbitalPropagatorOptimizingCondition
{
public:
   AscendingNodeCondition() { SetType(cTYPE); }

   AscendingNodeCondition* Clone() const override { return new AscendingNodeCondition{*this}; }

   std::pair<bool, double> InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const override;

   bool Objective(UtOrbitalPropagatorBase& aPropagator, const UtCalendar& aBaseEpoch, double aOffsetTime) const override;

   bool HandleSpecialCases(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "AT_ASCENDING_NODE"; }

   constexpr static const char* cTYPE{"ascending_node"};
};

//! A condition indicating the point at which the propagator passes the equatorial plane traveling southward.
class WSF_SPACE_EXPORT DescendingNodeCondition : public OrbitalPropagatorOptimizingCondition
{
public:
   DescendingNodeCondition() { SetType(cTYPE); }

   DescendingNodeCondition* Clone() const override { return new DescendingNodeCondition{*this}; }

   std::pair<bool, double> InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const override;

   bool Objective(UtOrbitalPropagatorBase& aPropagator, const UtCalendar& aBaseEpoch, double aOffsetTime) const override;

   bool HandleSpecialCases(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "AT_DESCENDING_NODE"; }

   constexpr static const char* cTYPE{"descending_node"};
};

//! A condition indicating the point at which the propagator enters into eclipse.
class WSF_SPACE_EXPORT EclipseEntryCondition : public OrbitalPropagatorCondition
{
public:
   EclipseEntryCondition() { SetType(cTYPE); }

   EclipseEntryCondition* Clone() const override { return new EclipseEntryCondition{*this}; }

   bool GetTimeToCondition(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "AT_ECLIPSE_ENTRY"; }

   constexpr static const char* cTYPE{"eclipse_entry"};
};

//! A condition indicating the point at which the propagator exits from eclipse.
class WSF_SPACE_EXPORT EclipseExitCondition : public OrbitalPropagatorCondition
{
public:
   EclipseExitCondition() { SetType(cTYPE); }

   EclipseExitCondition* Clone() const override { return new EclipseExitCondition{*this}; }

   bool GetTimeToCondition(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "AT_ECLIPSE_EXIT"; }

   constexpr static const char* cTYPE{"eclipse_exit"};
};

class RadiusCondition : public OrbitalPropagatorOptimizingCondition
{
public:
   RadiusCondition() = default;

   bool ProcessInput(UtInput& aInput) override;

   UtLengthValue GetRadius() const { return mRadius; }

   void SetRadius(const UtLengthValue& aRadius) { mRadius = aRadius; }

   bool ValidateParameterRanges() const override;

protected:
   UtLengthValue mRadius{};
};

//! A condition indicating the point at which the propagator passes a given radius while moving farther from the central body.
class WSF_SPACE_EXPORT AscendingRadiusCondition : public RadiusCondition
{
public:
   AscendingRadiusCondition() { SetType(cTYPE); }

   AscendingRadiusCondition* Clone() const override { return new AscendingRadiusCondition{*this}; }

   std::pair<bool, double> InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const override;

   bool Objective(UtOrbitalPropagatorBase& aPropagator, const UtCalendar& aBaseEpoch, double aOffsetTime) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "AT_ASCENDING_RADIUS"; }

   constexpr static const char* cTYPE{"ascending_radius"};
};

//! A condition indicating the point at which the propagator passes a given radius while moving closer to the central body.
class WSF_SPACE_EXPORT DescendingRadiusCondition : public RadiusCondition
{
public:
   DescendingRadiusCondition() { SetType(cTYPE); }

   DescendingRadiusCondition* Clone() const override { return new DescendingRadiusCondition{*this}; }

   std::pair<bool, double> InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const override;

   bool Objective(UtOrbitalPropagatorBase& aPropagator, const UtCalendar& aBaseEpoch, double aOffsetTime) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "AT_DESCENDING_RADIUS"; }

   constexpr static const char* cTYPE{"descending_radius"};
};

class WSF_SPACE_EXPORT IntersectionCondition : public OrbitalPropagatorOptimizingCondition
{
public:
   IntersectionCondition() = default;

   double GetRAAN() const { return mRAAN; }
   double GetInclination() const { return mInclination; }

   void SetRAAN(double aRAAN);
   void SetInclination(double aInclination);

   void SetIntersectionRAAN_Inclination(double aRAAN, double aInclination);

   bool HandleSpecialCases(const UtOrbitalPropagatorBase& aPropagator, double& aTimeToCondition) const override;

protected:
   UtVec3d OrbitNormal(double aRAAN, double aInclination) const;

   double mRAAN{0.0};
   double mInclination{0.0};
};

//! A condition indicating the point north of the equator at which the propagator's orbit intersects with an orbit with
//! different RAAN and inclination.
class WSF_SPACE_EXPORT NorthernIntersectionCondition : public IntersectionCondition
{
public:
   NorthernIntersectionCondition() { SetType(cTYPE); }

   NorthernIntersectionCondition* Clone() const override { return new NorthernIntersectionCondition{*this}; }

   std::pair<bool, double> InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const override;

   bool Objective(UtOrbitalPropagatorBase& aPropagator, const UtCalendar& aBaseEpoch, double aOffsetTime) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "AT_NORTHERN_INTERSECTION"; }

   constexpr static const char* cTYPE{"northern_intersection"};
};

//! A condition indicating the point south of the equator at which the propagator's orbit intersects with an orbit with
//! different RAAN and inclination.
class WSF_SPACE_EXPORT SouthernIntersectionCondition : public IntersectionCondition
{
public:
   SouthernIntersectionCondition() { SetType(cTYPE); }

   SouthernIntersectionCondition* Clone() const override { return new SouthernIntersectionCondition{*this}; }

   std::pair<bool, double> InitialGuess(const UtOrbitalPropagatorBase& aPropagator) const override;

   bool Objective(UtOrbitalPropagatorBase& aPropagator, const UtCalendar& aBaseEpoch, double aOffsetTime) const override;

   void Accept(OrbitalPropagatorConditionVisitor& aVisitor) override;

   std::string GetConditionString() const override { return "AT_SOUTHERN_INTERSECTION"; }

   constexpr static const char* cTYPE{"southern_intersection"};
};

} // namespace space
} // namespace wsf

#endif // WSFSPACEORBITALPROPAGATORCONDITION_HPP
