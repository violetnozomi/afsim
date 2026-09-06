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

#include "WsfOrbitalReferenceFrame.hpp"

namespace wsf
{
namespace space
{

std::string OrbitalReferenceFrameToIdentifier(OrbitalReferenceFrame aFrame)
{
   std::string retval;
   switch (aFrame)
   {
   case OrbitalReferenceFrame::cINERTIAL:
      retval = "inertial";
      break;
   case OrbitalReferenceFrame::cRIC:
      retval = "ric";
      break;
   case OrbitalReferenceFrame::cNTW:
      retval = "ntw";
      break;
   case OrbitalReferenceFrame::cUNKNOWN: // Intentional fall-through
   default:                              //
      retval.clear();
   }
   return retval;
}

OrbitalReferenceFrame OrbitalReferenceFrameFromIdentifier(const std::string& aIdentifier)
{
   OrbitalReferenceFrame retval{OrbitalReferenceFrame::cUNKNOWN};
   if (aIdentifier == "inertial")
   {
      retval = OrbitalReferenceFrame::cINERTIAL;
   }
   else if (aIdentifier == "ric")
   {
      retval = OrbitalReferenceFrame::cRIC;
   }
   else if (aIdentifier == "ntw")
   {
      retval = OrbitalReferenceFrame::cNTW;
   }
   return retval;
}

} // namespace space
} // namespace wsf
