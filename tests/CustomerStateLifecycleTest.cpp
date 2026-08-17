#include "nrm/CustomerIngestionState.hpp"
#include "nrm/CustomerSnapshotAssembler.hpp"

#include <cassert>

int main()
{
   nrm::CustomerIngestionState state(4);
   auto first = state.Accept("run-a", "m-1", nrm::CustomerMessageDomain::cRESOURCE,
                             true, 10.0);
   assert(first.status == nrm::CustomerIngestionStatus::cACCEPTED);
   assert(!first.newRun);
   assert(state.Accept("run-a", "m-1", nrm::CustomerMessageDomain::cRESOURCE,
                       true, 10.0).status == nrm::CustomerIngestionStatus::cDUPLICATE);
   assert(state.Accept("run-a", "m-2", nrm::CustomerMessageDomain::cRESOURCE,
                       true, 9.0).status == nrm::CustomerIngestionStatus::cSTALE);
   const auto preview = state.Preview("run-a", "m-preview",
                                      nrm::CustomerMessageDomain::cNAVIGATION,
                                      true, 12.0);
   assert(preview.status == nrm::CustomerIngestionStatus::cACCEPTED);
   assert(state.Preview("run-a", "m-preview",
                        nrm::CustomerMessageDomain::cNAVIGATION,
                        true, 12.0).status ==
          nrm::CustomerIngestionStatus::cACCEPTED);
   const auto newRun = state.Accept("run-b", "m-3",
                                    nrm::CustomerMessageDomain::cRESOURCE,
                                    true, 1.0);
   assert(newRun.status == nrm::CustomerIngestionStatus::cACCEPTED);
   assert(newRun.newRun);
   assert(state.Accept("run-a", "m-4", nrm::CustomerMessageDomain::cRESOURCE,
                       true, 11.0).status == nrm::CustomerIngestionStatus::cSTALE);

   nrm::ResourceSnapshot current;
   current.snapshotVersion = 7;
   current.simTime = 20.0;
   current.navigation.valid = true;
   nrm::NavigationSample oldNavigation;
   oldNavigation.platformId = "platform-a";
   oldNavigation.platformName = "platform-a";
   oldNavigation.sampleTime = 18.0;
   current.navigation.platforms.push_back(oldNavigation);
   current.environment.valid = true;
   current.environment.providerId = "environment-a";

   nrm::ResourceSnapshot incoming;
   incoming.simTime = 21.0;
   nrm::NetworkSnapshot network;
   network.networkId = "link16-primary";
   incoming.networks.push_back(network);
   const nrm::ResourceSnapshot merged =
      nrm::CustomerSnapshotAssembler::MergeResources(current, incoming, false);
   assert(merged.snapshotVersion == 8);
   assert(merged.navigation.platforms.size() == 1);
   assert(merged.environment.providerId == "environment-a");

   nrm::NavigationSample platformB;
   platformB.platformId = "platform-b";
   platformB.platformName = "platform-b";
   platformB.sampleTime = 19.0;
   const nrm::ResourceSnapshot withB =
      nrm::CustomerSnapshotAssembler::UpsertNavigation(
         merged, platformB, "nav-provider", false);
   assert(withB.navigation.platforms.size() == 2);
   assert(withB.snapshotVersion == 9);
   nrm::NavigationSample platformC;
   platformC.platformId = "platform-c";
   platformC.platformName = "platform-c";
   platformC.sampleTime = 20.0;
   const nrm::ResourceSnapshot withC =
      nrm::CustomerSnapshotAssembler::UpsertNavigation(
         withB, platformC, "nav-provider", false);
   assert(withC.navigation.platforms.size() == 3);
   assert(withC.snapshotVersion == 10);
   nrm::NavigationSample replacement = oldNavigation;
   replacement.platformName = "display-name-may-change";
   replacement.sampleTime = 22.0;
   const nrm::ResourceSnapshot replaced =
      nrm::CustomerSnapshotAssembler::UpsertNavigation(
         withC, replacement, "nav-provider", false);
   assert(replaced.navigation.platforms.size() == 3);
   assert(replaced.navigation.platforms.front().sampleTime == 22.0);
   assert(replaced.navigation.platforms.front().platformName ==
          "display-name-may-change");

   nrm::EnvironmentSnapshot environment;
   environment.valid = true;
   environment.providerId = "environment-b";
   environment.sampleTime = 23.0;
   const nrm::ResourceSnapshot withEnvironment =
      nrm::CustomerSnapshotAssembler::MergeEnvironment(
         replaced, environment, false);
   assert(withEnvironment.environment.providerId == "environment-b");
   assert(withEnvironment.navigation.platforms.size() == 3);

   const nrm::ResourceSnapshot reset =
      nrm::CustomerSnapshotAssembler::MergeResources(withEnvironment, incoming, true);
   assert(reset.navigation.platforms.empty());
   assert(!reset.environment.valid);
   assert(reset.snapshotVersion == withEnvironment.snapshotVersion + 1);
   return 0;
}
