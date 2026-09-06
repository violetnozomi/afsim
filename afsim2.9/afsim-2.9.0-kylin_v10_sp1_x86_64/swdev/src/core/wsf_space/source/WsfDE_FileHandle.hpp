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

#ifndef WSFDEFILEHANDLE_HPP
#define WSFDEFILEHANDLE_HPP

#include "WsfDE_File.hpp"

//! A handle to a JPL DE file.
//!
//! JPL Development Ephemeris (DE) files can contain a good deal of data.
//! These handles provide a means for multiple clients to share the data
//! loaded from such a file. This is largely a pass-through class to the
//! underlying file object, but this object owns the WsfDE_File::Workspace
//! used during interpolation.
class WsfDE_FileHandle
{
public:
   explicit WsfDE_FileHandle(const WsfDE_File& aDE_File);
   WsfDE_FileHandle(const WsfDE_FileHandle& aOther) = default;
   ~WsfDE_FileHandle()                              = default;

   WsfDE_FileHandle& operator=(const WsfDE_FileHandle& aOther) = default;

   WsfDE_FileHandle* Clone() const { return new WsfDE_FileHandle{*this}; }

   void GetPlanetEphemeris(const WsfDE_File::Date& aEpoch,
                           WsfDE_File::Body        aTarget,
                           WsfDE_File::Body        aOrigin,
                           UtVec3d&                aPosition,
                           UtVec3d&                aVelocity) const;

   //! Return the DE number contained in the DE file to which this is a handle.
   int GetDE_Num() const { return mDE_File.GetDE_Num(); }

   //! Return the earliest Julian date for the ephemeris covered by this file.
   //!
   //! The returned value will be Julian date referenced to the TDB time scale.
   //!
   //! \return - Earliest Julian date (in TDB) covered by the DE file to which this is a handle.
   double GetFileStartJD() const { return mDE_File.GetFileStartJD(); }

   //! Return the latest Julian date for the ephemeris covered by this file.
   //!
   //! The returned value will be Julian date reference to the TDB time scale.
   //!
   //! \return - Latest Julian date (in TDB) covered by the DE file to which this is a handle.
   double GetFileEndJD() const { return mDE_File.GetFileEndJD(); }

   //! Return the name of the file that provided this ephemeris.
   const std::string& GetFileName() const { return mDE_File.GetFileName(); }

private:
   const WsfDE_File&             mDE_File;     //! Reference to the DE file.
   mutable WsfDE_File::Workspace mWorkspace{}; //! Workspace for interpolation.
};

#endif // WSFDEFILEHANDLE_HPP
