// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSPACEVERIFYTYPES_HPP
#define WSFSPACEVERIFYTYPES_HPP

#include <functional>

class UtCalendar;
class UtOrbitalElements;
class WsfOrbitalEvent;

namespace wsf
{
namespace space
{
namespace verify
{

//! The status of verification for a mission event.
enum class Status
{
   cNONE,         // no verification has been attempted
   cUNVERIFIABLE, // verification cannot be performed
   cFAIL,         // verification failed
   cPASS          // verification passed
};

using PreOperation = std::function<void(int, const WsfOrbitalEvent*, const UtCalendar&, const UtOrbitalElements&, double)>;

using PostOperation =
   std::function<void(int, const WsfOrbitalEvent*, const UtCalendar&, const UtOrbitalElements&, double, Status)>;

} // namespace verify
} // namespace space
} // namespace wsf

#endif // WSFSPACEVERIFYTYPES_HPP
