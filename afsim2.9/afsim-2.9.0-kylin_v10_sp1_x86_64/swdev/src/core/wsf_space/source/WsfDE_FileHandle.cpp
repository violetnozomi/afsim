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

#include "WsfDE_FileHandle.hpp"

//! Construct a handle to a DE file.
//!
//! This method should only be called by the WsfDE_FileManager, which acts
//! as a central repository for DE files loaded into AFSIM.
WsfDE_FileHandle::WsfDE_FileHandle(const WsfDE_File& aDE_File)
   : mDE_File(aDE_File)
{
}

//! Get the position and velocity for a body relative to another.
//!
//! Retrieve the position and velocity of the @p aTarget relative to
//! @p aOrigin at the given @p aEpoch. The position and velocity will
//! be given in the ECI frame. The input @p aEpoch is a two-component
//! Julian date, where the epoch at which the ephemeris will be given
//! is the sum of the two components. For simplicity, the full Julian
//! date can be placed in the first component. However, for highest
//! precision interpolation, @p aTimeJD should have its first component
//! equal to the most recent midnight at or before the interpolation
//! epoch, and the second component equal to the fraction of a day since
//! that midnight.
//!
//! The input time is on the TDB scale which is different from TT by
//! periodic variations of magnitude less that 1.6 ms.
//!
//! @param aEpoch             - The epoch at which to produce the ephemeris.
//! @param aTarget            - The target body.
//! @param aOrigin            - The body to use as origin.
//! @param aPosition [out]    - The ephemeris position in km.
//! @param aVelocity [out]    - The ephemeris velocity in km/s.
void WsfDE_FileHandle::GetPlanetEphemeris(const WsfDE_File::Date& aEpoch,
                                          WsfDE_File::Body        aTarget,
                                          WsfDE_File::Body        aOrigin,
                                          UtVec3d&                aPosition,
                                          UtVec3d&                aVelocity) const
{
   mDE_File.GetPlanetEphemeris(aEpoch, aTarget, aOrigin, mWorkspace, aPosition, aVelocity);
}
