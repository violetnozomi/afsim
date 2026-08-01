#include "nrm/ConstrainedPathSelector.hpp"

#include <iostream>
#include <limits>
#include <set>
#include <string>
#include <vector>

#define CHECK(condition) \
   do { if (!(condition)) { std::cerr << "CHECK failed at line " << __LINE__ << "\n"; return 1; } } while (false)

nrm::ConstrainedEdge Edge(const char* source, const char* destination,
                          double delay, double bandwidth)
{
   nrm::ConstrainedEdge edge;
   edge.sourceId = source;
   edge.destinationId = destination;
   edge.sourcePlatform = source;
   edge.destinationPlatform = destination;
   edge.networkType = nrm::NetworkType::cLINK16;
   edge.delayMs = delay;
   edge.delayValid = true;
   edge.pdrPercent = 99.0;
   edge.pdrValid = true;
   edge.bandwidthBps = bandwidth;
   edge.bandwidthValid = true;
   return edge;
}

int main()
{
   nrm::ConstrainedPathSelector::Adjacency graph;
   graph["A"].push_back(Edge("A", "B", 5.0, 50.0));
   graph["B"].push_back(Edge("B", "D", 5.0, 50.0));
   graph["A"].push_back(Edge("A", "C", 10.0, 100.0));
   graph["C"].push_back(Edge("C", "D", 10.0, 100.0));

   nrm::PathConstraintSet constraints;
   constraints.requiredBandwidthBps = 80.0;
   constraints.maximumDelayMs = 30.0;
   constraints.minimumPdrPercent = 95.0;
   nrm::PathSelectionOptions options;
   nrm::ConstrainedPathSelector selector;
   const std::set<std::string> destinations{"D"};
   const nrm::PathSelectionResult selected =
      selector.Select(graph, {"A"}, destinations, constraints, options, false, false);
   CHECK(selected.hasSelectedPath);
   CHECK(selected.selectedPathRank == 2);
   CHECK(selected.selectedPath.delayMs == 20.0);
   CHECK(selected.selectedPath.nodeIds[1] == "C");
   CHECK(selected.consideredPaths[0].failedConstraints[0] ==
         "BANDWIDTH_MARGIN_NEGATIVE");

   options.k = 1;
   const nrm::PathSelectionResult limited =
      selector.Select(graph, {"A"}, destinations, constraints, options, false, false);
   CHECK(!limited.hasSelectedPath);
   CHECK(limited.hasDiagnosticPath);

   options.k = 8;
   options.maximumHops = 1;
   CHECK(!selector.Select(graph, {"A"}, destinations, constraints,
                          options, false, false).hasDiagnosticPath);
   options.maximumHops = 16;
   options.k = 0;
   CHECK(selector.Select(graph, {"A"}, destinations, constraints,
                         options, false, false).invalidOptions);
   options.k = 33;
   CHECK(selector.Select(graph, {"A"}, destinations, constraints,
                         options, false, false).invalidOptions);

   options.k = 8;
   constraints.maximumDelayMs = std::numeric_limits<double>::quiet_NaN();
   CHECK(selector.Select(graph, {"A"}, destinations, constraints,
                         options, false, false).invalidConstraints);
   constraints.maximumDelayMs = 30.0;

   std::vector<std::string> route;
   for (int iteration = 0; iteration < 5; ++iteration)
   {
      const nrm::PathSelectionResult repeat =
         selector.Select(graph, {"A"}, destinations, constraints, options, false, false);
      CHECK(repeat.selectedPath.nodeIds == selected.selectedPath.nodeIds);
   }
   return 0;
}
