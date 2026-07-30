#include "NrmSnapshotReporter.hpp"

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>

int main()
{
   const std::string outputDirectory = "/tmp/nrm-snapshot-reporter-test";
   mkdir(outputDirectory.c_str(), 0700);

   nrm::ResourceSnapshot snapshot;
   snapshot.snapshotVersion      = 7;
   snapshot.simTime              = 12.5;
   snapshot.messages.transmitted = 3;

   nrm::NetworkSnapshot network;
   network.networkName   = "nrm_link16_test";
   network.networkType   = nrm::NetworkType::cLINK16;
   network.endpointCount = 2;
   network.onlineCount   = 2;
   nrm::WindowMetrics window;
   window.windowS             = 10.0;
   window.throughputBps.value = 1000.0;
   window.throughputBps.valid = true;
   window.throughputBps.unit  = "bit/s";
   network.windows.push_back(window);
   snapshot.networks.push_back(network);

   nrm::EndpointSnapshot endpoint;
   endpoint.endpointId   = "10.0.0.1";
   endpoint.platformName = "fighter";
   endpoint.commName     = "link16";
   endpoint.networkName  = network.networkName;
   endpoint.networkType  = nrm::NetworkType::cLINK16;
   endpoint.state        = nrm::ResourceState::cONLINE;
   snapshot.endpoints.push_back(endpoint);

   {
      WkNrm::SnapshotReporter reporter(outputDirectory);
      reporter.Enqueue(snapshot);
   }

   std::ifstream jsonInput(outputDirectory + "/resource_snapshots.jsonl");
   std::stringstream jsonBuffer;
   jsonBuffer << jsonInput.rdbuf();
   const std::string json = jsonBuffer.str();
   assert(json.find("\"snapshot_version\":7") != std::string::npos);
   assert(json.find("\"type\":\"LINK16\"") != std::string::npos);
   assert(json.find("\"platform\":\"fighter\"") != std::string::npos);
   assert(json.find("\"throughput_bps\"") != std::string::npos);

   std::ifstream csvInput(outputDirectory + "/network_summary.csv");
   std::stringstream csvBuffer;
   csvBuffer << csvInput.rdbuf();
   assert(csvBuffer.str().find("nrm_link16_test") != std::string::npos);
   assert(csvBuffer.str().find("1000.000") != std::string::npos);
   return 0;
}
