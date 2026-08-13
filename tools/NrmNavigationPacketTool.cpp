// Converts an AFSIM WsfNavigationErrors .neh time history into normalized JSONL.

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "nrm/AfsimNavigationPacketParser.hpp"
#include "nrm/NetworkTypeUtils.hpp"

namespace
{
std::string EscapeJson(const std::string& aValue)
{
   std::ostringstream output;
   for (char character : aValue)
   {
      switch (character)
      {
      case '\\': output << "\\\\"; break;
      case '"': output << "\\\""; break;
      case '\n': output << "\\n"; break;
      case '\r': output << "\\r"; break;
      case '\t': output << "\\t"; break;
      default: output << character; break;
      }
   }
   return output.str();
}

void WriteMetric(std::ostream& aOutput, const nrm::MetricValue<double>& aMetric)
{
   aOutput << "{\"value\":" << aMetric.value << ",\"unit\":\""
           << EscapeJson(aMetric.unit) << "\",\"valid\":"
           << (aMetric.valid ? "true" : "false") << ",\"source\":\""
           << nrm::ToString(aMetric.origin) << "\",\"confidence\":\""
           << nrm::ToString(aMetric.confidence) << "\",\"reasonCode\":\""
           << nrm::ToString(aMetric.reason) << "\",\"sampleTime\":"
           << aMetric.sampleTime << '}';
}

void WriteSample(std::ostream& aOutput, const nrm::NavigationSample& aSample)
{
   aOutput << "{\"schemaVersion\":\"nrm.navigation_sample.v1\""
           << ",\"packetFormat\":\"AFSIM_NAVIGATION_ERROR_HISTORY_NEH\""
           << ",\"platformName\":\"" << EscapeJson(aSample.platformName)
           << "\",\"rawStatus\":\"" << EscapeJson(aSample.rawStatus)
           << "\",\"mode\":\"" << nrm::ToString(aSample.mode)
           << "\",\"statusCode\":" << aSample.statusCode
           << ",\"valid\":" << (aSample.valid ? "true" : "false")
           << ",\"source\":\"" << nrm::ToString(aSample.origin)
           << "\",\"confidence\":\"" << nrm::ToString(aSample.confidence)
           << "\",\"sampleTime\":" << std::fixed << std::setprecision(6)
           << aSample.sampleTime << ",\"truthLatitudeDeg\":";
   WriteMetric(aOutput, aSample.truthLatitudeDeg);
   aOutput << ",\"truthLongitudeDeg\":";
   WriteMetric(aOutput, aSample.truthLongitudeDeg);
   aOutput << ",\"truthAltitudeM\":";
   WriteMetric(aOutput, aSample.truthAltitudeM);
   aOutput << ",\"headingDeg\":";
   WriteMetric(aOutput, aSample.headingDeg);
   aOutput << ",\"inTrackErrorM\":";
   WriteMetric(aOutput, aSample.inTrackErrorM);
   aOutput << ",\"crossTrackErrorM\":";
   WriteMetric(aOutput, aSample.crossTrackErrorM);
   aOutput << ",\"verticalErrorM\":";
   WriteMetric(aOutput, aSample.verticalErrorM);
   aOutput << ",\"totalPositionErrorM\":";
   WriteMetric(aOutput, aSample.totalPositionErrorM);
   aOutput << "}\n";
}
} // namespace

int main(int argc, char* argv[])
{
   if (argc != 3)
   {
      std::cerr << "Usage: nrm_navigation_packet_tool <platform-name> <input.neh>\n";
      return 2;
   }
   const nrm::NavigationPacketParseResult result =
      nrm::AfsimNavigationPacketParser().ParseFile(argv[2], argv[1]);
   if (!result.valid)
   {
      std::cerr << "Error: navigation packet parse failed: " << result.reason;
      if (result.line != 0) std::cerr << " at line " << result.line;
      std::cerr << '\n';
      return 1;
   }
   for (const nrm::NavigationSample& sample : result.samples)
      WriteSample(std::cout, sample);
   return 0;
}
