// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFUCI_WEAPONCOMPONENT_HPP
#define WSFUCI_WEAPONCOMPONENT_HPP

#include "wsf_oms_uci_export.h"

#include <iostream>
#include <string>
#include <unordered_set>

#include "uci/type/MessageStateEnum.h"

#include "UtLog.hpp"
#include "WsfPlatform.hpp"
#include "WsfUCI_Component.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_PartComponent.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfWeapon.hpp"

class     WsfScenario;
class     WsfSimulation;

namespace wsf
{

//! An example of a simple platform component (See WsfSimplePlatformCompoent).
//! UCI_ are meant to be single instances on platforms with only a defined quantity.
//! The WeaponComponent handles all weapons on a platform, therefore should only have one instance
//! on a platform.
class UCI_WeaponComponent : public UCI_PartComponent
{
   public:
      //! @name Definitions to support templated component methods.
      //@{

      // We must define the component role enum here as it is not a core value
      static const int cWSF_COMPONENT_UCI_WEAPON = 9876543;  // See WsfComponentRoles
      //@}

      UCI_WeaponComponent() = delete;
      UCI_WeaponComponent(const WsfScenario& aScenario);
      UCI_WeaponComponent(const UCI_WeaponComponent& aSrc);
      virtual ~UCI_WeaponComponent() = default;

      void* QueryInterface(int aRole) override;
      bool  Initialize2(double aSimTime) override;
      const int* GetComponentRoles() const override;
      bool ProcessInput(UtInput& aInput) override;

      WsfComponent*    CloneComponent() const override;
      WsfObject*       Clone() const { return new UCI_WeaponComponent(*this); }

   protected:

      void UpdateCapabilityStatus();
      virtual void SetOwnerProducer(const ISM_OwnerProducerType& aOwnerType) override;
      std::string GetCommandTypeString(uci::type::StrikeWeaponCommandType::StrikeWeaponCommandTypeChoice aType);
      void SetCommandAccepted(const uci::type::StrikeCapabilityCommandType& aCapabilityCommand,
                              const WsfWeapon::FireResult&                  aResult);
      bool HandleCommandMessage(double                         aSimTime,
                                const UCI_StrikeCommandMessage& aMessage);

      bool HandleCapabilityCommand(double                                        aSimTime,
                                   const uci::type::StrikeCapabilityCommandType& aCapabilityCommand);

      bool HandleActivityCommand(double                                      aSimTime,
                                 const uci::type::StrikeActivityCommandType& aActivityCommand);

      bool HandleDMPI_Message(double                  aSimTime,
                              const UCI_DMPI_Message& aMessage);

      bool CommandWeapon(double                                        aSimTime,
                         const uci::type::StrikeCapabilityCommandType& aCapabilityCommand);

      void ClearActivities() { mActivityMessagePtr->Get().getMessageData().getActivity().clear(); }

      void SendActivityMessage(double                                              aSimTime,
                               const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void SendCapabilityMessage(double                                              aSimTime,
                                 const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void SendCapabilityStatusMessage(double                                              aSimTime,
                                       const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void SendDMPI_StatusMessage(double                                                 aSimTime,
                                  const uci::type::MessageStateEnum::EnumerationItem&    aMessageState,
                                  const uci::type::DMPI_ID_Type&                         aDMPI_ID,
                                  const uci::type::DMPI_StoreStateEnum::EnumerationItem& aStoreState);

      void UpdateAllMessages();

      void SetAllCapabilitiesAvailability(const uci::type::CapabilityAvailabilityEnum::EnumerationItem& aAvailability) {}

      bool ReceiveMessage(double            aSimTime,
                          const WsfMessage& aMessage) override;

      void WeaponHitCB(double                     aSimTime,
                       const WsfWeaponEngagement* aEngagementPtr,
                       WsfPlatform*               aTargetPlatformPt);
      void WeaponTerminatedCB(double                     aSimTime,
                              const WsfWeaponEngagement* aEngagementPtr);
      void WeaponMissedCB(double                     aSimTime,
                          const WsfWeaponEngagement* aEngagementPtr,
                          WsfPlatform*               aTargetPlatformPtr);

      void WeaponFiredCB(double                     aSimTime,
                         const WsfWeaponEngagement* aEngagementPtr,
                         const WsfTrack*            aTargetPtr);

      /*void WeaponFireAbortedCB(double          aSimTime,
                               WsfWeapon*      aWeaponPtr,
                               const WsfTrack* aTargetTrackPtr,
                               double          aQuantity);*/

      std::unique_ptr<UCI_StrikeActivityMessage>         mActivityMessagePtr;
      std::unique_ptr<UCI_StrikeCapabilityMessage>       mCapabilityMessagePtr;
      std::unique_ptr<UCI_StrikeCapabilityStatusMessage> mCapabilityStatusMessagePtr;
      std::map<unsigned int, UCI_StrikeActivityType>     mSerialToActivity;

      std::map<std::string, std::string>                 mUUIDtoCapability;

   private:

      struct WeaponStatus
      {
         bool mSelectedForKeyLoad{ false };
         bool mWeaponArmed{ false };
         bool mReadyForRelease{ false };
         bool mSelectedForRelease{ false };
         bool mSelectedForJettison{ false };
      };

      struct StoreInfo
      {
         int mStoreType                                    { 0 };
         uci::type::StoreCategoryEnum::EnumerationItem mStoreCategory { uci::type::StoreCategoryEnum::enumNotSet };
      };

      int GetStoreType(const std::string& aWeapon);
      uci::type::StoreCategoryEnum::EnumerationItem GetStoreCategory(const std::string& aWeapon);
      uci::type::StoreCategoryEnum::EnumerationItem GetStoreCategoryEnum(const std::string& aCategory);

      void WeaponFireResults(double                     aSimTime,
                             const WsfWeaponEngagement* aEngagementPtr,
                             WsfPlatform*               aTargetPlatformPtr,
                             const std::string&         aStatus);

      bool SetFireLocation(double             aSimTime,
                           WsfWeapon*         aWeaponPtr,
                           const std::string& aUUID);

      void SetSelectedForKeyLoad(const std::string& aID,
                                 bool               aSelected);
      void SetWeaponArmed(const std::string& aID,
                          bool               aArmed);
      void SetReadyForRelease(const std::string& aID,
                              bool               aReady);
      void SetSelectedForRelease(const std::string& aID,
                                 bool               aSelected);
      void SetSelectedForJettison(const std::string& aID,
                                  bool               aSelected);

      bool HasDMPI_Message(const std::string& aDMPI_ID);

      bool WeaponAvailable(WsfWeapon*         aWeaponPtr,
                           const std::string& aUUID);

      bool MeetsDMPI_Requirements(const std::string& aDMPI_Id);

      std::string GetWeaponName(const std::string& aName);

      bool CanFire(double             aSimTime,
                   WsfWeapon*         aWeaponPtr,
                   const std::string& aUUID);

      WeaponStatus GetWeaponStatus(const std::string& aID);

      uci::type::CapabilityAvailabilityEnum::EnumerationItem GetWeaponAvailability(const WsfWeapon*   aWeaponPtr,
                                                                                   const std::string& aUUID);
      std::string GetAvailabilityString(uci::type::CapabilityAvailabilityEnum::EnumerationItem aAvailability);

      std::map<std::string, WeaponStatus> mWeaponStatus;
      std::map<std::string, StoreInfo> mStoreInfo;
      std::map<std::string, std::pair<UCI_DMPI_Message, uci::type::DMPI_StoreStateEnum::EnumerationItem> > mDMPI_Messages;
      bool mSendActivityMessage{ false };
      std::unordered_set<std::string> mFiredCapabilities;
      std::set<std::string> mIgnoredWeapons;
      bool mCurrentlyFiring{ false };
};


} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::UCI_WeaponComponent, wsf::UCI_WeaponComponent::cWSF_COMPONENT_UCI_WEAPON)

#endif
