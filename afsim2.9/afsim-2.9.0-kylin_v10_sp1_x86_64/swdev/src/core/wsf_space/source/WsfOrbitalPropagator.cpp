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

#include "WsfOrbitalPropagator.hpp"

namespace wsf
{
namespace space
{

OrbitalPropagator::OrbitalPropagator(std::unique_ptr<ut::OrbitalState> aInitialOrbitalStatePtr)
   : UtOrbitalPropagatorBase(std::move(aInitialOrbitalStatePtr))
{
}

OrbitalPropagator::OrbitalPropagator(const OrbitalPropagator& aOther)
   : UtOrbitalPropagatorBase(aOther)
   , mMoverPtr{nullptr}
{
   // NOTE: When copy constructing, this will not associate this with the mover of the
   //       argument. Typically these are occurring during a Clone(), which means it
   //       will be associated with a new mover in short order.
}

} // namespace space
} // namespace wsf
