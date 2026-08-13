#include "nrm/AfsimNavigationPacketParser.hpp"

#include <cassert>
#include <cmath>
#include <sstream>

int main()
{
   const nrm::AfsimNavigationPacketParser parser;
   std::istringstream input(
      "#--time-- stat -----lat----- -----lon------ ----alt--- --hdg-- -it-error-- -xt-error-- --v-error-- ----rss----\n"
      "0.0000 INS1 35:30:00.000N 118:15:00.000E 1000.000 90.000 1.000 2.000 3.000 3.742\n"
      "10.0000 GPS1 35:30:00.000N 118:15:00.000E 1001.000 91.000 8.000 2.000 3.000 8.775\n");
   const nrm::NavigationPacketParseResult result = parser.Parse(input, "aircraft-01");
   assert(result.valid);
   assert(result.samples.size() == 2);
   assert(result.samples[0].mode == nrm::NavigationMode::cINS);
   assert(result.samples[0].statusCode == -1);
   assert(std::abs(result.samples[0].truthLatitudeDeg.value - 35.5) < 1.0e-9);
   assert(std::abs(result.samples[0].truthLongitudeDeg.value - 118.25) < 1.0e-9);
   assert(result.samples[1].mode == nrm::NavigationMode::cGPS_ACTIVE);
   assert(result.samples[1].origin == nrm::DataOrigin::cREPLAY);

   std::istringstream invalidStatus(
      "0.0 BAD 35:00:00N 118:00:00E 0 0 0 0 0 0\n");
   const nrm::NavigationPacketParseResult badStatus =
      parser.Parse(invalidStatus, "aircraft-01");
   assert(!badStatus.valid);
   assert(badStatus.line == 1);
   assert(badStatus.samples.empty());

   std::istringstream decreasingTime(
      "10.0 GPS2 35:00:00N 118:00:00E 0 0 0 0 0 0\n"
      "9.0 GPS2 35:00:00N 118:00:00E 0 0 0 0 0 0\n");
   assert(!parser.Parse(decreasingTime, "aircraft-01").valid);
   return 0;
}
