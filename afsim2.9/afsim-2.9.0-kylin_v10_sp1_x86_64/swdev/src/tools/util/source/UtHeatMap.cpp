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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtHeatMap.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"

// Heat Map constants
namespace
{
//! Min/None/Max values for heat
const double cMAX_HEAT = 1.0;
const double cNO_HEAT  = 0.0;
const double cMAX_COLD = -1.0;
} // namespace

UtHeatMap::UtHeatMap(const double aGridCenterLLA[3], double aGridExtentMeters, double aGridCellSizeMeters)
   : mGridCenterLat(aGridCenterLLA[0])
   , mGridCenterLon(aGridCenterLLA[1])
   , mGridCenterAlt(aGridCenterLLA[2])
   , mGridExtentMeters(aGridExtentMeters)
   , mGridCellSizeMeters(aGridCellSizeMeters)
   , mNumberOfCells(int(aGridExtentMeters / aGridCellSizeMeters))
   , mMaxOffset(aGridExtentMeters + (aGridCellSizeMeters / 2))
   , mHeatDecay(0.1)
   , mHeatDecayInterval(30)
   , mHeatTimeout(240)
   , mInitialized(false)
{
}

UtHeatMap::UtHeatMap()
   : mGridCenterLat(0)
   , mGridCenterLon(0)
   , mGridCenterAlt(0)
   , mGridExtentMeters(0)
   , mGridCellSizeMeters(0)
   , mNumberOfCells(0)
   , mMaxOffset(0.0)
   , mHeatDecay(0.1)
   , mHeatDecayInterval(30)
   , mHeatTimeout(240)
   , mInitialized(false)
{
}

UtHeatMap::~UtHeatMap()
{
   ClearMap();
}

bool UtHeatMap::Initialize()
{
   // Make sure we have the inputs we need to build the heat map
   // We need a size for the grid cells, an overall size for the grid,
   // and the center position needs to be something other than the default 0,0,0
   mInitialized =
      (mGridCellSizeMeters > 0) && (mGridExtentMeters > 0) && ((mGridCenterLat + mGridCenterLon + mGridCenterAlt) != 0);

   if (mInitialized)
   {
      if (fmod((mGridExtentMeters * 2), mGridCellSizeMeters) != 0)
      {
         auto out = ut::log::warning() << "Heatmap grid cell size does not divide evenly into overall grid size.";
         out.AddNote() << "Heatmap Grid Size: " << mGridCellSizeMeters << " m";
         out.AddNote() << "Overall Grid Size: " << (mGridExtentMeters * 2) << " m";
         out.AddNote() << "Grid drawing may look strange on east and south edges.";
      }

      // Set values used in position calculations
      mCenterPoint.SetLocationLLA(mGridCenterLat, mGridCenterLon, mGridCenterAlt);
      mMaxOffset     = mGridExtentMeters + (mGridCellSizeMeters / 2);
      mNumberOfCells = int(mGridExtentMeters / mGridCellSizeMeters);

      // Initialize heat map X, Y
      int size = (mNumberOfCells * 2) + 1;
      mHeatMap.resize(size, std::vector<HeatMapData>(size));
   }

   return mInitialized;
}

void UtHeatMap::UpdateCold(double aSimTime,
                           double aLocationWCS[3],
                           double aSensorRange,
                           double aOrientationNED[3],
                           double aSensorMinAz,
                           double aSensorMaxAz)
{
   // Sensor Coverage
   //  Build vector of points that define a shape the sensor covers
   //  Notes:
   //    - Forward arc is represented by 5 points. This gives better
   //      representation for sensors with a wide field of view but
   //      is still limited to about +/- 45 deg
   //    - Orientation only uses Yaw for now to get the heading of the sensor
   std::vector<Point> senrorCoverage;

   // Temporary variables used to store calculated sensor coverage line points
   UtEntity temp;
   temp.SetLocationWCS(aLocationWCS);
   double tempLLA[3]   = {0};
   double offsetNED[3] = {0};
   Point  tempPoint;

   // Own location
   temp.GetLocationLLA(tempLLA[0], tempLLA[1], tempLLA[2]);
   tempPoint.mX = tempLLA[0];
   tempPoint.mY = tempLLA[1];
   tempPoint.mZ = tempLLA[2];
   senrorCoverage.push_back(tempPoint);

   // Add cold to map for Own position
   HeatMapCell own;
   GetGridCellXY(tempLLA, own.gridX, own.gridY);
   SetMapValue(aSimTime, own.gridX, own.gridY, cMAX_COLD);
   mCurrentlyCleared.push_back(own);

   // Left - expanded +1 degree to make sure we get good coverage
   offsetNED[0] = std::cos(aSensorMinAz - UtMath::cRAD_PER_DEG + aOrientationNED[0]) * aSensorRange;
   offsetNED[1] = std::sin(aSensorMinAz - UtMath::cRAD_PER_DEG + aOrientationNED[0]) * aSensorRange;
   temp.ConvertNEDToLLA(offsetNED, tempLLA[0], tempLLA[1], tempLLA[2]);
   tempPoint.mX = tempLLA[0];
   tempPoint.mY = tempLLA[1];
   tempPoint.mZ = tempLLA[2];
   senrorCoverage.push_back(tempPoint);

   // Center Left
   offsetNED[0] = std::cos(aSensorMinAz / 2 + aOrientationNED[0]) * aSensorRange;
   offsetNED[1] = std::sin(aSensorMinAz / 2 + aOrientationNED[0]) * aSensorRange;
   temp.ConvertNEDToLLA(offsetNED, tempLLA[0], tempLLA[1], tempLLA[2]);
   tempPoint.mX = tempLLA[0];
   tempPoint.mY = tempLLA[1];
   tempPoint.mZ = tempLLA[2];
   senrorCoverage.push_back(tempPoint);

   // Center
   offsetNED[0] = std::cos(aOrientationNED[0]) * aSensorRange;
   offsetNED[1] = std::sin(aOrientationNED[0]) * aSensorRange;
   temp.ConvertNEDToLLA(offsetNED, tempLLA[0], tempLLA[1], tempLLA[2]);
   tempPoint.mX = tempLLA[0];
   tempPoint.mY = tempLLA[1];
   tempPoint.mZ = tempLLA[2];
   senrorCoverage.push_back(tempPoint);

   // Center Right
   offsetNED[0] = std::cos(aSensorMaxAz / 2 + aOrientationNED[0]) * aSensorRange;
   offsetNED[1] = std::sin(aSensorMaxAz / 2 + aOrientationNED[0]) * aSensorRange;
   temp.ConvertNEDToLLA(offsetNED, tempLLA[0], tempLLA[1], tempLLA[2]);
   tempPoint.mX = tempLLA[0];
   tempPoint.mY = tempLLA[1];
   tempPoint.mZ = tempLLA[2];
   senrorCoverage.push_back(tempPoint);

   // Right - expanded +1 degree to make sure we get good coverage
   offsetNED[0] = std::cos(aSensorMaxAz + UtMath::cRAD_PER_DEG + aOrientationNED[0]) * aSensorRange;
   offsetNED[1] = std::sin(aSensorMaxAz + UtMath::cRAD_PER_DEG + aOrientationNED[0]) * aSensorRange;
   temp.ConvertNEDToLLA(offsetNED, tempLLA[0], tempLLA[1], tempLLA[2]);
   tempPoint.mX = tempLLA[0];
   tempPoint.mY = tempLLA[1];
   tempPoint.mZ = tempLLA[2];
   senrorCoverage.push_back(tempPoint);

   // Set "cold" in heat map
   SetSensorCoverage(aSimTime, senrorCoverage);
}

void UtHeatMap::Update(double aSimTime)
{
   if (mInitialized)
   {
      // Fill new heat values in the map
      ExpandHeat(aSimTime);

      // Reduce heat value of existing heat cells
      DecayHeat(aSimTime);

      // Clear list of currently covered "cold" cells
      mCurrentlyCleared.clear();
   }
}

void UtHeatMap::ClearMap()
{
   mHeatSourceList.clear();
   mHeatMap.clear();
}

void UtHeatMap::AddHeatSource(double aSimTime, double aLocationWCS[3], double aVelocityWCS[3])
{
   HeatSource newSource;

   // Store the "heat point" location and velocity
   // Calculate and Set grid location
   double lla[3] = {0};
   UtEntity::ConvertWCSToLLA(aLocationWCS, lla[0], lla[1], lla[2]);
   newSource.initialLLA[0] = lla[0];
   newSource.initialLLA[1] = lla[1];
   newSource.initialLLA[2] = lla[2];
   newSource.initialWCS[0] = aLocationWCS[0];
   newSource.initialWCS[1] = aLocationWCS[1];
   newSource.initialWCS[2] = aLocationWCS[2];
   newSource.currentWCS[0] = aLocationWCS[0];
   newSource.currentWCS[1] = aLocationWCS[1];
   newSource.currentWCS[2] = aLocationWCS[2];

   newSource.velociyWCS[0] = aVelocityWCS[0];
   newSource.velociyWCS[1] = aVelocityWCS[1];
   newSource.velociyWCS[2] = aVelocityWCS[2];

   GetGridCellXY(lla, newSource.location.gridX, newSource.location.gridY);

   // Check that the location is inside the heat map grid and the
   // location is not currently being cleared by a sensor, which could occur
   // when a track is destroyed
   if (InGrid(newSource.location.gridX) && InGrid(newSource.location.gridY) &&
       !CurrentlyCleared(newSource.location.gridX, newSource.location.gridY))
   {
      // Put initial cell in heat map
      SetMapValue(aSimTime, newSource.location.gridX, newSource.location.gridY, cMAX_HEAT);

      // Store add time as initial update time
      newSource.updateTime = aSimTime;
      newSource.expireTime = aSimTime + mHeatTimeout;

      // Store how often update the spread of the heat
      // Based on how long it takes to cover the distance of a grid cell
      // If no velocity is specified pick a default update time
      // TODO if it doesn't move put a single non-expanding cell?
      double speed = UtVec3d::Magnitude(aVelocityWCS);
      if (speed <= 0.0001)
      {
         speed = 1; // m/s
      }

      // Build initial queue of cells to update around center cell
      for (int x = newSource.location.gridX - 1; x <= newSource.location.gridX + 1; ++x)
      {
         for (int y = newSource.location.gridY - 1; y <= newSource.location.gridY + 1; ++y)
         {
            // Don't add to the expand queue if a cell is actively being cleared
            // and the coordinates are the source origin
            if (!CurrentlyCleared(x, y) && (x != newSource.location.gridX || y != newSource.location.gridY))
            {
               // Build expansion event and add it to the end of the list
               ExpandEvent expansion;
               expansion.location.gridX = x;
               expansion.location.gridY = y;
               expansion.time = aSimTime + (expansion.location.Distance(newSource.location) * mGridCellSizeMeters) / speed;
               newSource.expandList.push_back(expansion);
            }
         }
      }

      // Add to the list of sources that generate "heat"
      mHeatSourceList.push_back(newSource);
   }
}

void UtHeatMap::GetGridCenter(double aGridCenterLLA[3])
{
   aGridCenterLLA[0] = mGridCenterLat;
   aGridCenterLLA[1] = mGridCenterLon;
   aGridCenterLLA[2] = mGridCenterAlt;
}

void UtHeatMap::SetGridCenter(const double aGridCenterLLA[3])
{
   mGridCenterLat = aGridCenterLLA[0];
   mGridCenterLon = aGridCenterLLA[1];
   mGridCenterAlt = aGridCenterLLA[2];
}

void UtHeatMap::GetGridCellCenterLLA(const int aX, const int aY, double aLLA[3])
{
   // Validate X,Y are in grid
   if (InGrid(aX) && InGrid(aY))
   {
      // Calculate NED offset from center point
      double centerOffsetNED[3] = {0};
      centerOffsetNED[0]        = aY * mGridCellSizeMeters;
      centerOffsetNED[1]        = aX * mGridCellSizeMeters;
      centerOffsetNED[2]        = 0;

      // Convert to LLA
      mCenterPoint.ConvertNEDToLLA(centerOffsetNED, aLLA[0], aLLA[1], aLLA[2]);
   }
   else
   {
      aLLA[0] = mGridCenterLat;
      aLLA[1] = mGridCenterLon;
      aLLA[2] = mGridCenterAlt;
   }
}

void UtHeatMap::GetGridCellCornersLLA(const int aX,
                                      const int aY,
                                      double    aTopLeftLLA[3],
                                      double    aTopRightLLA[3],
                                      double    aBottomRightLLA[3],
                                      double    aBottomLeftLLA[3])
{
   // Calculate offsets from grid center point to cell center based on grid X,Y
   double centerN = aY * mGridCellSizeMeters;
   double centerE = aX * mGridCellSizeMeters;

   // Adjust offset to each of the four corners
   double halfCell          = mGridCellSizeMeters / 2;
   double topLeftNED[3]     = {(centerN + halfCell), (centerE - halfCell), 0};
   double topRightNED[3]    = {(centerN + halfCell), (centerE + halfCell), 0};
   double bottomRightNED[3] = {(centerN - halfCell), (centerE + halfCell), 0};
   double bottomLeftNED[3]  = {(centerN - halfCell), (centerE - halfCell), 0};

   mCenterPoint.ConvertNEDToLLA(topLeftNED, aTopLeftLLA[0], aTopLeftLLA[1], aTopLeftLLA[2]);
   mCenterPoint.ConvertNEDToLLA(topRightNED, aTopRightLLA[0], aTopRightLLA[1], aTopRightLLA[2]);
   mCenterPoint.ConvertNEDToLLA(bottomRightNED, aBottomRightLLA[0], aBottomRightLLA[1], aBottomRightLLA[2]);
   mCenterPoint.ConvertNEDToLLA(bottomLeftNED, aBottomLeftLLA[0], aBottomLeftLLA[1], aBottomLeftLLA[2]);
}

void UtHeatMap::GetGridCellXY(const double aPositionLLA[3], int& aX, int& aY)
{
   // Given a LLA, calculate NED offset from center point
   double offsetNED[3] = {0};
   mCenterPoint.ConvertLLAToNED(aPositionLLA[0], aPositionLLA[1], aPositionLLA[2], offsetNED);
   // Adjust N/E to account for center zero cells taking up half a
   // cell width in offset. This is ugly and could be much nicer
   // with c++11 copysign(), or use UtMath::Sign()
   double halfCell = mGridCellSizeMeters / 2;
   if (offsetNED[0] > 0)
   {
      offsetNED[0] += halfCell;
   }
   else if (offsetNED[0] < 0)
   {
      offsetNED[0] -= halfCell;
   }
   if (offsetNED[1] > 0)
   {
      offsetNED[1] += halfCell;
   }
   else if (offsetNED[1] < 0)
   {
      offsetNED[1] -= halfCell;
   }

   // Calculate the X,Y grid location by dividing
   // adjusted N/E by size of cells, truncate to an int
   aX = int(offsetNED[1] / mGridCellSizeMeters);
   aY = int(offsetNED[0] / mGridCellSizeMeters);
}

bool UtHeatMap::GetNearestHeat(const double aMyLLA[3], double aNearestLLA[3], double aHeatValue /*=0.0*/)
{
   // Set a huge initial distance
   double shortestDistance = 9999999;
   bool   heatFound        = false;

   // Get the location we will be comparing distances to
   // Note this (X,Y) value may be outside the actual map vector structure
   HeatMapCell myCell;
   GetGridCellXY(aMyLLA, myCell.gridX, myCell.gridY);

   // Loop through HeatMap list
   // TODO not efficient, search out from MyLLA position
   std::vector<std::vector<HeatMapData>>::const_iterator itX;
   std::vector<HeatMapData>::const_iterator              itY;
   for (itX = mHeatMap.begin(); itX != mHeatMap.end(); ++itX)
   {
      for (itY = itX->begin(); itY != itX->end(); ++itY)
      {
         const HeatMapData& data = *itY;
         // For any cell with a heat value > 0 and greater than or equal to the requested heat
         if (data.heatValue > cNO_HEAT && data.heatValue >= aHeatValue)
         {
            // Compare the distance from my location cell to the current hot cell
            // If it is closer than any other seen so far save that as the closest location
            HeatMapCell currentCell;
            currentCell.gridX = (int)(itX - mHeatMap.begin() - mNumberOfCells);
            currentCell.gridY = (int)(itY - itX->begin() - mNumberOfCells);
            double distance   = myCell.Distance(currentCell);
            if (distance < shortestDistance)
            {
               shortestDistance = distance;
               heatFound        = true;
               GetGridCellCenterLLA(currentCell.gridX, currentCell.gridY, aNearestLLA);
            }
         }
      }
   } // End map for loops

   // If a cell is found, return true, if not (maybe no "hot" cells) return false
   return heatFound;
}

bool UtHeatMap::GetNearestMaxHeat(const double aMyLLA[3], double aNearestLLA[3])
{
   return GetNearestHeat(aMyLLA, aNearestLLA, cMAX_HEAT);
}

double UtHeatMap::GetHeatValue(const int aX, const int aY)
{
   double heatValue = 0;

   if (InGrid(aX) && InGrid(aY))
   {
      // NOTE: The index into the data structure is mapped
      // by adding the number of cells to the edge to the X,Y
      // because the data structure starts (0,0) in the lower left
      // corner while the logical representation of the map has the
      // center cell as (0,0)
      heatValue = mHeatMap[aX + mNumberOfCells][aY + mNumberOfCells].heatValue;
   }

   return heatValue;
}

double UtHeatMap::GetHeatValue(const double aLLA[3])
{
   int x = 0;
   int y = 0;
   GetGridCellXY(aLLA, x, y);
   return GetHeatValue(x, y);
}

// PRIVATE

void UtHeatMap::DecayHeat(double aSimTime)
{
   // For any map cell that was last updated longer ago than the update interval
   // Reduce the value by the heat decay

   double deltaT = aSimTime - mHeatDecayInterval;

   std::vector<std::vector<HeatMapData>>::iterator itX;
   std::vector<HeatMapData>::iterator              itY;
   for (itX = mHeatMap.begin(); itX != mHeatMap.end(); ++itX)
   {
      for (itY = itX->begin(); itY != itX->end(); ++itY)
      {
         HeatMapData& data = *itY;

         if (data.heatValue > 0.0 && data.updateTime <= deltaT)
         {
            data.updateTime = aSimTime;
            double newHeat  = data.heatValue - mHeatDecay;
            if (newHeat > 0.0)
            {
               data.heatValue = newHeat;
            }
            // else keep heat, no expiration until cleared
         }
         else if (data.heatValue < 0.0 && data.updateTime <= deltaT)
         {
            data.updateTime = aSimTime;
            double newHeat  = data.heatValue + mHeatDecay;
            if (newHeat < 0.0)
            {
               data.heatValue = newHeat;
            }
            else
            {
               data.heatValue = 0.0;
            }
         }
      } // End heat cells Y loop
   }    // End heat cells X loop
}

void UtHeatMap::ExpandHeat(double aSimTime)
{
   for (std::list<HeatSource>::iterator it = mHeatSourceList.begin(); it != mHeatSourceList.end(); ++it)
   {
      HeatSource& heat = *it;
      heat.updateTime  = aSimTime;

      // Flood fill heat values to surrounding cells for any cell that has
      // reached its expansion time
      while ((!heat.expandList.empty()) && heat.expandList.begin()->time <= aSimTime)
      {
         ExpandEvent expand = *heat.expandList.begin();

         if (!CurrentlyCleared(expand.location.gridX, expand.location.gridY))
         {
            SetMapValue(expand.time, expand.location.gridX, expand.location.gridY, cMAX_HEAT);

            double speed = UtVec3d::Magnitude(heat.velociyWCS);
            if (speed <= 0.0001)
            {
               speed = 1; // m/s
            }

            // Build queue of new cells to update around center cell just expanded to
            for (int x = expand.location.gridX - 1; x <= expand.location.gridX + 1; ++x)
            {
               for (int y = expand.location.gridY - 1; y <= expand.location.gridY + 1; ++y)
               {
                  // Don't add to the expand queue if a cell is actively being cleared,
                  // the simulation time is beyond the heat expansion time,
                  // already has high heat in it, (TODO make this only on heat from me)
                  // and the coordinates are the source origin
                  if (InGrid(x) && InGrid(y) && aSimTime < heat.expireTime && !CurrentlyCleared(x, y) &&
                      GetHeatValue(x, y) < (cMAX_HEAT / 3) && (x != expand.location.gridX || y != expand.location.gridY))
                  {
                     // Push onto the queue
                     ExpandEvent newExpansion;
                     newExpansion.location.gridX = x;
                     newExpansion.location.gridY = y;
                     newExpansion.time =
                        expand.time + (newExpansion.location.Distance(expand.location) * mGridCellSizeMeters) / speed;
                     heat.expandList.push_back(newExpansion);
                  }
               }
            }
         } // End check for cleared
         // Remove this expansion event and move on to the next
         heat.expandList.pop_front();
      }
      // Done removing current expand events and adding new ones, resort list
      // to put sooner expansion times first
      heat.expandList.sort();

   } // End heat point iterator loop

   // Check for heat points to remove
   mHeatSourceList.remove_if(HeatSourceExpired());
}

bool UtHeatMap::CurrentlyCleared(int aX, int aY)
{
   bool cleared = false;

   HeatMapCell check;
   check.gridX = aX;
   check.gridY = aY;

   for (std::vector<HeatMapCell>::iterator it = mCurrentlyCleared.begin(); it != mCurrentlyCleared.end(); ++it)
   {
      if (check == *it)
      {
         cleared = true;
         break;
      }
   }

   return cleared;
}

void UtHeatMap::SetMapValue(double aSimTime, int aX, int aY, double aValue)
{
   if (InGrid(aX) && InGrid(aY))
   {
      // Make sure value is between -1 and 1
      double newValue = std::min(std::max(cMAX_COLD, aValue), cMAX_HEAT);

      mHeatMap[aX + mNumberOfCells][aY + mNumberOfCells].heatValue  = newValue;
      mHeatMap[aX + mNumberOfCells][aY + mNumberOfCells].updateTime = aSimTime;
   }
}

void UtHeatMap::RemoveHeatExpansion(double aSimTime, const HeatMapCell& aCleared)
{
   if (InGrid(aCleared))
   {
      // Create a temporary expansion event for the cleared location
      // that will be compared with the expansion lists
      ExpandEvent clearedLocation;
      clearedLocation.time     = aSimTime;
      clearedLocation.location = aCleared;

      // For each heat source remove any scheduled heat expansion at the location
      // that is currently being cleared
      for (std::list<HeatSource>::iterator it = mHeatSourceList.begin(); it != mHeatSourceList.end(); ++it)
      {
         HeatSource& heat = *it;
         heat.expandList.remove(clearedLocation);
      }
   }
}

void UtHeatMap::SetSensorCoverage(double aSimTime, std::vector<Point>& aPoints)
{
   // Get the X,Y corners of a box the sensor coverage is in.
   // This will reduce the number of cells checked
   double cellLLA[3] = {0};
   int    smallestX  = 0;
   int    largestX   = 0;
   int    smallestY  = 0;
   int    largestY   = 0;
   for (std::vector<Point>::const_iterator it = aPoints.begin(); it != aPoints.end(); ++it)
   {
      cellLLA[0] = it->mX;
      cellLLA[1] = it->mY;
      cellLLA[2] = it->mZ;
      int X      = 0;
      int Y      = 0;
      GetGridCellXY(cellLLA, X, Y);
      if (X < smallestX)
      {
         smallestX = X;
      }
      if (X > largestX)
      {
         largestX = X;
      }
      if (Y < smallestY)
      {
         smallestY = Y;
      }
      if (Y > largestY)
      {
         largestY = Y;
      }
   }
   // Search grid cells in the box that covers the
   // sensor area. Any that have their center
   // point in the sensor coverage area are "cold"
   for (int x = smallestX; x <= largestX; ++x)
   {
      for (int y = smallestY; y <= largestY; ++y)
      {
         GetGridCellCenterLLA(x, y, cellLLA);
         if (WithinPolygonSides(aPoints, cellLLA[0], cellLLA[1]))
         {
            SetMapValue(aSimTime, x, y, cMAX_COLD);
            // Add to list of currently covered cells
            // and clear any pending heat expansion to that cell
            HeatMapCell temp;
            temp.gridX = x;
            temp.gridY = y;
            mCurrentlyCleared.push_back(temp);
            RemoveHeatExpansion(aSimTime, temp);
         }
      }
   }
}

bool UtHeatMap::WithinPolygonSides(std::vector<Point>& aPoints, const double aXorLatValue, const double aYorLonValue) const
{
   // Check for point within a polygon.
   const double bndryTol = /*1.0E-20;*/ 1.0E-5;
   const double otherTol = /*1.0E-10;*/ 1.0E-2;

   double x = aXorLatValue;
   double y = aYorLonValue;

   double xc1 = aPoints[0].mX;
   double yc1 = aPoints[0].mY;

   double ang1 = 0.0;
   if ((fabs(yc1 - y) > otherTol) || (fabs(xc1 - x) > otherTol))
   {
      ang1 = atan2(yc1 - y, xc1 - x);
   }

   double angN1      = ang1;
   double angTot     = 0.0;
   bool   onBoundary = false;
   bool   inRegion   = false;

   double xc2;
   double yc2;
   double ang2;

   for (std::vector<Point>::size_type index = 1; (index <= aPoints.size()) && (!onBoundary); ++index)
   {
      if (index == aPoints.size())
      {
         xc2  = aPoints[0].mX;
         yc2  = aPoints[0].mY;
         ang2 = angN1;
      }
      else
      {
         xc2  = aPoints[index].mX;
         yc2  = aPoints[index].mY;
         ang2 = 0.0;
         if ((fabs(yc2 - y) > otherTol) || (fabs(xc2 - x) > otherTol))
         {
            ang2 = atan2(yc2 - y, xc2 - x);
         }
      }

      double delta = ang2 - ang1;

      if (fabs(delta) > UtMath::cPI)
      {
         delta -= 2.0 * UtMath::cPI * UtMath::Sign(delta);
      }

      angTot += delta;

      double bndry = -(y - yc1) * (xc2 - xc1) + (x - xc1) * (yc2 - yc1);
      double dist  = sqrt((xc2 - xc1) * (xc2 - xc1) + (yc2 - yc1) * (yc2 - yc1));

      if (fabs(bndry) <= (dist * bndryTol))
      {
         if ((((xc1 <= x) && (x <= xc2)) || ((xc2 <= x) && (x <= xc1))) &&
             (((yc1 <= y) && (y <= yc2)) || ((yc2 <= y) && (y <= yc1))))
         {
            onBoundary = true;
         }
      }

      ang1 = ang2;
      xc1  = xc2;
      yc1  = yc2;
   }

   if (fabs(angTot) > 1.0)
   {
      inRegion = true;
   }
   if (onBoundary)
   {
      inRegion = true;
   }

   return inRegion;
}

bool UtHeatMap::InGrid(const int aXY)
{
   return (aXY >= -mNumberOfCells && aXY <= mNumberOfCells);
}

bool UtHeatMap::InGrid(const HeatMapCell& aCell)
{
   return (InGrid(aCell.gridX) && InGrid(aCell.gridY));
}
