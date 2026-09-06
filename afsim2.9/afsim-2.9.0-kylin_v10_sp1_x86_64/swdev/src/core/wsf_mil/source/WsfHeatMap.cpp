// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfHeatMap.hpp"

#include <cmath>
#include <list>

#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEM_XmtrRcvr.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMover.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"

WsfHeatMap::WsfHeatMap(const double aGridCenterLLA[3], double aGridExtentMeters, double aGridCellSizeMeters)
   : UtHeatMap(aGridCenterLLA, aGridExtentMeters, aGridCellSizeMeters)
   , mPlatformPtr(nullptr)
   , mPreviousHeading()
   , mHeadingChangeLimit(5 * UtMath::cRAD_PER_DEG)
   , // Default of 5 deg
   mUseAssetPerception(false)
   , mSensorRange(0.0)
   , mDrawGrid(false)
   , mGridDrawn(false)
   , mDrawHeat(false)
   , mDrawSensor(false)
   , mDrawObjPtr(nullptr)

{
}

WsfHeatMap::WsfHeatMap(const WsfHeatMap& aSrc)
   : UtHeatMap(aSrc)
   , mPlatformPtr(nullptr)
   , mPreviousHeading()
   , mHeadingChangeLimit(aSrc.mHeadingChangeLimit)
   , mUseAssetPerception(aSrc.mUseAssetPerception)
   , mSensorRange(aSrc.mSensorRange)
   , mDrawGrid(aSrc.mDrawGrid)
   , mGridDrawn(false)
   , mDrawHeat(aSrc.mDrawHeat)
   , mDrawSensor(aSrc.mDrawSensor)
   , mDrawObjPtr(nullptr)
{
}

WsfHeatMap::WsfHeatMap()
   : UtHeatMap()
   , mPlatformPtr(nullptr)
   , mPreviousHeading()
   , mHeadingChangeLimit(5 * UtMath::cRAD_PER_DEG)
   , // Default of 5 deg
   mUseAssetPerception(false)
   , mSensorRange(0.0)
   , mDrawGrid(false)
   , mGridDrawn(false)
   , mDrawHeat(false)
   , mDrawSensor(false)
   , mDrawObjPtr(nullptr)
{
}

WsfHeatMap::~WsfHeatMap()
{
   delete mDrawObjPtr;
}

bool WsfHeatMap::Initialize(WsfPlatform* aPlatformPtr)
{
   // Save platform pointer, will need it to get position/heading/sensor
   mPlatformPtr = aPlatformPtr;

   if (mPlatformPtr != nullptr)
   {
      mDrawObjPtr = new WsfDraw(*mPlatformPtr->GetSimulation());
      // Check to see if a position for the heat map center was set
      // If not, set the center position of the position of the platform
      // we are attached to
      if (fabs(mGridCenterLat + mGridCenterLon + mGridCenterAlt) < 0.001)
      {
         aPlatformPtr->GetLocationLLA(mGridCenterLat, mGridCenterLon, mGridCenterAlt);
      }
   }

   return UtHeatMap::Initialize();
}

bool WsfHeatMap::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());
   if (command == "heat_map")
   {
      UtInputBlock inputBlock(aInput, "end_heat_map");

      while (inputBlock.ReadCommand(command))
      {
         if (command == "position")
         {
            aInput.ReadValueOfType(mGridCenterLat, UtInput::cLATITUDE);
            aInput.ReadValueOfType(mGridCenterLon, UtInput::cLONGITUDE);
         }
         else if (command == "altitude")
         {
            // Note: heat map altitude is always MSL
            aInput.ReadValueOfType(mGridCenterAlt, UtInput::cLENGTH);
         }
         else if (command == "grid_extent")
         {
            aInput.ReadValueOfType(mGridExtentMeters, UtInput::cLENGTH);
         }
         else if (command == "cell_size")
         {
            aInput.ReadValueOfType(mGridCellSizeMeters, UtInput::cLENGTH);
         }
         else if (command == "heat_decay")
         {
            aInput.ReadValue(mHeatDecay);
            aInput.ValueInClosedRange(mHeatDecay, 0.05, 0.9);
         }
         else if (command == "decay_interval")
         {
            aInput.ReadValueOfType(mHeatDecayInterval, UtInput::cTIME);
         }
         else if (command == "expansion_timeout")
         {
            aInput.ReadValueOfType(mHeatTimeout, UtInput::cTIME);
         }
         else if (command == "heading_change_limit")
         {
            aInput.ReadValueOfType(mHeadingChangeLimit, UtInput::cANGLE);
         }
         else if (command == "use_asset_perception")
         {
            aInput.ReadValue(mUseAssetPerception);
         }
         else if (command == "sensor_range")
         {
            aInput.ReadValueOfType(mSensorRange, UtInput::cLENGTH);
         }
         else if (command == "draw_grid")
         {
            aInput.ReadValue(mDrawGrid);
         }
         else if (command == "draw_heat")
         {
            aInput.ReadValue(mDrawHeat);
         }
         else if (command == "draw_sensor_outline")
         {
            aInput.ReadValue(mDrawSensor);
         }
         else
         {
            throw UtInput::UnknownCommand(inputBlock.GetInput());
         }
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

void WsfHeatMap::Update(double aSimTime, double aUpdateInterval, std::vector<WsfAssetPerception*>& aPerceivedAssets)
{
   if (!mInitialized)
   {
      return;
   }

   // If we are using perceived assets and we have some perceived assets
   // get the areas they are covering and update the heat map
   if (mUseAssetPerception && !aPerceivedAssets.empty())
   {
      // loop through assets
      std::vector<WsfAssetPerception*>::const_iterator asset_iter;
      for (asset_iter = aPerceivedAssets.begin(); asset_iter != aPerceivedAssets.end(); ++asset_iter)
      {
         WsfAssetPerception* assetPtr    = *asset_iter;
         WsfPlatform*        platformPtr = mPlatformPtr->GetSimulation()->GetPlatformByIndex(assetPtr->mIndex);

         if (platformPtr != nullptr) // make sure it still exists
         {
            UpdateCold(aSimTime, platformPtr, assetPtr->mLocationWCS, assetPtr->mOrientationNED);
         } // end valid platform check
      }    // end loop through assets
   }

   // Get position and orientation to then update the "cold" coverage
   // for the platform this processor/heat map is attached to
   double myWCS[3]            = {0.0};
   double myOrientationNED[3] = {0.0}; // radians
   mPlatformPtr->GetLocationWCS(myWCS);
   mPlatformPtr->GetOrientationNED(myOrientationNED[0], myOrientationNED[1], myOrientationNED[2]);

   UpdateCold(aSimTime, mPlatformPtr, myWCS, myOrientationNED);

   // Update the map based on current state of heat/cold
   UtHeatMap::Update(aSimTime);

   if (mDrawHeat)
   {
      DrawHeat(aUpdateInterval);
   }

   // If drawing the grid was requested, do it once and have it
   // persist to help performance with fewer draw commands
   if (mDrawGrid && !mGridDrawn)
   {
      DrawGrid(1.0E6); // Draw in excess of 10 days, effectively forever
      mGridDrawn = true;
   }
}

void WsfHeatMap::LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   if (!mInitialized)
   {
      return;
   }

   double trackLocationWCS[3] = {0.0};

   // Verify that the track has a location, otherwise we can't add it to the heat map
   // Use the time of the last reported position
   double lastUpdate = aLocalTrackPtr->GetUpdateTime();
   if (aLocalTrackPtr->GetExtrapolatedLocationWCS(lastUpdate, trackLocationWCS))
   {
      // TODO add check for entity being killed? Is that even necessary?
      // TODO pass this target index to heat map so a track can be uniquely handled?
      // unsigned int targetId = aLocalTrackPtr->GetTargetIndex();

      double trackVelocityWCS[3] = {0.0};
      if (aLocalTrackPtr->VelocityValid())
      {
         aLocalTrackPtr->GetVelocityWCS(trackVelocityWCS);
      }

      // Add to heatmap, use lastUpdate time as this is the last time the
      // track had valid data. The sim time right now is many seconds after
      // lastUpdate during which the lost track has been extrapolated
      AddHeatSource(lastUpdate, trackLocationWCS, trackVelocityWCS);
   }
}

void WsfHeatMap::DrawGrid(double aDrawDuration)
{
   if (mDrawObjPtr == nullptr)
   {
      return;
   }

   // Set starting offsets from the center point for the horizontal and
   // vertical line start points. These offsets will be used to calculate
   // the end point for the grid lines. Altitude offset puts the grid
   // slightly above where the squares are drawn
   double horizStartNED[3] = {mMaxOffset, -mMaxOffset, 10};  // upper left
   double horizEndNED[3]   = {mMaxOffset, mMaxOffset, 10};   // upper right
   double vertStartNED[3]  = {mMaxOffset, -mMaxOffset, 10};  // upper left
   double vertEndNED[3]    = {-mMaxOffset, -mMaxOffset, 10}; // lower left

   // Calculate number of grid lines drawn from an edge of the grid
   // (Extent / square size) will give half the number of squares across the grid
   // Add one grid line to account for the final edge squares
   int numberOfGridLines = int((mGridExtentMeters / mGridCellSizeMeters) * 2) + 1;

   // Temporary variables used to store calculated line points
   double tempLat = 0;
   double tempLon = 0;
   double tempAlt = 0;

   // mDrawObjPtr->SetColor(0.0, 0.0, 0.0, 0.6);
   mDrawObjPtr->SetColor(0.0, 0.0, 0.0);
   mDrawObjPtr->SetDuration(aDrawDuration);
   mDrawObjPtr->BeginLines();

   // Draw horizontal/vertical lines spaced at grid size
   for (int line = 0; line <= numberOfGridLines; ++line)
   {
      // Horizontal start
      mCenterPoint.ConvertNEDToLLA(horizStartNED, tempLat, tempLon, tempAlt);
      mDrawObjPtr->VertexLLA(tempLat, tempLon, tempAlt);
      // Horizontal end
      mCenterPoint.ConvertNEDToLLA(horizEndNED, tempLat, tempLon, tempAlt);
      mDrawObjPtr->VertexLLA(tempLat, tempLon, tempAlt);
      // Vertical start
      mCenterPoint.ConvertNEDToLLA(vertStartNED, tempLat, tempLon, tempAlt);
      mDrawObjPtr->VertexLLA(tempLat, tempLon, tempAlt);
      // Vertical end
      mCenterPoint.ConvertNEDToLLA(vertEndNED, tempLat, tempLon, tempAlt);
      mDrawObjPtr->VertexLLA(tempLat, tempLon, tempAlt);

      // Move start/end points to next line. Horizontal lines move south, vertical move east
      horizStartNED[0] -= mGridCellSizeMeters;
      horizEndNED[0] -= mGridCellSizeMeters;
      vertStartNED[1] += mGridCellSizeMeters;
      vertEndNED[1] += mGridCellSizeMeters;
   }
   mDrawObjPtr->End();
}

void WsfHeatMap::DrawHeat(double aDrawDuration)
{
   if (mDrawObjPtr == nullptr)
   {
      return;
   }
   mDrawObjPtr->SetDuration(aDrawDuration);

   for (int gridX = -mNumberOfCells; gridX <= mNumberOfCells; ++gridX)
   {
      for (int gridY = -mNumberOfCells; gridY <= mNumberOfCells; ++gridY)
      {
         double heatValue = GetHeatValue(gridX, gridY);

         if (heatValue > 0) // Red square
         {
            mDrawObjPtr->SetColor(heatValue, 0.0, 0.0, 0.7);
         }
         else if (heatValue < 0) // Blue square, take ABS() as "cold" is negative
         {
            mDrawObjPtr->SetColor(0.0, 0.0, std::abs(heatValue), 0.7);
         }
         else
         {
            // Not a red or blue square, skip to next square
            continue;
         }

         // LLAs of square corners
         double sqTL[3];
         double sqTR[3];
         double sqBR[3];
         double sqBL[3];
         // Get the corners of the square for this heat point X,Y and draw a square
         GetGridCellCornersLLA(gridX, gridY, sqTL, sqTR, sqBR, sqBL);
         mDrawObjPtr->BeginQuadrilateral();
         mDrawObjPtr->VertexLLA(sqTL[0], sqTL[1], sqTL[2]);
         mDrawObjPtr->VertexLLA(sqTR[0], sqTR[1], sqTR[2]);
         mDrawObjPtr->VertexLLA(sqBR[0], sqBR[1], sqBR[2]);
         mDrawObjPtr->VertexLLA(sqBL[0], sqBL[1], sqBL[2]);
         mDrawObjPtr->End();
      }
   }
}

// PRIVATE

void WsfHeatMap::UpdateCold(double aSimTime, WsfPlatform* aPlaformPtr, double aLocationWCS[3], double aOrientationNED[3])
{
   // NOTE Location and orientation are passed in as parameters
   // (instead of pulling directly from the platform) to minimize
   // the cheating done when we are using asset perception

   // Verify that the platform's heading has not changed by more
   //  then the limit since the last update. This prevents the cold
   // from beeing updated when a platform is making sharp turns
   // that sweep a sensor over an area very fast
   if (HeadingExceedsLimit(aPlaformPtr))
   {
      return;
   }

   // For each sensor on the platform check that it is on and
   // an active RF sensor. Get the orientation of the sensor
   // and the azimuth scan limits
   for (WsfComponentList::RoleIterator<WsfSensor> iter(*aPlaformPtr); !iter.AtEnd(); ++iter)
   {
      WsfSensor* sensorPtr = *iter;
      if (((sensorPtr) != nullptr) && sensorPtr->IsTurnedOn() && sensorPtr->IsClassRadio() && sensorPtr->IsClassActive())
      {
         double sensorYaw   = 0.; // in radians
         double sensorPitch = 0.;
         double sensorRoll  = 0.;
         sensorPtr->GetOrientation(sensorYaw, sensorPitch, sensorRoll);

         double cuedAz = 0.; // in radians
         double cuedEl = 0.;
         sensorPtr->GetActualCuedOrientation(cuedAz, cuedEl);

         double orientationNED[3] = {0.0};
         orientationNED[0]        = aOrientationNED[0] + sensorYaw + cuedAz;
         orientationNED[1]        = aOrientationNED[1] + sensorPitch + cuedEl;
         orientationNED[2]        = aOrientationNED[2] + sensorRoll;

         WsfEM_Xmtr* xmtrPtr = &sensorPtr->GetEM_Xmtr(0);
         if (xmtrPtr != nullptr)
         {
            WsfEM_Antenna* antennaPtr = xmtrPtr->GetAntenna();

            // Take the narrower Az, either scan limit or FOV limit
            // This attempts to avoid using the default of -180 to 180
            double minAzScan = 0.; // in radians
            double maxAzScan = 0.;
            double minAzFOV  = 0.; // in radians
            double maxAzFOV  = 0.;
            antennaPtr->GetAzimuthScanLimits(minAzScan, maxAzScan);
            antennaPtr->GetAzimuthFieldOfView(minAzFOV, maxAzFOV);
            double minAz = std::max(minAzScan, minAzFOV);
            double maxAz = std::min(maxAzScan, maxAzFOV);

            // Update sensor converage in the heat map
            UtHeatMap::UpdateCold(aSimTime,
                                  aLocationWCS,
                                  mSensorRange, // TODO calculate
                                  orientationNED,
                                  minAz,
                                  maxAz);
            if (mDrawSensor)
            {
               double myLLA[3] = {0.0};
               aPlaformPtr->GetLocationLLA(myLLA[0], myLLA[1], myLLA[2]);
               DrawSensorCoverage(myLLA, orientationNED, mSensorRange, minAz, maxAz);
            }
         } // check for valid xmtr
      }    // sensor checks
   }       // end sensors loop
}

bool WsfHeatMap::HeadingExceedsLimit(WsfPlatform* aPlaformPtr)
{
   bool exceedsLimit = true;

   size_t platformIndex = aPlaformPtr->GetIndex();
   double heading       = 0.;
   double pitch         = 0.;
   double roll          = 0.;
   aPlaformPtr->GetOrientationNED(heading, pitch, roll);

   // Find the requested platform in the map, if it does not exist, add it
   auto it = mPreviousHeading.find(platformIndex);
   if (it == mPreviousHeading.end())
   {
      mPreviousHeading.insert(std::pair<size_t, double>(platformIndex, heading));
      it = mPreviousHeading.find(platformIndex);
   }

   // Check the current heading difference with the previous heading
   if (std::abs(it->second - heading) <= mHeadingChangeLimit)
   {
      exceedsLimit = false;
   }

   // Store curent heading
   it->second = heading;

   return exceedsLimit;
}

void WsfHeatMap::DrawSensorCoverage(double aLocationLLA[3],
                                    double aOrientationNED[3],
                                    double aSensorRange,
                                    double aSensorMinAz,
                                    double aSensorMaxAz)
{
   if (mDrawObjPtr == nullptr)
   {
      return;
   }

   UtEntity temp;
   temp.SetLocationLLA(aLocationLLA[0], aLocationLLA[1], aLocationLLA[2]);
   temp.SetOrientationNED(aOrientationNED[0], aOrientationNED[1], aOrientationNED[2]);

   // Temporary variables used to store calculated line points
   double tempLat      = 0.;
   double tempLon      = 0.;
   double tempAlt      = 0.;
   double offsetNED[3] = {0.0};
   double heading      = aOrientationNED[0];

   mDrawObjPtr->SetColor(1.0, 0.0, 1.0);
   mDrawObjPtr->SetDuration(5.0);
   mDrawObjPtr->BeginPolyline();

   // Own location
   temp.GetLocationLLA(tempLat, tempLon, tempAlt);
   mDrawObjPtr->VertexLLA(tempLat, tempLon, tempAlt);
   // Left
   offsetNED[0] = std::cos(aSensorMinAz - UtMath::cRAD_PER_DEG + heading) * aSensorRange;
   offsetNED[1] = std::sin(aSensorMinAz - UtMath::cRAD_PER_DEG + heading) * aSensorRange;
   temp.ConvertNEDToLLA(offsetNED, tempLat, tempLon, tempAlt);
   mDrawObjPtr->VertexLLA(tempLat, tempLon, tempAlt);
   // Center Left
   offsetNED[0] = std::cos(aSensorMinAz / 2 + heading) * aSensorRange;
   offsetNED[1] = std::sin(aSensorMinAz / 2 + heading) * aSensorRange;
   temp.ConvertNEDToLLA(offsetNED, tempLat, tempLon, tempAlt);
   mDrawObjPtr->VertexLLA(tempLat, tempLon, tempAlt);
   // Center
   offsetNED[0] = std::cos(heading) * aSensorRange;
   offsetNED[1] = std::sin(heading) * aSensorRange;
   temp.ConvertNEDToLLA(offsetNED, tempLat, tempLon, tempAlt);
   mDrawObjPtr->VertexLLA(tempLat, tempLon, tempAlt);
   // Center Right
   offsetNED[0] = std::cos(aSensorMaxAz / 2 + heading) * aSensorRange;
   offsetNED[1] = std::sin(aSensorMaxAz / 2 + heading) * aSensorRange;
   temp.ConvertNEDToLLA(offsetNED, tempLat, tempLon, tempAlt);
   mDrawObjPtr->VertexLLA(tempLat, tempLon, tempAlt);
   // Right
   offsetNED[0] = std::cos(aSensorMaxAz + UtMath::cRAD_PER_DEG + heading) * aSensorRange;
   offsetNED[1] = std::sin(aSensorMaxAz + UtMath::cRAD_PER_DEG + heading) * aSensorRange;
   temp.ConvertNEDToLLA(offsetNED, tempLat, tempLon, tempAlt);
   mDrawObjPtr->VertexLLA(tempLat, tempLon, tempAlt);
   // Back to start
   temp.GetLocationLLA(tempLat, tempLon, tempAlt);
   mDrawObjPtr->VertexLLA(tempLat, tempLon, tempAlt);
   mDrawObjPtr->End();
}
