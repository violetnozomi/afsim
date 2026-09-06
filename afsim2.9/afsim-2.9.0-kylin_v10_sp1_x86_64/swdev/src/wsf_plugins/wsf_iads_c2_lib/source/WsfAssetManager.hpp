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
*   File: WsfAssetManager.hpp
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
*   Abstract: This is an afsim base class for all battle manager
*     implementation classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

#ifndef WSF_ASSET_MANAGER_HPP
#define WSF_ASSET_MANAGER_HPP

namespace il
{
   class BattleManagerInterface;
   class AssetManagerInterface;
   class DisseminateC2Interface;
   class WsfPlatform;
}

#include <map>
#include <memory>
#include <utility>

#include <script/WsfScriptProcessor.hpp>
#include <script/WsfScriptContext.hpp>
#include <UtCallbackHolder.hpp>

#include <WsfC2ComponentRefInterface.hpp>
#include <WsfScriptOverridableProcessor.hpp>

#include <WsfDefaultAssetManagerImpl.hpp>
#include "WsfBMAssetRecord.hpp"
#include "WsfAssetMap.hpp"
#include <WsfTrack.hpp>
#include <WsfBMTrackScriptCallbackHandler.hpp>
#include <WsfBMAssignmentMessage.hpp>
#include <WsfBMMOELogger.hpp>
#include <WsfApplication.hpp>
#include <WsfScriptAssetManagerClass.hpp>

class WsfAssetManager : public WsfScriptProcessor, public WsfC2ComponentContainer, public WsfScriptOverridableProcessor
{
   public:
      WsfAssetManager(WsfScenario&       aScenario,
                      const std::string& aPartClass = WsfScriptAssetManagerClass::SCRIPT_CLASS_NAME,
                      const std::string& aPartVarName = "PROCESSOR");
      WsfAssetManager(WsfScenario&       aScenario,
                      const std::string& aPartClass,
                      const std::string& aPartVarName,
                      const std::shared_ptr<il::AssetManagerInterface>&  am_component);
      virtual ~WsfAssetManager();

      WsfProcessor* Clone() const override;

      // Plugin Interface
      static WSF_IADS_C2_LIB_EXPORT void RegisterScriptTypes(WsfApplication* aApplicationPtr);
      static WSF_IADS_C2_LIB_EXPORT WsfProcessor* Create(WsfScenario& aScenario);
      static WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfAssetManager> CreateUnique(WsfScenario& aScenario);
      // End Plugin Interface

      bool ProcessInput(UtInput& aInput) override;

      const char* GetScriptClassName() const override;
      virtual const char* GetBaseClassName();

      // Ownership goes to caller - responsible for destroying
      virtual WsfAssetMap* GetAssetMapPtr();
      virtual WsfBMAssetRecord* GetAssetPtr();

      bool Initialize(double aSimTime) override;
      bool Initialize2(double aSimTime) override;
      virtual bool ConnectTrackDropCallback(const std::string& drop_handler_func);
      virtual bool ConnectTrackInitiatedCallback(const std::string& handler_func);
      virtual bool ConnectTrackUpdatedCallback(const std::string& handler_func);
      void Update(double aSimTime) override;
      void Run(const double aSimTime) override; // HELIOS C2 component interface for invoking processor periodic updates

      bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
      bool ReceiveMessage(double aSimTime, const WsfMessage& aMessage) override;

      virtual std::tuple<bool, double> ShouldDelayIncomingMessage(double aSimTime, const WsfMessage& aMessage);

      // Platform/Processor Callbacks
      virtual void LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);
      virtual void LocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);
      virtual void LocalTrackUpdated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);

      // Perception Handling
      virtual double TimeLastReceivedTrackUpdate(const WsfLocalTrack* track) const;

      // Accessors
      int GetMaxAssignments() const;
      int GetCurrentNumAssignments() const;
      double GetAssignmentDelaySecs() const;

   protected:

      WsfBMTrackScriptLocalHandler m_drop_monitor;
      WsfBMTrackScriptRemoteHandler m_initiated_monitor;
      WsfBMTrackScriptRemoteHandler m_updated_monitor;
      UtCallbackHolder      m_callbacks;

      WsfDefaultAssetManagerImpl m_default_am_impl;

      // Note: Copying the asset manager doesn't make sense because it needs to be initialized
      // and that initialization is tied to a specific platform, processor, etc. However,
      // the copy constructor is required because Clone() needs it in order to properly clone the
      // object (albeit with no initialization tied to it). so we'll basically use it to get a
      // copy of a cloned object that is yet to be initialized in  order for the prototypical
      // inheritance to work.
      WsfAssetManager(const WsfAssetManager& aSrc);

      virtual bool ProcessAssetManagementInput(UtInput& aInput);

      // behavior overrides
      bool m_override_internal_initialization;
      bool m_override_internal_initialization2;
      bool m_override_internal_on_update;
      bool m_override_internal_on_message;

   private:

      WsfAssetManager& operator=(const WsfAssetManager& rhs) = delete;;
};

#endif
