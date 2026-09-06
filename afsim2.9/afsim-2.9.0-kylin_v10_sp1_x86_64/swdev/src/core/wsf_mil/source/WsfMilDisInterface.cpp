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

#include "WsfMilDisInterface.hpp"

#include "DisPduEnums.hpp"
#include "DisSystem.hpp"
#include "UtInput.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfMil.hpp"
#include "WsfMilDisPlatform.hpp"
#include "WsfMilExtInterface.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"
#include "WsfWeaponTypes.hpp"
#include "dis/WsfDisDetonation.hpp"
#include "dis/WsfDisDirectedEnergyFire.hpp"
#include "dis/WsfDisEmission.hpp"
#include "dis/WsfDisEntityDamageStatus.hpp"
#include "dis/WsfDisFire.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisLaserDesignations.hpp"
#include "dis/WsfDisPduFactory.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisWeaponEvents.hpp"
#include "ext/WsfExtEmission.hpp"
#include "ext/WsfExtEntityType.hpp"
#include "ext/WsfExtInterface.hpp"

WsfMilDisInterface::WsfMilDisInterface(WsfMilExtension* aMilExtensionPtr)
   : mSuppressDirectedEnergyData(false)
   , mDisInterfacePtr(nullptr)
   , mExtMilPtr(nullptr)
   , mLaserDesignationsPtr(nullptr)
   , mWeaponEventsPtr(nullptr)
   , mMilExtensionPtr(aMilExtensionPtr)
   , mCallbacks()
{
}

WsfMilDisInterface::WsfMilDisInterface(const WsfMilDisInterface& aSrc)
   : mSuppressDirectedEnergyData(aSrc.mSuppressDirectedEnergyData)
   , mDisInterfacePtr(nullptr)
   , mExtMilPtr(nullptr)
   , mLaserDesignationsPtr(nullptr)
   , mWeaponEventsPtr(nullptr)
   , mMilExtensionPtr(aSrc.mMilExtensionPtr)
   , sIncomingTransferList(aSrc.sIncomingTransferList)
   , sOutgoingTransferList(aSrc.sOutgoingTransferList)
   , mImplicitWeaponEngagementMap()
   , // These will be populated at runtime
   mDirectedEnergyWeaponMap()
   , mCallbacks()
{
}

WsfMilDisInterface::~WsfMilDisInterface()
{
   delete mLaserDesignationsPtr;
   delete mWeaponEventsPtr;

   for (const auto& engagement : mImplicitWeaponEngagementMap)
   {
      WsfWeaponEngagement::Delete(engagement.second);
   }
}

WsfComponent* WsfMilDisInterface::CloneComponent() const
{
   return new WsfMilDisInterface(*this);
}

void* WsfMilDisInterface::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_MIL_DIS_EXTENSION)
   {
      return this;
   }
   return nullptr;
}

bool WsfMilDisInterface::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "incoming_weapon_transfer")
   {
      IncomingTransfer transfer;
      WsfDisInterface::ReadEntityType(aInput, transfer.mWeaponDisEntityType);
      std::string word;
      aInput.ReadValue(word);
      if (word == "from")
      {
         WsfDisInterface::ReadEntityId(aInput, transfer.mFiringDisEntityId);
         aInput.ReadValue(word);
      }
      aInput.StringEqual(word, "using");
      std::string weaponSystemType;
      aInput.ReadValue(weaponSystemType);
      transfer.mWeaponSystemType = weaponSystemType;
      sIncomingTransferList.push_back(transfer);
   }
   else if (command == "outgoing_weapon_transfer")
   {
      OutgoingTransfer transfer;
      std::string      weaponSystemType;
      aInput.ReadValue(weaponSystemType);
      transfer.mWeaponSystemType = weaponSystemType;
      sOutgoingTransferList.push_back(transfer);
   }
   else if (command == "suppress_directed_energy_data")
   {
      mSuppressDirectedEnergyData = aInput.ReadBool();
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void WsfMilDisInterface::AddedToDisInterface(WsfDisInterface* aDisInterfacePtr)
{
   mDisInterfacePtr             = aDisInterfacePtr;
   mWeaponEventsPtr             = new WsfDisWeaponEvents(this);
   WsfDisPduFactory* pduFactory = ((WsfDisPduFactory*)mDisInterfacePtr->GetPduFactory());
   pduFactory->SetPduCreateFunction(DisEnum::Pdu::Type::Fire, new WsfDisPduFactory::CreateFunctionT<WsfDisFire>);
   pduFactory->SetPduCreateFunction(DisEnum::Pdu::Type::DirectedEnergyFire,
                                    new WsfDisPduFactory::CreateFunctionT<WsfDisDirectedEnergyFire>);
   pduFactory->SetPduCreateFunction(DisEnum::Pdu::Type::Detonation,
                                    new WsfDisPduFactory::CreateFunctionT<WsfDisDetonation>);
   pduFactory->SetPduCreateFunction(DisEnum::Pdu::Type::DirectedEnergyDamageStatus,
                                    new WsfDisPduFactory::CreateFunctionT<WsfDisEntityDamageStatus>);

   mExtMilPtr = WsfMilExtInterface::Find(*aDisInterfacePtr->GetExtInterface());

   if (aDisInterfacePtr->GetSimulation().FindExtension("laser_designations") != nullptr)
   {
      mLaserDesignationsPtr = new WsfDisLaserDesignations(this);
   }
}

bool WsfMilDisInterface::Initialize(double aSimTime)
{
   WsfSimulation* simPtr = &(mDisInterfacePtr->GetSimulation());
   mCallbacks.Add(WsfObserver::WeaponFired(simPtr).Connect(&WsfMilDisInterface::WeaponFired, this));
   mCallbacks.Add(
      WsfObserver::DirectedEnergyWeaponBeginShot(simPtr).Connect(&WsfMilDisInterface::DirectedEnergyWeaponBeginShot, this));
   mCallbacks.Add(
      WsfObserver::DirectedEnergyWeaponUpdateShot(simPtr).Connect(&WsfMilDisInterface::DirectedEnergyWeaponUpdateShot,
                                                                  this));
   mCallbacks.Add(
      WsfObserver::DirectedEnergyWeaponEndShot(simPtr).Connect(&WsfMilDisInterface::DirectedEnergyWeaponEndShot, this));
   mCallbacks.Add(
      WsfObserver::ImplicitWeaponEndEngagement(simPtr).Connect(&WsfMilDisInterface::ImplicitWeaponEndEngagement, this));
   mCallbacks.Add(WsfObserver::WeaponTerminated(simPtr).Connect(&WsfMilDisInterface::WeaponTerminated, this));
   mCallbacks.Add(WsfObserver::WeaponTurnedOff(simPtr).Connect(&WsfMilDisInterface::WeaponTurnedOff, this));
   mCallbacks.Add(WsfObserver::WeaponTurnedOn(simPtr).Connect(&WsfMilDisInterface::WeaponTurnedOn, this));
   mCallbacks.Add(WsfObserver::JammingRequestCanceled(simPtr).Connect(&WsfMilDisInterface::JammingRequestCanceled, this));
   mCallbacks.Add(WsfObserver::JammingRequestInitiated(simPtr).Connect(&WsfMilDisInterface::JammingRequestInitiated, this));
   mCallbacks.Add(WsfObserver::JammingRequestUpdated(simPtr).Connect(&WsfMilDisInterface::JammingRequestUpdated, this));

   if ((mExtMilPtr->GetDebugWarfarePDU() == 0) && mDisInterfacePtr->IsTerseMode())
   {
      mExtMilPtr->SetDebugWarfarePDU(1);
   }

   return true;
}

bool WsfMilDisInterface::TransferPlatformToInternalControl(double         aSimTime,
                                                           WsfPlatform*   aPlatformPtr,
                                                           DisEntityId&   aEntityId,
                                                           DisEntityType& aEntityType)
{
   return mWeaponEventsPtr->TransferPlatformToInternalControl(aSimTime, aPlatformPtr, aEntityId, aEntityType);
}

bool WsfMilDisInterface::TransferPlatformToExternalControl(double aSimTime, WsfPlatform* aPlatformPtr)
{
   return mWeaponEventsPtr->TransferPlatformToExternalControl(aSimTime, aPlatformPtr);
}


// ============================================================================
//! Update a DIS System record from a Weapon object.
// public
void WsfMilDisInterface::UpdateSystemFromWeapon(WsfWeapon* aWeaponPtr, DisSystem* aSystemPtr)
{
   double xyz[3];
   aWeaponPtr->GetLocation(xyz);
   aSystemPtr->SetLocation(static_cast<DisFloat32>(xyz[0]),
                           static_cast<DisFloat32>(xyz[1]),
                           static_cast<DisFloat32>(xyz[2]));

   unsigned int beamCount    = aSystemPtr->GetNumberOfBeams();
   size_t       emitterCount = aWeaponPtr->GetEM_XmtrCount();
   if (beamCount == 0)
   {
      // System is being turned off.
   }
   else if (emitterCount == 0)
   {
      // Passive jammer... (we don't do anything for these yet...)
   }
   else if (emitterCount == beamCount)
   {
      auto beamIter(aSystemPtr->GetBeamBegin());

      for (size_t emitterIndex = 0; emitterIndex < emitterCount; ++emitterIndex)
      {
         double minAz = 0.0;
         double maxAz = 0.0;
         double minEl = 0.0;
         double maxEl = 0.0;

         DisBeam*    beamPtr = beamIter[emitterIndex];
         WsfEM_Xmtr& xmtr    = aWeaponPtr->GetEM_Xmtr(emitterIndex);
         mDisInterfacePtr->GetExtInterface()->GetEmission()->UpdateBeamLimits(xmtr, minAz, maxAz, minEl, maxEl);
         UpdateBeamFromXmtr(xmtr, beamPtr, minAz, maxAz, minEl, maxEl);

         // If this is a weapon which is pointed at a single location (such as a steered jammer),
         // then attempt to use the beam widths from the antenna definition.

         if (xmtr.GetMode() != nullptr)
         {
            WsfWeapon::WsfWeaponMode* modePtr = aWeaponPtr->GetModeEntry(xmtr.GetMode()->GetModeIndex());
            if (modePtr != nullptr)
            {
               WsfAntennaPattern* patternPtr = xmtr.GetAntennaPattern(xmtr.GetPolarization(), xmtr.GetFrequency());
               if (patternPtr != nullptr)
               {
                  double azBeamwidth = patternPtr->GetAzimuthBeamwidth(xmtr.GetFrequency(), 0.0, 0.0);
                  double elBeamwidth = patternPtr->GetElevationBeamwidth(xmtr.GetFrequency(), 0.0, 0.0);

                  // Don't use the beamwidth in directions that we scan. (e.g. for a height finder we don't use the
                  // elevation beamwidth because the elevation limits should come from the scan limits later on).
                  switch (xmtr.GetAntenna()->GetScanMode())
                  {
                  case WsfEM_Antenna::cSCAN_AZ:
                     azBeamwidth = 0.0;
                     break;
                  case WsfEM_Antenna::cSCAN_EL:
                     elBeamwidth = 0.0;
                     break;
                  case WsfEM_Antenna::cSCAN_AZ_EL:
                     azBeamwidth = 0.0;
                     elBeamwidth = 0.0;
                     break;
                  default:
                     break;
                  }

                  // Use a beamwidth for the limit if it is defined (or not canceled out above...)

                  if (azBeamwidth != 0.0)
                  {
                     maxAz = 0.5 * azBeamwidth;
                     minAz = -maxAz;
                  }
                  if (elBeamwidth != 0.0)
                  {
                     maxEl = 0.5 * elBeamwidth;
                     minEl = -maxEl;
                  }

                  beamPtr->SetAzimuthSweep(static_cast<DisFloat32>(0.5 * (maxAz - minAz)));
                  beamPtr->SetElevationSweep(static_cast<DisFloat32>(0.5 * (maxEl - minEl)));
               }
            }
         }
      }
   }
   else
   {
      // emitterCount != beamCount (should not happen)
   }
}

// ============================================================================
void WsfMilDisInterface::WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr)
{
   mDisInterfacePtr->UpdateLastSimTime(aSimTime);
   if (mDisInterfacePtr->HasDevice())
   {
      GetWeaponEvents()->WeaponFired(aSimTime, aEngagementPtr, aTargetTrackPtr);
   }
}

// ============================================================================
void WsfMilDisInterface::DirectedEnergyWeaponBeginShot(double                     aSimTime,
                                                       WsfDirectedEnergyWeapon*   aDirectedEnergyWeaponPtr,
                                                       const WsfWeaponEngagement* aEngagementPtr)
{
   mDisInterfacePtr->UpdateLastSimTime(aSimTime);
   if (mDisInterfacePtr->HasDevice() && (!mSuppressDirectedEnergyData))
   {
      GetWeaponEvents()->DirectedEnergyWeaponFired(aSimTime, aDirectedEnergyWeaponPtr, aEngagementPtr);
   }
}

// ============================================================================
void WsfMilDisInterface::DirectedEnergyWeaponUpdateShot(double                     aSimTime,
                                                        WsfDirectedEnergyWeapon*   aDirectedEnergyWeaponPtr,
                                                        const WsfWeaponEngagement* aEngagementPtr)
{
   mDisInterfacePtr->UpdateLastSimTime(aSimTime);
   if (mDisInterfacePtr->HasDevice() && (!mSuppressDirectedEnergyData))
   {
      GetWeaponEvents()->DirectedEnergyWeaponFired(aSimTime, aDirectedEnergyWeaponPtr, aEngagementPtr);
   }
}

// ============================================================================
void WsfMilDisInterface::DirectedEnergyWeaponEndShot(double                     aSimTime,
                                                     WsfDirectedEnergyWeapon*   aDirectedEnergyWeaponPtr,
                                                     const WsfWeaponEngagement* aEngagementPtr)
{
   mDisInterfacePtr->UpdateLastSimTime(aSimTime);
   if (mDisInterfacePtr->HasDevice() && (!mSuppressDirectedEnergyData))
   {
      GetWeaponEvents()->DirectedEnergyWeaponFired(aSimTime, aDirectedEnergyWeaponPtr, aEngagementPtr);
   }
}

void WsfMilDisInterface::ImplicitWeaponEndEngagement(double                     aSimTime,
                                                     WsfImplicitWeapon*         aImplicitWeapon,
                                                     const WsfWeaponEngagement* aEngagementPtr)
{
   mDisInterfacePtr->UpdateLastSimTime(aSimTime);
   if (mDisInterfacePtr->HasDevice())
   {
      GetWeaponEvents()->ImplicitWeaponEndEngagement(aSimTime, aImplicitWeapon, aEngagementPtr);
   }
}

// ============================================================================
void WsfMilDisInterface::WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   mDisInterfacePtr->UpdateLastSimTime(aSimTime);
   if (mDisInterfacePtr->HasDevice())
   {
      GetWeaponEvents()->WeaponTerminated(aSimTime, aEngagementPtr);
   }
}

// ============================================================================
void WsfMilDisInterface::WeaponTurnedOff(double aSimTime, WsfWeapon* aWeaponPtr)
{
   mDisInterfacePtr->UpdateLastSimTime(aSimTime);
   if ((mDisInterfacePtr->HasDevice() || mDisInterfacePtr->IsDeferredConnection()) &&
       (!aWeaponPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t platformIndex = aWeaponPtr->GetPlatform()->GetIndex();

      WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->GetDisPlatform(platformIndex);
      if (disPlatformPtr != nullptr)
      {
         WsfMilDisPlatform::Find(*disPlatformPtr)->WeaponTurnedOff(aSimTime, aWeaponPtr);
      }
   }
}

// ============================================================================
void WsfMilDisInterface::WeaponTurnedOn(double aSimTime, WsfWeapon* aWeaponPtr)
{
   mDisInterfacePtr->UpdateLastSimTime(aSimTime);
   if ((mDisInterfacePtr->HasDevice() || mDisInterfacePtr->IsDeferredConnection()) &&
       (!aWeaponPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aWeaponPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->GetDisPlatform(platformIndex);
      if (disPlatformPtr != nullptr)
      {
         WsfMilDisPlatform::Find(*disPlatformPtr)->WeaponTurnedOn(aSimTime, aWeaponPtr);
      }
   }
}

// ============================================================================
void WsfMilDisInterface::JammingRequestCanceled(double     aSimTime,
                                                WsfWeapon* aWeaponPtr,
                                                double     aFrequency,
                                                double     aBandwidth,
                                                size_t     aTargetIndex)
{
   mDisInterfacePtr->UpdateLastSimTime(aSimTime);
   if (mDisInterfacePtr->HasDevice() && (!aWeaponPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aWeaponPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->GetDisPlatform(platformIndex);
      if (disPlatformPtr != nullptr)
      {
         WsfMilDisPlatform::Find(*disPlatformPtr)->JammingRequestCanceled(aSimTime, aWeaponPtr, aTargetIndex);
      }
   }
}

// ============================================================================
void WsfMilDisInterface::JammingRequestInitiated(double      aSimTime,
                                                 WsfWeapon*  aWeaponPtr,
                                                 double      aFrequency,
                                                 double      aBandwidth,
                                                 WsfStringId aTechniqueId,
                                                 size_t      aTargetIndex)
{
   mDisInterfacePtr->UpdateLastSimTime(aSimTime);
   if (mDisInterfacePtr->HasDevice() && (!aWeaponPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aWeaponPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->GetDisPlatform(platformIndex);
      if (disPlatformPtr != nullptr)
      {
         WsfMilDisPlatform::Find(*disPlatformPtr)->JammingRequestInitiated(aSimTime, aWeaponPtr, aTargetIndex);
      }
   }
}

// ============================================================================
void WsfMilDisInterface::JammingRequestUpdated(double      aSimTime,
                                               WsfWeapon*  aWeaponPtr,
                                               double      aFrequency,
                                               double      aBandwidth,
                                               WsfStringId aTechniqueId,
                                               size_t      aTargetIndex)
{
   mDisInterfacePtr->UpdateLastSimTime(aSimTime);
   if (mDisInterfacePtr->HasDevice() && (!aWeaponPtr->GetPlatform()->IsExternallyControlled()))
   {
      size_t          platformIndex  = aWeaponPtr->GetPlatform()->GetIndex();
      WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->GetDisPlatform(platformIndex);
      if (disPlatformPtr != nullptr)
      {
         WsfMilDisPlatform::Find(*disPlatformPtr)->JammingRequestUpdated(aSimTime, aWeaponPtr, aTargetIndex);
      }
   }
}

// ============================================================================
void WsfMilDisInterface::PrepareComponent(double aSimTime)
{
   WsfSimulation& sim = mDisInterfacePtr->GetSimulation();
   for (unsigned int i = 0; i < sim.GetPlatformCount(); ++i)
   {
      WsfPlatform* platPtr = sim.GetPlatformEntry(i);
      for (WsfComponentList::RoleIterator<WsfWeapon> iter(*platPtr); !iter.AtEnd(); ++iter)
      {
         if (iter->IsTurnedOn())
         {
            WeaponTurnedOn(aSimTime, *iter);
         }
      }
   }
}

// ============================================================================
WsfMilDisInterface* WsfMilDisInterface::Find(const WsfDisInterface& aDisInterface)
{
   WsfMilDisInterface* milDisPtr = nullptr;
   aDisInterface.GetComponents().FindByRole(milDisPtr);
   return milDisPtr;
}

// ============================================================================
void WsfMilDisInterface::PlatformCreated(WsfDisPlatform* aPlatformPtr)
{
   aPlatformPtr->AddComponent(new WsfMilDisPlatform(aPlatformPtr));
}


const WsfScenario& WsfMilDisInterface::GetScenario() const
{
   return mDisInterfacePtr->GetScenario();
}

// ============================================================================
//! Removes unused xmtrs from a weapon.
//!
//! @param aEmissionPtr       [input] Pointer to the DIS emission PDU.
//! @param aSystemPtr         [input] Pointer to the DIS emission PDU system.
//! @param aWeaponPtr         [input] Pointer to the weapon to update.
void WsfMilDisInterface::RemoveXmtrs(WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr, WsfWeapon* aWeaponPtr)
{
   // Beam count is less than the emitter count for the system
   WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->FindDisPlatform(aEmissionPtr->GetEmittingEntityId());
   if (disPlatformPtr != nullptr)
   {
      // Save xmtrs
      std::list<unsigned int> xmtrList;
      size_t                  xmtrCount = aWeaponPtr->GetEM_XmtrCount();
      for (size_t xmtrIndex = 0; xmtrIndex < xmtrCount; ++xmtrIndex)
      {
         xmtrList.push_back(aWeaponPtr->GetEM_Xmtr(xmtrIndex).GetUniqueId());
      }

      // Loop through the beams and remove matches to xmtrs
      auto beamIter(aSystemPtr->GetBeamBegin());

      while (beamIter != aSystemPtr->GetBeamEnd())
      {
         DisBeam*               beamPtr = *beamIter;
         WsfDisPlatform::SB_Key sbKey(aSystemPtr->GetNumber(), beamPtr->GetNumber());
         unsigned int           uniqueId = disPlatformPtr->GetSysBeamIdToPartId(sbKey);
         xmtrList.remove(uniqueId);
         ++beamIter;
      }

      // Stop remaining xmtrs left in the list
      auto       iter = xmtrList.begin();
      WsfTrackId trackId(nullptr, 0);
      while (iter != xmtrList.end())
      {
         // update the xmtrCount as it may change if a StopJamming or something above changed it.
         xmtrCount = aWeaponPtr->GetEM_XmtrCount();
         for (unsigned int xIndex = 0; xIndex < xmtrCount; ++xIndex)
         {
            if (aWeaponPtr->GetEM_Xmtr(xIndex).GetUniqueId() == (*iter))
            {
               aWeaponPtr->StopJamming(mDisInterfacePtr->GetSimTime(),
                                       aWeaponPtr->GetEM_Xmtr(xIndex).GetFrequency(),
                                       aWeaponPtr->GetEM_Xmtr(xIndex).GetBandwidth(),
                                       0,
                                       trackId);
               break;
            }
         }
         ++iter;
      }
   }
}
// ============================================================================
//! Updates the weapon from the DIS emitter system data.
//!
//! @param aEmissionPtr       [input] Pointer to the DIS emission PDU.
//! @param aSystemPtr         [input] Pointer to the DIS emission PDU system.
//! @param aWeaponPtr         [input] Pointer to the weapon to update.
void WsfMilDisInterface::UpdateWeapon(WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr, WsfWeapon* aWeaponPtr)
{
   // Update location
   aEmissionPtr->UpdateLocation(aSystemPtr, aWeaponPtr);

   unsigned int beamCount       = aSystemPtr->GetNumberOfBeams();
   size_t       emitterCount    = aWeaponPtr->GetEM_XmtrCount();
   unsigned int maxEmitterCount = aWeaponPtr->GetMaximumRequestCount();

   if (beamCount == 0)
   {
      // Turn system off
      mDisInterfacePtr->GetSimulation().TurnPartOff(mDisInterfacePtr->GetSimTime(), aWeaponPtr);
   }
   else
   {
      // Need to shut off some spots
      if (beamCount < emitterCount)
      {
         RemoveXmtrs(aEmissionPtr, aSystemPtr, aWeaponPtr);
      }

      // Once spots are removed beamCount == emitterCount
      if (beamCount >= emitterCount)
      {
         // Update the xmtr from the beam data
         WsfTrackId   trackId(nullptr, 0);
         bool         systemOn(false);
         auto         beamIter(aSystemPtr->GetBeamBegin());
         unsigned int beamIndex = 1; // 1-based
         size_t       targetIndex;
         while ((beamIter != aSystemPtr->GetBeamEnd()) && (beamIndex <= maxEmitterCount))
         {
            DisBeam* beamPtr = *beamIter;
            // TODO -AWK- Multi-function systems may have beams with more than one function.
            // Will need to parse the PDU beams before deciding system type in the future.
            if (beamPtr->GetFunction() == 12)
            {
               bool isNewRequest(false);

               targetIndex = 0;
               std::vector<DisTrackJam> tjList(beamPtr->GetTargetVector());
               auto                     tjIter(tjList.begin());
               if (tjIter != tjList.end())
               {
                  DisTrackJam     trackJam    = *tjIter;
                  WsfDisPlatform* platformPtr = mDisInterfacePtr->FindDisPlatform(trackJam.GetEntityId());
                  if (platformPtr != nullptr)
                  {
                     targetIndex = platformPtr->GetPlatform()->GetIndex();
                  }
               }

               // If the PDU contains more beams than jammer spots then add jamming requests
               if (beamIndex > emitterCount)
               {
                  // Add jamming requests
                  isNewRequest =
                     aWeaponPtr->StartJamming(mDisInterfacePtr->GetSimTime(),
                                              beamPtr->GetFrequency(),
                                              beamPtr->GetFrequencyRange(),
                                              0,
                                              mExtMilPtr->GetTechniqueTypeId(beamPtr->GetJammingTechniqueRecord()()),
                                              trackId,
                                              targetIndex);
               }

               // Update the emitter count (may have been updated by start jamming)
               emitterCount = aWeaponPtr->GetEM_XmtrCount();
               if (beamIndex <= emitterCount)
               {
                  WsfEM_Xmtr& xmtr = aWeaponPtr->GetEM_Xmtr(beamIndex - 1);

                  if (!isNewRequest)
                  {
                     // Update jamming request
                     aWeaponPtr->UpdateJamming(mDisInterfacePtr->GetSimTime(),
                                               beamIndex - 1,
                                               beamPtr->GetFrequency(),
                                               beamPtr->GetFrequencyRange(),
                                               mExtMilPtr->GetTechniqueTypeId(beamPtr->GetJammingTechniqueRecord()()),
                                               trackId,
                                               targetIndex);
                  }

                  aEmissionPtr->UpdateXmtr(beamPtr, &xmtr, systemOn);
                  aEmissionPtr->UpdateSysBeamMapping(aSystemPtr->GetNumber(), beamPtr->GetNumber(), xmtr.GetUniqueId());

                  // Increment the beamIndex
                  ++beamIndex;
               }
            }

            // Get the next beam
            ++beamIter;
         } // while loop

         // Turn the system on or off based on the power level
         if (systemOn)
         {
            mDisInterfacePtr->GetSimulation().TurnPartOn(mDisInterfacePtr->GetSimTime(), aWeaponPtr);
         }
         else
         {
            mDisInterfacePtr->GetSimulation().TurnPartOff(mDisInterfacePtr->GetSimTime(), aWeaponPtr);
         }
      }
   }
}

bool WsfMilDisInterface::UpdateEmissionSystem(WsfPlatform* aPlatformPtr, WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr)
{
   unsigned int emitterNameDIS     = aSystemPtr->GetName();
   unsigned int emitterIdNumberDIS = aSystemPtr->GetNumber();
   WsfStringId  sensorTypeId       = mDisInterfacePtr->GetExtEmission()->GetPartTypeId(aSystemPtr->GetName());

   // look for a weapon
   WsfWeapon* weaponPtr = WsfMilExtInterface::GetWeapon(aPlatformPtr, sensorTypeId, emitterNameDIS, emitterIdNumberDIS);
   if (weaponPtr != nullptr)
   {
      WsfExtEmission* extEmissionPtr = mDisInterfacePtr->GetExtEmission();
      if ((extEmissionPtr->GetDebugMask() & (WsfExtEmission::cLEVEL_ONE_DEBUG | WsfExtEmission::cLEVEL_TWO_DEBUG)) != 0)
      {
         auto out = ut::log::debug() << "Emission PDU: Update Weapon:";
         out.AddNote() << "T = " << mDisInterfacePtr->GetSimTime();
         out.AddNote() << "Weapon: " << weaponPtr->GetName();
         out.AddNote() << "Weapon Type: " << weaponPtr->GetType();
         out.AddNote() << "DIS: " << emitterNameDIS << "." << emitterIdNumberDIS;
      }
      UpdateWeapon(aEmissionPtr, aSystemPtr, weaponPtr);
      return true;
   }
   return false;
}

bool WsfMilDisInterface::AddEmissionSystem(WsfPlatform* aPlatformPtr, WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr)
{
   unsigned int emitterNameDIS     = aSystemPtr->GetName();
   unsigned int emitterIdNumberDIS = aSystemPtr->GetNumber();
   WsfStringId  sensorTypeId       = mDisInterfacePtr->GetExtEmission()->GetPartTypeId(aSystemPtr->GetName());

   WsfMilExtInterface* milExtPtr      = WsfMilExtInterface::Find(*mDisInterfacePtr->GetExtInterface());
   WsfExtEmission*     extEmissionPtr = mDisInterfacePtr->GetExtEmission();
   // try to add weapon
   WsfWeapon* weaponPtr =
      milExtPtr->AddWeapon(mDisInterfacePtr->GetSimTime(), aPlatformPtr, sensorTypeId, emitterNameDIS, emitterIdNumberDIS);
   if (weaponPtr != nullptr)
   {
      if ((extEmissionPtr->GetDebugMask() & (WsfExtEmission::cLEVEL_ONE_DEBUG | WsfExtEmission::cLEVEL_TWO_DEBUG)) != 0)
      {
         auto out = ut::log::debug() << "Emission PDU: Added Weapon:";
         out.AddNote() << "T = " << mDisInterfacePtr->GetSimTime();
         out.AddNote() << "Weapon: " << weaponPtr->GetName();
         out.AddNote() << "Weapon Type: " << weaponPtr->GetType();
         out.AddNote() << "DIS: " << emitterNameDIS << "." << emitterIdNumberDIS;
      }
      UpdateWeapon(aEmissionPtr, aSystemPtr, weaponPtr);
      return true;
   }
   return false;
}


// ============================================================================
//! Update a DIS Beam record from a Transmitter object.
// public
void WsfMilDisInterface::UpdateBeamFromXmtr(WsfEM_Xmtr& aXmtr,
                                            DisBeam*    aBeamPtr,
                                            double      aMinAz,
                                            double      aMaxAz,
                                            double      aMinEl,
                                            double      aMaxEl)
{
   mDisInterfacePtr->UpdateBeamFromXmtr(aXmtr, aBeamPtr, aMinAz, aMaxAz, aMinEl, aMaxEl);

   WsfEW_EA_EP* eaPtr = WsfEW_EA::GetElectronicAttack(aXmtr);
   if (eaPtr != nullptr)
   {
      if (eaPtr->TechniquesAreInUse())
      {
         // Get the first technique in the list to send
         WsfStringId techniqueId = eaPtr->GetTechniquesInUse()[0];
         aBeamPtr->SetJammingTechniqueRecord(static_cast<DisUint32>(mExtMilPtr->GetJamModeSequence(techniqueId)));
      }
   }
}

void WsfMilDisInterface::AddImplicitWeaponEngagement(WsfWeaponEngagement* aEngagementPtr, const DisEventId& aId)
{
   mImplicitWeaponEngagementMap[aId] = aEngagementPtr;
}

WsfWeaponEngagement* WsfMilDisInterface::FindImpicitWeaponEngagement(const DisEventId& aId) const
{
   auto engagementIter = mImplicitWeaponEngagementMap.find(aId);
   if (engagementIter != mImplicitWeaponEngagementMap.end())
   {
      return engagementIter->second;
   }
   return nullptr;
}

void WsfMilDisInterface::AddDirectedEnergyWeapon(std::unique_ptr<WsfDirectedEnergyWeapon> aWeaponPtr, const DisEventId& aId)
{
   mDirectedEnergyWeaponMap[aId] = std::move(aWeaponPtr);
}

WsfDirectedEnergyWeapon* WsfMilDisInterface::FindDirectedEnergyWeapon(const DisEventId& aId) const
{
   auto weaponIter = mDirectedEnergyWeaponMap.find(aId);
   if (weaponIter != mDirectedEnergyWeaponMap.end())
   {
      return weaponIter->second.get();
   }
   return nullptr;
}

void WsfMilDisInterface::DeleteDirectedEnergyWeapon(const DisEventId& aId)
{
   mDirectedEnergyWeaponMap.erase(aId);
}

bool WsfMilDisInterface::UpdateXmtrFromBeam(WsfDisEmission* aEmissionPtr, DisBeam* aBeamPtr, WsfEM_Xmtr* aXmtrPtr, bool& aBeamOn)
{
   WsfEM_Antenna* antennaPtr = aXmtrPtr->GetAntenna();
   if (antennaPtr == nullptr)
   {
      return false;
   }

   // Protect the setting of the Az & El center for only jammers currently
   // to allow for steering via these inputs from the PDU.
   // Assume for now that the angles are wrt to the platform and not the ArticulatedPart()/Antenna
   if ((aXmtrPtr->GetFunction() == WsfEM_Xmtr::cXF_INTERFERER) && (aBeamPtr->GetNumberOfTargets() == 0))
   {
      // Azimuth & Elevation center
      double azCenter = aBeamPtr->GetAzimuthCenter();
      double elCenter = aBeamPtr->GetElevationCenter();

      double relPointedLocWCS[3];
      double pointedLocWCS[3];
      double pltfrmLocWCS[3];

      // Get the relative WCS location at a considerable distance from the location of the platform
      aXmtrPtr->GetPlatform()->GetRelativeLocationWCS(azCenter, elCenter, 100000.0, relPointedLocWCS);
      aXmtrPtr->GetPlatform()->GetLocationWCS(pltfrmLocWCS);
      UtVec3d::Add(pointedLocWCS, pltfrmLocWCS, relPointedLocWCS);
      WsfRF_Jammer* jammerPtr = static_cast<WsfRF_Jammer*>(aXmtrPtr->GetArticulatedPart());
      jammerPtr->SetTargetedLocationWCS(aXmtrPtr->GetUniqueId(), pointedLocWCS);

      // NOTE: The below is for future implementation of the part centric instead of platform centric az and el centers
      //       This has NOT been tested yet!!!! It also needs a if-else wrapper.
      //
      // Get the actual articulated part's yaw and pitch to allow for correcting of the location to be
      // calculated later if the as and el enters are wrt the articulated part.
      // double antennaLocWCS[3];

      // double yaw = antennaPtr->GetArticulatedPart()->GetYaw();
      // double pitch = antennaPtr->GetArticulatedPart()->GetPitch();

      // Get the relative WCS location at a considerable distance from the location of the part.
      // antennaPtr->GetRelativeLocationWCS((azCenter - yaw), (elCenter - pitch), 100000.0,  relPointedLocWCS);
      // antennaPtr->GetLocationWCS(antennaLocWCS);
      // UtVec3d::Add(pointedLocWCS, antennaLocWCS, relPointedLocWCS);

      if ((mDisInterfacePtr->GetExtEmission()->GetDebugMask() & WsfExtEmission::cLEVEL_TWO_DEBUG) != 0)
      {
         double azCue(0.0);
         double elCue(0.0);
         auto   out = ut::log::debug() << "Targeted:";
         if (jammerPtr->TargetedLocationValid(aXmtrPtr->GetUniqueId()))
         {
            // WsfRF_Jammer* jammerPtr = static_cast<WsfRF_Jammer*>(aXmtrPtr->GetArticulatedPart());
            jammerPtr->GetTargetedLocationWCS(aXmtrPtr->GetUniqueId(), pointedLocWCS);
            double lat;
            double lon;
            double alt;
            UtEntity::ConvertWCSToLLA(pointedLocWCS, lat, lon, alt);
            antennaPtr->GetRelativeLocationWCS(pointedLocWCS, relPointedLocWCS);
            antennaPtr->GetArticulatedPart()->ComputeAspect(relPointedLocWCS, azCue, elCue);
            out.AddNote() << "Lat: " << UtLatPos(lat);
            out.AddNote() << "Lon: " << UtLonPos(lon);
            out.AddNote() << "Alt: " << alt << " m";
         }
         out.AddNote() << "Az: " << (azCue * UtMath::cDEG_PER_RAD) << " deg";
         out.AddNote() << "El: " << (elCue * UtMath::cDEG_PER_RAD) << " deg";
      }
      return true;
   }
   return false;
}
