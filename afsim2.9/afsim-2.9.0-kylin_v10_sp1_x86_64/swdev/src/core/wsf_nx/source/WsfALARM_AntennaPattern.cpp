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

#include "WsfALARM_AntennaPattern.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>

#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "WsfEM_Util.hpp"
#include "WsfScenario.hpp"
#include "WsfSystemLog.hpp"

WsfALARM_AntennaPattern::WsfALARM_AntennaPattern(ALARM_Data* aBaseDataPtr)
   : WsfAntennaPattern(aBaseDataPtr == nullptr ? new ALARM_Data() : aBaseDataPtr)
{
   mSharedDataPtr = dynamic_cast<ALARM_Data*>(mSharedDataPtr);
}

//! Factory method for WsfAntennaPatternTypes.
//! This is called by WsfAntennaPatternTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
WsfAntennaPattern* WsfALARM_AntennaPattern::ObjectFactory(const std::string& aTypeName)
{
   WsfAntennaPattern* patternPtr = nullptr;
   if (aTypeName == "alarm_pattern")
   {
      patternPtr = new WsfALARM_AntennaPattern(nullptr);
   }
   return patternPtr;
}

// virtual
WsfAntennaPattern* WsfALARM_AntennaPattern::Clone() const
{
   return new WsfALARM_AntennaPattern(*this);
}

// virtual
double WsfALARM_AntennaPattern::GetAzimuthBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const
{
   return ApplyEBS(AlarmData()->mPatternMap[AlarmData()->mSetPolarization].mAzBeamwidth, aEBS_Azimuth, 0.0);
}

// virtual
double WsfALARM_AntennaPattern::GetElevationBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const
{
   return ApplyEBS(AlarmData()->mPatternMap[AlarmData()->mSetPolarization].mElBeamwidth, 0.0, aEBS_Elevation);
}

// virtual
double WsfALARM_AntennaPattern::GetPeakGain(double aFrequency) const
{
   return AlarmData()->PerformGainAdjustment(aFrequency, AlarmData()->mPatternMap[AlarmData()->mSetPolarization].mPeakGain);
}

// =================================================================================================
// Nested class 'ALARM_Data'.
// =================================================================================================
WsfALARM_AntennaPattern::ALARM_Data::ALARM_Data()
   : BaseData()
   , mApertureShape(cAS_UNKNOWN)
   , mMinGain()
   , mSetPolarization(WsfEM_Types::cPOL_DEFAULT)
   , mPatternMap()
   , mOneMinusE_2(0.0)
   , mGainCorrection(1.0)
   , mTitle()
   , mClassification()
   , mWavelength()
   , mInputIs2D(true)
   , mInputIsDB(true)
{
}

bool WsfALARM_AntennaPattern::ALARM_Data::ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "file")
   {
      aInput.ReadValueQuoted(mFileName);
      mFileName = aInput.LocateFile(mFileName);
      try
      {
         ReadPattern(WsfScenario::FromInput(aInput), mFileName);
      }
      catch (std::exception& exception)
      {
         throw UtInput::BadValue(aInput,
                                 "While processing ALARM antenna pattern file: " + mFileName +
                                    "\nReason: " + std::string(exception.what()));
      }
   }
   else if (command == "polarization")
   {
      std::string polarization;
      aInput.ReadValue(polarization);
      if (!WsfEM_Util::StringToEnum(mSetPolarization, polarization))
      {
         throw UtInput::BadValue(aInput, "Invalid polarization: " + polarization);
      }
   }
   else if (command == "gain_correction")
   {
      // Alias for gain_adjustment in the base class
      aInput.ReadValueOfType(mGainAdjustment, UtInput::cRATIO);
      aInput.ValueGreater(mGainAdjustment, 0.0);
   }
   else
   {
      myCommand = WsfAntennaPattern::BaseData::ProcessInput(aPattern, aInput);
   }
   return myCommand;
}

bool WsfALARM_AntennaPattern::ALARM_Data::Initialize(WsfAntennaPattern& aAntennaPattern)
{
   if (!InitializeBase())
   {
      return false;
   }
   return BaseData::Initialize(aAntennaPattern);
   ;
}

// virtual
double WsfALARM_AntennaPattern::ALARM_Data::GetGain(double aFrequency,
                                                    double aTargetAz,
                                                    double aTargetEl,
                                                    double aEBS_Az,
                                                    double aEBS_El)
{
   // If the table is symmetric then change the incoming angles into the proper domain.
   //
   // NOTE: SUPPRESSOR had some fabs() values that checked for the min/max values being CLOSE to zero
   //       while ALARM only checked for zero. In order to eliminate the fabs() calls, Read2D_File() will
   //       set the min/max value to zero if they are 'close'.

   PatternData& patternData = mPatternMap[mSetPolarization];

   double azLook = aTargetAz;     // Assume non-symmetric range
   if (patternData.mAzMin == 0.0) // azimuth range is [0 .. mAzMax]
   {
      azLook = fabs(aTargetAz);
   }
   else if (patternData.mAzMax == 0.0) // azimuth range is [mAzMin .. 0]
   {
      azLook = -fabs(aTargetAz);
   }

   double elLook = aTargetEl;     // Assume non-symmetric range
   if (patternData.mElMin == 0.0) // elevation range is [0 .. mElMax]
   {
      elLook = fabs(aTargetEl);
   }
   else if (patternData.mElMax == 0.0) // elevation range is [mElMax .. 0]
   {
      elLook = -fabs(aTargetEl);
   }

   // NOTE: 'mMinimumGain' in the base class and 'mMinGain' in this class have two different functions,
   //       so don't get them messed up. 'mMinimumGain' is the minimum value that will be RETURNED,
   //       and 'mMinGain' is a normalized minimum gain that is only used internally.

   double gain = mMinimumGain;
   if ((azLook >= patternData.mAzMin) && (azLook <= patternData.mAzMax) && (elLook >= patternData.mElMin) &&
       (elLook <= patternData.mElMax))
   {
      switch (mApertureShape)
      {
      case cAS_CIRCULAR:
         gain = GetCircularApertureGain(azLook, elLook);
         break;

      case cAS_RECTANGULAR:
         gain = GetRectangularApertureGain(azLook, elLook);
         break;

      case cAS_ELLIPTICAL:
         gain = GetEllipticalApertureGain(azLook, elLook);
         break;

      default:
         break;
      }
      // Un-normalized the gain.
      gain *= patternData.mPeakGain;
      // Perform user-specified gain correction/adjustment and lower bound limiting.
      gain = PerformGainAdjustment(aFrequency, gain);
   }
   return gain;
}

//! Perform common initialization for derived classes.
bool WsfALARM_AntennaPattern::ALARM_Data::InitializeBase()
{
   // Capture the minimum and maximum angle values.
   // Note that for a circular aperture, the elevation values may not be present.

   if (find(mPolarizations.begin(), mPolarizations.end(), mSetPolarization) == mPolarizations.end())
   {
      std::ostringstream oss;
      if ((mPolarizations.size() == 1) && (mPolarizations[0] == WsfEM_Types::cPOL_DEFAULT))
      {
         oss << "ALARM antenna file " << mFileName << " does not define specific polarizations, but\n";
         oss << " a polarization of '" << WsfEM_Util::EnumToString(mSetPolarization);
         oss << "' was specified using the 'polarization' command in the 'antenna_pattern' input block.\n";
         oss << " Please remove this input.";
      }
      else
      {
         oss << "At least one specific polarization is defined in ALARM antenna file " << mFileName
             << "\nChoose one of the following using the 'polarization' command in the 'antenna_pattern' input block: ";
         for (auto mPolarization : mPolarizations)
         {
            oss << "\n " << WsfEM_Util::EnumToString(mPolarization);
         }
      }
      throw UtException(oss.str());
   }

   PatternData& patternData = mPatternMap[mSetPolarization];

   patternData.mAzMin = patternData.mAzAngles.front();
   patternData.mAzMax = patternData.mAzAngles.back();

   patternData.mElMin = 0.0;                // Assume circular aperture...
   patternData.mElMax = UtMath::cPI_OVER_2; // ...
   if (!patternData.mElAngles.empty())
   {
      patternData.mElMin = patternData.mElAngles.front();
      patternData.mElMax = patternData.mElAngles.back();
   }

   // Compute the eccentricity for elliptical patterns.

   if (mApertureShape == cAS_ELLIPTICAL)
   {
      if ((patternData.mAzBeamwidth <= 0.0) || (patternData.mElBeamwidth <= 0.0))
      {
         std::ostringstream oss;
         oss << "Azimuth and elevation beamwidth must be greater than zero for elliptical pattern in file " << mFileName;
         throw UtException(oss.str());
      }

      // Determine the major axis (a) and the minor axis (b) of the ellipse.
      double a = 0.5 * patternData.mAzBeamwidth;
      double b = 0.5 * patternData.mElBeamwidth;
      if (b > a)
      {
         std::swap(a, b);
      }
      // Compute the eccentricity.
      double e     = sqrt((a * a) - (b * b)) / a;
      mOneMinusE_2 = 1.0 - (e * e);
   }

   return true;
}

//! Read an ALARM antenna pattern file.
//! @param aScenario The scenario
//! @param aFileName The file to be read.
//! @throws A UtException if an error occurs.
// virtual
void WsfALARM_AntennaPattern::ALARM_Data::ReadPattern(WsfScenario& aScenario, const std::string& aFileName)
{
   std::string              line;
   std::vector<std::string> words;
   double                   peakGain;

   std::ifstream ifs(aFileName.c_str());
   if (!ifs)
   {
      throw UtException("Unable to open file");
   }

   // Read the classification and title lines

   ReadLine(ifs, mClassification);
   ReadLine(ifs, mTitle);

   // Read the pattern header
   //   <wavelength> <gain> <min_gain> <db | lin> <2d | 3d> [<rect | ellip | circ>]
   ReadLine(ifs, line);
   ParseLine(line, words);
   if (words.size() < 5)
   {
      throw UtException("Invalid header");
   }
   ConvertValue(words[0], mWavelength);
   ConvertValue(words[1], peakGain);
   ConvertValue(words[2], mMinGain);

   mInputIsDB = false;
   if ((words[3] == "DB") || (words[3] == "db"))
   {
      mInputIsDB = true;
      peakGain   = UtMath::DB_ToLinear(peakGain);
      mMinGain   = UtMath::DB_ToLinear(mMinGain);
   }
   else if ((words[3] == "lin") || (words[3] == "LIN"))
   {
   }
   else
   {
      throw UtException("Unsupported units: " + words[3]);
   }

   mInputIs2D = true;
   if ((words[4] != "2D") && (words[4] != "2d"))
   {
      throw UtException("Unsupported pattern type: " + words[4]);
   }

   mApertureShape = cAS_RECTANGULAR;
   if (words.size() > 5)
   {
      if ((words[5] == "RECT") || (words[5] == "rect"))
      {
         mApertureShape = cAS_RECTANGULAR;
      }
      else if ((words[5] == "ELLIP") || (words[5] == "ellip"))
      {
         mApertureShape = cAS_ELLIPTICAL;
      }
      else if ((words[5] == "CIRC") || (words[5] == "circ"))
      {
         mApertureShape = cAS_CIRCULAR;
      }
      else
      {
         throw UtException("Unsupported aperture shape: " + words[5]);
      }
   }

   if (words.size() > 6)
   {
      for (size_t i = 6; i < words.size(); ++i)
      {
         if (words[i] == "HORIZ")
         {
            mPolarizations.push_back(WsfEM_Types::cPOL_HORIZONTAL);
         }
         else if (words[i] == "VERT")
         {
            mPolarizations.push_back(WsfEM_Types::cPOL_VERTICAL);
         }
         else
         {
            throw UtException("Unsupported polarization type: " + words[i]);
         }
      }
   }
   else
   {
      mPolarizations.push_back(WsfEM_Types::cPOL_DEFAULT);
   }

   for (size_t i = 0; i < mPolarizations.size(); ++i)
   {
      if (i > 0)
      {
         ReadLine(ifs, line);
         ParseLine(line, words);
         if ((words[0].find("HORIZ") != std::string::npos) && (mPolarizations[i] == WsfEM_Types::cPOL_HORIZONTAL))
         {
         }
         else if ((words[0].find("VERT") != std::string::npos) && (mPolarizations[i] == WsfEM_Types::cPOL_VERTICAL))
         {
         }
         else
         {
            throw UtException("Unmatched polarization type: " + words[0]);
         }

         ReadLine(ifs, line);
         ParseLine(line, words);
         ConvertValue(words[0], peakGain);
         if (mInputIsDB)
         {
            peakGain = UtMath::DB_ToLinear(peakGain);
         }
      }
      mPatternMap[mPolarizations[i]].mPeakGain = peakGain;
      Read2D_File(ifs, mPolarizations[i]);
   }

   aScenario.GetSystemLog().WriteLogEntry("file " + aFileName);
   aScenario.GetSystemLog().WriteLogEntry("version " + mTitle);
}

// private
void WsfALARM_AntennaPattern::ALARM_Data::ConvertValue(const std::string& aString, double& aValue) const
{
   std::istringstream iss(aString);
   if (!(iss >> aValue))
   {
      throw UtException("Invalid numeric value: " + aString);
   }
}

// private
void WsfALARM_AntennaPattern::ALARM_Data::ConvertValue(const std::string& aString, int& aValue) const
{
   std::istringstream iss(aString);
   if (!(iss >> aValue))
   {
      throw UtException("Invalid numeric value: " + aString);
   }
}

//! Return the lower index of the interval that contains the specified value.
//! @param aTable The table to be searched.
//! @param aValue The value to be found in the table.
//! @return The lower bound of the index of the interval that contains the specified value.
//! @note The caller must have already ensured that the value is within the limits of the table.
//! @note SUPPRESSOR treated a value at the right endpoint as not being in the table while we do...
// protected
std::vector<double>::size_type WsfALARM_AntennaPattern::ALARM_Data::GetIndex(const std::vector<double>& aTable,
                                                                             double                     aValue) const
{
   TableIndex tableSize = aTable.size();
   TableIndex ileft     = 0;
   TableIndex iright    = tableSize;
   while (iright != (ileft + 1))
   {
      TableIndex imidle = (ileft + iright) / 2;
      if (aValue >= aTable[imidle])
      {
         ileft = imidle;
      }
      else
      {
         iright = imidle;
      }
   }

   // If the incoming value exactly matched the right endpoint then we have to adjust the return so as
   // to return a lower bound of the interval for which the upper bound is also in the table.

   if (ileft >= (tableSize - 1))
   {
      ileft = tableSize - 2;
   }
   return ileft;
}

// protected
double WsfALARM_AntennaPattern::ALARM_Data::GetCircularApertureGain(double aAzLook, double aElLook)
{
   // From SUPPRESSOR antgr.f, SUBROUTINE GEXTRP, ISHAPE = 1

   PatternData& patternData = mPatternMap[mSetPolarization];

   double rssang = sqrt(aAzLook * aAzLook + aElLook * aElLook);
   double tazang = rssang; // tazang = sign(rssang, aAzLook);
   if (aAzLook < 0.0)
   {
      tazang = -rssang;
   }
   double gsubt = mMinGain;
   if ((tazang >= patternData.mAzMin) && (tazang <= patternData.mAzMax))
   {
      TableIndex iazt   = GetIndex(patternData.mAzAngles, tazang);
      double     delaz  = tazang - patternData.mAzAngles[iazt];
      double     azfact = delaz / (patternData.mAzAngles[iazt + 1] - patternData.mAzAngles[iazt]);
      gsubt             = (patternData.mAzGains[iazt] * (1.0 - azfact)) + (patternData.mAzGains[iazt + 1] * azfact);
   }
   return gsubt;
}

// protected
double WsfALARM_AntennaPattern::ALARM_Data::GetEllipticalApertureGain(double aAzLook, double aElLook)
{
   // From SUPPRESSOR antgr.f, SUBROUTINE GEXTRP, ISHAPE=2

   // Rotate back to the primary axes using an elliptical projection that
   // is based on the beamwidth eccentricity.

   // NOTE: The eccentricity 'e' and (1 - e^2) is computed in ReadPattern;

   PatternData& patternData = mPatternMap[mSetPolarization];

   double tazang;
   double telang;
   if (patternData.mElBeamwidth <= patternData.mAzBeamwidth) // Major axis is along azimuth axis
   {
      double x = aAzLook;
      double y = aElLook;
      double a = sqrt((x * x) + ((y * y) / mOneMinusE_2));
      double b = sqrt((a * a) * mOneMinusE_2);
      tazang   = a; // tazang = sign(a, aAzLook);
      if (aAzLook < 0.0)
      {
         tazang = -a;
      }
      telang = b; // telang = sign(b, aElLook);
      if (aElLook < 0.0)
      {
         telang = -b;
      }
   }
   else // Major axis is along elevation axis
   {
      double x = aElLook;
      double y = aAzLook;
      double a = sqrt((x * x) + ((y * y) / mOneMinusE_2));
      double b = sqrt((a * a) * mOneMinusE_2);
      tazang   = b; // tazang = sign(b, aAzLook);
      if (aAzLook < 0.0)
      {
         tazang = -b;
      }
      telang = a; // telang = sign(a, aElLook);
      if (aElLook < 0.0)
      {
         telang = -a;
      }
   }

   // Ensure the projected angles are within the table limits.

   double gsubt = mMinGain;
   if ((tazang >= patternData.mAzMin) && (tazang <= patternData.mAzMax) && (telang >= patternData.mElMin) &&
       (telang <= patternData.mElMax))
   {
      // Get the indices for the projected angles.

      TableIndex iazt = GetIndex(patternData.mAzAngles, tazang);
      TableIndex jelt = GetIndex(patternData.mElAngles, telang);

      // Determine the interpolation factors for the projected angles.

      double delaz  = aAzLook - patternData.mAzAngles[iazt];
      double azfact = delaz / (patternData.mAzAngles[iazt + 1] - patternData.mAzAngles[iazt]);
      double delel  = aElLook - patternData.mElAngles[jelt];
      double elfact = delel / (patternData.mElAngles[jelt + 1] - patternData.mElAngles[jelt]);

      // Compute angle and place in correct quadrant to define an arc
      // with a curvature defined by TAZANG and TELANG.

      double tang = UtMath::cPI_OVER_2;
      if (aAzLook != 0.0)
      {
         tang = fabs(atan(aElLook / aAzLook));
      }

      double tval[2][2];
      for (int is = 0; is < 2; ++is)
      {
         for (int js = 0; js < 2; ++js)
         {
            // Determine the on-axis antenna gains at the azimuth and elevation angles.
            // NOTE: We don't have to worry about the array indices here because we've already
            //       ensured the values are in range.
            double taz = patternData.mAzGains[iazt + is];
            double tel = patternData.mElGains[jelt + js];

            // Determine the off-axis antenna gains at the azimuth and elevation angles.

            if (taz > tel)
            {
               double anfact = fabs((UtMath::cPI_OVER_2 - tang) / UtMath::cPI_OVER_2);
               tval[is][js]  = tel * pow(taz / tel, anfact);
            }
            else
            {
               double anfact = tang / UtMath::cPI_OVER_2;
               tval[is][js]  = taz * pow(tel / taz, anfact);
            }
         }
      }

      // Determine the two dimensional transmit antenna pattern's
      // left-upper, left-lower, right-upper and right-lower antenna gains.

      double tgltup = std::max(mMinGain, tval[0][1]);
      double tgltlo = std::max(mMinGain, tval[0][0]);
      double tgrtup = std::max(mMinGain, tval[1][1]);
      double tgrtlo = std::max(mMinGain, tval[1][0]);

      // Determine interpolating factors.

      double gazj   = (tgltlo * (1.0 - azfact)) + (tgrtlo * azfact);
      double gazjp1 = (tgltup * (1.0 - azfact)) + (tgrtup * azfact);

      gsubt = (gazj * (1.0 - elfact)) + (gazjp1 * elfact);
   }
   return std::max(gsubt, mMinGain);
}

// protected
double WsfALARM_AntennaPattern::ALARM_Data::GetRectangularApertureGain(double aAzLook, double aElLook)
{
   // From SUPPRESSOR antgr.f, SUBROUTINE GEXTRP, ISHAPE = 3

   PatternData& patternData = mPatternMap[mSetPolarization];

   TableIndex iaz    = GetIndex(patternData.mAzAngles, aAzLook);
   double     delaz  = aAzLook - patternData.mAzAngles[iaz];
   double     azfact = delaz / (patternData.mAzAngles[iaz + 1] - patternData.mAzAngles[iaz]);
   double     azgain = (patternData.mAzGains[iaz] * (1.0 - azfact)) + (patternData.mAzGains[iaz + 1] * azfact);

   TableIndex jel    = GetIndex(patternData.mElAngles, aElLook);
   double     delel  = aElLook - patternData.mElAngles[jel];
   double     elfact = delel / (patternData.mElAngles[jel + 1] - patternData.mElAngles[jel]);
   double     elgain = (patternData.mElGains[jel] * (1.0 - elfact)) + (patternData.mElGains[jel + 1] * elfact);

   double gsubt = azgain * elgain;
   return std::max(gsubt, mMinGain);
}

// private
void WsfALARM_AntennaPattern::ALARM_Data::ParseLine(const std::string& aLine, std::vector<std::string>& aWords) const
{
   // The contents of 'space' are the same characters tested by <cctype> 'isspace'. The '\r' is needed
   // so Windows CR-LF file endings are treated properly on Unix (otherwise the last word contains '\r').
   static const std::string space(" \f\n\r\t\v");
   static const std::string spaceAndComma(" \f\n\r\t\v,");

   aWords.clear();

   std::string::size_type begPos = 0;
   while ((begPos != std::string::npos) && (begPos < aLine.size()))
   {
      // Skip over any intervening whitespace and locate the start of the next value.
      begPos = aLine.find_first_not_of(space, begPos);
      if (begPos != std::string::npos)
      {
         if (aLine[begPos] != ',')
         {
            // Find the end of the string.
            std::string::size_type endPos = aLine.find_first_of(spaceAndComma, begPos);
            if (endPos != std::string::npos)
            {
               aWords.push_back(aLine.substr(begPos, endPos - begPos));
               // A comma may be used to separate words, but there may be white space in front of the comma
               // that needs to be skipped. Therefore, skip any white that follows the word and check to see
               // if the following character is a comma.
               begPos = aLine.find_first_not_of(space, endPos);
               if ((begPos != std::string::npos) && (aLine[begPos] == ','))
               {
                  ++begPos; // Skip over the comma that is the logical separator
               }
            }
            else
            {
               // Last value on the line with no trailing white space.
               aWords.push_back(aLine.substr(begPos));
               begPos = aLine.size();
            }
         }
         else
         {
            // This is either a comma at the start of the line (preceded by optional white space) or
            // two adjacent commas (separated by optional white space). This represents an empty value.
            aWords.emplace_back();
            ++begPos;
         }
      }
   }
}

// private
void WsfALARM_AntennaPattern::ALARM_Data::Read2D_File(std::istream& aIn, WsfEM_Types::Polarization aPolarization)
{
   std::string              line;
   std::vector<std::string> words;
   int                      i;
   int                      azPoints;
   int                      elPoints;
   PatternData&             patternData = mPatternMap[aPolarization];

   // Read the azimuth parameters
   //   <az_beamwidth> <az_points> [ <az_min_angle> <az_incr> ]

   ReadLine(aIn, line);
   ParseLine(line, words);
   if (words.size() < 2)
   {
      throw UtException("Invalid azimuth header");
   }
   ConvertValue(words[0], patternData.mAzBeamwidth);
   patternData.mAzBeamwidth *= UtMath::cRAD_PER_DEG;
   ConvertValue(words[1], azPoints);
   double azMin = 0.0;
   double azInc = 0.0;
   if (words.size() > 2)
   {
      if (words.size() < 4)
      {
         throw UtException("Invalid azimuth header");
      }
      ConvertValue(words[2], azMin);
      ConvertValue(words[3], azInc);
   }

   // Read the elevation parameters
   //   <el_beamwidth> <el_points> [ <el_min_angle> <el_incr> ]

   ReadLine(aIn, line);
   ParseLine(line, words);
   if (words.size() < 2)
   {
      throw UtException("Invalid elevation header");
   }
   ConvertValue(words[0], patternData.mElBeamwidth);
   patternData.mElBeamwidth *= UtMath::cRAD_PER_DEG;
   ConvertValue(words[1], elPoints);
   double elMin = 0.0;
   double elInc = 0.0;
   if (words.size() > 2)
   {
      if (words.size() < 4)
      {
         throw UtException("Invalid elevation header");
      }
      ConvertValue(words[2], elMin);
      ConvertValue(words[3], elInc);
   }

   double az;
   double gain;

   // Read the azimuth data

   patternData.mAzAngles.resize(azPoints);
   patternData.mAzGains.resize(azPoints);
   ReadLine(aIn, line); // Skip the AZCUT line
   for (i = 0; i < azPoints; ++i)
   {
      if (azInc != 0.0)
      {
         aIn >> gain;
         az = azMin + (i * azInc);
      }
      else
      {
         aIn >> az >> gain;
      }
      if (!aIn)
      {
         throw UtException("Error reading azimuth cut");
      }
      if (mInputIsDB)
      {
         gain = UtMath::DB_ToLinear(gain);
      }
      patternData.mAzAngles[i] = az * UtMath::cRAD_PER_DEG;
      patternData.mAzGains[i]  = gain;
   }
   ReadLine(aIn, line); // Skip the remainder of the last line

   if (azPoints < 2)
   {
      throw UtException("Table must have at least 2 azimuth points");
   }

   // Check for near-zero values at the front or back and clamp them to zero.

   if (fabs(patternData.mAzAngles.front()) < 0.0001)
   {
      patternData.mAzAngles.front() = 0.0;
   }
   else if (fabs(patternData.mAzAngles.back()) < 0.0001)
   {
      patternData.mAzAngles.back() = 0.0;
   }

   // Read the elevation cut.
   // This will NOT be read for a SUPPRESSOR CIRCULAR aperture.

   if (mApertureShape != cAS_CIRCULAR)
   {
      if (elPoints < 2)
      {
         throw UtException("Table must have at least 2 elevation points");
      }

      patternData.mElAngles.resize(elPoints);
      patternData.mElGains.resize(elPoints);

      double el;

      ReadLine(aIn, line); // Skip the ELCUT line
      for (i = 0; i < elPoints; ++i)
      {
         if (elInc != 0.0)
         {
            aIn >> gain;
            el = elMin + (i * elInc);
         }
         else
         {
            aIn >> el >> gain;
         }
         if (!aIn)
         {
            throw UtException("Error reading elevation cut");
         }
         if (mInputIsDB)
         {
            gain = UtMath::DB_ToLinear(gain);
         }
         patternData.mElAngles[i] = el * UtMath::cRAD_PER_DEG;
         patternData.mElGains[i]  = gain;
      }
      ReadLine(aIn, line); // Skip the remainder of the last line

      // Check for near-zero values at the front or back and clamp them to zero.

      if (fabs(patternData.mElAngles.front()) < 0.0001)
      {
         patternData.mElAngles.front() = 0.0;
      }
      else if (fabs(patternData.mElAngles.back()) < 0.0001)
      {
         patternData.mElAngles.back() = 0.0;
      }
   }
}

// private
void WsfALARM_AntennaPattern::ALARM_Data::ReadLine(std::istream& aIn, std::string& aLine)
{
   if (!std::getline(aIn, aLine))
   {
      throw UtException("Unexpected end-of-file");
   }
}
