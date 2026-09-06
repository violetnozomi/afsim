// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VerificationResults.hpp"

#include "UtEarth.hpp"

namespace wsfg
{

namespace spaceg
{

std::string VerificationStatusDescriptor(wsf::space::verify::Status aStatus)
{
   std::string retval{};
   switch (aStatus)
   {
   case wsf::space::verify::Status::cNONE:
      // Empty string is the intended result for this case.
      break;
   case wsf::space::verify::Status::cUNVERIFIABLE:
      retval = "Unverifiable";
      break;
   case wsf::space::verify::Status::cFAIL:
      retval = "FAIL";
      break;
   case wsf::space::verify::Status::cPASS:
      retval = "PASS";
      break;
   }
   return retval;
}

VerificationResults::VerificationResults()
   : mStatus{wsf::space::verify::Status::cNONE}
   , mTimeBefore{}
   , mTimeAfter{}
   , mSecondsBefore{-1.0}
   , mSecondsAfter{-1.0}
   , mElementsBefore{ut::EarthWGS84()}
   , mElementsAfter{ut::EarthWGS84()}
   , mDeltaV_AvailBefore{0.0}
   , mDeltaV_AvailAfter{0.0}
   , mPreviewColor{1.0f, 1.0f, 1.0f}
{
}

void VerificationResults::Clear()
{
   mStatus = wsf::space::verify::Status::cNONE;
}

} // namespace spaceg

} // namespace wsfg
