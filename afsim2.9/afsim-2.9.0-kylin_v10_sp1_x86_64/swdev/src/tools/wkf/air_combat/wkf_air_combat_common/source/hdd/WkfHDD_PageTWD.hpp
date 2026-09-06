// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFHDD_PAGETWD_HPP
#define WKFHDD_PAGETWD_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_Page.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PageTWD : public Page
{
public:
   struct TWD_Data
   {
      // Countermeasures
      int    numChaff{0};      ///< Number of chaff
      int    numFlares{0};     ///< Number of flares
      int    numDecoys{0};     ///< Number of decoys
      double heading_deg{0.0}; ///< Heading for the aircraft in degrees
      double simTime{0.0};     ///< Sim time

      std::map<size_t, SimData::TargetTrackItem> tracks; ///< Map of target index to tracks. These will be RWR/ESM tracks
   };

   PageTWD(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   ~PageTWD() override = default;

   /** Set the heading for the TWD.  The compass is oriented to this.
    * @param aInput heading to set.
    */
   void SetOwnshipHeading_deg(const double aInput_deg) { mData.heading_deg = aInput_deg; }

   /** Set the countermeasures for this platform (chaff, flares, decoys).
    * @param aNumChaff chaff to set.
    * @param aNumFlares flares to set.
    * @param aNumDecoys decoys to set.
    */
   void SetCountermeasures(const int aNumChaff, const int aNumFlares, const int aNumDecoys);

   /** Set the ESM and RWR track data for the TWD.
    * @param aData track processor list data to pull from
    * @param aESM_TrackProcessor name of ESM/RWR track processor (may be an empty string if not defined)
    */
   void Set_ESM_RWR_TrackData(const std::vector<SimData::TrackProcessor>& aData, const std::string aESM_TrackProcessor);

   /** Set the current sim time for the TWD.  This is used for filtering new/existing tracks.
    * @param aSimTime sim time to set.
    */
   void SetSimTime(const double aSimTime) { mData.simTime = aSimTime; }

private:
   /** Draw this page. */
   void Draw() override;

   /** Draw the TWD compass.  This will have ticks every 15 degrees, and large ticks every 45 degrees.
    * The compass rotates with the platform's heading.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the compass.
    */
   void DrawCompass(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aR);

   /** Draw the heading box.  This draws the current heading in degrees.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale of the drawing.
    */
   void DrawHeadingBox(osg::ref_ptr<osg::Group> aVectorGroup,
                       const double             aX,
                       const double             aY,
                       const double             aW,
                       const double             aH,
                       const double             aScale);

   /** Draw the countermeasures labels.  This includes chaff, decoys, and flares.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale of the drawing.
    */
   void DrawCountermeasures(osg::ref_ptr<osg::Group> aVectorGroup,
                            const double             aX,
                            const double             aY,
                            const double             aW,
                            const double             aH,
                            const double             aScale);

   /** Draw an angle only entity, given a track.
    * @param aVectorGroup vector group to add geometry to.
    * @param aTrack track to draw.
    * @param aR radius of the compass.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawTrack(osg::ref_ptr<osg::Group>             aVectorGroup,
                  const HDD::SimData::TargetTrackItem& aTrack,
                  const double                         aR,
                  const double                         aX,
                  const double                         aY,
                  const double                         aW,
                  const double                         aH);

   /** Attempt to get an ID string for the track drawing.
    * @param aTrack track to check for ID.
    */
   const std::string GetIDString(const HDD::SimData::TargetTrackItem& aTrack);

   // Compass vars
   double   mCompassX{0.0};      ///< Compass x position, this will be used to translate the hover regions
   double   mCompassY{0.0};      ///< Compass y position, this will be used to translate the hover regions
   double   mCompassRadius{0.4}; ///< Radius of the compass
   TWD_Data mData;               ///< Data for this page
};
} // namespace HDD
#endif // WKFHDD_PAGETWD_HPP
