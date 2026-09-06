// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>
#include <unordered_map>

#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "WsfBMMOELogger.hpp"
#include "WsfBMPluginUtilities.hpp"
#include "WsfScriptSensorsManagerFovClass.hpp"
#include "WsfSensorsManagerFov.hpp"
#include "WsfSensorsManagerFovImpl.hpp"
#include "WsfSimpleSensorsManagerImpl.hpp"
#include "iadsLib/assignmentContainers.hpp"
#include "iadsLib/assetRecord.hpp"
#include "iadsLib/positionRecord.hpp"
#include "iadsLib/sensorsManagerInterface.hpp"
#include "iadsLib/sensorRecordInterface.hpp"
#include "iadsLib/trackRecord.hpp"
#include "vclMath/Constants.hpp"
#include "vclMath/Vector3.hpp"

#define PI vm::PI

void WsfSensorsManagerFov::RegisterScriptTypes(WsfApplication* application)
{
   if (!IsClassTypeRegistered(application, WsfScriptSensorsManagerFovClass::GetScriptClassName()))
   {
      RegisterBaseTypes(application);
      UtScriptTypes* scriptTypes = application->GetScriptTypes();
      scriptTypes->Register(ut::make_unique<WsfScriptSensorsManagerFovClass>(scriptTypes));
   }
}

WsfProcessor* WsfSensorsManagerFov::Create(WsfScenario& scenario)
{
   auto sm = std::make_shared<WsfSensorsManagerFovImpl>();

#ifdef IC2DEBUG
   HCL_INFO_LOGGER() << "WsfSensorsManagerFov::Create(): sm = " << sm;
#endif

   return new WsfSensorsManagerFov(scenario, sm);
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfSensorsManagerFov> WsfSensorsManagerFov::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfSensorsManagerFov>(aScenario);
}

WsfSensorsManagerFov::WsfSensorsManagerFov(WsfScenario& scenario,
                                           std::shared_ptr<il::SensorsManagerInterface> sensorsManagerComponent/* = std::make_shared<WsfSensorsManagerFovImpl>()*/)
   : WsfSimpleSensorsManager(scenario, WsfScriptSensorsManagerFovClass::GetScriptClassName(), sensorsManagerComponent)
{ }

WsfSensorsManagerFov::WsfSensorsManagerFov(const WsfSensorsManagerFov& src)
   : WsfSimpleSensorsManager(src)
{ }

// virtual
WsfProcessor* WsfSensorsManagerFov::Clone() const
{
#ifdef IC2DEBUG
   HCL_TRACE_LOGGER() << "WsfSensorsManagerFov::Clone(before)";
#endif
   return new WsfSensorsManagerFov(*this);
}

// virtual
bool WsfSensorsManagerFov::ProcessInput(UtInput& input)
{
   return WsfSimpleSensorsManager::ProcessInput(input);
}

// virtual
bool WsfSensorsManagerFov::Initialize(double simTime)
{
   return WsfSimpleSensorsManager::Initialize(simTime);
}

// virtual
bool WsfSensorsManagerFov::Initialize2(double simTime)
{
   return WsfSimpleSensorsManager::Initialize2(simTime);
}

// virtual
const char* WsfSensorsManagerFov::GetScriptClassName() const
{
   return WsfScriptSensorsManagerFovClass::GetScriptClassName();
}

const char* WsfSensorsManagerFov::GetBaseClassName()
{
   return WsfScriptSensorsManagerFovClass::GetBaseClassName();
}

struct ComputeAzimuthTarget
{
   double azRad;
   // this field is 1 if tracking, 0 if not tracking
   unsigned char tracking;
};

// Given a set of targets, computes an azimuth to cue to that first maximizes,
// the number of tracked targets in the sensor's field of view, then maximizes
// the number of targets (tracked or not) in the sensor's field of view, then
// minimizes the variance of the target azimuth's in the sensor's field of view
inline static double computeCueAzimuth(double                            azExtentRad,
                                       std::vector<ComputeAzimuthTarget> targets)
{
   assert(azExtentRad >= -PI && azExtentRad <= PI);

   // The algorithm below finds the azimuth window with the most tracks. Given two
   // windows with the same number of tracks, it chooses the one with the most
   // targets overall. Given two windows that have the same number of targets, it
   // chooses the one with the most tracks. Given that two windows have the same
   // number of targets and tracks, it chooses the one where the target azimuths
   // are closest together (that is, it chooses the one with the lowest target azimuth
   // variance). The cue azimuth in this window is computed as the mean of all
   // target azimuths.
   //
   // The algorithm works by starting with an azimuth window between the first target
   // and the maximum azimuth, and sets this as the best window. During each iteration,
   // it slides the window forward until a target enters or exits the window. When this
   // happens, the target count, tracking count, and azimuth variance are recomputed and
   // compared to the current best window. If these values are better, then this window
   // is set to the new best window. The algorithm ends when the window has slid through
   // all possible azimuths.

   // Sort the targets by azimuth. This allows the algorithm
   // to be performed much more efficiently.
   auto compareAzimuth = [](ComputeAzimuthTarget a, ComputeAzimuthTarget b)->bool { return a.azRad < b.azRad; };
   std::sort(targets.begin(), targets.end(), compareAzimuth);

   // minIdx is the index of the first target in the azimuth window,
   // inclusive; and maxIdx the index of the last target, exclusive
   size_t minIdx = 0;
   size_t maxIdx = 0;
   double minAzRad = targets[0].azRad;
   double maxAzRad = minAzRad + azExtentRad;

   // Append the target list to itself. This obviates the need for
   // azimuth wraparound checks in the remainder of the algorithm.
   size_t validTargetCount = targets.size();
   targets.reserve(validTargetCount * 2);
   for (size_t i = 0; i < validTargetCount; ++i)
   {
      ComputeAzimuthTarget target = targets[i];
      target.azRad += 2.0 * PI;
      if (target.azRad >= maxAzRad)
      {
         break;
      }
      targets.push_back(target);
   }

   double azSumRad = 0.0;
   size_t maxTrackingCount = 0;

   // initialize values for the first azimuth window
   for (;;)
   {
      ComputeAzimuthTarget target = targets[maxIdx];
      ++maxIdx;
      azSumRad += target.azRad;
      maxTrackingCount += target.tracking;
      // Exit the loop if the next target is outside the initial azimuth
      // window, or if the window enters the duplicated half of the
      // target array.
      if (maxIdx >= validTargetCount || targets[maxIdx].azRad > maxAzRad)
      {
         break;
      }
   }
   size_t maxTargetCount = maxIdx - minIdx;

   // compute the azimuth and variance for the first window
   double cueAzRad, minAzVariance;
   {
      double meanAzRad = azSumRad / (double)(maxIdx - minIdx);
      cueAzRad = meanAzRad;
      minAzVariance = 0.0;
      for (size_t i = minIdx; i < maxIdx; ++i)
      {
         double diff = targets[i].azRad - meanAzRad;
         minAzVariance += std::abs(diff);
      }
   }

   // iterate over the other azimuth windows
   size_t trackingCount = maxTrackingCount;
   for (;;)
   {
      // if this condition is true, we have iterated over all azimuth windows
      if (maxIdx >= targets.size())
      {
         break;
      }

      assert(minIdx < maxIdx);

      ComputeAzimuthTarget minTarget = targets[minIdx];
      ComputeAzimuthTarget nextMaxTarget = targets[maxIdx];

      // Determine how far to slide the azimuth window so that the target
      // count changes. Note that the tracking count and azimuth variance
      // can change only if the the target count changes.
      bool targetExitingWindow;
      {
         double maxAzRad = minAzRad + azExtentRad;
         double minDiffRad = minTarget.azRad - minAzRad;
         double maxDiffRad = nextMaxTarget.azRad - maxAzRad;
         assert(minDiffRad >= 0.0);
         assert(maxDiffRad >= 0.0);
         // If the min/max differences are equal the target at the maximum
         // extent should enter the window before the other target exits it.
         // Note that this case should rarely, if ever, happen in practice.
         targetExitingWindow = minDiffRad < maxDiffRad;
      }

      // slide the azimuth window forward
      if (targetExitingWindow)
      {
         ++minIdx;
         minAzRad = minTarget.azRad;
         trackingCount -= minTarget.tracking;
         azSumRad -= minTarget.azRad;
      }
      else
      {
         ++maxIdx;
         minAzRad = nextMaxTarget.azRad - azExtentRad;
         trackingCount += nextMaxTarget.tracking;
         azSumRad += nextMaxTarget.azRad;
      }

      // if the azimuth window contains no targets, advance it
      // until the next target enters the window
      if (minIdx == maxIdx)
      {
         ++maxIdx;
         ComputeAzimuthTarget t = targets[minIdx];
         minAzRad = t.azRad - azExtentRad;
         assert(minAzRad >= -PI);
         trackingCount = t.tracking;
         azSumRad = t.azRad;
      }

      size_t targetCount = maxIdx - minIdx;

      double meanAzRad = azSumRad / (double)targetCount;

      // I don't think there is a way to modify the variance incrementally (as
      // we do with the azimuth sum), so compute it from scratch.
      //
      // Technically, variance is the sum of squares. Since we are comparing
      // variances, it is sufficient to sum the absolute values instead. Using
      // absolute values instead of squares keeps the variance smaller, and so
      // lessens the chance for floating point rounding errors.
      double azVariance = 0.0;
      for (size_t i = minIdx; i < maxIdx; ++i)
      {
         double diff = targets[i].azRad - meanAzRad;
         azVariance += std::abs(diff);
      }

      bool better = false;
      if (trackingCount > maxTrackingCount)
      {
         better = true;
      }
      else if (trackingCount == maxTrackingCount)
      {
         if (targetCount > maxTargetCount)
         {
            better = true;
         }
         else if (targetCount == maxTargetCount)
         {
            if (azVariance < minAzVariance)
            {
               better = true;
            }
         }
      }

      if (better)
      {
         maxTargetCount = targetCount;
         maxTrackingCount = trackingCount;
         minAzVariance = azVariance;
         cueAzRad = meanAzRad;
      }
   }

   // normalize the azimuth to the range [-PI, PI)
   if (cueAzRad >= PI)
   {
      cueAzRad -= 2.0 * PI;
   }

   return cueAzRad;
}

inline static bool insideSensorFov(double centerAzRad,
                                   double azExtentRad,
                                   double testAzRad)
{
   assert(azExtentRad <= 2.0 * PI);
   assert(centerAzRad >= -PI && centerAzRad <= PI);
   assert(testAzRad >= -PI && testAzRad <= PI);

   // compute the minimum difference between the center and test azimuths
   double diffRad = PI - std::abs(std::abs(centerAzRad - testAzRad) - PI);
   // test if the difference is inside the extent
   return diffRad <= 0.5 * azExtentRad;
}

static int64_t makeTtrId(const il::sensorRecord& ttr)
{
   // The asset ID corresponds to an AFSIM platform ID, and sensor ID to the index of
   // a sensor on its carrying platform. Since a sensor manager could manage sensors
   // on multiple platforms, using the sensor ID alone is not sufficient. Combining
   // the platform and sensor ID together uniquely identifies a TTR.
   il::assetRecord& ttrAsset = *(ttr.GetParentAsset());
   int32_t assetId = ttrAsset.getID().getID();
   int32_t sensorId = ttr.GetSensorID().getID();
   int64_t ttrId = (((int64_t)assetId) << 32) | ((int64_t)sensorId);
   return ttrId;
}

// virtual
void WsfSensorsManagerFov::Run(const double simTime)
{
   WsfSimpleSensorsManager::Run(simTime);

   // build a map of TTRs to their assignments
   il::SensorsManagerInterface& sensorManager = *(this->GetCoreSMRef());
   il::ResponsibleAssignments& assignments = sensorManager.GetResponsibleAssignments();
   std::unordered_map<int64_t, std::vector<il::ResponsibleRadarAssignment*>> ttrIdsToAssignments;
   auto processAssignment = [&ttrIdsToAssignments](const il::moeLoggingInterface& aMOELogger, const il::tTrackID& globalTrackId, il::ResponsibleRadarAssignment& assignment)
   {
      if (!assignment.HasTTRResponsibility())
      {
         return;
      }

      il::sensorRecord& ttr = *(assignment.GetResponsibleTTR());
      if (ttr.GetParentAsset()->isGreen())
      {
         int64_t ttrId = makeTtrId(ttr);
         ttrIdsToAssignments[ttrId].push_back(&assignment);
      }
   };
   assignments.ForeachResponsibleAssignment(*IADSC2SimulationExtension::Find(*GetSimulation())->GetLogger(), processAssignment);

   // find all TTRs subordinate to this sensor manager
   std::vector<il::sensorRecord*> ttrs;
   auto gatherTtrs = [&ttrs](std::shared_ptr<il::assetRecord> asset)
   {
      std::vector<std::shared_ptr<il::sensorRecord>> sensors = asset->getSensors();
      for (size_t i = 0; i < sensors.size(); ++i)
      {
         il::sensorRecord* sensor = sensors[i].get();
         if (sensor->IsTTR())
         {
            ttrs.push_back(sensor);
         }
      }
   };
   const std::shared_ptr<il::assetRecord>& thisAsset = this->GetCoreAMRef()->getThisAsset();
   gatherTtrs(thisAsset);
   thisAsset->foreachSubordinateBFS(this->GetCoreAMRef()->getAssets(), gatherTtrs);

   // update each TTR
   for (size_t i = 0; i < ttrs.size(); ++i)
   {
      il::sensorRecord& ttr = *(ttrs[i]);
      int64_t ttrId = makeTtrId(ttr);
      auto it = ttrIdsToAssignments.find(ttrId);

      size_t assignmentCount = 0;
      if (it != ttrIdsToAssignments.end())
      {
         std::vector<il::ResponsibleRadarAssignment*>& v = it->second;
         assignmentCount = v.size();
         // record target information required to compute the cue azimuth
         // unused const il::positionRecord& sensorPosition = ttr.GetPosition();
         std::vector<ComputeAzimuthTarget> targets;
         targets.reserve(assignmentCount);
         size_t trackingCount = 0;
         if (assignmentCount > 0)
         {
            for (auto assignmentPtr : v)
            {
               il::ResponsibleRadarAssignment& assignment = *assignmentPtr;

               const il::trackRecord& track = *(assignment.GetTrackRecord());
               // unused const il::positionRecord& estimatedTrackPosition = track.getPropagatedPosition(simTime);

               // compute the azimuth to the track in the senor's PCS (part coordinate system)
               vm::Vector3<double> targetPcs = ttr.WcsToPcs(track.getPosition().getXYZ());
               double targetAzRad = atan2(targetPcs.getY(), targetPcs.getX());
               assert(targetAzRad >= -PI && targetAzRad <= PI);

               ComputeAzimuthTarget target;
               target.azRad = targetAzRad;
               target.tracking = assignment.IsTTRTracking() ? 1u : 0u;
               targets.push_back(target);

               trackingCount += target.tracking;
            }

            double sensorAzExtentRad = ttr.GetAzExtent();
            double cueAzRad = computeCueAzimuth(sensorAzExtentRad, std::move(targets));
            ttr.CueToAzimuth(cueAzRad);

            double sensorAzRad = ttr.GetAz();
            bool cueAzInSensorFov = insideSensorFov(sensorAzRad, sensorAzExtentRad, cueAzRad);
            if (trackingCount > 0 || cueAzInSensorFov)
            {
               ttr.TurnOn(simTime);
               ttr.SetAzSlewRate(ttr.GetFineSlewRate());
            }
            else
            {
               ttr.TurnOff(simTime);
               ttr.SetAzSlewRate(ttr.GetCoarseSlewRate());
            }
         }
      }

      // NOTE: It is safe to call TurnOff if a platform part is already off,
      //       and likewise to call TurnOn if a part is already on

      if (assignmentCount == 0)
      {
         ttr.TurnOff(simTime);
         ttr.SetAzSlewRate(ttr.GetCoarseSlewRate());
         ttr.CueToAzimuth(ttr.GetRestingAzimuth());
      }

   }
}
