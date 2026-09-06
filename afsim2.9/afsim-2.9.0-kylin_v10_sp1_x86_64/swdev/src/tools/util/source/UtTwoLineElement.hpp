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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTTWOLINEELEMENT_HPP
#define UTTWOLINEELEMENT_HPP

#include "ut_export.h"

#include <iosfwd>
#include <string>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
class UtInput;
#include "UtOrbitalElements.hpp"

/**
   The motion of the satellite is described by a standard 'two-line element' (TLE).
   The TLE captures the classical orbital elements along with some other terms useful to
   the NORAD propagators.
*/
class UT_EXPORT UtTwoLineElement : public UtOrbitalElements
{
public:
   UtTwoLineElement(const ut::CentralBody& aCentralBody = ut::EarthWGS84());
   UtTwoLineElement(const UtTwoLineElement& aElements) = default;
   UtTwoLineElement(const UtOrbitalElements& aElements);
   ~UtTwoLineElement() override = default;
   UtTwoLineElement& operator=(const UtTwoLineElement& aRhs) = default;

   UtTwoLineElement* Clone() const override { return new UtTwoLineElement(*this); }

   bool ProcessInput(UtInput& aInput) override;

   // "New" terms for handling Two-line elements:
   double GetEpochDate() const
   {
      return GetEpoch().GetJulianUTC_Date();
   } //! the Julian date of the TLE epoch (referenced to UTC)

   double GetMeanMotionFirstDerivative() const { return mMeanMotionFirstDerivative; }
   void   SetMeanMotionFirstDerivative(double aMeanMotionFirstDerivative)
   {
      mMeanMotionFirstDerivative = aMeanMotionFirstDerivative;
   }

   double GetMeanMotionSecondDerivative() const { return mMeanMotionSecondDerivative; }
   void   SetMeanMotionSecondDerivative(double aMeanMotionSecondDerivative)
   {
      mMeanMotionSecondDerivative = aMeanMotionSecondDerivative;
   }

   double GetBstarDrag() const { return mBstarDrag; }
   void   SetBstarDrag(double aBstar) { mBstarDrag = aBstar; }

   unsigned GetRevolutionNumber() const { return mRevolutionNumber; }
   void     SetRevolutionNumber(unsigned aRevolutionNumber) { mRevolutionNumber = aRevolutionNumber; }

   void ReadOrbitalData(UtInput& aInput);

   //! Return the two-line element's epoch year.
   int GetEpochYear() const { return (GetEpoch().GetYear() % 100); }

   char GetClassification() const { return mClassification; }
   int  GetElementNumber() const { return mElementNumber; }

   //! Return the two-line element's epoch day of year.
   double GetEpochDayOfYear() const { return GetEpoch().GetEpochDayOfYear(); }

   int  GetSatelliteNumber() const { return mSatelliteNumber; }
   void SetSatelliteNumber(int aSatNum);

   double GetEarthAngle() const;

   friend UT_EXPORT std::ostream& operator<<(std::ostream& aStream, const UtTwoLineElement& aTLE);

   static int GetChecksum(const std::string& aString);

   void PrintDiagnostic() const;

private:
   double ExtractValue(const std::string& aLine, unsigned int aStartColumn, unsigned int aEndColumn);

   // "New" terms for handling Two-line elements:
   double mMeanMotionFirstDerivative{0.0};
   double mMeanMotionSecondDerivative{0.0};
   double mBstarDrag{0.0};

   int      mSatelliteNumber{0};
   char     mClassification{'U'};
   int      mElementNumber{0};
   unsigned mRevolutionNumber{0};
};

#endif
