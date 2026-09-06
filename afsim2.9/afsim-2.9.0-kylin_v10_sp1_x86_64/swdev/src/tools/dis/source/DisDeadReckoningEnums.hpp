// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISDEADRECKONINGENUMS_HPP
#define DISDEADRECKONINGENUMS_HPP

//! Dead reckoning algorithm enumerations used in DIS PDUs are DisUint8 values.
namespace DisEnum
{
namespace DeadReckoning
{
enum AlgorithmType
{
   // See SISO-REF-010-2011.1, sec 4.5, or later:
   cOTHER   = 0,
   cSTATIC  = 1, // Entity does not move.)
   cDRM_FPW = 2, // DRM(F,P,W)
   cDRM_RPW = 3, // DRM(R,P,W)
   cDRM_RVW = 4, // DRM(R,V,W)
   cDRM_FVW = 5, // DRM(F,V,W)
   cDRM_FPB = 6, // DRM(F,P,B)
   cDRM_RPB = 7, // DRM(R,P,B)
   cDRM_RVB = 8, // DRM(R,V,B)
   cDRM_FVB = 9  // DRM(F,V,B)
};
} // end namespace DeadReckoning
} // end namespace DisEnum

#endif
