// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfDefaultAssetManagerImpl.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract: This class provides default implementations in C++ for
*    what could be overridden in script for the Asset Manager base class.
*    The intention was to provide the classical implementation of what was originally
*    done in script so default usage of the processor works without supplemental scripting.
*
*******************************************************************************/

#ifndef WSF_DEFAULT_ASSET_MANAGER_IMPL_HPP
#define WSF_DEFAULT_ASSET_MANAGER_IMPL_HPP

#include <memory>

#include <WsfBMCommon.hpp>
#include <WsfBMStatusMessage.hpp>
#include <WsfBMTrackMessage.hpp>
#include <WsfBMAssignmentMessage.hpp>
#include <WsfBMAssignmentStatusMessage.hpp>
#include <WsfBMCueMessage.hpp>

// forward declarations
class WsfPlatform;
class WsfWeapon;
class WsfBMSensorInfoRecord;
class WsfBMWeaponInfoRecord;
class WsfAssetManager;
class WsfSimulation;
class WsfLocalTrack;
class WsfMessage;
class WsfBattleManager;
class WsfBMDisseminateC2;
class WsfBMSensorsManager;
class WsfBMWeaponsManager;
class WsfBMAssignmentTrackMessage;

namespace il
{
   class AssetManagerInterface;
   class BattleManagerInterface;
   class assetMap;
   class assetRecord;
}

class WSF_IADS_C2_LIB_EXPORT WsfDefaultAssetManagerImpl
{
   public:
      WsfDefaultAssetManagerImpl(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger);
      virtual ~WsfDefaultAssetManagerImpl();

      virtual bool ProcessInput(UtInput& aInput);
      virtual void Initialize(WsfAssetManager* am);
      virtual void Initialize2(WsfAssetManager* am);

      virtual void on_update(const double sim_time_s, WsfPlatform* this_plat, WsfAssetManager* am);
      virtual bool on_message(const double sim_time_s, const WsfMessage& message, WsfAssetManager* am);
      virtual void LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, WsfAssetManager* am);
      virtual void LocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr, WsfAssetManager* am);
      virtual void LocalTrackUpdated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr, WsfAssetManager* am);

   protected:
      virtual void PrintStatus(WsfAssetManager* am, const bool direct_only = false) const;

   private:
      virtual void InitializeIADSChainOfCommand(const double sim_time_s, WsfPlatform* this_plat, WsfAssetManager* am);
      virtual void SetupIADSLaydown(WsfPlatform* initing_plat, WsfAssetManager* am);
      virtual void EstablishNewCmdrSubRelationship(const WsfPlatform* new_plat, std::shared_ptr<il::assetRecord>& new_asset, il::assetMap& asset_map);
      virtual std::shared_ptr<il::assetRecord> InitAssetRecord(const WsfPlatform* plat, il::assetMap& asset_map, WsfAssetManager* am);
      virtual std::shared_ptr<il::assetRecord> InitializeAssetAndSubordinates(const WsfPlatform* plat, il::assetMap& asset_map, WsfAssetManager* am);
      virtual void InitializeAssetCommanders(const WsfPlatform* plat, il::assetMap& asset_map, WsfAssetManager* am);
      virtual void InitializeAssetDAZones(const WsfPlatform* plat, std::shared_ptr<il::assetRecord>& asset_record) const;
      virtual void InitializeAssetZones(const WsfPlatform* plat, std::shared_ptr<il::assetRecord>& asset_record) const;

      // this will go away when asset managers are full supported on all platforms and HELIOS C2 status messages are flowing
      virtual void InitializeWeaponZones(const WsfPlatform* owning_plat, WsfWeapon* weapon, WsfBMWeaponInfoRecord* weapon_interface);

      // Perceptions
      virtual bool IsAssetPerceptionValid(WsfSimulation* sim, const WsfBMStatusMessage& status) const;
      virtual void HandleAssetStatusUpdate(WsfAssetManager* am, const WsfBMStatusMessage& status);
      virtual std::size_t UpdateThreatPerceptions(const double sim_time_s, const WsfPlatform* this_plat, WsfAssetManager* am);
      virtual bool UpdateTrack(const double sim_time_s, const WsfLocalTrack* track, WsfAssetManager* am);
      virtual WsfBMTrackMessage ConvertTrackUpdate(const double sim_time_s, WsfAssetManager* am, const WsfLocalTrack* track) const;
      virtual WsfBMTrackMessage ConvertTrackDrop(const WsfSimulation* sim, const WsfLocalTrack* drop_track) const;

      // Message handling
      virtual void HandleAssignmentStatusUpdate(const double sim_time_s, const WsfBMAssignmentStatusMessage& message, WsfAssetManager* am);
      virtual void HandleAssignmentMessage(const double aSimTime, const WsfBMAssignmentMessage& message, WsfAssetManager* am);
      virtual void HandleSensorCueMessage(const double aSimTime, WsfAssetManager* am, const WsfBMCueMessage& message);
      virtual void HandleAssignmentTrackMessage(const double aSimTime, WsfAssetManager* am, const WsfBMAssignmentTrackMessage& message);
      virtual bool IsSelfAssignment(const WsfBMAssignmentMessage& message, WsfAssetManager* am) const;
      virtual bool IsSelfCue(const WsfBMCueMessage& message, WsfAssetManager* am) const;
      virtual bool CorrelateMasterTrackWithAssignment(WsfBMAssignmentMessage& message, WsfAssetManager* am);
      virtual bool CorrelateMasterTrackWithCue(WsfBMCueMessage& message, WsfAssetManager* am);
      virtual void AddDelegation(const double sim_time_s, const WsfBMAssignmentMessage& message, WsfAssetManager* am);
      virtual void CheckPendingAssignmentCorrelations(const double sim_time_s, WsfSimulation* sim, WsfAssetManager* am);
      virtual void CheckPendingCueCorrelations(const double sim_time_s, WsfSimulation* sim, WsfAssetManager* am);
      virtual bool ShouldDelayIncomingMessage(double aSimTime, const WsfMessage& aMessage, const WsfAssetManager* am, double& amount_s);

      bool m_iads_initialized;

      WsfBattleManager* m_bm_ref;
      WsfBMDisseminateC2* m_dc2_ref;
      WsfBMWeaponsManager* m_wm_ref;
      WsfBMSensorsManager* m_sm_ref;
      bool m_threat_check_believed_alive;
      double m_max_track_grouping_distance_m;

      std::vector<std::pair<double, WsfBMAssignmentMessage>> m_uncorrelated_assignments; // time/TaskAssign pair
      std::vector<std::pair<double, WsfBMCueMessage>> m_uncorrelated_cues;

      std::weak_ptr<il::logger::GlobalLogger> mGlobalLogger;

      // friends
      friend class WsfAssetManager;
};

#endif
