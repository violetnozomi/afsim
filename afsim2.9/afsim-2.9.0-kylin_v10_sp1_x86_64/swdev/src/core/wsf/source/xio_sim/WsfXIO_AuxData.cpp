// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "xio_sim/WsfXIO_AuxData.hpp"

#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio_sim/WsfXIO_AutoDisMapping.hpp"
#include "xio_sim/WsfXIO_DisExtension.hpp"
#include "xio_sim/WsfXIO_Extension.hpp"
#include "xio_sim/WsfXIO_ObjectInfoLoad.hpp"
#include "xio_sim/WsfXIO_SimPacketRegistry.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

wsf::xio::AuxData::AuxData(WsfXIO_Simulation& aSimulation)
   : mSimulation(aSimulation)
   , mInterface(aSimulation.GetInterface())
{
   if (mInterface)
   {
      mCallbacks += mInterface->Connect(&AuxData::HandleIncomingAuxData, this);

      if (mInterface->mAuxDataUpdates)
      {
         mCallbacks += WsfAuxDataEnabled::AuxDataAccessed.Connect(&AuxData::AuxDataAccessed, this);
         mCallbacks += WsfAuxDataEnabled::AuxDataDestroyed.Connect(&AuxData::AuxDataDestroyed, this);
      }
   }
}

void wsf::xio::AuxData::HandleIncomingAuxData(WsfXIO_AuxDataPkt& aPkt)
{
   auto disExt = WsfXIO_Extension::FindDisExtension(mSimulation.GetSimulation());
   if (disExt)
   {
      auto mapping = disExt->GetAutoDisMapping();
      if (mapping)
      {
         auto xioConnection = dynamic_cast<WsfXIO_Connection*>(aPkt.GetSender());
         if (xioConnection)
         {
            auto platformInfo = mapping->FindPlatform(xioConnection->GetConnectionId(), aPkt.mPlatformIndex);
            if (platformInfo)
            {
               platformInfo->mPlatformPtr->SetAuxData(aPkt.mAuxData);
            }
         }
      }
   }
}

void wsf::xio::AuxData::SendAllPlatformAuxData(double aSimTime)
{
   std::lock_guard<std::mutex> lock(mAuxDataAccessedMutex);
   for (const auto* accessed : mAuxDataAccessed)
   {
      // static_cast should be safe here since only platforms were added to the set.
      auto platform = static_cast<const WsfPlatform*>(accessed);
      SendPlatformAuxData(aSimTime, platform);
   }
   mAuxDataAccessed.clear();
}

void wsf::xio::AuxData::SendPlatformAuxData(double aSimTime, const WsfPlatform* aPlatform)
{
   WsfXIO_AuxDataPkt pkt;
   pkt.mPlatformIndex = static_cast<int32_t>(aPlatform->GetIndex());
   if (aPlatform->HasAuxData())
   {
      pkt.mAuxData = aPlatform->GetAuxDataConst();
   }
   mInterface->SendToAllTCP(pkt);
}

void wsf::xio::AuxData::AuxDataAccessed(const WsfAuxDataEnabled* aSource)
{
   auto platform = dynamic_cast<const WsfPlatform*>(aSource);
   if (platform && !platform->IsExternallyControlled())
   {
      std::lock_guard<std::mutex> lock(mAuxDataAccessedMutex);
      if (mAuxDataAccessed.empty())
      {
         WsfSimulation& sim     = mSimulation.GetSimulation();
         double         simTime = sim.GetSimTime();
         sim.AddEventT<WsfOneShotEvent>(simTime, [this, simTime]() { SendAllPlatformAuxData(simTime); });
      }
      mAuxDataAccessed.insert(aSource);
   }
}

void wsf::xio::AuxData::AuxDataDestroyed(const WsfAuxDataEnabled* aDestroyed)
{
   std::lock_guard<std::mutex> lock(mAuxDataAccessedMutex);
   mAuxDataAccessed.erase(aDestroyed);
}
