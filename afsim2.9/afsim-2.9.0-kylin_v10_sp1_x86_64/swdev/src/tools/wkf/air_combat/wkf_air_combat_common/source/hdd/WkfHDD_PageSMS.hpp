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

#ifndef WKFHDD_PAGESMS_HPP
#define WKFHDD_PAGESMS_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_Page.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PageSMS : public Page
{
public:
   struct SMS_Weapon
   {
      std::map<size_t, bool> weaponSlots;
      size_t                 quantity;
   };

   struct SMS_Data
   {
      std::vector<HDD::SimData::WeaponNameQty> weaponList;           ///< List of weapon names and their quantities.
      std::string                              selectedWeapon{""};   ///< Name of the currently selected weapon.
      int                                      numSelectedWeapon{0}; ///< Number of the currently selected weapon.

      std::vector<HDD::SimData::EngagedTargetData> engagedTargetList; ///< List of engaged targets.

      size_t extTankCount{0}; ///< Number of external fuel tanks.
      int    numChaff{0};     ///< Number of chaff.
      int    numFlares{0};    ///< Number of flares.
      int    numDecoys{0};    ///< Number of decoys.

      bool hasDoors{true}; ///< True if the aircraft has weapons bay doors, false otherwise (Not-handled right now,
                           ///< defaults to true).
      bool weaponBayDoorsOpen{false}; ///< True if the weapon bay doors are open, false otherwise.
      bool supportingWeapon{false};   ///< True if the supporting weapon is active, false otherwise.
      bool masterWarning{false};      ///< True if the master warning is active, false otherwise.
      bool masterCaution{false};      ///< True if the master caution is active, false otherwise.
      bool shootCueActive{false};     ///< True if the shooting cue is active, false otherwise.
      bool shootCueBlink{false};      ///< True if the shooting cue is blinking, false otherwise.
      bool masterArmActive{false};    ///< True if the master arm switch is active, false otherwise.
      bool jammingDetected{false};    ///< True if jamming is being detected, false otherwise.
   };

   PageSMS(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   ~PageSMS() override = default;

   /** Set the weapon data for this page.
    * @param aInput weapon data to pull from.
    */
   void SetWeaponData(const HDD::SimData::WeaponDataSummary& aData);

   /** Set the number of external tanks this platform has.  This is pulled from the fuel data.
    * @param aInput number of external tanks.
    */
   void SetExternalFuelTankCount(const size_t aInput) { mData.extTankCount = aInput; }

private:
   /** Draw this page. */
   void Draw() override;

   /** Draw the wing lines that the weapons will be placed on.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawWingLines(osg::ref_ptr<osg::Group> aVectorGroup,
                      const double             aX,
                      const double             aY,
                      const double             aW,
                      const double             aH,
                      const double             aScale);

   /** Draw the external fuel tanks at the bottom.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawFuelTanks(osg::ref_ptr<osg::Group> aVectorGroup,
                      const double             aX,
                      const double             aY,
                      const double             aW,
                      const double             aH,
                      const double             aScale);

   /** Draw the countermeasures counts.  This is for the small version of the display only.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawCountermeasures(osg::ref_ptr<osg::Group> aVectorGroup,
                            const double             aX,
                            const double             aY,
                            const double             aW,
                            const double             aH,
                            const double             aScale);

   /** Draw the countermeasures counts, and if the weapon bay doors are open.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawCountermeasuresAndDoorsOpen(osg::ref_ptr<osg::Group> aVectorGroup,
                                        const double             aX,
                                        const double             aY,
                                        const double             aW,
                                        const double             aH,
                                        const double             aScale);

   /** (Debugging) This is a debugging function for seeing the weapon box (The space the weapon is drawn in).
    * This can be used to see how much space there is to work with when adding new weapon draw code.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawWeaponBox(osg::ref_ptr<osg::Group> aVectorGroup,
                      const double             aX,
                      const double             aY,
                      const double             aW,
                      const double             aH,
                      const double             aScale);

   /** Draw a bomb.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawWeaponBomb(osg::ref_ptr<osg::Group> aVectorGroup,
                       const double             aX,
                       const double             aY,
                       const double             aW,
                       const double             aH,
                       const size_t             aWeaponIndex,
                       const double             aScale);

   /** Draw a MRM.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawWeaponMRM(osg::ref_ptr<osg::Group> aVectorGroup,
                      const double             aX,
                      const double             aY,
                      const double             aW,
                      const double             aH,
                      const size_t             aWeaponIndex,
                      const double             aScale);

   /** Draw a SRM.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawWeaponSRM(osg::ref_ptr<osg::Group> aVectorGroup,
                      const double             aX,
                      const double             aY,
                      const double             aW,
                      const double             aH,
                      const size_t             aWeaponIndex,
                      const double             aScale);

   /** Draw the wing lines and then the 8 or 16 weapon slots.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aScale scale to use.
    */
   void DrawAllWeapons(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aScale);

   /** Draw the armed box (If a weapon is armed)
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawArmed(osg::ref_ptr<osg::Group> aVectorGroup,
                  const double             aX,
                  const double             aY,
                  const double             aW,
                  const double             aH,
                  const double             aScale);

   /** Draw the selected weapon (If a weapon is selected)
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawSelected(osg::ref_ptr<osg::Group> aVectorGroup,
                     const double             aX,
                     const double             aY,
                     const double             aW,
                     const double             aH,
                     const double             aScale);

   /** Get the weapon's draw location(slot) on the SMS, given an index.
    * @param aIndex index of the weapon.
    */
   const QPointF GetWeaponLocation(const size_t aIndex) const;

   /** This selects the first visible weapon of the selected weapon type.
    * Check the bomb, srm, and mrm list and grab the first visible index.
    * That will be the current selected weapon index.
    * @param aSelectedWeapon selected weapon type (bomb, srm, mrm).
    */
   void SetSelectedWeaponIndex(const std::string& aSelectedWeapon);

   SMS_Data mData;                             ///< Data for this page.
   bool     mConfigured{false};                ///< Whether or not the SMS weapons have been configured.
   bool     mUseTwoRows{false};                ///< The SMS will use two rows if more than 8 weapons are present.
   std::map<std::string, SMS_Weapon> mWeapons; ///< Map of weapon name to its slots and quantity.
   size_t mSelectedWeaponIndex{0};             ///< 0 if no weapon selected, otherwise the slot of the selected weapon.
};
} // namespace HDD
#endif // WKFHDD_PAGESMS_HPP
