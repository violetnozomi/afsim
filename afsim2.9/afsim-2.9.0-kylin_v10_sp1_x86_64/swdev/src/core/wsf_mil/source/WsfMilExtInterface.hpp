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

#ifndef WSFMILEXTINTERFACE_HPP
#define WSFMILEXTINTERFACE_HPP

#include "wsf_mil_export.h"

#include "WsfMilComponentRoles.hpp"
#include "ext/WsfExtInterface.hpp"
class WsfMilExtension;
class WsfWeapon;

class WSF_MIL_EXPORT WsfMilExtInterface : public WsfExtInterfaceComponent
{
public:
   static WsfMilExtInterface* Find(const WsfExtInterface& aExt);

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   typedef std::pair<WsfStringId, WsfStringId> WeaponTargetType;
   typedef std::map<WeaponTargetType, double>  PK_Table;
   //! A map of category ids to enumerations for output of warhead info.
   typedef std::map<WsfStringId, int> CategoryToWarheadMap;

   WsfMilExtInterface(WsfMilExtension* aMilExtensionPtr);

   void PrintEmitterTypes(WsfScenario& aScenario, std::ostream& aStream) override;

   bool ProcessInput(UtInput& aInput) override;
   bool CompleteLoad(WsfScenario& aScenario) override;

   //! Gets a weapon from the given platform.
   //!
   //! @param aPlatformPtr         [input] Pointer to the weapon's platform.
   //! @param aSensorTypeId        [input] Sensor type ID.
   //! @param aEmitterNameDIS      [input] DIS emitter system - emitter name
   //! @param aEmitterIdNumberDIS  [input] DIS emitter system - emitter ID number
   //!
   //! @return A pointer to the weapon just found.
   static WsfWeapon* GetWeapon(WsfPlatform* aPlatformPtr,
                               WsfStringId  aSensorTypeId,
                               unsigned int aEmitterNameDIS,
                               unsigned int aEmitterIdNumberDIS);

   //! Gets a weapon from the given platform.
   //!
   //! @param aScenario            [input] The current scenario.
   //! @param aSensorTypeId        [input] Sensor type ID.
   //! @param aEmitterNameDIS      [input] DIS emitter system - emitter name
   //! @param aEmitterIdNumberDIS  [input] DIS emitter system - emitter ID number
   //!
   //! @return A pointer to the weapon just found.
   WsfWeapon* GetWeapon(const WsfScenario& aScenario,
                        WsfStringId        aSensorTypeId,
                        unsigned int       aEmitterNameDIS,
                        unsigned int       aEmitterIdNumberDIS);

   // WsfWeapon* GetWeapon(WsfPlatform* aPlatformPtr,
   //                      WsfStringId aSensorTypeId,
   //                      unsigned int aEmitterNameDIS,
   //                      unsigned int aEmitterIdNumberDIS);
   // WsfWeapon* GetWeapon(WsfStringId aSensorTypeId,
   //                      unsigned int aEmitterNameDIS,
   //                      unsigned int aEmitterIdNumberDIS);
   WsfWeapon* AddWeapon(double       aSimTime,
                        WsfPlatform* aPlatformPtr,
                        WsfStringId  aSensorTypeId,
                        unsigned int aEmitterNameDIS,
                        unsigned int aEmitterIdNumberDIS);

   WsfStringId SelectWeaponType(WsfSimulation& aSimulation, const WsfExtEntityType& aEntityType);

   int GetWarheadEnum(const WsfPlatformPart* aWarheadPtr);

   int                    GetDebugWarfarePDU() { return mDebugWarfarePDU; }
   void                   SetDebugWarfarePDU(int aDebugWarfarePDU) { mDebugWarfarePDU = aDebugWarfarePDU; }
   std::string            GetExternalWeaponEffectsType() { return mExternalWeaponEffectsType; }
   double                 GetSimpleKillRange() { return mSimpleKillRange; }
   bool                   GetUseSimpleDetonations() { return mUseSimpleDetonations; }
   std::set<WsfStringId>& GetExcludedDetonationPlatformTypes() { return mExcludedDetonationPlatformTypes; }
   std::set<WsfStringId>& GetExcludedDetonationWeaponEffects() { return mExcludedDetonationWeaponEffects; }
   PK_Table&              GetPK_Table() { return sPK_Table; }

   void        ProcessEW_TechniqueType(UtInput& aInput);
   uint32_t    GetJamModeSequence(WsfStringId aTechTypeId);
   WsfStringId GetTechniqueTypeId(uint32_t aJamModeSequence);

private:
   int mDebugWarfarePDU = 0;

   WsfMilExtension*      mMilExtensionPtr = nullptr;
   std::string           mExternalWeaponEffectsType;
   double                mSimpleKillRange      = 100.0;
   bool                  mUseSimpleDetonations = true;
   std::set<WsfStringId> mExcludedDetonationPlatformTypes;
   std::set<WsfStringId> mExcludedDetonationWeaponEffects;
   PK_Table              sPK_Table;

   //! A map of category ids to enumerations for output of warhead info.
   CategoryToWarheadMap mCategoryToWarheadMap;

   //! The following maps a WSF EW technique type id to DIS technique type.
   std::map<WsfStringId, uint32_t> sTechTypeToJamModeSeq;
   std::map<uint32_t, WsfStringId> sJamModeSeqToTechType;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMilExtInterface, cWSF_COMPONENT_MIL_EXT_INTERFACE)

#endif
