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

#ifndef WSFDEFILE_HPP
#define WSFDEFILE_HPP

// This class is based on the equivalent functionality from NOVAS.
/*
  Naval Observatory Vector Astrometry Software (NOVAS)
  C Edition, Version 3.1

  eph_manager.h: Header file for eph_manager.c

  U. S. Naval Observatory
  Astronomical Applications Dept.
  Washington, DC
  http://www.usno.navy.mil/USNO/astronomical-applications
*/

#include <array>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "UtVec3.hpp"

//! This class provides an interface to a JPL Development Ephemeris (DE) file.
//!
//! This file may contain a good amount of data, so sharing of this data is
//! enabled via WsfDE_FileHandle, which provides the interface that clients
//! should use. This class supports the following ephemerides:
//! DE102, DE200, DE202, DE403, DE405, DE410, DE413, DE414, DE418, DE421,
//! DE422, DE423, DE424, DE430, DE431, DE433, DE434, DE435, DE436, DE438,
//! DE404, and DE406 format files.
//!
//! This class does not support the "t" variants of these files (DE430t,
//! DE432t, DE436t, and DE438t). Also note that some objects are not currently
//! exposed by this class (e.g., lunar libration offsets).
class WsfDE_File
{
public:
   explicit WsfDE_File(const std::string& aFileName);
   ~WsfDE_File() = default;

   //! The possible bodies that can be the target or center of the provided ephemeris.
   enum class Body : int
   {
      cMERCURY                 = 0,
      cVENUS                   = 1,
      cEARTH                   = 2,
      cMARS                    = 3,
      cJUPITER                 = 4,
      cSATURN                  = 5,
      cURANUS                  = 6,
      cNEPTUNE                 = 7,
      cPLUTO                   = 8,
      cMOON                    = 9,
      cSUN                     = 10,
      cSOLAR_SYSTEM_BARYCENTER = 11,
      cEARTH_MOON_BARYCENTER   = 12
   };

   //! Two-component Julian TDB Date
   //!
   //! The NOVAS routines obtain their highest precision when using
   //! a Julian date given by a whole part and a fractional part.
   //! Per the documentation in eph_manager.cpp: for maximum interpolation
   //! accuracy, set the first component to the most recent midnight
   //! at or before the interpolation epoch, and set the second
   //! component to be the fraction of a day since then.
   struct Date
   {
      double mParts[2];
   };

   //! Workspace data required by interpolation.
   //!
   //! When interpolating, the values of the Chebyshev polynomials at
   //! the (relative) evaluation time are needed. If subsequent interpolations
   //! occur for the same relative evaluation time, these values can be
   //! reused. The Workspace allows for this reuse.
   struct Workspace
   {
      Workspace()
      {
         // These Chebyshev polynomials are constant.
         mPc[0] = 1.0;
         mVc[0] = 0.0;
         mVc[1] = 1.0;
      }

      int                    mNumP{2};   //! Used to indicate which position polynomials need to be computed.
      int                    mNumV{3};   //! Used to indicate which velocity polynomials need to be computed.
      std::array<double, 18> mPc{};      //! Position polynomial values at current interpolation time.
      std::array<double, 18> mVc{};      //! Velocity polynomial values at current interpolation time.
      double                 mTwoT{0.0}; //! Twice the normalized Chebyshev time.
   };

   void GetPlanetEphemeris(const Date& aEpoch,
                           Body        aTarget,
                           Body        aOrigin,
                           Workspace&  aWorkspace,
                           UtVec3d&    aPosition,
                           UtVec3d&    aVelocity) const;

   //! Return the number of the ephemeris.
   int GetDE_Num() const { return mDE_Num; }

   //! Return the Julian date of the earliest time in the ephemeris.
   double GetFileStartJD() const { return mInitialJD; }

   //! Return the Julian date of the latest time in the ephemeris.
   double GetFileEndJD() const { return mFinalJD; }

   //! Return the name of the DE file this object represents.
   const std::string& GetFileName() const { return mFileName; }

   class Error : public std::runtime_error
   {
   public:
      explicit Error(const std::string& aWhat)
         : std::runtime_error(aWhat)
      {
      }
   };

private:
   class Record
   {
   public:
      explicit Record(int aRecordLength);
      void          Read(std::ifstream& aStream, int aRecordLength);
      const double* Data(int aOffset) const;

   private:
      std::vector<double> mData;
   };

   const Record& GetRecord(int aRecordNumber) const;

   void State(const Date& aTimeJD, Body aTarget, Workspace& aWorkspace, UtVec3d& aPosition, UtVec3d& aVelocity) const;
   void Interpolate(const double* aBuffer,
                    double        aInterpolationEpoch,
                    double        aIntervalSpan,
                    int           aNumCoeffPerComponent,
                    int           aNumSetsCoeff,
                    Workspace&    aWorkspace,
                    UtVec3d&      aPosition,
                    UtVec3d&      aVelocity) const;
   Date PrepareEpoch(const Date& aEpoch) const;
   int  ComputeRecordNumber(const Date& aEpoch) const;
   void ComputeInterpolationTimes(const Date& aEpoch, int aRecordNum, double& aInterpolationTime, double& aIntervalSpan) const;
   void        LoadAllRecords(std::ifstream& aFileStream);
   static void Split(double aValue, double& aWhole, double& aFraction);

   std::string                   mFileName{};     //! The file's name.
   std::uint32_t                 mDE_Num{};       //! The DE number in the file.
   double                        mEM_Factor{1.0}; //! Factor used for Earth-Moon barycenter. Related to mass ratio.
   std::array<std::uint32_t, 12> mRecordOffset{}; //! Offsets into record for each target.
   std::array<std::uint32_t, 12> mNumCoeffPerComponent{}; //! Number of coefficients per component for each target.
   std::array<std::uint32_t, 12> mNumSetsCoeff{};         //! Number of sets of coefficient for each target.
   double                        mInitialJD{0.0};         //! Initial Julian date supported by this file.
   double                        mFinalJD{0.0};           //! Final Julian date supported by this file.
   double                        mRecordInterval{0.0};    //! Time interval (in days) of the records in this file.
   int                           mRecordLength{0};        //! Length (in bytes) of each record.
   int                           mInitialRecordNum{};     //! The initial record number.
   int                           mFinalRecordNum{};       //! The final record number.
   std::vector<Record>           mRecords{};              //! The records.
};

#endif // WSFDEFILE_HPP
