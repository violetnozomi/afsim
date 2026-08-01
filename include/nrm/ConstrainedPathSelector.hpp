#ifndef NRM_CONSTRAINED_PATH_SELECTOR_HPP
#define NRM_CONSTRAINED_PATH_SELECTOR_HPP

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
struct ConstrainedEdge
{
   std::string sourceId;
   std::string destinationId;
   std::string sourcePlatform;
   std::string destinationPlatform;
   NetworkType networkType = NetworkType::cUNKNOWN;
   double delayMs = 0.0;
   double pdrPercent = 0.0;
   double bandwidthBps = 0.0;
   double distanceM = 0.0;
   bool delayValid = false;
   Confidence delayConfidence = Confidence::cLOW;
   bool pdrValid = false;
   Confidence pdrConfidence = Confidence::cLOW;
   bool bandwidthValid = false;
   Confidence bandwidthConfidence = Confidence::cLOW;
   bool distanceValid = false;
   Confidence distanceConfidence = Confidence::cLOW;
   bool candidate = false;
   std::string profileId;
};

struct PathConstraintSet
{
   double requiredBandwidthBps = 0.0;
   double maximumDelayMs = 0.0;
   double minimumPdrPercent = 0.0;
   bool requireDelayMetric = true;

   bool Valid() const
   {
      return std::isfinite(requiredBandwidthBps) && requiredBandwidthBps >= 0.0 &&
             std::isfinite(maximumDelayMs) && maximumDelayMs >= 0.0 &&
             std::isfinite(minimumPdrPercent) && minimumPdrPercent >= 0.0 &&
             minimumPdrPercent <= 100.0;
   }
};

struct PathSelectionOptions
{
   std::size_t k = 8;
   std::size_t maximumHops = 16;
   std::size_t maximumExpandedStates = 4096;

   bool Valid() const
   {
      return k >= 1 && k <= 32 && maximumHops >= 1 && maximumHops <= 64 &&
             maximumExpandedStates >= k;
   }
};

struct ConstrainedPath
{
   std::vector<const ConstrainedEdge*> edges;
   std::vector<std::string> nodeIds;
   double delayMs = 0.0;
   double pdrPercent = 0.0;
   double bottleneckBandwidthBps = 0.0;
   double totalDistanceM = 0.0;
   double maximumHopDistanceM = 0.0;
   bool delayValid = false;
   Confidence delayConfidence = Confidence::cLOW;
   bool pdrValid = false;
   Confidence pdrConfidence = Confidence::cLOW;
   bool bandwidthValid = false;
   Confidence bandwidthConfidence = Confidence::cLOW;
   bool distanceValid = false;
   Confidence distanceConfidence = Confidence::cLOW;
   bool feasible = false;
   std::size_t candidateEdgeCount = 0;
   std::vector<std::string> failedConstraints;
};

struct PathSelectionResult
{
   std::vector<ConstrainedPath> consideredPaths;
   ConstrainedPath selectedPath;
   ConstrainedPath diagnosticPath;
   bool hasSelectedPath = false;
   bool hasDiagnosticPath = false;
   bool searchLimitReached = false;
   bool invalidOptions = false;
   bool invalidConstraints = false;
   std::size_t selectedPathRank = 0;
   std::size_t expandedStateCount = 0;
};

class ConstrainedPathSelector
{
public:
   using Adjacency = std::map<std::string, std::vector<ConstrainedEdge>>;

   PathSelectionResult Select(const Adjacency& aAdjacency,
                              std::vector<std::string> aSourceIds,
                              const std::set<std::string>& aDestinationIds,
                              const PathConstraintSet& aConstraints,
                              const PathSelectionOptions& aOptions,
                              bool aAllowCandidates,
                              bool aRequireCandidate,
                              const std::set<std::string>& aForbiddenEdges =
                                 std::set<std::string>()) const
   {
      PathSelectionResult result;
      if (!aOptions.Valid())
      {
         result.invalidOptions = true;
         return result;
      }
      if (!aConstraints.Valid())
      {
         result.invalidConstraints = true;
         return result;
      }
      std::sort(aSourceIds.begin(), aSourceIds.end());
      aSourceIds.erase(std::unique(aSourceIds.begin(), aSourceIds.end()), aSourceIds.end());

      std::priority_queue<PartialPath, std::vector<PartialPath>, PartialGreater> queue;
      for (const std::string& sourceId : aSourceIds)
      {
         PartialPath start;
         start.nodeIds.push_back(sourceId);
         queue.push(start);
      }

      while (!queue.empty() && result.consideredPaths.size() < aOptions.k &&
             result.expandedStateCount < aOptions.maximumExpandedStates)
      {
         PartialPath partial = queue.top();
         queue.pop();
         ++result.expandedStateCount;
         const std::string& currentId = partial.nodeIds.back();
         if (aDestinationIds.count(currentId) != 0 && !partial.edges.empty())
         {
            ConstrainedPath complete = Evaluate(partial, aConstraints);
            if (!aRequireCandidate || complete.candidateEdgeCount > 0)
            {
               result.consideredPaths.push_back(complete);
            }
            continue;
         }
         if (partial.edges.size() >= aOptions.maximumHops)
         {
            continue;
         }

         const auto adjacencyIt = aAdjacency.find(currentId);
         if (adjacencyIt == aAdjacency.end())
         {
            continue;
         }
         std::vector<const ConstrainedEdge*> edges;
         for (const ConstrainedEdge& edge : adjacencyIt->second)
         {
            edges.push_back(&edge);
         }
         std::sort(edges.begin(), edges.end(),
                   [](const ConstrainedEdge* aLeft, const ConstrainedEdge* aRight)
                   {
                      if (aLeft->destinationId != aRight->destinationId)
                      {
                         return aLeft->destinationId < aRight->destinationId;
                      }
                      if (aLeft->candidate != aRight->candidate)
                      {
                         return !aLeft->candidate;
                      }
                      return EdgeKey(*aLeft) < EdgeKey(*aRight);
                   });
         for (const ConstrainedEdge* edge : edges)
         {
            if ((!aAllowCandidates && edge->candidate) ||
                aForbiddenEdges.count(EdgeKey(*edge)) != 0 ||
                std::find(partial.nodeIds.begin(), partial.nodeIds.end(), edge->destinationId) !=
                   partial.nodeIds.end())
            {
               continue;
            }
            PartialPath next = partial;
            next.edges.push_back(edge);
            next.nodeIds.push_back(edge->destinationId);
            next.delayCost += edge->delayValid ? edge->delayMs : cINVALID_DELAY_COST;
            if (edge->candidate)
            {
               ++next.candidateEdgeCount;
            }
            queue.push(next);
         }
      }
      result.searchLimitReached = !queue.empty() &&
                                  result.expandedStateCount >= aOptions.maximumExpandedStates;
      std::sort(result.consideredPaths.begin(), result.consideredPaths.end(), PathLess);
      if (!result.consideredPaths.empty())
      {
         result.diagnosticPath = result.consideredPaths.front();
         result.hasDiagnosticPath = true;
      }
      for (std::size_t index = 0; index < result.consideredPaths.size(); ++index)
      {
         if (result.consideredPaths[index].feasible)
         {
            result.selectedPath = result.consideredPaths[index];
            result.hasSelectedPath = true;
            result.selectedPathRank = index + 1;
            break;
         }
      }
      return result;
   }

   static std::string EdgeKey(const ConstrainedEdge& aEdge)
   {
      return aEdge.sourceId + "->" + aEdge.destinationId;
   }

private:
   static constexpr double cINVALID_DELAY_COST = 1.0e12;

   struct PartialPath
   {
      std::vector<const ConstrainedEdge*> edges;
      std::vector<std::string> nodeIds;
      double delayCost = 0.0;
      std::size_t candidateEdgeCount = 0;
   };

   struct PartialGreater
   {
      bool operator()(const PartialPath& aLeft, const PartialPath& aRight) const
      {
         if (aLeft.delayCost != aRight.delayCost)
         {
            return aLeft.delayCost > aRight.delayCost;
         }
         if (aLeft.candidateEdgeCount != aRight.candidateEdgeCount)
         {
            return aLeft.candidateEdgeCount > aRight.candidateEdgeCount;
         }
         return aLeft.nodeIds > aRight.nodeIds;
      }
   };

   static bool PathLess(const ConstrainedPath& aLeft, const ConstrainedPath& aRight)
   {
      if (aLeft.delayValid != aRight.delayValid)
      {
         return aLeft.delayValid;
      }
      if (aLeft.delayMs != aRight.delayMs)
      {
         return aLeft.delayMs < aRight.delayMs;
      }
      if (aLeft.candidateEdgeCount != aRight.candidateEdgeCount)
      {
         return aLeft.candidateEdgeCount < aRight.candidateEdgeCount;
      }
      return aLeft.nodeIds < aRight.nodeIds;
   }

   static Confidence MinConfidence(Confidence aLeft, Confidence aRight)
   {
      return static_cast<int>(aLeft) < static_cast<int>(aRight) ? aLeft : aRight;
   }

   static ConstrainedPath Evaluate(const PartialPath& aPartial,
                                   const PathConstraintSet& aConstraints)
   {
      ConstrainedPath path;
      path.edges = aPartial.edges;
      path.nodeIds = aPartial.nodeIds;
      path.delayValid = true;
      path.pdrValid = true;
      path.bandwidthValid = !path.edges.empty();
      path.distanceValid = !path.edges.empty();
      path.delayConfidence = Confidence::cHIGH;
      path.pdrConfidence = Confidence::cHIGH;
      path.bandwidthConfidence = Confidence::cHIGH;
      path.distanceConfidence = Confidence::cHIGH;
      path.pdrPercent = 100.0;
      path.bottleneckBandwidthBps = std::numeric_limits<double>::max();
      for (const ConstrainedEdge* edge : path.edges)
      {
         if (edge->candidate)
         {
            ++path.candidateEdgeCount;
         }
         if (!edge->delayValid || !std::isfinite(edge->delayMs) || edge->delayMs < 0.0)
         {
            path.delayValid = false;
         }
         else
         {
            path.delayMs += edge->delayMs;
            path.delayConfidence = MinConfidence(path.delayConfidence, edge->delayConfidence);
         }
         if (!edge->pdrValid || !std::isfinite(edge->pdrPercent) ||
             edge->pdrPercent < 0.0 || edge->pdrPercent > 100.0)
         {
            path.pdrValid = false;
         }
         else
         {
            path.pdrPercent *=
               std::max(0.0, std::min(100.0, edge->pdrPercent)) / 100.0;
            path.pdrConfidence = MinConfidence(path.pdrConfidence, edge->pdrConfidence);
         }
         if (!edge->bandwidthValid || !std::isfinite(edge->bandwidthBps) ||
             edge->bandwidthBps < 0.0)
         {
            path.bandwidthValid = false;
         }
         else
         {
            path.bottleneckBandwidthBps =
               std::min(path.bottleneckBandwidthBps, edge->bandwidthBps);
            path.bandwidthConfidence =
               MinConfidence(path.bandwidthConfidence, edge->bandwidthConfidence);
         }
         if (!edge->distanceValid || !std::isfinite(edge->distanceM) || edge->distanceM < 0.0)
         {
            path.distanceValid = false;
         }
         else
         {
            path.totalDistanceM += edge->distanceM;
            path.maximumHopDistanceM = std::max(path.maximumHopDistanceM, edge->distanceM);
            path.distanceConfidence =
               MinConfidence(path.distanceConfidence, edge->distanceConfidence);
         }
      }

      if (!path.delayValid &&
          (aConstraints.requireDelayMetric || aConstraints.maximumDelayMs > 0.0))
      {
         path.failedConstraints.push_back("DELAY_DATA_INVALID");
      }
      else if (aConstraints.maximumDelayMs > 0.0 &&
               path.delayMs > aConstraints.maximumDelayMs)
      {
         path.failedConstraints.push_back("DELAY_MARGIN_NEGATIVE");
      }
      if (aConstraints.minimumPdrPercent > 0.0)
      {
         if (!path.pdrValid)
         {
            path.failedConstraints.push_back("PDR_DATA_INVALID");
         }
         else if (path.pdrPercent < aConstraints.minimumPdrPercent)
         {
            path.failedConstraints.push_back("RELIABILITY_MARGIN_NEGATIVE");
         }
      }
      if (aConstraints.requiredBandwidthBps > 0.0)
      {
         if (!path.bandwidthValid)
         {
            path.failedConstraints.push_back("BANDWIDTH_DATA_INVALID");
         }
         else if (path.bottleneckBandwidthBps < aConstraints.requiredBandwidthBps)
         {
            path.failedConstraints.push_back("BANDWIDTH_MARGIN_NEGATIVE");
         }
      }
      path.feasible = path.failedConstraints.empty();
      return path;
   }
};
} // namespace nrm

#endif
