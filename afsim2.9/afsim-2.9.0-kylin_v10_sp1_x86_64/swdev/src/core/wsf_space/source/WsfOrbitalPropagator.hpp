// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITALPROPAGATOR_HPP
#define WSFORBITALPROPAGATOR_HPP

#include "wsf_space_export.h"

#include "UtOrbitalPropagatorBase.hpp"
class WsfSpaceMoverBase;

namespace wsf
{
namespace space
{

//! A class that provides a complete interface for implementing orbital propagation in derived classes.
class WSF_SPACE_EXPORT OrbitalPropagator : public UtOrbitalPropagatorBase
{
public:
   explicit OrbitalPropagator(std::unique_ptr<ut::OrbitalState> aInitialOrbitalState);
   OrbitalPropagator(const OrbitalPropagator& aOther);
   ~OrbitalPropagator() override = default;

   OrbitalPropagator& operator=(const OrbitalPropagator& aOther) = delete;

   WsfSpaceMoverBase* GetSpaceMover() const { return mMoverPtr; }
   virtual void       SetSpaceMover(WsfSpaceMoverBase* aMoverPtr) { mMoverPtr = aMoverPtr; }

private:
   WsfSpaceMoverBase* mMoverPtr{nullptr};
};

} // namespace space
} // namespace wsf

#endif
