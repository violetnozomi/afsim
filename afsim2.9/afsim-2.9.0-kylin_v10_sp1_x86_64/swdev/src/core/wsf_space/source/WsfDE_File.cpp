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

#include "WsfDE_File.hpp"

#include <sstream>

namespace
{

constexpr static const char* cMAGIC_PHRASE = "JPL Planetary Ephemeris DE";

}

WsfDE_File::WsfDE_File(const std::string& aFileName)
{
   std::ifstream fileStream(aFileName, std::ios::binary);
   if (fileStream.is_open())
   {
      // Check the initial bytes of the file for "JPL Planetary Ephemeris DE"
      char magicPhrase[27];
      fileStream.read(magicPhrase, 26);
      if (!fileStream.good())
      {
         throw Error{"Error reading in header phrase."};
      }
      magicPhrase[26] = '\0';
      std::string phrase{magicPhrase};
      if (phrase != cMAGIC_PHRASE)
      {
         throw Error{"File does not appear to be a JPL DE file."};
      }

      // Skip a couple unused blocks of data with total length of 2652 bytes.
      fileStream.seekg(2652);
      if (!fileStream.good())
      {
         throw Error{"Error skipping header data (first 2652 bytes)."};
      }

      // Read in the interval data for this file.
      fileStream.read(reinterpret_cast<char*>(&mInitialJD), sizeof(mInitialJD));
      if (!fileStream.good())
      {
         throw Error{"Error reading header data (initial Julian date)."};
      }

      fileStream.read(reinterpret_cast<char*>(&mFinalJD), sizeof(mFinalJD));
      if (!fileStream.good())
      {
         throw Error{"Error reading header data (final Julian date)."};
      }

      fileStream.read(reinterpret_cast<char*>(&mRecordInterval), sizeof(mRecordInterval));
      if (!fileStream.good())
      {
         throw Error{"Error reading header data (record interval)."};
      }

      // Skip a couple values that are not used.
      fileStream.seekg(sizeof(std::uint32_t) + sizeof(double), std::ios_base::cur);
      if (!fileStream.good())
      {
         throw Error{"Error skipping past header data (an integer and a real value)."};
      }

      // Read in the Earth-Moon mass ratio.
      double emRatio;
      fileStream.read(reinterpret_cast<char*>(&emRatio), sizeof(emRatio));
      if (!fileStream.good())
      {
         throw Error{"Error reading header data (Earth-Moon mass ratio)."};
      }
      mEM_Factor = 1.0 / (1.0 + emRatio);

      // Read in offset and count data for the various targets.
      for (int i = 0; i < 12; i++)
      {
         std::uint32_t inValue;

         fileStream.read(reinterpret_cast<char*>(&inValue), sizeof(inValue));
         if (!fileStream.good())
         {
            std::stringstream err{};
            err << "Error reading header data (record offset for target [" << i << "]).";
            throw Error{err.str()};
         }
         mRecordOffset[i] = inValue;

         fileStream.read(reinterpret_cast<char*>(&inValue), sizeof(inValue));
         if (!fileStream.good())
         {
            std::stringstream err{};
            err << "Error reading header data (number of coefficients per component for target [" << i << "]).";
            throw Error{err.str()};
         }
         mNumCoeffPerComponent[i] = inValue;

         fileStream.read(reinterpret_cast<char*>(&inValue), sizeof(inValue));
         if (!fileStream.good())
         {
            std::stringstream err{};
            err << "Error reading header data (number of sets of coefficients for target [" << i << "]).";
            throw Error{err.str()};
         }
         mNumSetsCoeff[i] = inValue;
      }

      // Read which DE number is provided by this file.
      fileStream.read(reinterpret_cast<char*>(&mDE_Num), sizeof(mDE_Num));
      if (!fileStream.good())
      {
         throw Error{"Error reading header data (DE number)."};
      }

      // Set the value of the record length according to which JPL ephemeris is being opened.
      switch (mDE_Num)
      {
      case 102:
         mRecordLength = 6184;
         break;
      case 200: // Intentional fall-through
      case 202: //
         mRecordLength = 6608;
         break;
      case 403: // Intentional fall-through
      case 405: //
      case 410: //
      case 413: //
      case 414: //
      case 418: //
      case 421: //
      case 422: //
      case 423: //
      case 424: //
      case 430: //
      case 431: //
      case 433: //
      case 434: //
      case 435: //
      case 436: //
      case 438: //
         mRecordLength = 8144;
         break;
      case 404: // Intentional fall-through
      case 406: //
         mRecordLength = 5824;
         break;
      default:
         std::stringstream err{};
         err << "Unsupported DE value: " << mDE_Num << ".";
         throw Error{err.str()};
         break;
      }

      LoadAllRecords(fileStream);

      // We made it through all reads, so we set this member.
      mFileName = aFileName;
   }
   else
   {
      std::stringstream err{};
      err << "Unable to open external ephemeris file '" << aFileName << "'.";
      throw Error{err.str()};
   }
}

//! Get the position and velocity for a body relative to another.
//!
//! Retrieve the position and velocity of the @p aTarget relative to
//! @p aOrigin at the given @p aEpoch. The position and velocity will
//! be given in the ICRF. The input @p aEpoch is a two-component
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
//! @param aWorkspace [inout] - The handle's workspace, used to store values that might be reused.
//! @param aPosition [out]    - The ephemeris position in km.
//! @param aVelocity [out]    - The ephemeris velocity in km/s.
void WsfDE_File::GetPlanetEphemeris(const Date& aEpoch,
                                    Body        aTarget,
                                    Body        aOrigin,
                                    Workspace&  aWorkspace,
                                    UtVec3d&    aPosition,
                                    UtVec3d&    aVelocity) const
{
   // Check for target point == center point.
   if (aTarget == aOrigin)
   {
      aPosition.Set(0.0);
      aVelocity.Set(0.0);
      return;
   }

   // Check for instances of target or center being Earth or Moon,
   // and for target or center being the Earth-Moon barycenter.
   UtVec3d moonPos{};
   UtVec3d moonVel{};
   if ((aTarget == Body::cEARTH) || (aOrigin == Body::cEARTH))
   {
      State(aEpoch, Body::cMOON, aWorkspace, moonPos, moonVel);
   }

   UtVec3d earthPos{};
   UtVec3d earthVel{};
   if ((aTarget == Body::cMOON) || (aOrigin == Body::cMOON) || (aTarget == Body::cEARTH_MOON_BARYCENTER) ||
       (aOrigin == Body::cEARTH_MOON_BARYCENTER))
   {
      State(aEpoch, Body::cEARTH, aWorkspace, earthPos, earthVel);
   }

   // Make call to State for target object.
   UtVec3d targetPos{};
   UtVec3d targetVel{};
   if (aTarget == Body::cEARTH_MOON_BARYCENTER)
   {
      targetPos = earthPos;
      targetVel = earthVel;
   }
   else if (aTarget != Body::cSOLAR_SYSTEM_BARYCENTER)
   {
      State(aEpoch, aTarget, aWorkspace, targetPos, targetVel);
   }

   // Make call to State for center object.
   UtVec3d centerPos{};
   UtVec3d centerVel{};
   if (aOrigin == Body::cEARTH_MOON_BARYCENTER)
   {
      centerPos = earthPos;
      centerVel = earthVel;
   }
   else if (aOrigin != Body::cSOLAR_SYSTEM_BARYCENTER)
   {
      State(aEpoch, aOrigin, aWorkspace, centerPos, centerVel);
   }

   // Check for cases of Earth as target and Moon as center or vice versa.
   if ((aTarget == Body::cEARTH) && (aOrigin == Body::cMOON))
   {
      aPosition = -centerPos;
      aVelocity = -centerVel;
      return;
   }
   else if ((aTarget == Body::cMOON) && (aOrigin == Body::cEARTH))
   {
      aPosition = targetPos;
      aVelocity = targetVel;
      return;
   }

   // Check for Earth as target, or as center.
   else if (aTarget == Body::cEARTH)
   {
      targetPos -= moonPos * mEM_Factor;
      targetVel -= moonVel * mEM_Factor;
   }
   else if (aOrigin == Body::cEARTH)
   {
      centerPos -= moonPos * mEM_Factor;
      centerVel -= moonVel * mEM_Factor;
   }

   // Check for Moon as target, or as center.
   else if (aTarget == Body::cMOON)
   {
      targetPos += earthPos - targetPos * mEM_Factor;
      targetVel += earthVel - targetVel * mEM_Factor;
   }
   else if (aOrigin == Body::cMOON)
   {
      centerPos += earthPos - centerPos * mEM_Factor;
      centerVel += earthVel - centerVel * mEM_Factor;
   }

   // Compute position and velocity vectors.
   aPosition = targetPos - centerPos;
   aVelocity = targetVel - centerVel;
}

const WsfDE_File::Record& WsfDE_File::GetRecord(int aRecordNumber) const
{
   if (aRecordNumber < mInitialRecordNum || aRecordNumber > mFinalRecordNum)
   {
      std::stringstream err{};
      err << "DE file record number " << aRecordNumber << " is outside of allowed range";
      err << "[" << mInitialRecordNum << ", " << mFinalRecordNum << "].";
      throw Error(err.str());
   }
   return mRecords[aRecordNumber - 3];
}

void WsfDE_File::State(const Date& aTimeJD, Body aTarget, Workspace& aWorkspace, UtVec3d& aPosition, UtVec3d& aVelocity) const
{
   // Validate and prepare input epoch.
   Date jd = PrepareEpoch(aTimeJD);

   // Calculate record number and relative time interval.
   int nr = ComputeRecordNumber(jd);

   // Compute interpolation time
   double interpolationEpoch;
   double intervalSpan;
   ComputeInterpolationTimes(jd, nr, interpolationEpoch, intervalSpan);

   // Check and interpolate for requested body.
   int targetIndex = static_cast<int>(aTarget);
   Interpolate(GetRecord(nr).Data(mRecordOffset[targetIndex] - 1),
               interpolationEpoch,
               intervalSpan,
               mNumCoeffPerComponent[targetIndex],
               mNumSetsCoeff[targetIndex],
               aWorkspace,
               aPosition,
               aVelocity);
}

void WsfDE_File::Interpolate(const double* aBuffer,
                             double        aInterpolationEpoch,
                             double        aIntervalSpan,
                             int           aNumCoeffPerComponent,
                             int           aNumSetsCoeff,
                             Workspace&    aWorkspace,
                             UtVec3d&      aPosition,
                             UtVec3d&      aVelocity) const
{
   // Get correct sub-interval number for this set of coefficients and
   // then get normalized Chebyshev time within that subinterval.
   double dna  = static_cast<double>(aNumSetsCoeff);
   double dt1  = static_cast<double>(static_cast<int>(aInterpolationEpoch));
   double temp = dna * aInterpolationEpoch;
   auto   l    = static_cast<int>(temp - dt1);

   // 'tc' is the normalized Chebyshev time (-1 <= tc <= 1).
   double tc = 2.0 * (fmod(temp, 1.0) + dt1) - 1.0;

   // Check to see whether Chebyshev time has changed, and compute new
   // polynomial values if it has.  (The element PC[1] is the value of
   // t1[tc] and hence contains the value of 'tc' on the previous call.)
   if (tc != aWorkspace.mPc[1])
   {
      aWorkspace.mNumP  = 2;
      aWorkspace.mNumV  = 3;
      aWorkspace.mPc[1] = tc;
      aWorkspace.mTwoT  = tc + tc;
   }

   // Be sure that at least mNumP polynomials have been evaluated and
   // are stored in the array mPc.
   if (aWorkspace.mNumP < aNumCoeffPerComponent)
   {
      for (int i = aWorkspace.mNumP; i < aNumCoeffPerComponent; i++)
      {
         aWorkspace.mPc[i] = aWorkspace.mTwoT * aWorkspace.mPc[i - 1] - aWorkspace.mPc[i - 2];
      }
      aWorkspace.mNumP = aNumCoeffPerComponent;
   }

   // Interpolate to get position for each component.
   for (int i = 0; i < 3; i++)
   {
      aPosition[i] = 0.0;
      for (int j = aNumCoeffPerComponent - 1; j >= 0; j--)
      {
         int k = j + (i * aNumCoeffPerComponent) + (l * (3 * aNumCoeffPerComponent));
         aPosition[i] += aWorkspace.mPc[j] * aBuffer[k];
      }
   }

   // If velocity interpolation is desired, be sure enough derivative
   // polynomials have been generated and stored.
   double vfac       = (2.0 * dna) / aIntervalSpan;
   aWorkspace.mVc[2] = 2.0 * aWorkspace.mTwoT;
   if (aWorkspace.mNumV < aNumCoeffPerComponent)
   {
      for (int i = aWorkspace.mNumV; i < aNumCoeffPerComponent; i++)
      {
         aWorkspace.mVc[i] = aWorkspace.mTwoT * aWorkspace.mVc[i - 1] + aWorkspace.mPc[i - 1] + aWorkspace.mPc[i - 1] -
                             aWorkspace.mVc[i - 2];
      }
      aWorkspace.mNumV = aNumCoeffPerComponent;
   }

   // Interpolate to get velocity for each component.
   for (int i = 0; i < 3; i++)
   {
      aVelocity[i] = 0.0;
      for (int j = aNumCoeffPerComponent - 1; j > 0; j--)
      {
         int k = j + (i * aNumCoeffPerComponent) + (l * (3 * aNumCoeffPerComponent));
         aVelocity[i] += aWorkspace.mVc[j] * aBuffer[k];
      }
      aVelocity[i] *= vfac;
   }
}

// Split the given @p aEpoch into the two-components expected by the interpolation.
WsfDE_File::Date WsfDE_File::PrepareEpoch(const Date& aEpoch) const
{
   Date   retval{};
   double b;
   double c;

   Split(aEpoch.mParts[0] - 0.5, retval.mParts[0], b);
   Split(aEpoch.mParts[1], c, retval.mParts[1]);
   retval.mParts[0] += c + 0.5;
   b += retval.mParts[1];
   Split(b, c, retval.mParts[1]);
   retval.mParts[0] += c;

   // Throw Error if epoch is out of range.
   if ((retval.mParts[0] < mInitialJD) || ((retval.mParts[0] + retval.mParts[1]) > mFinalJD))
   {
      throw Error("Epoch out of range in query of DE state.");
   }

   return retval;
}

//! Compute the record number for the given @p aEpoch.
int WsfDE_File::ComputeRecordNumber(const Date& aEpoch) const
{
   int retval = static_cast<int>((aEpoch.mParts[0] - mInitialJD) / mRecordInterval) + 3;
   if (aEpoch.mParts[0] == mFinalJD)
   {
      retval -= 2;
   }
   return retval;
}

//! Compute the times needed for interpolation.
void WsfDE_File::ComputeInterpolationTimes(const Date& aEpoch,
                                           int         aRecordNum,
                                           double&     aInterpolationTime,
                                           double&     aIntervalSpan) const
{
   aInterpolationTime =
      ((aEpoch.mParts[0] - (static_cast<double>(aRecordNum - 3) * mRecordInterval + mInitialJD)) + aEpoch.mParts[1]) /
      mRecordInterval;
   aIntervalSpan = mRecordInterval * 86400.0;
}

//! Load all the data records from the file.
void WsfDE_File::LoadAllRecords(std::ifstream& aFileStream)
{
   mInitialRecordNum = 3; // See ComputeRecordNumber with aEpoch = {mInitialJD, 0.0}
   mFinalRecordNum   = ComputeRecordNumber(Date{mFinalJD, 0.0});
   aFileStream.seekg((mInitialRecordNum - 1) * mRecordLength);
   for (int record = mInitialRecordNum; record <= mFinalRecordNum; ++record)
   {
      mRecords.emplace_back(mRecordLength);
      mRecords[mRecords.size() - 1].Read(aFileStream, mRecordLength);
   }
}

//! Break a double into a double integer and fractional part.
//!
//! Breaks a double into its whole number and fractional number parts. When
//! the input value is negative, special care is taken that the whole number
//! part is the next most negative number when the fractional part is nonzero.
//!
//! \param aValue - A real value.
//! \returns      - Two doubles, the first of which holds the integer part, the second
//!                 of which holds the fractional part.
void WsfDE_File::Split(double aValue, double& aWhole, double& aFraction)
{
   // Get integer and fractional parts.
   aWhole    = static_cast<double>(static_cast<int>(aValue));
   aFraction = aValue - aWhole;

   // Make adjustments for negative input number.
   if ((aValue < 0.0) && (aFraction != 0.0))
   {
      aWhole    = aWhole - 1.0;
      aFraction = aFraction + 1.0;
   }
}

WsfDE_File::Record::Record(int aRecordLength)
   : mData(aRecordLength / 8)
{
}

void WsfDE_File::Record::Read(std::ifstream& aStream, int aRecordLength)
{
   aStream.read(reinterpret_cast<char*>(mData.data()), aRecordLength);
   if (!aStream.good())
   {
      throw WsfDE_File::Error("Error reading ephemeris file record.");
   }
}

const double* WsfDE_File::Record::Data(int aOffset) const
{
   return mData.data() + aOffset;
}
