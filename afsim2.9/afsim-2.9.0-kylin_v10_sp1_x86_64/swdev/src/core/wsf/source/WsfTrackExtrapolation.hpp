//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#ifndef WSFTRACKEXTRAPOLATION_HPP
#define WSFTRACKEXTRAPOLATION_HPP

#include "wsf_export.h"

#include "UtAttribute.hpp"
#include "UtAttributeBase.hpp"
#include "UtMemory.hpp"
class WsfMeasurement;
#include "WsfObject.hpp"

namespace wsf
{

//! A simple set of standard extrapolation types to be supported.
namespace TrackExtrapolationIds
{
static constexpr const char* cDEFAULT_KINEMATIC_STATE_EXTRAPOLATION = "DEFAULT_KINEMATIC_STATE_EXTRAPOLATION";
static constexpr const char* cSPACE_KINEMATIC_STATE_EXTRAPOLATION   = "SPACE_KINEMATIC_STATE_EXTRAPOLATION";
}; // namespace TrackExtrapolationIds

//! A common base class of track extrapolation objects.
class WSF_EXPORT TrackExtrapolationBase : public WsfObject
{
public:
   TrackExtrapolationBase()                                   = default;
   TrackExtrapolationBase(const TrackExtrapolationBase& aSrc) = default;
   TrackExtrapolationBase* Clone() const override             = 0;

   ~TrackExtrapolationBase() override = default;
};

//! A base class from which to derive classes that provide extrapolation of specific data within a measurement or track.
template<typename T>
class TrackExtrapolation : public TrackExtrapolationBase
{
public:
   TrackExtrapolation()                               = default;
   TrackExtrapolation(const TrackExtrapolation& aSrc) = default;
   ~TrackExtrapolation() override                     = default;

   //! Extrapolate the information forward to the desired time since the time of information collection.
   //! This method should be overridden in subclasses.
   //! @param aSimTime The simulation time to which the measurement should be extrapolated.
   //! @param aData The data used to extrapolate the information.
   //! @return The extrapolated information, or nullptr if aData did not contain sufficient information to extrapolate.
   virtual std::unique_ptr<UtAttribute<T>> Extrapolate(double aSimTime, const WsfMeasurement& aData) const = 0;
   //! Return an instance of the extrapolated type (useful for more transparent type deduction).
   static T Type() { return T(); }
};

} // namespace wsf

#endif
