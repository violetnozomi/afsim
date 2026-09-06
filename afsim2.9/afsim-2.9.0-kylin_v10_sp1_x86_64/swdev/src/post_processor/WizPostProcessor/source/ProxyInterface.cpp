// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "ProxyInterface.hpp"

#include "ProjectWorkspace.hpp"
#include "ProxyMerge.hpp"
#include "ProxyWatcher.hpp"
#include "WsfPM_MilPlatform.hpp"
#include "WsfPM_MilRoot.hpp"

PostProcessor::ProxyInterface::ProxyInterface()
{
   mCallbacks += wizard::ProxyWatcher::ProxyAvailable.Connect(&ProxyInterface::ProxyAvailable, this);
}

void PostProcessor::ProxyInterface::ProxyAvailable(WsfPProxy* aProxy)
{
   mProxyNode = WsfPM_Root(wizard::ProxyWatcher::GetActiveProxy());
}

void PostProcessor::ProxyInterface::Update() // read from proxy
{
   WsfPM_MilRoot                  milRoot(mProxyNode.GetProxy());

   WsfPM_MilPlatformMap           platforms = milRoot.platforms();

   std::vector<WsfPM_MilPlatform> allPlatforms;
   platforms.GetValuesT(allPlatforms);

   QVector<Platform> newPlatforms;
   QVector<Platform> newWeaponPlatforms;
   QVector<PlatformPart>   newSensors;
   QVector<PlatformPart>   newWeapons;
   QVector<PlatformPart>     newTransmitters;
   QVector<PlatformPart>     newReceivers;
   QVector<PlatformPart>     newMovers;

   QMap<Platform, bool> addedPlatforms;
   QVector<Platform> removedPlatforms;
   QVector<PlatformPart>   addedSensors;
   QVector<PlatformPart>   removedSensors;
   QVector<PlatformPart>   addedWeapons;
   QVector<PlatformPart>   removedWeapons;
   QVector<PlatformPart>     addedTransmitters;
   QVector<PlatformPart>     removedTransmitters;
   QVector<PlatformPart>     addedReceivers;
   QVector<PlatformPart>     removedReceivers;
   QVector<PlatformPart>     addedMovers;
   QVector<PlatformPart>     removedMovers;

   // Platforms
   for (const auto& p : allPlatforms)
   {
      Platform platform(p.GetName().c_str(), p.GetTypeName().c_str(), p.Side().c_str(), p.Mover().GetTypeName().c_str());
      newPlatforms.push_back(platform);

      // Weapons
      WsfPM_WeaponMap weapons = p.Weapons();
      std::vector<WsfPM_Weapon> allWeapons;
      weapons.GetValuesT(allWeapons);
      for (const auto& w : allWeapons)
      {
         newWeapons.push_back((PlatformPart(w.GetName().c_str(), w.GetTypeName().c_str(), platform)));
         std::string type = w.LaunchedPlatformType();
         if (!type.empty())
         {
            std::string platformName = p.GetName() + "_" + w.GetName() + "_*";
            newWeaponPlatforms.push_back(Platform(platformName.c_str(), type.c_str(), p.Side().c_str(), p.Mover().GetTypeName().c_str()));
         }
      }

      // Sensors
      WsfPM_SensorMap sensors = p.Sensors();
      std::vector<WsfPM_Sensor> allSensors;
      sensors.GetValuesT(allSensors);
      for (const auto& s : allSensors)
      {
         newSensors.push_back(PlatformPart(s.GetName().c_str(), s.GetTypeName().c_str(),
                                           Platform(p.GetName().c_str(), p.GetTypeName().c_str(), p.Side().c_str(), p.Mover().GetTypeName().c_str())));
      }

      // Comms
      WsfPM_CommMap comms = p.Comms();
      std::vector<WsfPM_Comm> allComms;
      comms.GetValuesT(allComms);
      for (const auto& c : allComms)
      {
         if ((c.GetTypeName() == "WSF_COMM_XMTR") || (c.GetTypeName() == "WSF_COMM_TRANSCEIVER"))
         {
            newTransmitters.push_back(PlatformPart(c.GetName().c_str(), c.GetTypeName().c_str(),
                                                   Platform(p.GetName().c_str(), p.GetTypeName().c_str(), p.Side().c_str(), p.Mover().GetTypeName().c_str())));
         }
         if ((c.GetTypeName() == "WSF_COMM_RCVR") || (c.GetTypeName() == "WSF_COMM_TRANSCEIVER"))
         {
            newReceivers.push_back(PlatformPart(c.GetName().c_str(), c.GetTypeName().c_str(),
                                                Platform(p.GetName().c_str(), p.GetTypeName().c_str(), p.Side().c_str(), p.Mover().GetTypeName().c_str())));
         }
      }

      // Movers
      WsfPM_MoverMap movers = p.Mover();
      newMovers.push_back(PlatformPart(movers.GetName().c_str(), movers.GetTypeName().c_str(),
                                       Platform(p.GetName().c_str(), p.GetTypeName().c_str(), p.Side().c_str(), p.Mover().GetTypeName().c_str())));
      
   }

   // --------------------------------------------------------------------------------------------------
   WsfProxy::Time endTime = milRoot.simulation().GetEndTime();

   GetDiff(newPlatforms, mPlatforms, addedPlatforms, removedPlatforms);
   GetDiff(newWeaponPlatforms, mWeaponPlatforms, addedPlatforms, removedPlatforms, true, true); // append to addedPlatforms, removedPlatforms

   GetDiff(newSensors, mSensors, addedSensors, removedSensors);
   GetDiff(newWeapons, mWeapons, addedWeapons, removedWeapons);
   GetDiff(newTransmitters, mTransmitters, addedTransmitters, removedTransmitters);
   GetDiff(newReceivers, mReceivers, addedReceivers, removedReceivers);
   GetDiff(newMovers, mMovers, addedMovers, removedMovers);

   if (endTime != mEndTime)
   {
      emit EndTimeChanged(endTime);
   }
   if (!removedPlatforms.empty())
   {
      emit PlatformRemoved(removedPlatforms);
   }
   if (!addedPlatforms.empty())
   {
      emit PlatformAdded(addedPlatforms);
   }
   if (!addedSensors.empty())
   {
      emit SensorAdded(addedSensors);
   }
   if (!removedSensors.empty())
   {
      emit SensorRemoved(removedSensors);
   }
   if (!addedWeapons.empty())
   {
      emit WeaponAdded(addedWeapons);
   }
   if (!removedWeapons.empty())
   {
      emit WeaponRemoved(removedWeapons);
   }
   if (!addedTransmitters.empty())
   {
      emit TransmitterAdded(addedTransmitters);
   }
   if (!removedTransmitters.empty())
   {
      emit TransmitterRemoved(removedTransmitters);
   }
   if (!addedReceivers.empty())
   {
      emit ReceiverAdded(addedReceivers);
   }
   if (!removedReceivers.empty())
   {
      emit ReceiverRemoved(removedReceivers);
   }
   if (!addedMovers.empty())
   {
      emit MoverAdded(addedMovers);
   }
   if (!removedMovers.empty())
   {
      emit MoverRemoved(removedMovers);
   }

   mPlatforms = newPlatforms;
   mWeaponPlatforms = newWeaponPlatforms;
   mSensors = newSensors;
   mTransmitters = newTransmitters;
   mReceivers = newReceivers;
   mEndTime = endTime;
   mMovers = newMovers;
}

void PostProcessor::ProxyInterface::GetDiff(QVector<PlatformPart>& aNew,
                                            QVector<PlatformPart>& aOld,
                                            QVector<PlatformPart>& aAdded,
                                            QVector<PlatformPart>& aRemoved,
                                            bool                   aAppend)
{
   // Check to see if any platform parts were added/removed ---------------------------------------------------------------
   std::sort(aNew.begin(), aNew.end());
   std::sort(aOld.begin(), aOld.end());

   if (!aAppend)
   {
      aAdded.clear();
      aRemoved.clear();
   }

   auto it_n = aNew.begin(); // new platform parts
   auto it_o = aOld.begin(); // old platform parts

   while (it_n != aNew.end() && it_o != aOld.end())
   {
      if (*it_o < *it_n)
      {
         aRemoved.push_back(*it_o);
         ++it_o;
      }
      else if (*it_n < *it_o)
      {
         aAdded.push_back(*it_n);
         ++it_n;
      }
      else
      {
         ++it_o;
         ++it_n;
      }
   }

   while (it_n != aNew.end())
   {
      aAdded.push_back(*it_n);
      ++it_n;
   }
   while (it_o != aOld.end())
   {
      aRemoved.push_back(*it_o);
      ++it_o;
   }
}

void PostProcessor::ProxyInterface::GetDiff(QVector<Platform>&    aNew,
                                            QVector<Platform>&    aOld,
                                            QMap<Platform, bool>& aAdded,
                                            QVector<Platform>&    aRemoved,
                                            bool                  aAppend,
                                            bool                  aIsWeapon)
{
   std::sort(aNew.begin(), aNew.end());
   std::sort(aOld.begin(), aOld.end());

   if (!aAppend)
   {
      aAdded.clear();
      aRemoved.clear();
   }

   auto it_n = aNew.begin(); // new platforms
   auto it_o = aOld.begin(); // old platforms

   while (it_n != aNew.end() && it_o != aOld.end())
   {
      if (*it_o < *it_n)
      {
         aRemoved.push_back(*it_o);
         ++it_o;
      }
      else if (*it_n < *it_o)
      {
         aAdded.insert(*it_n, !aIsWeapon);
         ++it_n;
      }
      else
      {
         ++it_o;
         ++it_n;
      }
   }

   while (it_n != aNew.end())
   {
      aAdded.insert(*it_n, !aIsWeapon);
      ++it_n;
   }
   while (it_o != aOld.end())
   {
      aRemoved.push_back(*it_o);
      ++it_o;
   }
}
