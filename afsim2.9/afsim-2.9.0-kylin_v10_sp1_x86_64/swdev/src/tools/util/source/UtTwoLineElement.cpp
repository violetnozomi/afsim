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

#include "UtTwoLineElement.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtOrbitalState.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"

namespace
{
constexpr double cMIN_PER_DAY                = 1440.0;
constexpr double cSEC_PER_DAY                = 86400.0;
constexpr double cMOMENT_CONVERSION_CONSTANT = UtMath::cTWO_PI / (cMIN_PER_DAY * cMIN_PER_DAY);
constexpr size_t TLE_LINE_LENGTH             = 68;
} // namespace

UtTwoLineElement::UtTwoLineElement(const ut::CentralBody& aCentralBody)
   : UtOrbitalElements(aCentralBody)
{
}

//! Sometimes we have a set of orbital elements but want a TLE (especially, in order use operator<< to write to output).
UtTwoLineElement::UtTwoLineElement(const UtOrbitalElements& aElements)
   : UtOrbitalElements(aElements)
{
}

bool UtTwoLineElement::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        processed = true;

   if (command == "orbit")
   {
      ReadOrbitalData(aInput);
      aInput.ReadCommand(command);
      if (command != "end_orbit")
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   else
   {
      processed = UtOrbitalElements::ProcessInput(aInput);
   }

   return processed;
}

namespace
{
//! Check the line formatting for more obvious errors.
//! This error checking is not comprehensive; it is still possible to have malformed TLEs.
void CheckLineFormat(UtInput& aInput, const std::string& aLine)
{
   if (aLine.substr(0, 9) == "end_orbit")
   {
      throw UtInput::BadValue(aInput, "Line(s) missing in two-line element.");
   }
   int checksum = 0;

   std::string line{aLine};
   if ((aLine.size() == TLE_LINE_LENGTH + 1) ||                // Do not include existing checksum
       ((line.substr(0, 2) == "1 ") && (line.size() == 61)) || // Support (nonstandard) TLEs with ephemeris number,
       ((line.substr(0, 2) == "2 ") && (line.size() == 63)))   // checksums and rev numbers missing.
   {
      line.resize(TLE_LINE_LENGTH, ' ');
   }
   checksum = UtTwoLineElement::GetChecksum(line);

   if (checksum < 0)
   {
      switch (checksum)
      {
      case -1:
         throw UtInput::BadValue(aInput, "Two-line element lines should begin with '1 ' and '2 ' (not found).");
         break;
      case -2:
         throw UtInput::BadValue(aInput, "Invalid character(s) found in two-line element.");
         break;
      case -3:
         throw UtInput::BadValue(aInput, "Two-line element line is longer than 68 characters.");
         break;
      case -4:
         throw UtInput::BadValue(aInput, "Two-line element line is shorter than 68 characters.");
         break;
      default:
         break;
      }
   }
}
} // namespace

/** This is an input routine that reads a standard Two-Line-Element (TLE) from an input file.
 */
void UtTwoLineElement::ReadOrbitalData(UtInput& aInput)
{
   std::string line1;
   std::string line2;

   // The documentation for the TwoLineElement format was retrieved from celestrak.com

   // Accept either a two line or a three line format. The two line format is simply the
   // standard TwoLineElement. The three-line format was our original format where the TwoLineElement was preceded
   // by an additional descriptor line that was simply read and ignored.
   aInput.ReadLine(line1);
   UtStringUtil::TrimWhiteSpace(line1);
   aInput.ReadLine(line2);
   UtStringUtil::TrimWhiteSpace(line2);

   if (line2.substr(0, 2) == "1 ")
   {
      // Assume line 1 is the descriptor line, and line 2 is the actual start of the TLE
      mDescriptor = line1;
      line1       = line2;
      aInput.ReadLine(line2);
      UtStringUtil::TrimWhiteSpace(line2);

      // If mDescriptor is prefixed as line number zero (three line element form),
      // strip prefix "0 " from mDescriptor.
      if (mDescriptor.compare(0, 2, "0 ") == 0)
      {
         mDescriptor.erase(0, 2);
      }
   }

   // Make sure the lines are not malformed.
   CheckLineFormat(aInput, line1);
   CheckLineFormat(aInput, line2);

   // Extract data from the first line of the TwoLineElement

   // Epoch year
   int    epochYear      = static_cast<int>(ExtractValue(line1, 19, 20));
   double epochDayOfYear = ExtractValue(line1, 21, 32);

   // Epoch day
   SetEpoch(epochYear * 1000 + epochDayOfYear);

   // Satellite Number
   mSatelliteNumber = static_cast<int>(ExtractValue(line1, 3, 7));
   // Classification
   mClassification = line1[7];
   // Designator
   mDesignator.assign(line1, 9, 8);
   // First time derivative of mean motion
   mMeanMotionFirstDerivative = ExtractValue(line1, 34, 43) * cMOMENT_CONVERSION_CONSTANT;
   // Second time derivative of mean motion (decimal point assumed)
   mMeanMotionSecondDerivative = ExtractValue(line1, 45, 52) * cMOMENT_CONVERSION_CONSTANT / cMIN_PER_DAY;
   // mBstarDrag drag term (decimal point assumed)
   mBstarDrag = ExtractValue(line1, 54, 61);

   // TLEs should always contain 68 characters per line; however,
   // because the deep-space test case from SpaceTrack Report #3 is truncated,
   // keep the following check to ignore ephemeris and element number in line1,
   // and revolution number in line 2.
   if (line1.size() >= 64) // Read ephemeris type and element number; otherwise it's missing.
   {
      // Ephemeris type
      SetEphemerisType(static_cast<unsigned>(ExtractValue(line1, 63, 63)));
      // Element number
      mElementNumber = static_cast<int>(ExtractValue(line1, 65, 68));
      // Checksum (modulo 10)
      // mChecksum1 = static_cast<int>(ExtractValue(line1, 69, 69));
   }

   // Extract data from the second line of the TwoLineElement
   // Inclination (degrees)
   SetInclination(UtMath::cRAD_PER_DEG * ExtractValue(line2, 9, 16));
   // Right ascension of the ascending node (degrees)
   SetRAAN(UtMath::cRAD_PER_DEG * ExtractValue(line2, 18, 25));
   // Eccentricity (decimal point assumed)
   SetEccentricity(ExtractValue(line2, 27, 33) / 10000000.0);
   // Argument of the perigee (degrees)
   SetArgumentOfPeriapsis(UtMath::cRAD_PER_DEG * ExtractValue(line2, 35, 42));
   // Mean anomaly (degrees)
   SetMeanAnomaly(UtMath::cRAD_PER_DEG * ExtractValue(line2, 44, 51));
   // Mean motion (revs per day)
   SetMeanMotion(ExtractValue(line2, 53, 63) * UtMath::cTWO_PI / cSEC_PER_DAY);

   if (line2.size() >= 64) // Read rev number at epoch; otherwise it's missing.
   {
      // Revolution number at epoch (revs)
      mRevolutionNumber = static_cast<unsigned>(ExtractValue(line2, 64, 68));
      // Checksum (modulo 10)
      // mChecksum2 = static_cast<int>(ExtractValue(line2, 69, 69));
   }

   // Because this is a standard TLE, the data are considered averaged.
   SetHasMeanElements(true);
}

void UtTwoLineElement::SetSatelliteNumber(int aSatNum)
{
   if (aSatNum < 0)
   {
      auto out = ut::log::error() << "Ignoring negative satellite number.";
      out.AddNote() << "Satellite Number: " << aSatNum;
      return;
   }

   if (aSatNum > 99999)
   {
      auto out =
         ut::log::warning() << "Satellite number provided is more than five digits, compatibility may be limited!";
      out.AddNote() << "Satellite Number: " << aSatNum;
   }
   mSatelliteNumber = aSatNum;
}

void ConvertToModifiedDecimal(double aValue, std::string& aModifiedValue)
{
   // Exponent is one greater than standard exponent due to fixed leading
   // decimal (i.e. value range is [0.1-1.0) instead of [1.0-10.0).
   int exp = 0;
   if (aValue != 0)
   {
      exp = 1 + (int)std::floor(std::log10(std::fabs(aValue)));
   }
   // Get normalized mantissa [0.1-1.0) using negative exp to shift decimal position
   double mantissa = aValue * std::pow(10, -exp);
   // TLE field has five characters, get 10^5 integral value, rounding the first decimal
   int leadingDecMantissa = (int)std::round(mantissa * std::pow(10, 5));

   // Mantissa sign
   if (aValue < 0.0)
   {
      aModifiedValue.push_back('-');
   }
   else
   {
      // Positive is implicit with space, but always emit positive for clarity
      aModifiedValue.push_back('+');
   }

   // Mantissa value
   std::ostringstream oss;
   oss << std::setfill('0') << std::setw(5) << std::abs(leadingDecMantissa);
   aModifiedValue.append(oss.str());

   // Exponent sign (zero value uses negative sign by convention)
   if (exp <= 0)
   {
      aModifiedValue.push_back('-');
   }
   else
   {
      aModifiedValue.push_back('+');
   }

   // Exponent value
   if (std::abs(exp) > 9)
   {
      ut::log::warning() << "Two line element exponent is greater than allowed width.";
   }
   aModifiedValue.append(std::to_string(std::abs(exp)));
}

/** Does a checksum modulo 10 on the given line.
Digits = their numeric value, '-' = 1, all other valid chars = 0.
@returns checksum value in range 0-9 if string is reasonably TLE looking,
         otherwise returns negative value if definitely not a TLE line.
@note Error codes are the following:
   <ul>
   <li> -1 : Line does not begin with a '1' or '2'
   <lu> -2 : Invalid character.
   <li> -3 : Line is longer than 68 characters.
   <li> -4 : Line is shorter than 68 characters.
   <li> -5 : Line is empty.
   </ul>
*/
int UtTwoLineElement::GetChecksum(const std::string& aString)
{
   // Ensure that the base C/ASCII locale is used for character valuation
   static const std::locale cLocale("C");

   if (aString.empty())
   {
      return -5;
   }
   else if (aString.length() > TLE_LINE_LENGTH)
   {
      return -3;
   }
   else if (aString.length() < TLE_LINE_LENGTH)
   {
      return -4;
   }

   // Ensure first characters are TLE looking
   std::string lead = aString.substr(0, 2);
   if (lead != "1 " && lead != "2 ")
   {
      return -1;
   }

   int accumulate = 0;
   for (const char& c : aString)
   {
      // Allowed characters are A-Z, a-z, 0-9, '+', '-', ' ', '.'
      if (std::isdigit(c, cLocale))
      {
         // Since numerics are sequential, use direct base offset value
         accumulate += c - '0';
      }
      else if (c == '-')
      {
         // Minus sign defined as one in checksum algorithm
         accumulate += 1;
      }
      else if (c != ' ' && c != '.' && c != '+' && !std::isalpha(c, cLocale))
      {
         // Invalid character specified
         return -2;
      }
   }
   return accumulate % 10;
}

// friend
/**
   Write the TLE in standard format to an output stream.
*/
std::ostream& operator<<(std::ostream& aStream, const UtTwoLineElement& aTLE)
{
   // descriptor if available
   if (!aTLE.GetDescriptor().empty())
   {
      aStream << aTLE.GetDescriptor() << std::endl;
   }
   // line 1
   std::ostringstream line1;
   line1 << "1 " << std::setfill('0') << std::setw(5) << aTLE.mSatelliteNumber << aTLE.mClassification << ' ';

   std::string designator{aTLE.mDesignator};
   designator.resize(8, ' ');
   line1 << designator;

   double epoch = aTLE.GetEpoch().GetEpoch();
   line1 << ' ' << std::showpoint << std::setfill('0') << std::setw(14) << std::fixed << std::setprecision(8) << epoch;

   std::string mmfdStr = [&aTLE]()
   {
      std::ostringstream oss;
      double             meanMotionFirstDerivative = aTLE.mMeanMotionFirstDerivative / cMOMENT_CONVERSION_CONSTANT;
      oss << ' ' << std::showpoint << std::setw(8) << std::fixed << std::setprecision(8) << meanMotionFirstDerivative;
      return oss.str();
   }();
   int startLoc = mmfdStr.at(1) != '0' ? 1 : 0; // Must not display a leading zero; the following code handles that
   mmfdStr.at(startLoc)     = ' ';
   mmfdStr.at(startLoc + 1) = aTLE.mMeanMotionFirstDerivative >= 0.0 ? ' ' : '-';
   line1 << mmfdStr.substr(startLoc);

   std::string secondDerivative;
   ConvertToModifiedDecimal(aTLE.mMeanMotionSecondDerivative / cMOMENT_CONVERSION_CONSTANT * cMIN_PER_DAY,
                            secondDerivative);
   line1 << ' ' << secondDerivative;

   std::string bStarDragDrag;
   ConvertToModifiedDecimal(aTLE.mBstarDrag, bStarDragDrag);
   line1 << ' ' << bStarDragDrag;

   line1 << ' ' << aTLE.GetEphemerisType();
   line1 << ' ' << std::setfill('0') << std::setw(4) << aTLE.mElementNumber % 10000;

   int checksum1 = UtTwoLineElement::GetChecksum(line1.str());
   line1 << std::left << std::setw(1) << checksum1;
   aStream << line1.str() << std::endl;

   // line 2
   std::ostringstream line2;
   line2 << "2 " << std::setfill('0') << std::setw(5) << aTLE.mSatelliteNumber << ' ';

   double inclination = aTLE.mInclination * UtMath::cDEG_PER_RAD;
   line2 << std::showpoint << std::setfill('0') << std::setw(8) << std::fixed << std::setprecision(4) << inclination;

   double raan = aTLE.mRAAN * UtMath::cDEG_PER_RAD;
   line2 << ' ' << std::showpoint << std::setfill('0') << std::setw(8) << std::fixed << std::setprecision(4) << raan;

   std::string eccStr = [&aTLE]()
   {
      std::ostringstream oss;
      oss << std::showpoint << std::setfill('0') << std::fixed << std::setprecision(7) << aTLE.mEccentricity;
      return oss.str();
   }();
   eccStr.at(1) = ' '; // no decimal point
   line2 << eccStr.substr(1);

   double argumentOfPeriapsis = aTLE.mArgumentOfPeriapsis * UtMath::cDEG_PER_RAD;
   line2 << ' ' << std::showpoint << std::setfill('0') << std::setw(8) << std::fixed << std::setprecision(4)
         << argumentOfPeriapsis;

   double meanAnomaly = aTLE.mMeanAnomaly * UtMath::cDEG_PER_RAD;
   line2 << ' ' << std::showpoint << std::setfill('0') << std::setw(8) << std::fixed << std::setprecision(4) << meanAnomaly;

   double meanMotion = aTLE.GetMeanMotion() / UtMath::cTWO_PI * cSEC_PER_DAY;
   line2 << ' ' << std::showpoint << std::setfill('0') << std::setw(11) << std::fixed << std::setprecision(8) << meanMotion;

   line2 << std::setfill('0') << std::setw(5) << aTLE.mRevolutionNumber % 100000;

   int checksum2 = UtTwoLineElement::GetChecksum(line2.str());
   line2 << std::left << std::setw(1) << checksum2;
   aStream << line2.str() << std::endl;

   return aStream;
}

// private
double UtTwoLineElement::ExtractValue(const std::string& aLine, unsigned int aStartColumn, unsigned int aEndColumn)
{
   char         stringValue[32];
   double       returnValue = 0.0;
   unsigned int columnCount = aEndColumn - aStartColumn + 1;
   for (unsigned int i = 0; i < columnCount; ++i)
   {
      stringValue[i] = aLine[aStartColumn - 1 + i];
   }
   stringValue[columnCount] = '\0';
   if (columnCount >= 2)
   {
      char exponentSign = stringValue[columnCount - 2];
      if ((exponentSign == '-') || (exponentSign == '+'))
      {
         std::string mantissaStr;
         std::string expStr;
         mantissaStr.assign(stringValue, 0, (columnCount - 2));
         expStr.assign(stringValue, (columnCount - 2), 2);
         double exponent = atof(expStr.c_str());
         double mantissa = atof(mantissaStr.c_str()) / 100000.0;
         returnValue     = (mantissa * pow(10.0, exponent));
      }
      else
      {
         returnValue = atof(stringValue);
      }
   }
   else
   {
      returnValue = atof(stringValue);
   }
   return returnValue;
}

/** A delegator function that retrieves the earth angle corresponding to the epoch time
   of the TLE.
*/
double UtTwoLineElement::GetEarthAngle() const
{
   return GetEpoch().GetEarthAngle();
}

void UtTwoLineElement::PrintDiagnostic() const
{
   auto out = ut::log::info() << "Two-Line Element Diagnostics:";
   out.AddNote() << "Epoch Year: " << GetEpochYear();
   out.AddNote() << "Epoch DOY: " << GetEpochDayOfYear();
   out.AddNote() << "dm/dt: " << mMeanMotionFirstDerivative;                // 1st time derivative of mean motion
   out.AddNote() << "d2m/dt2: " << mMeanMotionSecondDerivative;             // 2nd time derivative of mean motion
   out.AddNote() << "mBstarDrag: " << mBstarDrag;                           //"mBstarDrag" drag term
   out.AddNote() << "inclination: " << mInclination * UtMath::cDEG_PER_RAD; // inclination
   out.AddNote() << "RAAN: " << mRAAN * UtMath::cDEG_PER_RAD; // right ascension of the ascending node (RAAN)
   out.AddNote() << "Eccentricity: " << mEccentricity;        // eccentricity
   out.AddNote() << "omega: " << mArgumentOfPeriapsis * UtMath::cDEG_PER_RAD;         // argument of the periapsis
   out.AddNote() << "Mean anomaly: " << mMeanAnomaly * UtMath::cDEG_PER_RAD;          // mean anomaly
   out.AddNote() << "Revs/Day: " << GetMeanMotion() / UtMath::cTWO_PI * cSEC_PER_DAY; // mean motion
}
