#ifndef NRM_AFSIM_NAVIGATION_PACKET_PARSER_HPP
#define NRM_AFSIM_NAVIGATION_PACKET_PARSER_HPP

// Parser for the whitespace-delimited .neh time-history format emitted by
// AFSIM 2.9 WsfNavigationErrors::WriteTimeHistory.

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
struct NavigationPacketParseResult
{
   bool valid = false;
   std::size_t line = 0;
   std::string reason;
   std::vector<NavigationSample> samples;
};

class AfsimNavigationPacketParser
{
public:
   NavigationPacketParseResult ParseFile(const std::string& aPath,
                                         const std::string& aPlatformName) const
   {
      std::ifstream input(aPath.c_str());
      if (!input)
      {
         NavigationPacketParseResult result;
         result.reason = "FILE_OPEN_FAILED";
         return result;
      }
      return Parse(input, aPlatformName);
   }

   NavigationPacketParseResult Parse(std::istream& aInput,
                                     const std::string& aPlatformName) const
   {
      NavigationPacketParseResult result;
      if (aPlatformName.empty())
      {
         result.reason = "PLATFORM_NAME_EMPTY";
         return result;
      }
      std::string line;
      std::size_t lineNumber = 0;
      double previousTime = -1.0;
      while (std::getline(aInput, line))
      {
         ++lineNumber;
         const std::size_t first = line.find_first_not_of(" \t\r");
         if (first == std::string::npos || line[first] == '#') continue;
         NavigationSample sample;
         std::string latitude;
         std::string longitude;
         double altitude = 0.0;
         double heading = 0.0;
         double inTrack = 0.0;
         double crossTrack = 0.0;
         double vertical = 0.0;
         double rss = 0.0;
         std::istringstream stream(line);
         if (!(stream >> sample.sampleTime >> sample.rawStatus >> latitude >> longitude >>
               altitude >> heading >> inTrack >> crossTrack >> vertical >> rss) ||
             HasTrailing(stream) || !std::isfinite(sample.sampleTime) ||
             sample.sampleTime < 0.0 || sample.sampleTime < previousTime ||
             !ParseStatus(sample.rawStatus, sample.mode, sample.statusCode) ||
             !ParseCoordinate(latitude, 90.0, sample.truthLatitudeDeg.value) ||
             !ParseCoordinate(longitude, 180.0, sample.truthLongitudeDeg.value) ||
             !AllFinite(altitude, heading, inTrack, crossTrack, vertical, rss) || rss < 0.0)
         {
            result.line = lineNumber;
            result.reason = "INVALID_NEH_RECORD";
            result.samples.clear();
            return result;
         }
         sample.platformName = aPlatformName;
         sample.navigationType =
            sample.mode == NavigationMode::cINS ? "INS" : "GNSS";
         sample.valid = true;
         sample.origin = DataOrigin::cREPLAY;
         sample.confidence = Confidence::cHIGH;
         Set(sample.truthLatitudeDeg, sample.truthLatitudeDeg.value, "deg", sample);
         Set(sample.truthLongitudeDeg, sample.truthLongitudeDeg.value, "deg", sample);
         Set(sample.truthAltitudeM, altitude, "m", sample);
         Set(sample.headingDeg, heading, "deg", sample);
         Set(sample.inTrackErrorM, inTrack, "m", sample);
         Set(sample.crossTrackErrorM, crossTrack, "m", sample);
         Set(sample.verticalErrorM, vertical, "m", sample);
         Set(sample.totalPositionErrorM, rss, "m", sample);
         result.samples.push_back(sample);
         previousTime = sample.sampleTime;
      }
      if (result.samples.empty())
      {
         result.reason = "NO_NAVIGATION_RECORDS";
         return result;
      }
      result.valid = true;
      result.reason = "NONE";
      return result;
   }

private:
   static bool HasTrailing(std::istringstream& aStream)
   {
      std::string extra;
      return static_cast<bool>(aStream >> extra);
   }

   static bool ParseStatus(const std::string& aText, NavigationMode& aMode,
                           int& aStatusCode)
   {
      if (aText == "PERFECT")
      {
         aMode = NavigationMode::cPERFECT;
         aStatusCode = 0;
         return true;
      }
      if (aText == "GPS1")
      {
         aMode = NavigationMode::cGPS_ACTIVE;
         aStatusCode = 1;
         return true;
      }
      if (aText == "GPS2")
      {
         aMode = NavigationMode::cGPS_DEGRADED;
         aStatusCode = 2;
         return true;
      }
      if (aText == "GPS3")
      {
         aMode = NavigationMode::cGPS_EXTERNAL;
         aStatusCode = 3;
         return true;
      }
      if (aText.size() > 3 && aText.substr(0, 3) == "INS")
      {
         char* endPtr = nullptr;
         const long index = std::strtol(aText.c_str() + 3, &endPtr, 10);
         if (endPtr == aText.c_str() + aText.size() && index > 0)
         {
            aMode = NavigationMode::cINS;
            aStatusCode = -static_cast<int>(index);
            return true;
         }
      }
      return false;
   }

   static bool ParseCoordinate(std::string aText, double aLimit, double& aValue)
   {
      if (aText.empty()) return false;
      int sign = 1;
      const char suffix = aText.back();
      if (suffix == 'S' || suffix == 's' || suffix == 'W' || suffix == 'w') sign = -1;
      if (suffix == 'N' || suffix == 'n' || suffix == 'S' || suffix == 's' ||
          suffix == 'E' || suffix == 'e' || suffix == 'W' || suffix == 'w')
         aText.pop_back();
      for (char& character : aText)
         if (character == ':') character = ' ';
      std::istringstream stream(aText);
      double degrees = 0.0;
      double minutes = 0.0;
      double seconds = 0.0;
      if (!(stream >> degrees)) return false;
      if (stream >> minutes)
      {
         if (!(stream >> seconds) || minutes < 0.0 || minutes >= 60.0 ||
             seconds < 0.0 || seconds >= 60.0 || HasTrailing(stream)) return false;
         degrees = std::abs(degrees) + minutes / 60.0 + seconds / 3600.0;
      }
      if (!std::isfinite(degrees)) return false;
      aValue = sign * degrees;
      return std::abs(aValue) <= aLimit;
   }

   static bool AllFinite(double a, double b, double c, double d, double e, double f)
   {
      return std::isfinite(a) && std::isfinite(b) && std::isfinite(c) &&
             std::isfinite(d) && std::isfinite(e) && std::isfinite(f);
   }

   static void Set(MetricValue<double>& aMetric, double aValue, const char* aUnit,
                   const NavigationSample& aSample)
   {
      aMetric.value = aValue;
      aMetric.unit = aUnit;
      aMetric.valid = true;
      aMetric.origin = aSample.origin;
      aMetric.confidence = aSample.confidence;
      aMetric.sampleTime = aSample.sampleTime;
      aMetric.reason = MetricReason::cNONE;
   }
};
} // namespace nrm

#endif
