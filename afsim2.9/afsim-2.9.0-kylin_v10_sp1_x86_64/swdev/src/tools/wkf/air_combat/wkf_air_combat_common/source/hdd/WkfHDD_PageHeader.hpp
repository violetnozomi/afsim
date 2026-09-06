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

#ifndef WKFHDD_PAGEHEADER_HPP
#define WKFHDD_PAGEHEADER_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_Page.hpp"
#include "hdd/WkfHDD_DataContainer.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PageHeader : public Page
{
public:
   // Data for the header is seperated into each box that needs it.
   struct HeaderData
   {
      // Eng Box
      double thrustNormalized{0.0}; ///< Thrust normalized value (0-2).
      double throttleInput{0.0};    ///< Throttle input value (0-2).
      bool   throttleValid{true};   ///< True if the throttle input is valid.

      // Fuel Box
      double fuelValid{false};      ///< True if the fuel is valid, false otherwise.
      double extFuelRemaining{0.0}; ///< External fuel
      double intFuelRemaining{0.0}; ///< Internal fuel
      double weight{0.0};           ///< Gross total weight

      // Gun Box
      std::map<std::string, size_t> weapons;        ///< List of weapons and their quantities.
      std::string                   selectedWeapon; ///< The currently selected weapon (if any).

      // Landing Gear Box - <0.01 is up, >0.99 is down.
      double landingGearNose{0.0};  ///< Nose landing gear.
      double landingGearLeft{0.0};  ///< Left landing gear.
      double landingGearRight{0.0}; ///< Right landing gear.

      // ICAWS Box
      bool masterWarning{false}; ///< Master warning alert.
      bool stallWarning{false};  ///< Stall warning alert.
      bool masterCaution{false}; ///< Master caution alert.

      // Flight Data Box
      double altBaro_ft{0.0};  ///< Altitude in ft.
      double heading_deg{0.0}; ///< Heading in degrees.
      double ktas{0.0};        ///< KTAS.

      double simTime; ///< Current sim time.
   };

   /** Set the heading of this page.  The Flight Data box uses this.
    * @param aInput heading to set.
    */
   void SetHeading(const double aInput) { mData.heading_deg = aInput; }

   /** Set the altitude of this page.  The Flight Data box uses this.
    * @param aInput altitude to set.
    */
   void SetAltitude(const double aInput) { mData.altBaro_ft = aInput; }

   /** Set the ktas of this page.  The Flight Data box uses this.
    * @param aInput ktas to set.
    */
   void SetKTAS(const double aInput) { mData.ktas = aInput; }

   /** Set the fuel data for this page.  The Fuel box uses this
    * @param aData fuel data to pull from.
    */
   void SetFuelData(const HDD::SimData::FuelData& aData);

   /** Set the weapon data for this page.  The Gun box and ICAWS uses this.
    * @param aData weapon data to pull from.
    */
   void SetWeaponData(const HDD::SimData::WeaponDataSummary& aData);

   /** Set the if the platform is stalling.  The ICAWS box uses this.
    * @param aInput true if stalling, false otherwise.
    */
   void SetStalling(const bool aInput) { mData.stallWarning = aInput; }

   /** Set the landing gear position.  The Landing Gear box uses this.
    * @param aInput landing gear value to set.
    */
   void SetLandingGear(const double aInput);

   /** Set the thrust and throttle values.  The ENG box uses this.
    * @param aThrust thrust value to set (0-2).
    * @param aThrottleValid true if the throttle value is valid.
    * @param aThrottle throttle value to set (0-2).
    */
   void SetThrustThrottle(const double aThrust, const bool aThrottleValid, const double aThrottle);

   /** Set the time for the header.  The Time box uses this.
    * @param aInput sim time to set.
    */
   void SetWallTime(const double aInput) { mData.simTime = aInput; }

   PageHeader(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   ~PageHeader() override = default;

private:
   /** Draw this page. */
   void Draw() override;

   /** Draw a box.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawBox(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aW, const double aH);

   /** Draw the engine header box.  This shows the thrust and throttle.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawEngBox(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aW, const double aH);

   /** Draw the fuel box.  This shows the gross weight, total fuel, internal fuel, and external fuel remaining.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawFuelBox(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aW, const double aH);

   /** Draw the gun box.  This shows up to 4 weapons and their quantities.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawGunBox(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aW, const double aH);

   /** Draw the landing gear box.  Filled green means the gear is down, empty green means it's up, and red means it's in
    * transit.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawLandingGearBox(osg::ref_ptr<osg::Group> aVectorGroup,
                           const double             aX,
                           const double             aY,
                           const double             aW,
                           const double             aH);

   /** Draw the ICAWS box.  This shows warning, stall, and caution.  If any of these are true, they will show and blink.
    * Otherwise, the region will just show 'ICAWS'.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    */
   void DrawIcawsBox(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aW, const double aH);

   /** Draw the Comms box.  This is non-functional and just for show.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.s
    */
   void DrawCommsBox(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aW, const double aH);

   /** Draw the Nav box.  This is non-functional and just for show.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.s
    */
   void DrawNavBox(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aW, const double aH);

   /** Draw the ADF/XPDR box.  This is non-functional and just for show.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.s
    */
   void DrawAdfBox(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aW, const double aH);

   /** Draw the Menu box.  This is non-functional and just for show.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.s
    */
   void DrawMenuBox(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aW, const double aH);

   /** Draw the flight data box.  This shows, altitude, heading, and speed in ktas.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.s
    */
   void DrawFlightDataBox(osg::ref_ptr<osg::Group> aVectorGroup,
                          const double             aX,
                          const double             aY,
                          const double             aW,
                          const double             aH);

   /** Draw the time box.  This shows the time.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.s
    */
   void DrawTimeBox(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aW, const double aH);

   HeaderData mData; ///< Data for this page.
};
} // namespace HDD
#endif // WKFHDD_PAGEHEADER_HPP
