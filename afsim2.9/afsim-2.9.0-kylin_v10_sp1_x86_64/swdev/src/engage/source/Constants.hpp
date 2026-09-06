// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace engage
{

//! An enumeration that defines the current phase of the simulation, or the applicable phase of an output slot.
enum Phase
{
   cALL,                //!< Only valid as an output phase selector. Indicates valid for any phase
   cSEARCH,             //!< (ISRCHM=1)
   cACQUIRING,          //!< Attempting to acquire the target (IACQM=2)
   cTRACKING,           //!< Tracking the target (ITRKM=3)
   cFIRING,             //!< Fire command issued (IFIREM=4)
   cACQUISITION_FAILED, //!< Acquisition mode failed (IACQFL=5)
   cTRACK_FAILED,       //!< Track establishment mode failed (ITRKFL=6)
   cFLYING,             //!< Weapon is flying (IFLYNG=7)
   cPHASE_COUNT
};

enum RadarType
{
   cRT_ACQ   = 1,
   cRT_TRK   = 2,
   cRT_SEEK  = 3,
   cRT_ILLUM = 4
};

} // namespace engage

#endif
