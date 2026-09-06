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

#ifndef WSFHEATMAP_HPP
#define WSFHEATMAP_HPP

#include <map>
#include <vector>

#include "UtHeatMap.hpp"
class UtInput;
class UtInputBlock;
#include "wsf_mil_export.h"

#include "WsfAssetPerception.hpp"
#include "WsfDraw.hpp"
class WsfLocalTrack;
#include "WsfPlatform.hpp"

//! This class is used to simulate perception of the last known location
//! of a track. It is intended to be used with the WsfPerceptionProcessor
//! and provides an interface to the UtHeatMap. It provides the input
//! to the base utility class and handles the drawn representation of
//! the heat map.
class WSF_MIL_EXPORT WsfHeatMap : public UtHeatMap
{
public:
   WsfHeatMap(const double aGridCenterLLA[3], double aGridExtentMeters, double aGridCellSizeMeters);
   WsfHeatMap(const WsfHeatMap& aSrc);
   WsfHeatMap& operator=(const WsfHeatMap&) = delete;
   WsfHeatMap();

   ~WsfHeatMap() override;

   virtual bool Initialize(WsfPlatform* aPlatformPtr);

   bool ProcessInput(UtInput& aInput);

   //! Updates the state of the heat map values in the grid
   //! @param aSimTime Current simulation time in seconds
   //! @param aUpdateInterval How often the processor is updating in seconds
   //! @param aPerceivedAssets A list of asset platforms
   void Update(double aSimTime, double aUpdateInterval, std::vector<WsfAssetPerception*>& aPerceivedAssets);

   //! Called to add a track to the heat map
   void LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   //! @name HeatMap Display Methods
   //! These will generate WsfDraw commands to visualize the heat map
   //! for the length of time specified by the DrawDuration argument
   //@{
   void DrawGrid(double aDrawDuration);
   void DrawHeat(double aDrawDuration);
   //@}

private:
   using UtHeatMap::Initialize;
   using UtHeatMap::Update;

   WsfPlatform*             mPlatformPtr; //!< Owning platform, used to get sensor related info
   std::map<size_t, double> mPreviousHeading; //!< Stores the heading (radians) at last update of platforms contributing "cold"
   double mHeadingChangeLimit;   //!< The max amount a platform heading can change (radians) between updates and still
                                 //!< contribute "cold"
   bool     mUseAssetPerception; //!< Flag that indicates heat map should use asset sensors in cold coverage
   double   mSensorRange;        //!< Max range in meters, temporary until calculation figured out
   bool     mDrawGrid;           //!< Flag that controls activation of drawing the heatmap grid
   bool     mGridDrawn;          //!< Flag that indicates drawing the heatmap grid is complete
   bool     mDrawHeat;           //!< Flag that controls drawing of "heat" sqauares from map
   bool     mDrawSensor;         //!< Flag that controls drawing of sensor outlines
   WsfDraw* mDrawObjPtr;         //!< Draw object used to draw grid and heat squares

   //! Method called by Update() to set the "cold"
   //! sensor coverage values in the heat map
   //! @param aSimTime Current simulation time in seconds
   //! @param aPlatformPtr The platform we are adding cold for
   //! @param aLocationWCS The WCS location of the platform
   //! @param aOrientationNED the NED orientation of the platform in radians
   void UpdateCold(double aSimTime, WsfPlatform* aPlaformPtr, double aLocationWCS[3], double aOrientationNED[3]);

   //! Method called by UpdateCold() to verify that the
   //! platform heading has not changed by more then the
   //! heading limit since the last update
   bool HeadingExceedsLimit(WsfPlatform* aPlaformPtr);

   // Test method for verifying heat map functions
   void DrawSensorCoverage(double aLocationLLA[3],
                           double aOrentationNED[3],
                           double aSensorRange,
                           double aSensorMinAz,
                           double aSensorMaxAz);
};

#endif
