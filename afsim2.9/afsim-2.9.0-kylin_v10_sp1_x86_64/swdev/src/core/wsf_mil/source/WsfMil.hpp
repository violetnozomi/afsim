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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFMIL_HPP
#define WSFMIL_HPP

#include "wsf_mil_export.h"

#include <map>
#include <memory>

#include "UtCallbackHolder.hpp"
class WsfPlatform;
class WsfScenario;
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"

class WsfAcousticSignatureTypes;
class WsfEW_EA_EP_Types;
class WsfEW_EffectManager;
class WsfEW_EffectTypes;
class WsfEW_TechniqueTypes;
class WsfFalseTargetScreenerTypes;
class WsfFalseTargetTypes;
class WsfGuidanceProgramTypes;
class WsfHighEnergyLaserTypes;
class WsfInfraredSignatureTypes;
class WsfInherentContrastTypes;
class WsfLaunchComputerTypes;
class WsfMilEventPipe;
class WsfOpticalPathCoefficientTypes;
class WsfOpticalReflectivityTypes;
class WsfOpticalSignatureTypes;
struct WsfQuantumTaskerObserver;
class WsfWeapon;
class WsfWeaponEffectsTypes;
struct WsfWeaponObserver;
struct WsfWeaponTaskObserver;
class WsfWeaponTypes;

namespace wsf
{
class CN2_Types;
}

namespace WsfPk
{
class TableManager;
}

class WSF_MIL_EXPORT WsfMilExtension : public WsfScenarioExtension
{
public:
   static WsfMilExtension* Find(const WsfScenario& aScenario);
   static WsfMilExtension& Get(const WsfScenario& aScenario);

   WsfMilExtension();
   ~WsfMilExtension() override;

   void AddedToScenario() override;
   bool ProcessInput(UtInput& aInput) override;

   WsfPk::TableManager& GetPkTableManager() const { return *mPkTableManagerPtr; }

   WsfAcousticSignatureTypes&      GetAcousticSignatureTypes() const { return *mAcousticSignatureTypes; }
   wsf::CN2_Types&                 GetCN2_Types() const { return *mCN2_TypesPtr; }
   WsfEW_EA_EP_Types&              GetEA_EP_Types() const { return *mEA_EP_Types; }
   WsfEW_EffectTypes&              GetEW_EffectTypes() const { return *mEW_EffectTypes; }
   WsfEW_TechniqueTypes&           GetEW_TechniqueTypes() const { return *mEW_TechniqueTypes; }
   WsfFalseTargetTypes&            GetFalseTargetTypes() const { return *mFalseTargetTypes; }
   WsfFalseTargetScreenerTypes&    GetFalseTargetScreenerTypes() const { return *mFalseTargetScreenerTypes; }
   WsfGuidanceProgramTypes&        GetGuidanceProgramTypes() const { return *mGuidanceProgramTypes; }
   WsfHighEnergyLaserTypes&        GetHighEnergyLaserTypes() const { return *mHighEnergyLaserTypes; }
   WsfInfraredSignatureTypes&      GetInfraredSignatureTypes() const { return *mInfraredSignatureTypes; }
   WsfInherentContrastTypes&       GetInherentContrastTypes() const { return *mInherentContrastTypes; }
   WsfLaunchComputerTypes&         GetLaunchComputerTypes() const { return *mLaunchComputerTypes; }
   WsfOpticalPathCoefficientTypes& GetOpticalPathCoefficientTypes() const { return *mOpticalPathCoefficientTypes; }
   WsfOpticalReflectivityTypes&    GetOpticalReflectivityTypes() const { return *mOpticalReflectivityTypes; }
   WsfOpticalSignatureTypes&       GetOpticalSignatureTypes() const { return *mOpticalSignatureTypes; }
   WsfWeaponEffectsTypes&          GetWeaponEffectsTypes() const { return *mWeaponEffectsTypes; }
   WsfWeaponTypes&                 GetWeaponTypes() const { return *mWeaponTypes; }

private:
   void SimulationCreated(WsfSimulation& aSimulation) override;

   WsfPk::TableManager* mPkTableManagerPtr;

   WsfAcousticSignatureTypes*      mAcousticSignatureTypes;
   wsf::CN2_Types*                 mCN2_TypesPtr;
   WsfEW_EA_EP_Types*              mEA_EP_Types;
   WsfEW_EffectTypes*              mEW_EffectTypes;
   WsfEW_TechniqueTypes*           mEW_TechniqueTypes;
   WsfFalseTargetScreenerTypes*    mFalseTargetScreenerTypes;
   WsfFalseTargetTypes*            mFalseTargetTypes;
   WsfGuidanceProgramTypes*        mGuidanceProgramTypes;
   WsfHighEnergyLaserTypes*        mHighEnergyLaserTypes;
   WsfInfraredSignatureTypes*      mInfraredSignatureTypes;
   WsfInherentContrastTypes*       mInherentContrastTypes;
   WsfLaunchComputerTypes*         mLaunchComputerTypes;
   WsfOpticalPathCoefficientTypes* mOpticalPathCoefficientTypes;
   WsfOpticalReflectivityTypes*    mOpticalReflectivityTypes;
   WsfOpticalSignatureTypes*       mOpticalSignatureTypes;
   WsfWeaponEffectsTypes*          mWeaponEffectsTypes;
   WsfWeaponTypes*                 mWeaponTypes;
};

//! The simulation extension for the 'mil' extension
class WSF_MIL_EXPORT WsfMilInterface : public WsfSimulationExtension
{
public:
   WsfMilInterface();
   ~WsfMilInterface() override;

   static WsfMilInterface* Find(const WsfSimulation& aSimulation);

   static WsfMilInterface* Find(const WsfSimulation* aSimulationPtr);

   void AddedToSimulation() override;

   WsfQuantumTaskerObserver& GetQuantumTaskerObserver() { return *mQuantumTaskerObserverPtr; }

   WsfWeaponObserver& GetWeaponObserver() { return *mWeaponObserverPtr; }

   WsfWeaponTaskObserver& GetWeaponTaskObserver() { return *mWeaponTaskObserverPtr; }

   WsfEW_EffectManager& GetEW_EffectManager() const { return *mEW_EffectManagerPtr; }

   void KillPlatform(double aSimTime, WsfPlatform* aPlatformPtr);

private:
   void FrameObjectUpdate(double aCurrentFrameTime);
   void WeaponTurnedOff(double aSimTime, WsfWeapon* aWeaponPtr);
   void WeaponTurnedOn(double aSimTime, WsfWeapon* aWeaponPtr);
   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   std::unique_ptr<WsfEW_EffectManager>      mEW_EffectManagerPtr;
   std::unique_ptr<WsfQuantumTaskerObserver> mQuantumTaskerObserverPtr;
   std::unique_ptr<WsfWeaponObserver>        mWeaponObserverPtr;
   std::unique_ptr<WsfWeaponTaskObserver>    mWeaponTaskObserverPtr;
   std::unique_ptr<WsfMilEventPipe>          mMilEventPipePtr;

   std::map<WsfWeapon*, bool> mWeaponsChangingState;
   std::vector<WsfWeapon*>    mWeapons;
   UtCallbackHolder           mCallbacks;
};

// Helper macro for observer objects to implement their callback accessors
#define WSF_MIL_OBSERVER_CALLBACK_DEFINE(OBSERVER, EVENT)                                \
   WsfObserver::EVENT##Callback& WsfObserver::EVENT(const WsfSimulation* aSimulationPtr) \
   {                                                                                     \
      return WsfMilInterface::Find(aSimulationPtr)->Get##OBSERVER##Observer().EVENT;     \
   }

#endif
