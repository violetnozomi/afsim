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

#ifndef UTHEATMAP_HPP
#define UTHEATMAP_HPP

#include "ut_export.h"

#include <cmath>
#include <list>
#include <string>
#include <vector>

#include "UtEntity.hpp"

//! @brief This class is used to simulate perception of the last known location of a track.
//! @par
//! All positions are translated onto a 2D grid representation
//! with "heat" representing possible threat/track locations and "cold"
//! representing known/sensor covered areas. "Cold" areas eventually decay
//! to a zero value while "hot" areas remain at the lowest value before they
//! decay to zero.
//! @par
//! The grid is built by providing a center location and grid cell size
//! The center position becomes the center of the cell at (0,0) and the
//! grid expands in all directions. North is positive Y axis and East
//! is positive X axis.
//! @par
//! This class provides the representation and data of the map. It is intended
//! to be a base class that is then extended. The extended class will set the
//! protected attribute to configure the heat map (read input),  provide the
//! interface to entities that fill the map, and provide drawing functions.
class UT_EXPORT UtHeatMap
{
   // TODO Improvements list
   // - Improve heat expansion algorithm to use platform speed/maneuverability
   //   to improve how heat expands
   // - Update map storage to keep heat contributions for multiple sources in a cell
   // - Track each heat source individually so it can be removed if re-detected
   // - Add support for finding a "safe" path between two points that avoids heat
public:
   UtHeatMap();
   UtHeatMap(const double aGridCenterLLA[3], double aGridExtentMeters, double aGridCellSizeMeters);
   virtual ~UtHeatMap();

   //! Sets up the heat map for use. Must be called once before Update()
   virtual bool Initialize();

   //! Update the state of cold areas on the heat map
   //! This could be areas covered by our own sensors
   //! Separate from main Update() to support multiple "cold" contributions
   //! Call before Update() for the most accurate heat map representation
   //! @param aSimTime Current simulation time in seconds
   //! @param aLocationWCS Own location in WCS
   //! @param aSensorRange Sensor range meters
   //! @param aOrientationNED Orientation (NED) of the sensor in radians
   //! @param aSensorMinAz Sensor min AZ radians
   //! @param aSensorMaxAz Sensor max AZ radians
   virtual void UpdateCold(double aSimTime,
                           double aLocationWCS[3],
                           double aSensorRange,
                           double aOrientation[3],
                           double aSensorMinAz,
                           double aSensorMaxAz);

   //! Update the state of the heat map
   //! Uses heat and cold sources to build map
   //! @param aSimTime Current simulation time in seconds
   virtual void Update(double aSimTime);

   //! Clears all heat/cold values from the heatmap
   void ClearMap();

   //! Puts a heat source (e.g. lost track) on the map
   //! @param aSimTime Time location and velocity information were last valid.
   //!                 Note: this does not have to be current time
   //! @param aLocationWCS Last known location of lost track
   //! @param aVelocityWCS Last known velocity of lost track
   void AddHeatSource(double aSimTime, double aLocationWCS[3], double aVelocityWCS[3]);
   // TODO add unique id?

   //! @name Grid size and location Get/Set Methods
   //! Lat/Long are in degrees. Lengths are in meters.
   //@{
   void GetGridCenter(double aGridCenterLLA[3]);
   void SetGridCenter(const double aGridCenterLLA[3]);

   double GetGridExtent() { return mGridExtentMeters; }
   void   SetGridExtent(double aGridExtentMeters) { mGridExtentMeters = aGridExtentMeters; }

   double GetGridSize() { return mGridCellSizeMeters; }
   void   SetGridSize(double aGridCellSizeMeters) { mGridCellSizeMeters = aGridCellSizeMeters; }
   //@}

   //! Given an X,Y position in the grid, return the LLA of the center of the grid cell
   //! @param aX Grid X coordinate
   //! @param aY Grid Y coordinate
   //! @param aLLA Returned array of Latitude, Longitude, Altitude (degrees/meters)
   void GetGridCellCenterLLA(const int aX, const int aY, double aLLA[3]);

   //! Given an X,Y position in the grid, return the LLA of the four corners of the grid cell
   //! @param aX Grid X coordinate
   //! @param aY Grid Y coordinate
   //! @param aTopLeftLLA Returned array of Latitude, Longitude, Altitude (degrees/meters) of top left corner of the cell
   //! @param aTopRightLLA Returned array of Latitude, Longitude, Altitude (degrees/meters) of top right corner of the cell
   //! @param aBottomRightLLA Returned array of Latitude, Longitude, Altitude (degrees/meters) of the bottom right corner of the cell
   //! @param aBottomLeftLLA Returned array of Latitude, Longitude, Altitude (degrees/meters) bottom left corner of the cell
   void GetGridCellCornersLLA(const int aX,
                              const int aY,
                              double    aTopLeftLLA[3],
                              double    aTopRightLLA[3],
                              double    aBottomRightLLA[3],
                              double    aBottomLeftLLA[3]);

   //! Given a LLA position return the X,Y heat map grid cell location
   //! Grid axes start at the center point of the grid. +Y is North. +X is East.
   //! The center cell is 0,0
   //! @param aPositionLLA Array of Latitude, Longitude, Altitude (degrees/meters) to get grid X,Y for
   //! @param aX Returned grid X coordinate corresponding to position
   //! @param aY Returned grid Y coordinate corresponding to position
   void GetGridCellXY(const double aPositionLLA[3], int& aX, int& aY);

   //! Get the LLA of the center of the closest cell to our location that has heat
   //! @param aMyLLA Array of Latitude, Longitude, Altitude (degrees/meters)
   //! @param aNearestLLA Returned array of Latitude, Longitude, Altitude (degrees/meters) of closest "hot" location to my LLA
   //! @param aHeatValue Optional value (0.0 - 1.0), if set finds the nearest cell that has a value greater or equal
   //! @return Returns true if a near location with heat is found, false if none found
   bool GetNearestHeat(const double aMyLLA[3], double aNearestLLA[3], double aHeatValue = 0.0);

   //! Get the LLA of the center of the closest cell to our location that has maximum heat
   //! @param aMyLLA Array of Latitude, Longitude, Altitude (degrees/meters)
   //! @param aNearestLLA Returned array of Latitude, Longitude, Altitude (degrees/meters) of closest maximum "hot"
   //! location to my LLA
   //! @return Returns true if a near location with heat is found, false if none found
   bool GetNearestMaxHeat(const double aMyLLA[3], double aNearestLLA[3]);

   //! Returns the Heat value [-1.0, 1.0] by the cell that contains the passed in LLA
   //! @param aLLA Array of Latitude, Longitude, Altitude (degrees/meters)
   //! @return Returns the heat value [-1.0 , 1.0] at the requested location
   double GetHeatValue(const double aLLA[3]);

   //! Returns the Heat value [-1.0, 1.0] by the cell X,Y coordinates
   //! @param aX/Y Grid cell location X or Y
   //! @return Returns the heat value [-1.0 , 1.0] at the requested location
   double GetHeatValue(const int aX, const int aY);

   //! Returns the number of heat sources that are active (not timed out)
   //! on the map. This can be used to tell if we have lost a track and
   //! is a faster than GetNearestHeat() for asking the map if there is
   //! any heat present
   size_t ActiveHeatSources() { return mHeatSourceList.size(); }

   //! Defines cell locations in the heat map
   struct HeatMapCell
   {
      int gridX; //!< HeatMap grid X location
      int gridY; //!< HeatMap grid Y location

      bool operator==(const HeatMapCell& aRhs) const { return (gridX == aRhs.gridX && gridY == aRhs.gridY); }
      bool operator!=(const HeatMapCell& aRhs) const { return (gridX != aRhs.gridX || gridY != aRhs.gridY); }
      bool operator<(const HeatMapCell& aRhs) const
      {
         return (gridX < aRhs.gridX || (gridX == aRhs.gridX && gridY < aRhs.gridY));
      }
      //! Return the computed distance between two grid cells
      double Distance(const HeatMapCell& aCell) const
      {
         double distance = 0.0;
         double xDist    = (double)(gridX - aCell.gridX);
         double yDist    = (double)(gridY - aCell.gridY);
         if (xDist != 0 || yDist != 0)
         {
            distance = std::sqrt(xDist * xDist + yDist * yDist);
         }
         return distance;
      }
   };

   //! Stores data for heat map cells.
   //! Heat from sources or from our/friendly location and sensor coverage
   struct HeatMapData
   {
      double updateTime; //!< Sim time in seconds the cell was last updated
      double heatValue;  //!< [-1, 1] value of a cell's "heat"
      // TODO May need more data here to track contributing heat
   };

   //! Type definition for a heat map that maps X,Y position to a heat data
   using HeatMap = std::vector<std::vector<HeatMapData>>;

   //! Stores the time and location of a heat expansion event.
   struct ExpandEvent
   {
      double      time;     //!< Expansion time (sec)
      HeatMapCell location; //!< Heat map location to expand heat

      //! Compare operation that will put sooner expansion times first
      bool operator<(const ExpandEvent& rhs) const { return time < rhs.time; }

      //! Equality operation that evaluates only expansion locations.
      //! Used to remove expansion events from the list
      bool operator==(const ExpandEvent& rhs) const { return location == rhs.location; }
   };

   //! Defines a source in the map that generates "heat"
   //! Stores data from a lost track
   struct HeatSource
   {
      double      initialLLA[3]; //!< Location of event that will cause heat on the map (e.g. lost track)
      double      initialWCS[3];
      double      currentWCS[3];
      HeatMapCell location;      //!< HeatMap cell X,Y location of the heat source origin
      double      velociyWCS[3]; //!< Velocity of the event, if known, when added to the heat map
      double      updateTime;    //!< The simulation time (sec) that the heat source was last updated
      double      expireTime;    //!< The simulation time (sec) heat source will stop updating/expanding
      // int updateIncrements;  //!< The number of update increments this heat has existed, helps determine extent/intensity

      std::list<ExpandEvent> expandList;
   };
   //! Defines a list of heat sources
   using HeatSourceList = std::list<HeatSource>;

   //! Predicate that returns True if heat source can be removed from the list
   struct HeatSourceExpired
   {
      bool operator()(const HeatSource& aSource) { return (aSource.updateTime >= aSource.expireTime); }
   };

   //! Defines an X,Y,Z point, used in calculations for sensor coverage
   //! X/Y/Z can be used to represent a coordinate in a graph, LLA, etc.
   struct Point
   {
      double mX; //!< X location
      double mY; //!< Y location
      double mZ; //!< Z location
   };

protected:
   //! Latitude (rad) location of the center point of the center cell of the heat map grid
   double mGridCenterLat;

   //! Longitude (rad) location of the center point of the center cell of the heat map grid
   double mGridCenterLon;

   //! Altitude (m) of the center point of the center cell of the heat map grid
   double mGridCenterAlt;

   //! UtEntity that represents the center point of the grid. Used to make position calculations in the grid
   UtEntity mCenterPoint;

   //! Distance from the center point to the center of an edge cell of the heat map grid in meters
   double mGridExtentMeters;

   //! Size of one side of a heat map grid cell in meters
   double mGridCellSizeMeters;

   //! Number of cells the grid extends from the center cell, excluding the center cell
   int mNumberOfCells;

   //! North or East offset distance in meters from the center to an edge of the grid
   double mMaxOffset;

   //! How much a heat value is reduced by each update. Default of 0.1.
   double mHeatDecay;

   //! Heat decay interval, how often a grid value is updated, default of 30 sec.
   double mHeatDecayInterval;

   //! Maximum time a heat source will last/expand, default of 240 sec (4 min).
   double mHeatTimeout;

   //! Flag, true when all heat map values required to operate are valid
   bool mInitialized;

private:
   //! @name Heat/Cold Data storage
   //! HeatSourceList stores items that generate heat in the map
   //! The HeatMap combines the heat/cold lists to provide cells that have
   //!   a heat/cold value and represents the overall heat map
   //! CurrentlyCleared stores the active "cold" points in the map, cells covered
   //!   by our location or sensors
   //@{
   HeatSourceList           mHeatSourceList;
   HeatMap                  mHeatMap;
   std::vector<HeatMapCell> mCurrentlyCleared;
   //@}

   // TODO Add single event queue to handle heat expansion fill?

   void DecayHeat(double aSimTime);

   //! Uses the heat generating points list to populate the map with heat
   void ExpandHeat(double aSimTime);

   //! Checks the currently cleared "cold" cells and returns true
   //! if the cell is actively being cleared
   bool CurrentlyCleared(int aX, int aY);

   //! Sets the Heat Map with the passed in value
   //! If there is no value at the location currently the passed in value is put on the map
   //! It there is already a value at the location, the current value is set to the passed in value
   //! Constrains values to -1.0 to 1.0 range
   void SetMapValue(double aSimTime, int aX, int aY, double aValue);

   //! Removes future heat expansion events
   //! Used by SetSensorCoverage to remove any future heat expansion
   //! events that are currently cleared by "cold" sensor coverage
   void RemoveHeatExpansion(double aSimTime, const HeatMapCell& aCleared);

   //! Set the area inside a polygon defined by the points
   //! as "cold" covered by the sensor
   void SetSensorCoverage(double aSimTime, std::vector<Point>& aPoints);

   //! Checks is a grid cell center point is in the sensor area defined by the points
   // Borrowed from WsfZoneDefinition
   bool WithinPolygonSides(std::vector<Point>& aPoints, const double aXorLatValue, const double aYorLonValue) const;

   //! Validates that an X or Y coordinate is in the heat map grid
   //! Single function as the grid is a square
   bool InGrid(const int aXY);
   //! Validates that a heat map cell is in the heat map grid
   bool InGrid(const HeatMapCell& aCell);
};

#endif
