#include "NrmSnapshotReporter.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#define CHECK(condition) \
   do { if (!(condition)) { std::cerr << "CHECK failed at line " << __LINE__ << "\n"; return 1; } } while (false)

bool Contains(const std::string& path, const std::string& token)
{
   std::ifstream input(path);
   std::stringstream buffer;
   buffer << input.rdbuf();
   return buffer.str().find(token) != std::string::npos;
}

int main()
{
   const std::string root =
      "/tmp/nrm-reporter-recovery-" + std::to_string(getpid());
   std::string firstRun;
   {
      WkNrm::SnapshotReporter reporter(root);
      firstRun = reporter.GetRunDirectory();
      nrm::ResourceSnapshot snapshot;
      snapshot.snapshotVersion = 1;
      reporter.Enqueue(snapshot);
   }
   CHECK(Contains(firstRun + "/manifest.json", "\"complete\":true"));
   CHECK(Contains(firstRun + "/resource_snapshots.jsonl", "\"snapshot_version\":1"));

   std::string secondRun;
   {
      WkNrm::SnapshotReporter reporter(root);
      secondRun = reporter.GetRunDirectory();
      nrm::ResourceSnapshot snapshot;
      snapshot.snapshotVersion = 2;
      reporter.Enqueue(snapshot);
   }
   CHECK(firstRun != secondRun);
   CHECK(Contains(firstRun + "/resource_snapshots.jsonl", "\"snapshot_version\":1"));
   CHECK(Contains(secondRun + "/resource_snapshots.jsonl", "\"snapshot_version\":2"));

   {
      WkNrm::SnapshotReporter reporter(root, "overflow-test", 2, false);
      for (std::uint64_t version = 1; version <= 5; ++version)
      {
         nrm::ResourceSnapshot snapshot;
         snapshot.snapshotVersion = version;
         reporter.Enqueue(snapshot);
      }
      CHECK(reporter.GetStatus().droppedSnapshotCount == 3);
      reporter.Start();
   }

   {
      WkNrm::SnapshotReporter reporter(
         "/proc/nrm-output-not-writable", "failure-test", 2, false);
      CHECK(!reporter.GetStatus().healthy);
      nrm::ResourceSnapshot snapshot;
      reporter.Enqueue(snapshot);
   }
   return 0;
}
