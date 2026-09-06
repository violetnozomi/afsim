// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "WsfWeaponServer.hpp"

#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "Dis.hpp"
#include "DisData.hpp"
#include "DisIO.hpp"
#include "GenBufIManaged.hpp"
#include "GenHostName.hpp"
#include "GenIConvertBigEndian.hpp"
#include "GenInternetSocketAddress.hpp"
#include "GenSocket.hpp"
#include "GenTCP_IO.hpp"
#include "PakSocketReactor.hpp"
#include "PakTCP_Server.hpp"
#include "UtCast.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtSleep.hpp"
#include "UtSphericalEarth.hpp"
#include "UtStringUtil.hpp"
#include "WsfAirToAirLaunchComputer.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfCommandChain.hpp"
#include "WsfDisObserver.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMil.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfProcessor.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfUtil.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"
#include "WsfWeaponTypes.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisSetData.hpp"
#include "dis/WsfDisWeaponEvents.hpp"
#include "ext/WsfExtInterface.hpp"

// Link-16 interface
#ifdef WSF_WITH_L16
#include "ComputerProcessor.hpp"
#include "Interface.hpp"
#include "Interface_J11.hpp"
#endif

WsfWeaponServer::PostLaunchCallback WsfWeaponServer::PostLaunch;

// ============================================================================
void WSF_WEAPON_SERVER_EXPORT Register_wsf_weapon_server(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_weapon_server"))
   {
      aApplication.RegisterExtension("wsf_weapon_server",
                                     ut::make_unique<WsfDefaultApplicationExtension<WsfWeaponServerExtension>>());
      aApplication.RegisterFeature("weapon_server", "wsf_weapon_server"); // Indicate the feature is present
      aApplication.ExtensionDepends("wsf_weapon_server", "wsf_mil", true);
      aApplication.ExtensionDepends("wsf_weapon_server", "dis_interface", true);
#ifdef WSF_WITH_L16
      // If L16 is present then it must occur before this.
      aApplication.ExtensionDepends("wsf_weapon_server", "wsf_l16", true);
#endif
   }
}

// ============================================================================
WsfWeaponServer* WsfWeaponServer::Find(const WsfSimulation& aSimulation)
{
   return static_cast<WsfWeaponServer*>(aSimulation.FindExtension("wsf_weapon_server"));
}

// ============================================================================
// This is a callback method invoked by the Link-16 interface.
// This calls plugs the address of this method into WsfL16::Interface, which will invoke this
// method via the pointer when it wants the track numbers. This removes the dependence of the
// Link-16 interface
// static
void WsfWeaponServer::GetWeaponTrackNumbers(WsfSimulation& aSimulation,
                                            size_t         aWeaponPlatformIndex,
                                            unsigned int&  aWeaponTrackNumber,
                                            unsigned int&  aControllerTrackNumber)
{
   WsfWeaponServer* serverPtr = Find(aSimulation);
   if (serverPtr != nullptr) // should always be true, but just in case...
   {
      serverPtr->GetWeaponServerNumbers(aWeaponPlatformIndex, aWeaponTrackNumber, aControllerTrackNumber);
   }
}

// ============================================================================
WsfWeaponServer::WsfWeaponServer(WsfWeaponServerInput& aScenario,
                                 WsfSimulation*        aSimulationPtr,
                                 WsfDisInterface*      aInterfacePtr)
   : // TODO Make Thread-safe WsfThread(),
   WsfWeaponServerInput(aScenario)
   , mWeaponServerRequest(false)
   , mWeaponDisidOverride(false)
   , mWeaponIdOverride(false)
   , mUseWpnIdInPDUs(false)
   , mWarningStringSet(false)
   , mServer(nullptr)
   , mConnections()
   , mCurrentConnection(nullptr)
   , mAtm(aSimulationPtr->GetScenario().GetAtmosphere())
   , mShooterIdMap()
   , mControllerTrackNumberMap()
   , mWeaponTrackNumberMap()
   , mPlatformIndexMap()
   , mCurrentShooterId()
   , mCurrentWeaponId()
   , mCurrentControllerTrackNumber(0)
   , mCurrentWeaponTrackNumber(0)
   , mCurrentFrequency(0.0)
   , mCurrentWpnId(0)
   , mOutgoingClientConnection(nullptr)
   , mOutgoingWeaponTrackNumbers(0)
   , mCallbacks()
   , mDisInterfacePtr(aInterfacePtr)
   , mSimulationPtr(aSimulationPtr)
   , mWeaponPlatformPtr(nullptr)
{
   // TODO Make Thread-safe SetSleepTime(0.25);
}

// ============================================================================
// virtual
WsfWeaponServer::~WsfWeaponServer()
{
   if (mServer != nullptr)
   {
      delete mServer;
      mServer = nullptr;
   }

   if (mOutgoingClientConnection != nullptr)
   {
      delete mOutgoingClientConnection;
      mOutgoingClientConnection = nullptr;
   }
}

// NOTE: Weapon server is not thread-safe, pulling out code until this can be worked.
//       Calling an event to poll the TCP/IP connection
void WsfWeaponServer::AddedToSimulation()
{
   if ((mWeaponServerIncluded) && (mHostTrue_ClientFalse))
   {
      auto tcpUpdateEventPtr         = ut::make_unique<TCPUpdateEvent>();
      tcpUpdateEventPtr->mUpdateTime = 0.05;
      tcpUpdateEventPtr->SetTime(mSimulationPtr->GetSimTime() + 0.05);
      tcpUpdateEventPtr->mWeaponServerPtr = this;
      mSimulationPtr->AddEvent(std::move(tcpUpdateEventPtr));
   }
}

// ============================================================================
//! Called from the main thread to initialize the thread.
bool WsfWeaponServer::Initialize()
{
   assert(mWeaponServerIncluded); // Shouldn't get here otherwise

#ifdef WSF_WITH_L16
   // Plug the address of our method that returns the weapon and controller track numbers for
   // the Link-16 interface.

   WsfL16::Interface* l16_InterfacePtr = dynamic_cast<WsfL16::Interface*>(GetSimulation().FindExtension("wsf_l16"));
   if (l16_InterfacePtr != nullptr)
   {
      l16_InterfacePtr->SetWeaponTrackNumbersFunction(GetWeaponTrackNumbers);
   }
   else
   {
      ut::log::error() << "WSF Weapon Server: Could not find l16 interface.";
   }
#endif
   // if this is configured as a host, setup the PakTCP_Server to listen for connections
   // else this is configured as a client, setup the outgoing connection to prepare for sending commands out
   if (mHostTrue_ClientFalse)
   {
      if (mServer == nullptr)
      {
         mServer = new PakTCP_Server;
         mServer->Init(mPort, true);
      }
      if (mServer == nullptr)
      {
         auto out = ut::log::error() << "WSF Weapon Server: TCP connection for port.";
         out.AddNote() << "WSF Weapon Server not started.";
         out.AddNote() << "Port: " << mPort;
         return false;
      }

      // NOTE: Weapon server is not thread-safe, pulling out code until this can be worked.
      // TODO:WsfThread::Start();             // Create the actual thread and start it working
      // TODO:AssignWork();        //sets the thread's function to "assigned" so that dowork() is called
      InitiateCallbacks();
   }
   else
   {
      if (!mOutGoingHostName.empty())
      {
         mOutgoingClientConnection = new GenTCP_IO();
         if (!mOutgoingClientConnection->Init(mOutGoingHostName, mOutGoingPort))
         {
            delete mOutgoingClientConnection;
            mOutgoingClientConnection = nullptr;
            auto out = ut::log::error() << "WSF Weapon Server: Could not init outgoing client connection.";
            out.AddNote() << "Host Name: " << mOutGoingHostName;
            out.AddNote() << "Port: " << mOutGoingPort;
            out.AddNote() << "WSF Weapon Server still started in case connection is possible later.";
         }

         // this is our one special hook into the simulation (used this way when functioning as a weapon server client
         // for outgoing weapon commands)
         mCallbacks.Add(
            WsfObserver::WeaponPlatformPendingAdd(&GetSimulation()).Connect(&WsfWeaponServer::SendToOtherWeaponServer, this));
      }
      else
      {
         auto out = ut::log::error() << "WSF Weapon Server: Confusion about type of server (incoming or outgoing).";
         out.AddNote() << "Please define PORT or OUTGOING_WEAPON_SERVER.";
         return false;
      }
   }
   return true;
}

void WsfWeaponServer::InitiateCallbacks()
{
   // this is our one special hook into the simulation (used this way when functioning as a weapon server host for
   // recording weapon data on fire)
   mCallbacks.Add(WsfObserver::PlatformInitialized(&GetSimulation()).Connect(&WsfWeaponServer::PlatformInitialized, this));
   mCallbacks.Add(WsfObserver::PlatformAdded(&GetSimulation()).Connect(&WsfWeaponServer::PlatformAdded, this));
   mCallbacks.Add(WsfObserver::PlatformDeleted(&GetSimulation()).Connect(&WsfWeaponServer::PlatformDeleted, this));
   mCallbacks.Add(WsfObserver::DisSetDataReceived(&GetSimulation()).Connect(&WsfWeaponServer::HandleSetDataPDU, this));
}

WsfSimulation& WsfWeaponServer::GetSimulation()
{
   if (mSimulationPtr == nullptr)
   {
      return WsfSimulationExtension::GetSimulation();
   }
   else
   {
      return *mSimulationPtr;
   }
}

// virtual
bool WsfWeaponServerExtension::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "wsf_weapon_server")
   {
      mWeaponServerIncluded = true;
      myCommand             = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         std::transform(command.begin(), command.end(), command.begin(), ::tolower);
         if (command == "debug")
         {
            mDebugEnabled = true;
         }
         else if (command == "outgoing_weapon_server")
         {
            aInput.ReadValue(mOutGoingHostName);
            aInput.ReadValue(mOutGoingPort);
            mHostTrue_ClientFalse = false; // used as client (for outgoing commands), not host
         }
         else if (command == "outgoing_weapon_type")
         {
            std::string weaponSystemType;
            aInput.ReadValue(weaponSystemType);
            mOutgoingWeaponTypes.emplace_back(weaponSystemType);
         }
         else if (command == "host") // typically the local IP of the machine
         {
            aInput.ReadValue(mHostName);
         }
         else if (command == "port")
         {
            int port;
            aInput.ReadValue(port); // port #
            mPort                 = ut::safe_cast<unsigned int, int>(port);
            mHostTrue_ClientFalse = true;
         }
         else if (command == "rmax")
         {
            aInput.ReadValue(mTestRMax);
         }
         else if (command == "rmax_tof")
         {
            aInput.ReadValue(mTestRMaxTOF);
         }
         else if (command == "rne")
         {
            aInput.ReadValue(mTestRNe);
         }
         else if (command == "rne_tof")
         {
            aInput.ReadValue(mTestRNeTOF);
         }
         else if (command == "rmin")
         {
            aInput.ReadValue(mTestRMin);
         }
         else if (command == "rmin_tof")
         {
            aInput.ReadValue(mTestRMinTOF);
         }
         else if (command == "use_test_lar_data")
         {
            aInput.ReadValue(mUseTestLARData);
         }
         else if (command == "lar_msg_type_id_rcv")
         {
            aInput.ReadValue(mLarMsgTypeIdIn);
         }
         else if (command == "lar_msg_type_id_send")
         {
            aInput.ReadValue(mLarMsgTypeIdOut);
         }
         else if (command == "rpi_multiplier")
         {
            aInput.ReadValue(mRpiMultiplier);
         }
         else if (command == "tn_to_disid_mapping")
         {
            std::string  cmdMapping;
            UtInputBlock mapBlock(aInput);
            while (mapBlock.ReadCommand(cmdMapping))
            {
               UtStringUtil::ToLower(cmdMapping);
               if (cmdMapping == "tn_to_disid")
               {
                  int         tn       = 0;
                  std::string disIdStr = "";
                  aInput.ReadValue(tn);
                  aInput.ReadValue(disIdStr);
                  std::vector<std::string> disIdParts;
                  UtStringUtil::Parse(disIdStr, disIdParts, ":");
                  if (disIdParts.size() != 3)
                  {
                     throw UtInput::BadValue(mapBlock.GetInput(),
                                             "Incorrectly formatted disid - expect <TN site:app:entity> entry");
                  }
                  int         site   = (int)atoi(disIdParts[0].c_str());
                  int         app    = (int)atoi(disIdParts[1].c_str());
                  int         entity = (int)atoi(disIdParts[2].c_str());
                  DisEntityId controllerId((DisUint16)site, (DisUint16)app, (DisUint16)entity);
                  mTrackNumberToDisEntityIdMap[(unsigned int)tn] = controllerId;
               }
               else
               {
                  throw UtInput::UnknownCommand(mapBlock.GetInput());
               }
            } // end map while loop
         }
         // set up weapon type mapping for external entities
         else if (command == "weapon_lar_mapping")
         {
            std::string  cmdMapping;
            UtInputBlock mapBlock(aInput);
            while (mapBlock.ReadCommand(cmdMapping))
            {
               UtStringUtil::ToLower(cmdMapping);
               if (cmdMapping == "enum_to_type")
               {
                  int         theirWeaponEnum = 0;
                  std::string ourWeaponName   = "";
                  aInput.ReadValue(theirWeaponEnum);
                  aInput.ReadValue(ourWeaponName);
                  mWeaponEnumToWeaponType[theirWeaponEnum] = WsfStringId(ourWeaponName);
               }
               else
               {
                  throw UtInput::UnknownCommand(mapBlock.GetInput());
               }
            } // end map while loop
         }
         // set up weapon type mapping for external entities
         else if (command == "weapon_mapping")
         {
            // input can either be a mapping of an external application weapon name
            // to an WSF weapon name.  All external applications will use the same mapping.
            // 'name_in_wpn_type_data_field' is the data field input associated with the keyword, 'wpn_type='.
            //
            // weapon_name_mapping <name_in_wpn_type_data_field> <wsf_weapon_system_name>
            //
            // OR
            // In the case, there are crew stations firing one of the WSF weapons off of a
            // station, multiple crew stations may have different load outs.  In that case,
            // set up the mapping on a per platform per station mapping.
            // The station_name value is the name that comes over in data field of the
            // wpn_type=<data> key/data pairing.
            //
            // platform
            //   disid <site:app:entity>  // Assumes a DIS connection
            //      OR
            //   name <name>
            //   station_mapping <name_in_wpn_type_data_field> <wsf_weapon_system_name>
            //   station_mapping <name_in_wpn_type_data_field> <wsf_weapon_system_name>
            //    ...
            // end_platform
            // platform
            //   # do NOT use the disid option for an internal WSF platform, use name option.
            //   disid <site:app:entity>  // Assumes a DIS connection; assumes the platform is external
            //      OR
            //   name <name>  // assumes an internal WSF player, but can be used for an external body if
            //                // the name is in this format: <dis>site:app:entity
            //   station_mapping <name_in_wpn_type_data_field> <wsf_weapon_system_name>
            //   station_mapping <name_in_wpn_type_data_field> <wsf_weapon_system_name>
            //    ...
            // end_platform
            //

            std::string  cmdMapping;
            UtInputBlock mapBlock(aInput);
            while (mapBlock.ReadCommand(cmdMapping))
            {
               UtStringUtil::ToLower(cmdMapping);
               if (cmdMapping == "weapon_name_mapping")
               {
                  std::string theirWeaponName = "";
                  std::string ourWeaponName   = "";
                  aInput.ReadValue(theirWeaponName);
                  aInput.ReadValue(ourWeaponName);
                  UtStringUtil::ToLower(theirWeaponName);
                  mWeaponNameMapTheirsToOurs[theirWeaponName] = ourWeaponName;
               }
               else if (cmdMapping == "lar")
               {
                  unsigned int theirWeaponEnum = 0;
                  std::string  ourWeaponName   = "";
                  aInput.ReadValue(theirWeaponEnum);
                  aInput.ReadValue(ourWeaponName);
                  mWeaponEnumToWeaponType[theirWeaponEnum] = ourWeaponName;
               }
               else if (cmdMapping == "platform")
               {
                  WsfStringId  platformNameId;
                  std::string  platformName     = "";
                  std::string  theirStationName = "";
                  std::string  ourWeaponName    = "";
                  std::string  cmdPlatform;
                  UtInputBlock platformBlock(aInput);
                  while (platformBlock.ReadCommand(cmdPlatform))
                  {
                     UtStringUtil::ToLower(cmdPlatform);
                     if (cmdPlatform == "disid")
                     {
                        std::string platformDisId;
                        aInput.ReadValue(platformDisId);
                        std::vector<std::string> SiteAppEnt;
                        if (3 != UtStringUtil::Parse(platformDisId, SiteAppEnt, ':'))
                        {
                           mErrorString = "bad format for DISID parameter, expected DISID site:app:entity";
                           mErrorString += " not ";
                           mErrorString += platformDisId;
                           throw UtInput::BadValue(platformBlock.GetInput(), mErrorString);
                        }
                        // This platform is assumed to be an external entity.  DIS may not have been setup yet in
                        // the input files, so the site and application numbers for this WSF application may not yet be
                        // set. Any comparisons to Dis::Site and Dis::Application may not be valid.
                        platformName = "<dis>";
                        platformName += platformDisId;
                        platformNameId = platformName;
                     }
                     else if (cmdPlatform == "name")
                     {
                        aInput.ReadValue(platformName);
                        platformNameId = platformName;
                     }
                     else if (cmdPlatform == "station_mapping")
                     {
                        aInput.ReadValue(theirStationName);
                        aInput.ReadValue(ourWeaponName);

                        WsfStringId                         stationNameId   = theirStationName;
                        WsfStringId                         ourWeaponNameId = ourWeaponName;
                        std::pair<WsfStringId, WsfStringId> platformStationPair(platformNameId, stationNameId);
                        mPlatformStationToOurWeaponName[platformStationPair] = ourWeaponNameId;
                     }
                     else
                     {
                        throw UtInput::UnknownCommand(platformBlock.GetInput());
                     }
                  } // end platform while block
               }
               else
               {
                  throw UtInput::UnknownCommand(mapBlock.GetInput());
               }
            } // end map while loop
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

WsfWeaponServerExtension::WsfWeaponServerExtension() {}

void WsfWeaponServerExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   if (!mWeaponServerIncluded)
   {
      return;
   }
   WsfDisInterface& disInterface = dynamic_cast<WsfDisInterface&>(aSimulation.GetExtension("dis_interface"));
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfWeaponServer>(*this, &aSimulation, &disInterface));
}

bool WsfWeaponServer::IsConnected()
{
   if (!mServer)
   {
      return false;
   }
   // return mServer->IsConnected();
   return mServer->ConnectionCount() > 0;
}

bool WsfWeaponServer::ProcessCommand(const std::string& command)
{
   if (mDebugEnabled == true)
   {
      auto out = ut::log::debug() << "WSF Weapon Server: Processing command.";
      out.AddNote() << "Command: " << command;
   }

   // split the received command into a vector of strings (1st one is command, rest are parameters)
   std::vector<std::string> pieces;
   UtStringUtil::Parse(command, pieces, ' ');
   if (pieces.empty())
   {
      mErrorString = "unrecognized command";
      auto out     = ut::log::error() << "WSF Weapon Server: Unrecognized command.";
      out.AddNote() << "Command: " << command;
      return false;
   }

   std::string commandName = pieces[0];
   UtStringUtil::ToUpper(commandName);
   if (commandName == "START")
   {
      if (mDebugEnabled == true)
      {
         ut::log::debug() << "WSF Weapon Server: Started.";
      }
      return true;
   }
   else if (commandName == "STOP")
   {
      if (mDebugEnabled == true)
      {
         ut::log::debug() << "WSF Weapon Server: Stopped.";
      }
      return true;
   }
   else if (commandName == "RELEASE_STORE")
   {
      // weapon server: fire weapon parameters
      // shooter
      //  reset any old error messages
      { // RAII block
         auto out = ut::log::info() << "RELEASE_STORE RECEIVED:";
         out.AddNote() << "Command: " << command;
      }
      mErrorString      = "";
      mWarningString    = "";
      mWarningStringSet = false;

      IdentifierType ShooterIdType = BY_NAME;
      WsfStringId    shooterPlatformId;
      DisEntityId    shooterDisId;
      int            weaponControllerNumber = -1;
      // weapon
      std::string    weaponType       = "MISSILE"; // MUST BE over-written
      int            weaponTailNumber = -1;
      int            mid              = -1;
      bool           useTailNumPdus   = false;
      IdentifierType WeaponIdType     = NO_IDENTIFIER;
      DisEntityId    weaponDisId;
      int            launchType             = 0; // 0=air, 1=ground
      bool           bSuppressDetonationPDU = false;
      bool           bInfinite              = false;
      // target
      // IdentifierType TargetIdType = NO_IDENTIFIER;
      int          TargetIdType = NO_IDENTIFIER;
      WsfStringId  targetPlatformId;
      DisEntityId  targetDisId;
      double       targetLat = -9999999.9; // valid: [-90, 90]
      double       targetLon = -9999999.9; // valid: [-180, 180]
      double       targetAlt = 0.0;        // default is zero
      std::string  targetTag = "";
      double       freq      = 0;
      double       posn      = 0;
      double       pose      = 0;
      double       posd      = 0;
      double       veln      = 0;
      double       vele      = 0;
      double       veld      = 0;
      unsigned int bValidId  = 0;

      // count number of parameters (expect 3 at minimum)
      int N = (int)pieces.size();
      if (N < 4) // take off 1 for the command
      {
         mErrorString = "not enough parameters received for RELEASE_STORE command";
         auto out = ut::log::error() << "WSF Weapon Server: Not enough parameters received for RELEASE_STORE command.";
         out.AddNote() << "Received: " << N;
         out.AddNote() << "Expected: 4";
         return false;
      }
      // iterate over the parameters to understand the command
      for (int iter = 1; iter < N; ++iter)
      {
         // bool explodeOnImpact = false;
         std::string              parameter = pieces[iter];
         std::vector<std::string> KeyValuePair;
         // split each parameter string into its key value pair (key=value)
         if (2 != UtStringUtil::Parse(parameter, KeyValuePair, '='))
         {
            // mErrorString = "bad parameter format, expected key=value pair";
            mWarningString =
               ";bad parameter format;expected key=value pair;check for extra spaces between keyword/value pairs";
            auto out = ut::log::warning() << "WSF Weapon Server: Bad parameter format.";
            out.AddNote() << "Received: " << parameter;
            out.AddNote() << "Expected: key=value";
            out.AddNote() << "Check for extra spaces between keyword/value pairs.";
            mWarningStringSet = true;
            continue;
            // return false;
         }
         std::string key = KeyValuePair[0];
         std::string val = KeyValuePair[1];
         UtStringUtil::ToUpper(key);
         if (key == "NAMEID")
         {
            shooterPlatformId = val;
            ShooterIdType     = BY_NAME;
         }
         else if (key == "DISID")
         {
            std::vector<std::string> SiteAppEnt;
            if (3 != UtStringUtil::Parse(val, SiteAppEnt, ':'))
            {
               mErrorString = "bad format for DISID parameter, expected DISID=site:app:entity";
               auto out     = ut::log::error() << "WSF Weapon Server: Bad format for DISID parameter.";
               out.AddNote() << "Received: " << val;
               out.AddNote() << "Expected: site:app:entity";
               return false;
            }
            DisUint16 site = (DisUint16)UtStringUtil::ToInt(SiteAppEnt[0]);
            DisUint16 app  = (DisUint16)UtStringUtil::ToInt(SiteAppEnt[1]);
            DisUint16 ent  = (DisUint16)UtStringUtil::ToInt(SiteAppEnt[2]);
            shooterDisId   = DisEntityId(site, app, ent);
            ShooterIdType  = BY_DIS_ID;
         }
         else if (key == "WPN_TYPE")
         {
            weaponType = val;
            if (!mWeaponNameMapTheirsToOurs.empty())
            {
               std::string lowerWeaponType = weaponType;
               UtStringUtil::ToLower(lowerWeaponType);
               WsfStringId lowerWeaponTypeId(lowerWeaponType);
               auto        iter = mWeaponNameMapTheirsToOurs.find(lowerWeaponType);
               if (iter != mWeaponNameMapTheirsToOurs.end())
               {
                  // mapping from an external weapon type to our weapon type was found.
                  WsfStringId weaponTypeId = (*iter).second;
                  // use our weapon type name
                  weaponType = weaponTypeId;

                  auto out = ut::log::info() << "WSF Weapon Server: Sent weapon type to WSF weapon type.";
                  out.AddNote() << "Weapon Type: " << val;
                  out.AddNote() << "WSF Weapon Type: " << weaponType;
               }
            }
         }
         else if (key == "WPN_TAIL_NUMBER" || key == "WPN_ID")
         {
            if (val[0] == 'o' || val[0] == 'O') // octal value
            {
               weaponTailNumber = DecimalToOctal(UtStringUtil::ToInt(val.substr(1)));
            }
            else // decimal value
            {
               weaponTailNumber = UtStringUtil::ToInt(val);
            }
            mWeaponIdOverride = true;
         }
         else if (key == "USETAILNUMINPDUS")
         {
            UtStringUtil::ToUpper(val);
            if (val == "TRUE")
            {
               useTailNumPdus = true; // this overwrites WPN_DISID if weaponTailNumber is set, check at end of parsing
            }
         }
         else if (key == "WPN_DISID")
         {
            std::vector<std::string> SiteAppEnt;
            if (3 != UtStringUtil::Parse(val, SiteAppEnt, ':'))
            {
               mErrorString = "bad format for WPN_DISID parameter, expected WPN_DISID=site:app:entity";
               auto out     = ut::log::error() << "WSF Weapon Server: Bad format for WPN_DISID parameter.";
               out.AddNote() << "Received: " << val;
               out.AddNote() << "Expected: site:app:entity";
               return false;
            }
            DisUint16 site       = (DisUint16)UtStringUtil::ToInt(SiteAppEnt[0]);
            DisUint16 app        = (DisUint16)UtStringUtil::ToInt(SiteAppEnt[1]);
            DisUint16 ent        = (DisUint16)UtStringUtil::ToInt(SiteAppEnt[2]);
            weaponDisId          = DisEntityId(site, app, ent);
            WeaponIdType         = BY_DIS_ID; // check at end of parsing if (useTailNumPdus==true && weaponTailNumber>0)
            mWeaponDisidOverride = true;
         }
         else if (key == "LCH_TYPE")
         {
            launchType = UtStringUtil::ToInt(val);
            launchType = (launchType == 1) ? (1) : (0); // only a value of 1 can break the default of zero
         }
         else if (key == "TARGET_DISID")
         {
            std::vector<std::string> SiteAppEnt;
            if (3 != UtStringUtil::Parse(val, SiteAppEnt, ':'))
            {
               mErrorString = "bad format for TARGET_DISID parameter, expected TARGET_DISID=site:app:entity";
               auto out     = ut::log::error() << "WSF Weapon Server: Bad format for TARGET_DISID parameter.";
               out.AddNote() << "Received: " << val;
               out.AddNote() << "Expected: site:app:entity";
               return false;
            }
            DisUint16 site = (DisUint16)UtStringUtil::ToInt(SiteAppEnt[0]);
            DisUint16 app  = (DisUint16)UtStringUtil::ToInt(SiteAppEnt[1]);
            DisUint16 ent  = (DisUint16)UtStringUtil::ToInt(SiteAppEnt[2]);
            targetDisId    = DisEntityId(site, app, ent);
            TargetIdType |= BY_DIS_ID;
         }
         else if (key == "TARGET_NAMEID")
         {
            targetPlatformId = val;
            TargetIdType |= BY_NAME;
            // WsfPlatform* targetPlatformPtr = GetSimulation().GetPlatformByName(targetPlatformId);
         }
         else if (key == "LAT")
         {
            targetLat = UtStringUtil::ToDouble(val);
            // do this valid location check here, because target id type could be overwritten by later parameters
            if (targetLat >= -90 && targetLat <= 90)
            {
               TargetIdType |= BY_LAT;
            }
         }
         else if (key == "LON")
         {
            targetLon = UtStringUtil::ToDouble(val);
            // do this valid location check here, because target id type could be overwritten by later parameters
            if (targetLon >= -180 && targetLon <= 180)
            {
               TargetIdType |= BY_LON;
            }
         }
         else if (key == "ALT")
         {
            // ASCII Interface defines altitude in feet
            targetAlt = UtStringUtil::ToDouble(val) * UtMath::cM_PER_FT;
         }
         else if (key == "MID")
         {
            if (val[0] == 'o' || val[0] == 'O') // octal value
            {
               mid = DecimalToOctal(UtStringUtil::ToInt(val.substr(1)));
            }
            else // decimal value
            {
               mid = UtStringUtil::ToInt(val);
            }
            mCurrentWpnId = mid;
            bValidId |= cMID;
         }
         else if (key == "FREQ")
         {
            freq              = UtStringUtil::ToDouble(val);
            mCurrentFrequency = freq;
            bValidId |= cFREQ;
         }
         else if (key == "POSN")
         {
            posn = UtStringUtil::ToDouble(val);
            bValidId |= cPOSN;
         }
         else if (key == "POSE")
         {
            pose = UtStringUtil::ToDouble(val);
            bValidId |= cPOSE;
         }
         else if (key == "POSD")
         {
            posd = UtStringUtil::ToDouble(val);
            bValidId |= cPOSD;
         }
         else if (key == "VELN")
         {
            veln = UtStringUtil::ToDouble(val);
            bValidId |= cVELN;
         }
         else if (key == "VELE")
         {
            vele = UtStringUtil::ToDouble(val);
            bValidId |= cVELE;
         }
         else if (key == "INFINITE_COUNT")
         {
            std::string countStr = val;
            UtStringUtil::ToLower(countStr);
            if (countStr == "false")
            {
               bInfinite = false;
            }
            else if (countStr == "true")
            {
               bInfinite = true;
            }
            else
            {
               mErrorString = "Invalid input for keyword, infinite_count";
               return false;
            }
         }
         else if (key == "VELD")
         {
            veld = UtStringUtil::ToDouble(val);
            bValidId |= cVELD;
         }
         else if (key == "TAG")
         {
            targetTag = val;
            // if (targetLat >=  -90 && targetLat <= 90 &&
            //    targetLon >= -180 && targetLon <= 180 &&
            //    targetTag != "")
            //{
            //    TargetIdType |= BY_LOCATION;
            // }
         }
         else if (key == "CTRL_TRACK_NUM")
         {
            if (val[0] == 'o' || val[0] == 'O') // octal value
            {
               weaponControllerNumber = DecimalToOctal(UtStringUtil::ToInt(val.substr(1)));
            }
            else // decimal value
            {
               weaponControllerNumber = UtStringUtil::ToInt(val);
            }
         }
         else if (key == "IMPACT_VALUE")
         {
            UtStringUtil::ToUpper(val);
            if (val == "TRUE")
            {
               // explodeOnImpact = true;
            }
            else
            {
               // explodeOnImpact = false;
               bSuppressDetonationPDU = true;
            }
         }
         else if (key == "NON_IMPACT_VALUE")
         {
            UtStringUtil::ToUpper(val);
            // if (val == "TRUE")
            //{
            //    explodeOnImpact = false;
            // }
         }
         else
         {
            mErrorString = "unknown parameter, ";
            mErrorString += key;
            auto out = ut::log::error() << "WSF Weapon Server: Unknown parameter.";
            out.AddNote() << "Parameter: " << key;
            return false;
         }
      }

      if (useTailNumPdus == true && weaponTailNumber >= 0)
      {
         WeaponIdType = BY_TAIL_NUMBER;
      }

      if (mDebugEnabled == true)
      {
         auto out = ut::log::debug() << "WSF Weapon Server processed RELEASE_STORE command.";
         switch (ShooterIdType)
         {
         case BY_DIS_ID:
         {
            out.AddNote() << "Shooter DIS ID: " << shooterDisId;
         }
         break;
         case BY_NAME:
         default:
         {
            out.AddNote() << "Shooter: " << shooterPlatformId;
         }
         break;
         };

         if (TargetIdType & BY_DIS_ID)
         {
            out.AddNote() << "Target DIS ID: " << targetDisId;
         }
         if (TargetIdType & BY_NAME)
         {
            out.AddNote() << "Target: " << targetPlatformId;
         }
         if (TargetIdType & BY_LOCATION)
         {
            out.AddNote() << "Target LLA: " << targetLat << ", " << targetLon << ", " << targetAlt;
            //<< "Tag = " << targetTag << endl;
         }
         if (TargetIdType == NO_IDENTIFIER)
         {
            out.AddNote() << "Target not given.";
         }

         switch (WeaponIdType)
         {
         case BY_DIS_ID:
         {
            out.AddNote() << "Weapon DIS ID: " << weaponDisId;
         }
         break;
         case BY_TAIL_NUMBER:
         {
            out.AddNote() << "Weapon Tail Number: " << weaponTailNumber;
         }
         break;
         case NO_IDENTIFIER:
         default:
         {
            out.AddNote() << "Weapon: NO IDENTIFIER given (optional)";
         }
         };
         out.AddNote() << "Weapon Type: " << weaponType;
         out.AddNote() << "Launched At: " << ((launchType == 0) ? "air" : "ground");
      }
      double simTime = GetSimulation().GetSimTime();
      // get the shooter from the simulation
      WsfPlatform* shooterPlatformPtr = nullptr;
      if (ShooterIdType == BY_DIS_ID)
      {
         WsfDisPlatform* shooterDisPtr = mDisInterfacePtr->FindDisPlatform(shooterDisId);
         if (shooterDisPtr != nullptr)
         {
            shooterPlatformPtr = shooterDisPtr->GetPlatform();
         }
      }
      else if (ShooterIdType == BY_NAME)
      {
         shooterPlatformPtr = GetSimulation().GetPlatformByName(shooterPlatformId);
      }
      if (shooterPlatformPtr == nullptr)
      {
         mErrorString = "shooter platform not found";
         ut::log::error() << "WSF Weapon Server: Shooter platform not found.";
         return false;
      }
      // Check to see if there is station mapping
      if (!mPlatformStationToOurWeaponName.empty())
      {
         std::string lowerWeaponType = weaponType;
         UtStringUtil::ToLower(lowerWeaponType);
         WsfStringId                         weaponTypeNameId = lowerWeaponType;
         std::pair<WsfStringId, WsfStringId> shooterStationPair(shooterPlatformId, weaponTypeNameId);
         std::map<std::pair<WsfStringId, WsfStringId>, WsfStringId>::iterator iter =
            mPlatformStationToOurWeaponName.find(shooterStationPair);
         if (iter != mPlatformStationToOurWeaponName.end())
         {
            // mapping from an external weapon type to our weapon type was found.
            WsfStringId weaponTypeId = (*iter).second;
            // use our weapon type name
            weaponType = weaponTypeId;
            auto out   = ut::log::info() << "WSF Weapon Server: Sent station name for WSF weapon type.";
            out.AddNote() << "Station Name: " << lowerWeaponType;
            out.AddNote() << "WSF Weapon Type: " << weaponType;
         }
      }

      // find or make a track for the given target
      WsfTrack*    targetTrackPtr    = nullptr; // used to make the shot
      WsfPlatform* targetPlatformPtr = nullptr; // used if target identified by name or dis id
      if (TargetIdType & BY_DIS_ID)
      {
         // find the platform & make a track out of it
         WsfDisPlatform* targetDisPtr = mDisInterfacePtr->FindDisPlatform(targetDisId);
         if (targetDisPtr != nullptr)
         {
            targetPlatformPtr = targetDisPtr->GetPlatform();
         }
         if (targetPlatformPtr != nullptr)
         {
            if (targetPlatformPtr->GetIndex() == shooterPlatformPtr->GetIndex())
            {
               mErrorString = "Shooter and target same platform";
               return false;
            }
            targetTrackPtr = new WsfTrack();
            WsfUtil::MakeTrack(simTime, *targetTrackPtr, *targetPlatformPtr, *shooterPlatformPtr);
         }
         else
         {
            if (!mWarningString.empty())
            {
               mWarningString += ";";
            }
            mWarningString += "Releasing weapon against a target no longer in simulation";
            mWarningStringSet = true;
            auto out          = ut::log::warning() << "WSF Weapon Server: Target platform not found from DIS ID.";
            out.AddNote() << "DIS ID: " << targetDisId;
         }
      }
      if ((TargetIdType & BY_NAME) && targetTrackPtr == nullptr)
      {
         targetPlatformPtr = GetSimulation().GetPlatformByName(targetPlatformId);
         if (targetPlatformPtr != nullptr)
         {
            if (targetPlatformPtr->GetIndex() == shooterPlatformPtr->GetIndex())
            {
               mErrorString = "Shooter and target same platform";
               return false;
            }
            targetTrackPtr = new WsfTrack();
            WsfUtil::MakeTrack(simTime, *targetTrackPtr, *targetPlatformPtr, *shooterPlatformPtr);
         }
         else
         {
            if (!mWarningString.empty())
            {
               mWarningString += ";";
            }
            mWarningString += "Releasing weapon against a target no longer in simulation";
            mWarningStringSet = true;
            auto out          = ut::log::warning() << "WSF Weapon Server: Target platform not found from name!";
            out.AddNote() << "Name: " << targetPlatformId;
         }
      }
      if ((TargetIdType & BY_LOCATION) && targetTrackPtr == nullptr)
      {
         // TODO???: use the targetTag (aux data?) later, just using LLA for now
         // lat & lon should be valid at this point
         // make a track much like the  WsfUtil::MakeTrack() method (but without a known target platform, just with locations)
         targetTrackPtr = new WsfTrack();
         targetTrackPtr->SetPlatformOriginator(shooterPlatformPtr);
         targetTrackPtr->Initialize(simTime, shooterPlatformPtr->GetNextTrackId(), GetSimulation());
         double vecWCS[3];
         shooterPlatformPtr->GetLocationWCS(vecWCS);
         targetTrackPtr->SetOriginatorLocationWCS(vecWCS);
         targetTrackPtr->SetLocationLLA(targetLat, targetLon, targetAlt);
         targetTrackPtr->Set3D(true);
         vecWCS[0] = vecWCS[1] = vecWCS[2] = 0;
         targetTrackPtr->SetVelocityWCS(vecWCS);
         // BAR
         targetTrackPtr->SetVelocityValid(false);
         // targetTrackPtr->SetSpatialDomain((launchType == 0) ? (WSF_SPATIAL_DOMAIN_AIR) : (WSF_SPATIAL_DOMAIN_LAND));
         //  BAR
         float        elevM = 0;
         wsf::Terrain terrain(GetSimulation().GetTerrainInterface());
         if (terrain.IsEnabled())
         {
            terrain.GetElevApprox(targetLat, targetLon, elevM);
         }
         if (launchType == 1)
         {
            targetTrackPtr->SetSpatialDomain(WSF_SPATIAL_DOMAIN_LAND);
         }
         else if (targetAlt > (double)elevM)
         {
            // override and set it to air domain if target alt is above terrain
            targetTrackPtr->SetSpatialDomain(WSF_SPATIAL_DOMAIN_AIR);
         }
         else
         {
            targetTrackPtr->SetSpatialDomain(WSF_SPATIAL_DOMAIN_LAND);
         }
         // don't know any truth info (type, side, target index, etc...)
         targetTrackPtr->SetTypeIdValid(false);
         targetTrackPtr->SetSideIdValid(false);
      }
      if (TargetIdType == NO_IDENTIFIER)
      {
         bool bPosNEDEntered = false;
         bool bPosNEDValid   = false;
         if (((bValidId & cPOSN) != 0) && ((bValidId & cPOSE) != 0) && ((bValidId & cPOSD) != 0))
         {
            bPosNEDEntered = true;
            if ((posn != 0.0) || (pose != 0.0) || (posd != 0.0))
            {
               bPosNEDValid = true;
            }
         }
         // There is a case when all 0's are sent, in that case create a point out in front of shooter
         if (bPosNEDEntered)
         {
            if (bPosNEDValid)
            {
               // This is not the way to do this, but for now can use it to get some point out in space.
               targetTrackPtr = CreateATargetPoint(simTime, shooterPlatformPtr, posn, pose, posd);
               if (targetTrackPtr != nullptr)
               {
                  double velNED[3];
                  velNED[0] = veln;
                  velNED[1] = vele;
                  velNED[2] = veld;
                  UtEntity entity;
                  double   lat, lon, alt;
                  shooterPlatformPtr->GetLocationLLA(lat, lon, alt);
                  entity.SetLocationLLA(lat, lon, alt);
                  double velWCS[3];
                  entity.ConvertNEDVectorToWCS(velWCS, velNED);
                  targetTrackPtr->SetVelocityValid(true);
                  targetTrackPtr->SetLLA_LocationValid(true);
                  targetTrackPtr->SetVelocityWCS(velWCS);
               }
            }
            else
            {
               // Create a point out into space and create a track with that location
               targetTrackPtr = CreateATargetPoint(simTime, shooterPlatformPtr, 10.0);
            }
         }
         else
         {
            // Create a point out into space and create a track with that location
            targetTrackPtr = CreateATargetPoint(simTime, shooterPlatformPtr);
         }
         if (targetTrackPtr == nullptr)
         {
            mErrorString = "target not specified";
            ut::log::error() << "WSF Weapon Server: Target not specified.";
            return false;
         }
      }

      // set the target track as the shooter's target
      if (targetTrackPtr != nullptr)
      {
         shooterPlatformPtr->GetTrackManager().SetCurrentTarget(simTime, *targetTrackPtr);
      }
      else
      {
         // mErrorString = "Track was not create for target platform ";
         // cout << "Track was not create for target platform " << endl;

         // OR
         if (((bValidId & cPOSN) != 0) && ((bValidId & cPOSE) != 0) && ((bValidId & cPOSD) != 0))
         {
            // This is not the way to do this, but for now can use it to get some point out in space.
            // This is not the way to do this, but for now can use it to get some point out in space.
            targetTrackPtr = CreateATargetPoint(simTime, shooterPlatformPtr, posn, pose, posd);
            if (targetTrackPtr != nullptr)
            {
               double velNED[3];
               velNED[0] = veln;
               velNED[1] = vele;
               velNED[2] = veld;
               UtEntity entity;
               double   lat, lon, alt;
               shooterPlatformPtr->GetLocationLLA(lat, lon, alt);
               entity.SetLocationLLA(lat, lon, alt);
               double velWCS[3];
               entity.ConvertNEDVectorToWCS(velWCS, velNED);
               targetTrackPtr->SetVelocityValid(true);
               targetTrackPtr->SetLLA_LocationValid(true);
               targetTrackPtr->SetVelocityWCS(velWCS);
            }
         }
         else
         {
            // Create a point out into space and create a track with that location
            targetTrackPtr = CreateATargetPoint(simTime, shooterPlatformPtr);
         }

         // return false;
      }

      WsfWeapon* weaponToShootPtr = nullptr;
      // keep track of actual weapon count on platform
      double wpnCount = -1;
      for (WsfComponentList::RoleIterator<WsfWeapon> iter(*shooterPlatformPtr); !iter.AtEnd(); ++iter)
      {
         if (iter->GetType() == weaponType)
         {
            weaponToShootPtr = *iter;
            // save off the actual weapon count on the platform
            wpnCount = iter->GetQuantityRemaining();
            if (bInfinite)
            {
               // In order for the weapon to be launched its quantity must be > 0.
               // Choosing large value to ensure weapons are always available.
               // Needs to be moved above AddWeapon.  AddWeapon does a quantity check.
               iter->SetQuantityRemaining(100.0);
            }
            break;
         }
      }
      // if this weapon type isn't already defined as part of the platform type, then add it
      if (weaponToShootPtr == nullptr)
      {
         std::unique_ptr<WsfWeapon> instancePtr(WsfWeaponTypes::Get(GetScenario()).Clone(weaponType));
         if (instancePtr.get() != nullptr)
         {
            instancePtr->SetName(weaponType);
            if (shooterPlatformPtr->AddComponent(instancePtr.get()))
            {
               WsfWeapon* weaponPtr = instancePtr.release();
               if (weaponPtr != nullptr)
               {
                  weaponPtr->Initialize(simTime);
                  weaponPtr->SetQuantityRemaining(100.0);
                  weaponPtr->SetPlatform(shooterPlatformPtr);
                  weaponToShootPtr = weaponPtr;
               }
               else
               {
                  mErrorString = "could not add weapon to shooting platform";
                  auto out     = ut::log::error() << "WSF Weapon Server: Could not add weapon to shooting platform.";
                  out.AddNote() << "Shooting Platform: " << shooterPlatformId;
                  return false;
               }
            }
            else
            {
               mErrorString = "could not add weapon type to shooting platform";
               auto out     = ut::log::error() << "WSF Weapon Server: Could not add weapon type to shooting platform.";
               out.AddNote() << "Shooting Platform: " << shooterPlatformId;
               out.AddNote() << "Weapon Type: " << weaponType;
               return false;
            }
         }
         else
         {
            mErrorString = "weapon type not found in the simulation (not defined) => ";
            mErrorString += weaponType;
            auto out = ut::log::error() << "WSF Weapon Server: Weapon type not defined in the simulation.";
            out.AddNote() << "Weapon Type: " << weaponType;
            return false;
         }
      }

      if (weaponToShootPtr != nullptr)
      {
         // if a weapon platform already exists from this shooter with the given controller & track numbers, delete it
         DisEntityId     shooterDisId;
         WsfDisPlatform* shooterDisPlatformPtr = mDisInterfacePtr->GetDisPlatform(shooterPlatformPtr->GetIndex());
         if (shooterDisPlatformPtr != nullptr)
         {
            shooterDisId = shooterDisPlatformPtr->GetEntityId();
         }
         // If the weapon tail number option was activated, then a non-unique tail number could possibly
         // have been assigned to the weapon.  Test against that condition and if a non-unique value
         // was sent in, send back a message and delete the previous missile from the simulation
         // and allow the second weapon to fly-out.
         if (weaponTailNumber >= 0)
         {
            std::map<UniqueWeaponKey, size_t>::iterator it;
            it = mPlatformIndexMap.find(UniqueWeaponKey(shooterDisId, weaponControllerNumber, weaponTailNumber));
            if (it != mPlatformIndexMap.end())
            {
               // delete it, shooting a new weapon with this tail number
               WsfPlatform* platPtr = GetSimulation().GetPlatformByIndex(it->second);
               if (platPtr != nullptr)
               {
                  if (!mWarningString.empty())
                  {
                     mWarningString += ";";
                  }
                  mWarningString += "Weapon tail number conflict; removing platform, ";
                  mWarningString += platPtr->GetName();
                  mWarningString += ", from the simulation";
                  mWarningStringSet = true;
                  // this will call WsfObserver::WeaponTerminated() before putting the delete event on the queue
                  // so the J11 interface can clean up its mappings
                  // must do that before we fire a new weapon with same track numbers below
                  GetSimulation().DeletePlatform(simTime, platPtr);
               }
            }
         }
         int  weaponEntityId = weaponTailNumber;
         bool bOverrideDisId = false;
         if (WeaponIdType == BY_DIS_ID)
         {
            if (mDisInterfacePtr != nullptr)
            {
               WsfDisPlatform* conflictPlatformPtr = mDisInterfacePtr->FindDisPlatform(weaponDisId);
               if (conflictPlatformPtr == nullptr)
               {
                  // no platform in the simulation matches the dis id sent in for the weapon.
                  weaponEntityId = (int)weaponDisId.GetEntity();
                  // override the dis id for the weapon platform once it is created; used after the Fire() method called.
                  bOverrideDisId = true;
               }
               else
               {
                  mErrorString = "Wpn_DisId=";
                  std::stringstream tmp("");
                  tmp << (int)weaponDisId.GetSite() << ":" << (int)weaponDisId.GetApplication() << ":"
                      << (int)weaponDisId.GetEntity();
                  mErrorString += tmp.str();
                  mErrorString += " already in use";
                  return false;
               }
            }
         }

         bool shot = false;
         // int wtNum = (useTailNumPdus)?(weaponTailNumber):(0);  //weapon ID in the fire() commands defaults to zero
         int wtNum = (useTailNumPdus || bOverrideDisId) ? (weaponEntityId) :
                                                          (0); // weapon ID in the fire() commands defaults to zero

         mCurrentShooterId =
            shooterDisId; // save this as class member, so the PlatformAdded callback can register it (if fired)
         if (mWeaponDisidOverride)
         {
            mCurrentWeaponId =
               weaponDisId; // save this as class member, so the PlatformAdded callback can update the weapon's disid
         }
         mCurrentControllerTrackNumber = ut::safe_cast<unsigned short, size_t>(
            weaponControllerNumber); // save this as class member, so the PlatformAdded callback can register it (if fired)
         mCurrentWeaponTrackNumber = ut::safe_cast<unsigned int, int>(
            weaponTailNumber); // save this as class member, so the PlatformAdded callback can register it (if fired)
         mUseWpnIdInPDUs = useTailNumPdus;

         // set this to skip the PlatformAdded code if the weapon server did not cause the weapon to be fired!!!!!!!
         mWeaponServerRequest = true;
         WsfWeapon::FireOptions settings;
         settings.mWeaponId = wtNum;
         shot               = weaponToShootPtr->Fire(simTime, targetTrackPtr, settings);
         // Reinitialize the weapon count based on what was defined for the platform originally, in the event a command
         // comes in with the infinite count set 'false'.  Allows the weapon count to go to zero if that condition is
         // required. Set the TN of the weapon on its link-16 computer
         if ((mCurrentWeaponTrackNumber > 0) && (mWeaponPlatformPtr != nullptr))
         {
            if (mDebugEnabled)
            {
               auto out = ut::log::debug() << "WSF Weapon Server: Weapon platform added. Setting its track number.";
               out.AddNote() << "Platform: " << mWeaponPlatformPtr->GetName();
               out.AddNote() << "Track Number: " << mCurrentWeaponTrackNumber;
            }
#ifdef WSF_WITH_L16
            WsfL16::Interface::SetPlatformTrackNumber(mWeaponPlatformPtr, mCurrentWeaponTrackNumber);
#endif
         }

         if (wpnCount >= 0)
         {
            if (wpnCount > 0)
            {
               if (shot)
               {
                  // decrement the weapon count, since the weapon was successfully fired
                  weaponToShootPtr->SetQuantityRemaining(wpnCount - 1);
               }
               else
               {
                  weaponToShootPtr->SetQuantityRemaining(wpnCount);
               }
            }
            else
            {
               weaponToShootPtr->SetQuantityRemaining(0);
            }
         }
         mWeaponServerRequest = false;

         mCurrentShooterId = DisEntityId(); // clear (in case a weapon is fired by some other means, we don't want to
                                            // erroneously store it's shooter id)
         mCurrentWeaponId = DisEntityId();  // clear (in case a weapon is fired by some other means, we don't want to
                                            // erroneously store it's shooter id)
         mCurrentControllerTrackNumber = 0; // clear (in case a weapon is fired by some other means, we don't want to
                                            // erroneously store it's controller number)
         mCurrentWeaponTrackNumber = 0;     // clear (in case a weapon is fired by some other means, we don't want to
                                            // erroneously store it's track number)
         mCurrentFrequency = 0.0; // clear (in case a weapon is fired by some other means, we dont' want to erroneously
                                  // store it's frequency)
         mCurrentWpnId        = 0;
         mWeaponDisidOverride = false;
         mWeaponIdOverride    = false;
         mUseWpnIdInPDUs      = false;

         if (!shot)
         {
            mErrorString = "weapon fire failed";
            auto out     = ut::log::error() << "WSF Weapon Server: Weapon fire failed.";
            if (weaponToShootPtr->GetQuantityRemaining() == 0)
            {
               mErrorString += "; quantity=0";
               out.AddNote() << "No shots remaining.";
            }
            return false;
         }
         else if (((WeaponIdType == BY_DIS_ID) && (bOverrideDisId)) ||
                  ((WeaponIdType == BY_TAIL_NUMBER) && (useTailNumPdus)))
         {
            size_t wpnIndex = 0;
            size_t pos      = mWarningString.find("index=");
            if (pos != std::string::npos)
            {
               // found the index string for the weapon; now get the index
               size_t posDelim = mWarningString.substr((pos + 1)).find(";");
               if (posDelim == std::string::npos)
               {
                  posDelim = mWarningString.length();
               }
               size_t      len      = posDelim - (pos + 6);
               std::string indexStr = mWarningString.substr((pos + 6), len);
               wpnIndex             = atoi(indexStr.c_str());
            }
            else
            {
               if (!mWarningStringSet)
               {
                  DisEntityId disId;
                  if (GetDisEntityId(mWarningString, disId))
                  {
                     WsfDisPlatform* dpPtr = mDisInterfacePtr->FindDisPlatform(disId);
                     if ((dpPtr != nullptr) && (dpPtr->GetPlatform() != nullptr))
                     {
                        wpnIndex = dpPtr->GetPlatform()->GetIndex();
                     }
                  }
               }
            }
            if (wpnIndex > 0)
            {
               WsfPlatform* wpnPlatformPtr = GetSimulation().GetPlatformByIndex(wpnIndex);
               if (wpnPlatformPtr)
               {
                  if (bSuppressDetonationPDU)
                  {
                     wpnPlatformPtr->GetAuxData().AddBool("suppress_detonation_pdu", true);
                  }
                  if (mDisInterfacePtr != nullptr)
                  {
                     // will set the entity id value for either BY_DIS_ID or BY_TAIL_NUM inputs
                     if (wtNum > 0)
                     {
                        // allow the entity id to filter into all the platform name to entity id maps
                        mDisInterfacePtr->GetExtInterface()->ProcessEntityId(wpnPlatformPtr->GetName(),
                                                                             weaponDisId.GetEntity());
                     }
                     WsfDisPlatform* disWpnPlatformPtr = mDisInterfacePtr->GetDisPlatform(wpnIndex);
                     if (disWpnPlatformPtr != nullptr)
                     {
                        DisUint16       wpnEntityIdIndex = (DisUint16)wpnPlatformPtr->GetIndex();
                        DisEntityId     launcherId;
                        WsfDisPlatform* disLauncherPlatformPtr =
                           mDisInterfacePtr->GetDisPlatform(shooterPlatformPtr->GetIndex());
                        if (disLauncherPlatformPtr != nullptr)
                        {
                           launcherId = disLauncherPlatformPtr->GetEntityId();
                           DisEntityId wpnDisIdByIndex(launcherId.GetSite(), launcherId.GetApplication(), wpnEntityIdIndex);
                           // if the site:app:entity using the weapon's index is actually assigned to the weapon
                           // platform, delete it from the dis interface list, since at the time the weapon platform was
                           // entered, it did not know about the reassignment.  Remove it from the list and then add it
                           // back with the new weapon disid sent in by the user. Inefficient to loop, but have no
                           // choice, since index and assigned entity id do not correlate. BAR: This would not need to
                           // happen here, but the assignment was taken out of WsfExplicitWeapon.
                           std::map<DisEntityId, WsfDisPlatform*>& entityIdToDisPlatformList =
                              mDisInterfacePtr->GetEntityIdToDisPlatform();
                           std::map<DisEntityId, WsfDisPlatform*>::iterator findIter = entityIdToDisPlatformList.begin();
                           for (; findIter != entityIdToDisPlatformList.end(); ++findIter)
                           {
                              if (((*findIter).second) && ((*findIter).second->GetPlatform()) &&
                                  ((*findIter).second->GetPlatform()->GetIndex() == wpnEntityIdIndex))
                              {
                                 // Indeed, the dis entity id is for the weapon; now erase the old entry containing the
                                 // index as the entity id
                                 mDisInterfacePtr->GetEntityIdToDisPlatform().erase(findIter);
                                 break;
                              }
                           }
                        }

                        if (WeaponIdType == BY_DIS_ID)
                        {
                           // Now reset the platform entity id with what was sent in.  Could possibly be using a
                           // site:app that does not match with what was set in the dis_interface block.
                           disWpnPlatformPtr->ResetEntityId(weaponDisId);
                           // Reinsert the weapon's dis entity id back into the list.
                           mDisInterfacePtr->GetEntityIdToDisPlatform()[weaponDisId] = disWpnPlatformPtr;
                        }
                        else
                        {
                           WsfDisPlatform* disLauncherPlatformPtr =
                              mDisInterfacePtr->GetDisPlatform(shooterPlatformPtr->GetIndex());
                           if (disLauncherPlatformPtr != nullptr)
                           {
                              // use the launcher's site and app values.
                              DisEntityId weaponDisId;
                              weaponDisId.SetSite(launcherId.GetSite());
                              weaponDisId.SetApplication(launcherId.GetApplication());
                              weaponDisId.SetEntity((DisUint16)wtNum);
                              disWpnPlatformPtr->ResetEntityId(weaponDisId);
                              // Reinsert the weapon's dis entity id back into the list.
                              mDisInterfacePtr->GetEntityIdToDisPlatform()[weaponDisId] = disWpnPlatformPtr;
                           }
                        }
                     }
                  }
               }
            }
         }
         else if (bSuppressDetonationPDU)
         {
            size_t wpnIndex = 0;
            size_t pos      = mWarningString.find("index=");
            if (pos != std::string::npos)
            {
               // found the index string for the weapon; now get the index
               size_t posDelim = mWarningString.substr((pos + 1)).find(";");
               if (posDelim == std::string::npos)
               {
                  posDelim = mWarningString.length();
               }
               size_t      len      = posDelim - (pos + 6);
               std::string indexStr = mWarningString.substr((pos + 6), len);
               wpnIndex             = atoi(indexStr.c_str());
            }
            else
            {
               if (!mWarningStringSet)
               {
                  DisEntityId disId;
                  if (GetDisEntityId(mWarningString, disId))
                  {
                     WsfDisPlatform* dpPtr = mDisInterfacePtr->FindDisPlatform(disId);
                     if ((dpPtr != nullptr) && (dpPtr->GetPlatform() != nullptr))
                     {
                        wpnIndex = dpPtr->GetPlatform()->GetIndex();
                     }
                  }
               }
            }
            if (wpnIndex > 0)
            {
               WsfPlatform* wpnPlatformPtr = GetSimulation().GetPlatformByIndex(wpnIndex);
               if (wpnPlatformPtr)
               {
                  wpnPlatformPtr->GetAuxData().AddBool("suppress_detonation_pdu", true);
               }
            }
         }
      }
      else
      {
         mErrorString = "unknown weapon error, should not ever get this message";
         ut::log::error() << "WSF Weapon Server: Unknown weapon error. This message should never occur.";
         return false;
      }
      return true;
   }
   else
   {
      mErrorString = "unrecognized command";
      auto out     = ut::log::error() << "WSF Weapon Server: Unrecognized command.";
      out.AddNote() << "Command: " << commandName;
      return false;
   }
}

std::string WsfWeaponServer::GenerateCommand(const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr)
{
   std::stringstream ss;
   ss << "RELEASE_STORE";

   // populate mandatory shooter fields
   WsfDisPlatform* shooterPlatform = mDisInterfacePtr->GetDisPlatform(aEngagementPtr->GetFiringPlatformIndex());
   if (shooterPlatform != nullptr)
   {
      // try to use dis id first
      const DisEntityId& id = shooterPlatform->GetEntityId();
      ss << " DISID=" << id.GetSite() << ":" << id.GetApplication() << ":" << id.GetEntity();
   }
   else
   {
      // fall back on using the name
      ss << " NAMEID=" << aEngagementPtr->GetFiringPlatformName();
   }

   WsfPlatform* shooterPtr = aEngagementPtr->GetFiringPlatform();

#ifdef WSF_WITH_L16
   // controller track number (if it exists, use callback)
   if (shooterPtr != nullptr)
   {
      unsigned int controllerTrackNumber = WsfL16::ComputerProcessor::GetPlatformTrackNumber(*shooterPtr);
      if (controllerTrackNumber > 0)
      {
         ss << " CTRL_TRACK_NUM=" << controllerTrackNumber;
      }
   }
#endif

   // populate mandatory weapon fields
   ss << " WPN_TYPE=" << aEngagementPtr->GetWeaponSystem()->GetTypeId();
   ss << " WPN_TAIL_NUMBER=" << ++mOutgoingWeaponTrackNumbers;
   ss << " USETAILNUMINPDUS=FALSE";

   if (aTargetTrackPtr != nullptr)
   {
      // populate mandatory target fields
      WsfSpatialDomain dom = aTargetTrackPtr->GetSpatialDomain();
      ss << " LCH_TYPE="
         << ((dom == WSF_SPATIAL_DOMAIN_LAND || dom == WSF_SPATIAL_DOMAIN_SURFACE) ? ("1") : ("0")); // 0 = air is default

      WsfDisPlatform* targetPlatformPtr = mDisInterfacePtr->GetDisPlatform(aEngagementPtr->GetTargetPlatformIndex());
      if (targetPlatformPtr != nullptr)
      {
         // try to use dis id first
         const DisEntityId& id = targetPlatformPtr->GetEntityId();
         ss << " TARGET_DISID=" << id.GetSite() << ":" << id.GetApplication() << ":" << id.GetEntity();
      }
      else
      {
         // fall back on using the name
         const WsfPlatform* targetPlatPtr = aEngagementPtr->GetTargetPlatform();
         if (targetPlatPtr != nullptr)
         {
            ss << " TARGET_NAMEID=" << targetPlatPtr->GetName();
         }
         else
         {
            // nothing worked yet?  try target location lastly...
            double lat, lon, alt;
            aTargetTrackPtr->GetLocationLLA(lat, lon, alt);
            ss << " LAT=" << lat;
            ss << " LON=" << lon;
            ss << " ALT=" << alt;
            // tag must be included with location, make it represent the local track id for now
            const WsfTrackId& id = aTargetTrackPtr->GetTrackId();
            ss << " TAG=track_id_" << id.GetOwningPlatformId() << "_" << id.GetLocalTrackNumber();
         }
      }
   }
   else
   {
      // TODO: generate a fake target ahead of the shooter?  or fail with error?
      // generate a fake a target for now
      // WsfPlatform* shooterPtr = aEngagementPtr->GetFiringPlatform();  //already have pointer
      if (shooterPtr != nullptr)
      {
         // put fake target 10 miles out on the ground
         ss << " LCH_TYPE=1"; // 1 = ground

         double refLat, refLon, refAlt;
         shooterPtr->GetLocationLLA(refLat, refLon, refAlt);
         double heading, pitch, roll;
         shooterPtr->GetOrientationNED(heading, pitch, roll);
         double fakeLat, fakeLon;
         // put fake target 10 miles out ahead of the shooter
         UtSphericalEarth::ExtrapolateGreatCirclePosition(refLat, refLon, heading * UtMath::cDEG_PER_RAD, 18520, fakeLat, fakeLon);

         ss << " LAT=" << fakeLat;
         ss << " LON=" << fakeLon;
         ss << " ALT=" << 0.0;
         // tag must be included with location
         ss << " TAG=fake_location_10nm";
      }
   }

   ss << '\0';
   return ss.str();
}


unsigned int WsfWeaponServer::DecimalToOctal(unsigned int aDecimalValue)
{
   unsigned int octal      = 0;
   int          multiplier = 1;
   for (int i = 0; i < 5; ++i)
   {
      // int digit = std::min(7u, aDecimalValue % 10);
      int digit = std::min(7u, aDecimalValue % 10);
      octal += multiplier * digit;
      aDecimalValue /= 10;
      multiplier *= 8;
   }
   return octal;
}


//-----------------------------------------------------------------------------
bool WsfWeaponServer::GetDisEntityId(const std::string& aStr, DisEntityId& aDisEntityId)
{
   bool ok = true;
   // with sDisId find the platform
   // First strip off the site, app, entity

   size_t found   = aStr.find_first_of(":");
   bool   delimOk = false;
   if (found != std::string::npos)
   {
      delimOk = true;
   }
   if (delimOk)
   {
      char sDisValue[12];
      int  outId[3] = {0, 0, 0};

      size_t i       = 0;
      size_t lastPos = 0;
      while ((delimOk) && (i < 2))
      {
         size_t count = found - lastPos;
         aStr.substr(0).copy(sDisValue, count, lastPos);
         sDisValue[count] = '\0';
         outId[i]         = atoi(sDisValue);
         // if there is a format error return
         if (outId[i] <= 0 || outId[i] > 65535)
         {
            return false;
         }
         lastPos = found + 1;
         found   = aStr.find_last_of(":");
         if (found == std::string::npos)
         {
            delimOk = false;
         }
         ++i;
      }
      if (i < 2)
      {
         // format error, did not find second colon
         return false;
      }
      outId[2] = atoi(aStr.substr(lastPos).c_str());
      // if there is a format error return
      if (outId[2] <= 0 || outId[2] > 65535)
      {
         return false;
      }

      aDisEntityId.SetSite((DisUint16)outId[0]);
      aDisEntityId.SetApplication((DisUint16)outId[1]);
      aDisEntityId.SetEntity((DisUint16)outId[2]);
   }
   else
   {
      // if there is a format error return error indication
      ok = false;
   }

   return ok;
}

void WsfWeaponServer::SendToOtherWeaponServer(double                     aSimTime,
                                              WsfPlatform*               aPlatformPtr,
                                              const WsfWeaponEngagement* aEngagementPtr,
                                              const WsfTrack*            aTrackPtr)
{
   // check if we need to setup the outgoing TCP connection (maybe it failed on init)
   if ((mOutgoingClientConnection == nullptr) && (!mOutGoingHostName.empty()))
   {
      mOutgoingClientConnection = new GenTCP_IO();
      if (!mOutgoingClientConnection->Init(mOutGoingHostName, mOutGoingPort))
      {
         delete mOutgoingClientConnection;
         mOutgoingClientConnection = nullptr;
         auto out = ut::log::error() << "WSF Weapon Server: Still could not initialize outgoing client connection.";
         out.AddNote() << "Host Name: " << mOutGoingHostName;
         out.AddNote() << "Port: " << mOutGoingPort;
      }
   }

   if (mOutgoingClientConnection != nullptr) // if we are setup as a client to send out weapon server commands
   {
      // check for any status messages from previous fires
      bool keepGoing = false;
      do
      {
         keepGoing = false;
         mOutgoingClientConnection->ReceiveContinue(0);
         int bytesAvailable = mOutgoingClientConnection->GetRecvBuffer()->GetNumValidBytes();
         if (bytesAvailable >= 2)
         {
            GenBuffer      buf((char*)mOutgoingClientConnection->GetRecvBuffer()->GetBuffer(), 1);
            unsigned char* positionEOS =
               std::find(mOutgoingClientConnection->GetRecvBuffer()->GetBuffer(),
                         (mOutgoingClientConnection->GetRecvBuffer()->GetBuffer() + bytesAvailable),
                         '\0');
            int packet_length = (int)(positionEOS - mOutgoingClientConnection->GetRecvBuffer()->GetBuffer() + 1);
            if (bytesAvailable >= packet_length)
            {
               char* text = new char[packet_length];
               memcpy(text, buf.GetBuffer(), packet_length);
               mOutgoingClientConnection->GetRecvBuffer()->SetGetOffset(packet_length, GenBuf::FromBeg);
               mOutgoingClientConnection->RemoveProcessedBufferData();
               // cout << "Weapon Server Response: " << text << endl;
               keepGoing = true; // see if another command is on the input buffer
               delete[] text;
            }
         }
      } while (keepGoing);

      // check if this weapon is external (hosted by a weapon server)
      WsfStringId wpn = aEngagementPtr->GetWeaponSystem()->GetTypeId();
      auto        it  = find(mOutgoingWeaponTypes.begin(), mOutgoingWeaponTypes.end(), wpn);
      if (it != mOutgoingWeaponTypes.end())
      {
         // generate the weapon command & send it
         std::string newCommand = GenerateCommand(aEngagementPtr, aTrackPtr);

         if (mDebugEnabled == true)
         {
            std::string address = mOutgoingClientConnection->GetSenderAddress();
            auto        out     = ut::log::debug() << "WSF Weapon Server: Processing weapon fire to other server.";
            out.AddNote() << "Server Address: " << address;
            out.AddNote() << "Outgoing command: " << newCommand;
         }

         mOutgoingClientConnection->Put(newCommand.c_str());
         mOutgoingClientConnection->Send();
         aPlatformPtr->SetIsExternallyControlled(true);
      }
   }
}

bool WsfWeaponServer::GetWeaponServerNumbers(const size_t  weaponIndex,
                                             unsigned int& weaponTnumber,
                                             unsigned int& controllerTnumber)
{
   if (mWeaponTrackNumberMap.find(weaponIndex) != mWeaponTrackNumberMap.end() &&
       mControllerTrackNumberMap.find(weaponIndex) != mControllerTrackNumberMap.end())
   {
      weaponTnumber     = mWeaponTrackNumberMap[weaponIndex];
      controllerTnumber = mControllerTrackNumberMap[weaponIndex];
      return true;
   }
   return false;
}


void WsfWeaponServer::PlatformAdded(double aTime, WsfPlatform* aPlatformPtr)
{
   // only save off values for weapons (platforms that have a weapon engagement)
   if (WsfWeaponEngagement::Find(*aPlatformPtr) == nullptr)
   {
      return;
   }

   // Call this ONLY if the platform was added by means of a weapon server request.
   // PlatformAdded can be triggered by other conditions
   if (mWeaponServerRequest)
   {
      // save off the weapon platform pointer
      mWeaponPlatformPtr = aPlatformPtr;
      // save off the weapon T number & controller T number, mapped by weapon platform index
      // the class variables will only have relevant values during a call to weapon.Fire()
      size_t index                     = aPlatformPtr->GetIndex();
      mShooterIdMap[index]             = mCurrentShooterId;
      mControllerTrackNumberMap[index] = mCurrentControllerTrackNumber;
      mWeaponTrackNumberMap[index]     = mCurrentWeaponTrackNumber;
      mPlatformIndexMap[UniqueWeaponKey(mCurrentShooterId, mCurrentControllerTrackNumber, mCurrentWeaponTrackNumber)] =
         index;

      SetCommander(aPlatformPtr);
      // TO DO: BAR - Added check for WsfNetComm, if present then handle as IFC weapon, else return and process normally.
      int wpnId = mCurrentWeaponTrackNumber;
      if (mCurrentWpnId >= 0)
      {
         wpnId = mCurrentWpnId;
      }
      WsfWeaponServer::PostLaunch(aPlatformPtr, mCurrentControllerTrackNumber, mCurrentFrequency, wpnId, true);

      if (mDisInterfacePtr == nullptr)
      {
         // allow other applications opportunity to retrieve information about this weapon
         std::stringstream idStr("");
         if (!mWarningString.empty())
         {
            idStr << ";";
         }
         idStr << "index=" << index;
         mWarningString += idStr.str();
      }
      else
      {
         // Return result message in the format documented in the ASCII Interface Manual.
         WsfDisPlatform* dpPtr = mDisInterfacePtr->GetDisPlatform(aPlatformPtr->GetIndex());
         if (dpPtr != nullptr)
         {
            if (mWeaponDisidOverride)
            {
               // erase the entry for the platform and replace it with the new DIS Id
               std::map<DisEntityId, WsfDisPlatform*>::iterator iter =
                  mDisInterfacePtr->GetEntityIdToDisPlatform().find(dpPtr->GetEntityId());
               if (iter != mDisInterfacePtr->GetEntityIdToDisPlatform().end())
               {
                  mDisInterfacePtr->GetEntityIdToDisPlatform().erase(iter);
               }
               dpPtr->SetEntityId(mCurrentWeaponId);
               // re-add entry back to the dis id to dis platform list
               mDisInterfacePtr->GetEntityIdToDisPlatform().insert(std::make_pair(mCurrentWeaponId, dpPtr));
               if (mDebugEnabled)
               {
                  WsfDisPlatform* tmpDisPlatformPtr = mDisInterfacePtr->FindDisPlatform(mCurrentWeaponId);
                  if (tmpDisPlatformPtr != nullptr)
                  {
                     auto out = ut::log::debug() << "WSF Weapon Server: Platform has new DIS ID.";
                     out.AddNote() << "Platform: " << tmpDisPlatformPtr->GetPlatform()->GetName();
                     out.AddNote() << "DIS ID: " << tmpDisPlatformPtr->GetEntityId();
                  }
               }
            }
            else if ((mWeaponIdOverride) && (mUseWpnIdInPDUs))
            {
               // erase the entry for the platform and replace it with the new DIS Id
               std::map<DisEntityId, WsfDisPlatform*>::iterator iter =
                  mDisInterfacePtr->GetEntityIdToDisPlatform().find(dpPtr->GetEntityId());
               if (iter != mDisInterfacePtr->GetEntityIdToDisPlatform().end())
               {
                  mDisInterfacePtr->GetEntityIdToDisPlatform().erase(iter);
               }
               DisEntityId currentId = dpPtr->GetEntityId();
               currentId.SetEntity(mCurrentWeaponTrackNumber);
               dpPtr->SetEntityId(currentId);
               // re-add entry back to the dis id to dis platform list
               mDisInterfacePtr->GetEntityIdToDisPlatform().insert(std::make_pair(currentId, dpPtr));
               if (mDebugEnabled)
               {
                  WsfDisPlatform* tmpDisPlatformPtr = mDisInterfacePtr->FindDisPlatform(mCurrentWeaponId);
                  if (tmpDisPlatformPtr != nullptr)
                  {
                     auto out = ut::log::debug() << "WSF Weapon Server: Platform has new DIS ID.";
                     out.AddNote() << "Platform: " << tmpDisPlatformPtr->GetPlatform()->GetName();
                     out.AddNote() << "DIS ID: " << tmpDisPlatformPtr->GetEntityId();
                  }
               }
            }
            // set up the command's return info, in this case the DIS id of the weapon just fired.
            DisEntityId       lEntityId = dpPtr->GetEntityId();
            std::stringstream disIdStr("");
            if (!mWarningString.empty())
            {
               disIdStr << ";";
            }
            disIdStr << (int)lEntityId.GetSite() << ":";
            disIdStr << (int)lEntityId.GetApplication() << ":";
            disIdStr << (int)lEntityId.GetEntity();
            mWarningString += disIdStr.str();
         }
      }
      auto out = ut::log::info() << "WSF Weapon Server: Weapon fired.";
      out.AddNote() << "Status: " << mWarningString;
   }

   // This is set up for the new adhoc networking to report data needed to be stored on
   // a post launch data structure
   // Call this ONLY if the platform was added by means of a weapon server request.
   // PlatformAdded can be triggered by other conditions
   if (mWeaponServerRequest)
   {
      // TO DO: BAR - Added check for WsfNetComm, if present then handle as IFC weapon, else return and process normally.
      int wpnId = mCurrentWeaponTrackNumber;
      if (mCurrentWpnId >= 0)
      {
         wpnId = mCurrentWpnId;
      }
      WsfWeaponServer::PostLaunch(aPlatformPtr, mCurrentControllerTrackNumber, mCurrentFrequency, wpnId, true);
   }

   // TODO: assumption: this method of recording the track numbers only works because
   // the weapon server doesn't handle salvo commands and only fires one weapon at a time
   // if salvo's were fired, how would we update mCurrentWeaponTrackNumber for each weapon platform added?
}

void WsfWeaponServer::PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr)
{
   WsfWeaponEngagement* weaponEngagementPtr = WsfWeaponEngagement::Find(*aPlatformPtr);
   if (weaponEngagementPtr == nullptr)
   {
      return;
   }
   if (WsfWeaponEngagement::Find(*aPlatformPtr) != nullptr)
   {
      if (weaponEngagementPtr->GetFiringPlatform() != nullptr)
      {
         if (aPlatformPtr->GetSideId() != weaponEngagementPtr->GetFiringPlatform()->GetSideId())
         {
            aPlatformPtr->SetSide(weaponEngagementPtr->GetFiringPlatform()->GetSide());
         }
      }
   }
}

bool WsfWeaponServer::SetCommander(WsfPlatform* aPlatformPtr)
{
   // need to set the commander for this weapon.
   // first use platform associated with the controller track number if available
   // then use the firing platform
   WsfPlatform* commanderPtr = nullptr;
   if (mCurrentControllerTrackNumber > 0)
   {
#ifdef WSF_WITH_L16
      WsfL16::Interface::GetControllerFromTrackNumber(aPlatformPtr->GetSimulation(), mCurrentControllerTrackNumber);
#endif
      // if not found search through external TN list
      if (commanderPtr == nullptr)
      {
         auto iter = mTrackNumberToDisEntityIdMap.find(mCurrentControllerTrackNumber);
         if (iter != mTrackNumberToDisEntityIdMap.end())
         {
            WsfDisPlatform* dpPtr = mDisInterfacePtr->FindDisPlatform((*iter).second);
            if (dpPtr != nullptr)
            {
               commanderPtr = dpPtr->GetPlatform();
            }
         }
      }
   }
   else
   {
      WsfWeaponEngagement* weaponEngagementPtr = WsfWeaponEngagement::Find(*aPlatformPtr);
      if (weaponEngagementPtr != nullptr)
      {
         commanderPtr = weaponEngagementPtr->GetFiringPlatform();
      }
   }
   if (commanderPtr != nullptr)
   {
      WsfCommandChain* chainPtr = aPlatformPtr->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
      if (chainPtr != nullptr)
      {
         chainPtr->SetCommander(commanderPtr);
      }
   }
   return commanderPtr != nullptr;
}

void WsfWeaponServer::PlatformDeleted(double aTime, WsfPlatform* aPlatformPtr)
{
   // TODO:REVIEW -- This code looks suspicious; it is a global observer    REVIEW:RAVER
   if (WsfWeaponEngagement::Find(*aPlatformPtr) == nullptr)
   {
      return;
   }

   // clean up subsystems (for WsfFrameStepSimulation)
   // this is necessary because we bypass the WsfSimulation::DeletePlatform() call
   // we do that because we want the platform gone now, we don't want a scheduled event to be processed later
   for (WsfComponentList::RoleIterator<WsfPlatformPart> partIter(*aPlatformPtr); !partIter.AtEnd(); ++partIter)
   {
      GetSimulation().TurnPartOff(aTime, *partIter);
   }

   size_t index = aPlatformPtr->GetIndex();
   // if one map exists, the others should too
   if (mWeaponTrackNumberMap.find(index) != mWeaponTrackNumberMap.end())
   {
      unsigned int wtNum = mWeaponTrackNumberMap[index];
      unsigned int ctNum = mControllerTrackNumberMap[index];
      DisEntityId  sId   = mShooterIdMap[index];
      mWeaponTrackNumberMap.erase(index);
      mControllerTrackNumberMap.erase(index);
      mShooterIdMap.erase(index);
      mPlatformIndexMap.erase(UniqueWeaponKey(sId, ctNum, wtNum));
   }
}
/* // TODO: Make Thread-safe
// ============================================================================
//! Performs the thread work.
//!
//! @return Function state of the thread. (See WsfThread)
WsfWeaponServer::FunctionType WsfWeaponServer::DoWork()
{
   GenTCP_IO* lIO;
   for (unsigned int i = 0; i < mConnections.size(); ++i)
   {
      lIO = mConnections[i];
      if (!lIO->IsConnected() && mServer->ConnectionCount() > 0)
      {
         mServer->OnDisconnect(lIO);   // callback to disconnect
      }
   }
   GenTCP_IO* newConnection = mServer->Accept(0);
   if (newConnection != nullptr)
   {
      if (!newConnection->IsConnected())
      {
         if (mServer->ConnectionCount() > 0)
         {
            mServer->OnDisconnect(newConnection);
         }
      }
      else
      {
         vector<GenTCP_IO*>::iterator i = find(mConnections.begin(), mConnections.end(), newConnection);
         if (i == mConnections.end())
         {
            mConnections.push_back(newConnection);
            cout << "WSF Weapon Server ACCEPTED connection on port " << mPort << endl;
         }
      }
   }
   else
   {
      // else check if any connections have been made, then read the packets.
      GenTCP_IO* lIO;
      for (unsigned int i = 0; i < mConnections.size(); ++i)
      {
         lIO = mConnections[i];
         if (!lIO->IsConnected())
         {
            if (mServer->ConnectionCount() > 0)
            {
               mServer->OnDisconnect(lIO);   // callback on disconnect
            }
            continue;
         }

         bool keepGoing = false;
         do
         {
            keepGoing = false;
            //ASCIICommandPacket pkt;
            lIO->ReceiveContinue(0);
            int bytesAvailable = lIO->GetRecvBuffer()->GetNumValidBytes();
            if (bytesAvailable >= 3)
            {
               //mTotalTimer.ResetClock();
               GenBuffer buf((char*)lIO->GetRecvBuffer()->GetBuffer(), 1);
               unsigned char* positionEOS = find(lIO->GetRecvBuffer()->GetBuffer(),
                                                      (lIO->GetRecvBuffer()->GetBuffer() + bytesAvailable),
                                                      '\0');
               int packet_length = (int)(positionEOS - lIO->GetRecvBuffer()->GetBuffer() + 1);
               //cout << "From TCP Buffer packet length = " << packet_length << endl;
               if (bytesAvailable >= packet_length)
               {
                  char* text = new char[packet_length];
                  memcpy(text, buf.GetBuffer(), packet_length);
                  lIO->GetRecvBuffer()->SetGetOffset(packet_length, GenBuf::FromBeg);
                  lIO->RemoveProcessedBufferData();
                  // Set up Send with the current sender for return message
                  mCurrentConnection = lIO;
                  if (ProcessCommand(string(text)) == true)
                  {
                     string okString = "OK ";
                     if (!this->GetLastWarning().empty())
                     {
                        okString += this->GetLastWarning();
                     }
                     okString += '\0';
                     //success, return "OK \0"  string
                     //lIO->Put("OK \0");
                     lIO->Put(okString.c_str());
                     lIO->Send();
                  }
                  else
                  {
                     //failure, return "FAIL <error string> \0"  string
                     string errStr = this->GetLastError();  //mErrorString
                     ostringstream oss;
                     //oss << "FAIL " << errStr << '\0' << endl;
                     oss << "FAIL " << mErrorString << '\0' << endl;
                     lIO->Put(oss.str().c_str());
                     lIO->Send();
                  }
                  keepGoing = true; //see if another command is on the input buffer
               }
            }
         }
         while (keepGoing);
      }
   }

   return Pause();   //let the thread rest for a second, it will continue after small sleep time
}
*/
// Track data was insufficient, create a point out into space at which to aim
WsfTrack* WsfWeaponServer::CreateATargetPoint(double aSimTime, WsfPlatform* aPlatformPtr, double aAtRangeNM)
{
   double lat, lon, alt;
   aPlatformPtr->GetLocationLLA(lat, lon, alt);
   double headingRad, p, r;
   aPlatformPtr->GetOrientationNED(headingRad, p, r);
   double distance = aAtRangeNM * UtMath::cM_PER_NM; // send it 40 nm out in front of aircraft
   double aimPtLat, aimPtLon;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(lat, lon, (headingRad * UtMath::cDEG_PER_RAD), distance, aimPtLat, aimPtLon);
   // create a track with that location
   // make a track much like the  WsfUtil::MakeTrack() method (but without a known target platform, just with locations)
   WsfTrack* targetTrackPtr = new WsfTrack();
   targetTrackPtr->SetPlatformOriginator(aPlatformPtr);
   targetTrackPtr->Initialize(aSimTime, aPlatformPtr->GetNextTrackId(), GetSimulation());
   double vecWCS[3];
   aPlatformPtr->GetLocationWCS(vecWCS);
   targetTrackPtr->SetOriginatorLocationWCS(vecWCS);
   targetTrackPtr->SetLocationLLA(aimPtLat, aimPtLon, (alt + 1000.0)); // add 1000 meters in the event being launched
                                                                       // from a ground site
   targetTrackPtr->Set3D(true);
   vecWCS[0] = vecWCS[1] = vecWCS[2] = 0;
   targetTrackPtr->SetVelocityWCS(vecWCS);
   targetTrackPtr->SetVelocityValid(true);
   int launchType = 0;
   targetTrackPtr->SetSpatialDomain((launchType == 0) ? (WSF_SPATIAL_DOMAIN_AIR) : (WSF_SPATIAL_DOMAIN_LAND));
   // don't know any truth info (type, side, target index, etc...)
   targetTrackPtr->SetTypeIdValid(false);
   targetTrackPtr->SetSideIdValid(false);

   return targetTrackPtr;
}

// Track data was insufficient, create a point out into space at which to aim
WsfTrack* WsfWeaponServer::CreateATargetPoint(double aSimTime, WsfPlatform* aPlatformPtr, double aPosNM, double aPosEM, double aPosDM)
{
   double lat, lon, alt;
   aPlatformPtr->GetLocationLLA(lat, lon, alt);
   double bearingRad = atan2(aPosEM, aPosNM);
   double bearingDeg = UtMath::cDEG_PER_RAD * bearingRad;
   double rangeM     = sqrt(aPosNM * aPosNM + aPosEM * aPosEM + aPosDM * aPosDM);
   double aimPtLat, aimPtLon;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(lat, lon, bearingDeg, rangeM, aimPtLat, aimPtLon);
   // create a track with that location
   // make a track much like the  WsfUtil::MakeTrack() method (but without a known target platform, just with locations)
   WsfTrack* targetTrackPtr = new WsfTrack();
   targetTrackPtr->SetPlatformOriginator(aPlatformPtr);
   targetTrackPtr->Initialize(aSimTime, aPlatformPtr->GetNextTrackId(), GetSimulation());
   double vecWCS[3];
   aPlatformPtr->GetLocationWCS(vecWCS);
   targetTrackPtr->SetOriginatorLocationWCS(vecWCS);
   targetTrackPtr->SetLocationLLA(aimPtLat, aimPtLon, (alt - aPosDM)); // subtract the posD; not correct, but will get
                                                                       // us in the ballpark for a launch
   targetTrackPtr->Set3D(true);
   vecWCS[0] = vecWCS[1] = vecWCS[2] = 0;
   targetTrackPtr->SetVelocityWCS(vecWCS);
   targetTrackPtr->SetVelocityValid(true);
   int launchType = 0;
   targetTrackPtr->SetSpatialDomain((launchType == 0) ? (WSF_SPATIAL_DOMAIN_AIR) : (WSF_SPATIAL_DOMAIN_LAND));
   // don't know any truth info (type, side, target index, etc...)
   targetTrackPtr->SetTypeIdValid(false);
   targetTrackPtr->SetSideIdValid(false);

   return targetTrackPtr;
}


// Handle Set Data PDU with message type 23456
void WsfWeaponServer::HandleSetDataPDU(WsfDisInterface* aInterfacePtr, const WsfDisSetData& aPdu)
{
   if (aPdu.GetNumFixedDatums() > 0)
   {
      // All expected SetData PDUs for a weapon server release will no fixed datum fields
      return;
   }
   const std::vector<DisVariableDatum>& varArray = aPdu.GetData().GetVariableDatums();

   const DisDatumSpec& lDatumSpec = aPdu.GetData();

   DisUint32 datumId(0);
   // BAR work here to get
   unsigned int         iDatum(0);
   const unsigned char* buffer;
   /* Deprecated
   unsigned int msgType = 0;
   unsigned char* fixedBuffer = 0;
   for (iDatum = 0; iDatum < lDatumSpec.GetNumFixedDatums(); ++iDatum)
   {
      const vector<DisFixedDatum>& fixedArray  = lDatumSpec.GetFixedDatums();
      const DisFixedDatum& fixedItem = fixedArray[iDatum];

      datumId = fixedItem.GetId();
      if (datumId == 10000)
      {
         unsigned char          c4BytesChar[4];
         GenIConvertBigEndian   converter;

         fixedItem.GetData(c4BytesChar);
         converter.GetFromBuf(c4BytesChar, msgType);
      }
   }
   */
   /* Deprecated
   if (msgType != mLarMsgTypeIdIn)
   {
      if (mDebugSetData)
      {
         cout << "Not our SetData PDU, msgType = " << datumId << " looking for " << mLarMsgTypeIdIn << endl;
      }
      // NOT our SetData PDU
      return;
   }
   */
   unsigned int wpnEnum = 0;

   size_t iExpect3Index = 0;

   unsigned int  varLength = 0;
   LARInputType  LARInput;
   LAROutputType LAROutput;
   unsigned int  inputValid(false);
   unsigned int  inputValidMask = 0x0000001F;

   for (iDatum = 0; iDatum < lDatumSpec.GetNumVariableDatums(); ++iDatum)
   {
      const DisVariableDatum& varItem = varArray[iDatum];

      varItem.GetData(buffer, varLength);

      varLength /= 8;
      unsigned char* dataBuffer = new unsigned char[varLength];
      memcpy(dataBuffer, buffer, varLength);
      GenMemIO* lgenMemIO = new GenMemIO((unsigned char*)dataBuffer, varLength, GenBuf::BigEndian, varLength);
      datumId             = varItem.GetId();
      switch (datumId)
      {
      case 1000:
      {
         for (iExpect3Index = 0; iExpect3Index < 3; ++iExpect3Index)
         {
            lgenMemIO->Get(LARInput.ownshipLLA[iExpect3Index]);
         }
         inputValid |= 0x01;
      }
      break;
      case 2:
      {
         for (iExpect3Index = 0; iExpect3Index < 3; ++iExpect3Index)
         {
            lgenMemIO->Get(LARInput.ownshipVelWCS[iExpect3Index]);
         }
         inputValid |= 0x02;
      }
      break;
      case 3:
      {
         for (iExpect3Index = 0; iExpect3Index < 3; ++iExpect3Index)
         {
            lgenMemIO->Get(LARInput.targetLLA[iExpect3Index]);
         }
         inputValid |= 0x04;
      }
      break;
      case 4:
      {
         for (iExpect3Index = 0; iExpect3Index < 3; ++iExpect3Index)
         {
            lgenMemIO->Get(LARInput.targetVelWCS[iExpect3Index]);
         }
         inputValid |= 0x08;
      }
      break;
      case 5:
      {
         lgenMemIO->Get(wpnEnum);
         LARInput.weaponEnum = wpnEnum;
         inputValid |= 0x10;
      }
      break;
      }
      delete[] dataBuffer;
      delete lgenMemIO;
   }
   if ((inputValid & inputValidMask) != 0)
   {
      // cppcheck-suppress uninitvar (LARInput is known to be valid because of previous check)
      if (GetLARData(LARInput, LAROutput))
      {
         SendLARData(aPdu, LAROutput);
      }
   }
}

// Get LAR data from weapon type launch computer

bool WsfWeaponServer::GetLARData(LARInputType aLARInput, LAROutputType& aLAROutput)
{
   bool bOk = true;
   // can the output for now for testing
   aLAROutput.rmax    = mTestRMax * UtMath::cM_PER_NM;
   aLAROutput.rmaxTOF = mTestRMaxTOF;
   aLAROutput.rpi     = aLAROutput.rmax * mRpiMultiplier;
   aLAROutput.rpiTOF  = aLAROutput.rmaxTOF * mRpiMultiplier;
   aLAROutput.rNe     = mTestRNe * UtMath::cM_PER_NM;
   aLAROutput.rNeTOF  = mTestRNeTOF;
   aLAROutput.rmin    = mTestRMin * UtMath::cM_PER_NM;
   aLAROutput.rminTOF = mTestRMinTOF;
   aLAROutput.DMC     = 0;
   // Get Weapon Type and if it has a valid Launch Computer process
   if (aLARInput.weaponEnum != 0) // if "valid" enum entered find it's matching weapon type
   {
      // Find the weapon type
      // 1 maps to SL_MEDIUM_RNG for unclass example
      auto iter = mWeaponEnumToWeaponType.find(aLARInput.weaponEnum);
      if (iter != mWeaponEnumToWeaponType.end())
      {
         auto& scenarioObj = mSimulationPtr->GetScenario();

         WsfWeapon* clonedWeaponPtr = WsfMilExtension::Get(scenarioObj).GetWeaponTypes().Clone((*iter).second.GetString());

         if (clonedWeaponPtr != nullptr)
         {
            clonedWeaponPtr->SetName((*iter).second.GetString());
            WsfLaunchComputer*         launchComputerPtr    = clonedWeaponPtr->GetLaunchComputer();
            WsfAirToAirLaunchComputer* a2aLaunchComputerPtr = dynamic_cast<WsfAirToAirLaunchComputer*>(launchComputerPtr);
            if (a2aLaunchComputerPtr != nullptr)
            {
               double   ownshipVelNED[3];
               UtEntity ownshipEntity;
               ownshipEntity.SetLocationLLA(aLARInput.ownshipLLA[0], aLARInput.ownshipLLA[1], aLARInput.ownshipLLA[2]);
               ownshipEntity.ConvertWCSVectorToNED(ownshipVelNED, aLARInput.ownshipVelWCS);
               double shooterInHeading = atan2(ownshipVelNED[1], ownshipVelNED[0]);
               double shooterInSpeed   = UtVec3d::Magnitude(ownshipVelNED);
               double shooterInMach    = shooterInSpeed / mAtm.SonicVelocity(aLARInput.ownshipLLA[2]);

               UtEntity tgtEntity;
               tgtEntity.SetLocationLLA(aLARInput.targetLLA[0], aLARInput.targetLLA[1], aLARInput.targetLLA[2]);
               double targetVelNED[] = {0.0, 0.0, 0.0};
               tgtEntity.ConvertWCSVectorToNED(targetVelNED, aLARInput.targetVelWCS);

               double targetInHeading = atan2(targetVelNED[1], targetVelNED[0]);
               double targetInSpeed   = UtVec3d::Magnitude(targetVelNED);
               double targetInMach    = targetInSpeed / mAtm.SonicVelocity(aLARInput.targetLLA[2]);

               double targetRelLocECS[3];
               ownshipEntity.GetRelativeLocationECS(&tgtEntity, targetRelLocECS);
               double targetAzimuth = atan2(targetRelLocECS[1], targetRelLocECS[0]);

               // Now we need to calculate whether the target "lead"
               // is crossing left to right or right to left:
               double targetRelVelECS[3];
               ownshipEntity.ConvertWCSVectorToECS(targetRelVelECS, aLARInput.targetVelWCS);
               double sign            = UtMath::Sign(targetRelVelECS[1]);
               double targetInLead    = sign * targetAzimuth;
               double targetInBearing = targetAzimuth + shooterInHeading;
               double targetInAspect =
                  fabs(UtMath::NormalizeAngleMinusPi_Pi(targetInHeading - targetInBearing - UtMath::cPI));

               double shooterOutAlt, shooterOutMach, TargetOutAlt, targetOutMach, targetOutAspect, targetOutLead;

               a2aLaunchComputerPtr->LookupResult(aLARInput.ownshipLLA[2],
                                                  shooterInMach,
                                                  aLARInput.targetLLA[2],
                                                  targetInMach,
                                                  targetInAspect,
                                                  targetInLead,
                                                  shooterOutAlt,
                                                  shooterOutMach,
                                                  TargetOutAlt,
                                                  targetOutMach,
                                                  targetOutAspect,
                                                  targetOutLead,
                                                  aLAROutput.rmax,
                                                  aLAROutput.rmaxTOF,
                                                  aLAROutput.rNe,
                                                  aLAROutput.rNeTOF,
                                                  aLAROutput.rmin,
                                                  aLAROutput.rminTOF);
               // As of 7/15 rpi is a percentage of rmax until the launch computer tables are generated with Rpi.
               aLAROutput.rpi    = aLAROutput.rmax * mRpiMultiplier;
               aLAROutput.rpiTOF = aLAROutput.rmaxTOF * mRpiMultiplier;
               aLAROutput.DMC    = 0;
            }
            else
            {
               // if aux data is associated with this weapon type, then use that if a launch computer is not defined
               // if (clonedWeaponPtr->GetAuxDataConst().AttributeExists("uni")
               // for now use canned data
               aLAROutput.rmax    = 90 * UtMath::cM_PER_NM;
               aLAROutput.rmaxTOF = 120;
               aLAROutput.rNe     = 60 * UtMath::cM_PER_NM;
               aLAROutput.rNeTOF  = 90;
               aLAROutput.rmin    = 20 * UtMath::cM_PER_NM;
               aLAROutput.rminTOF = 60;
               aLAROutput.DMC     = 0;
            }
         }
         else
         {
            bOk = false;
         }
      }
      else
      {
         bOk = false;
      }
   }
   return bOk;
}

void WsfWeaponServer::SendLARData(const WsfDisSetData& aPdu, LAROutputType aLARData)
{
   DisData* pduPtr = new DisData();

   if (pduPtr == nullptr)
   {
      return;
   }

   // Originator
   DisEntityId originator(Dis::GetSite(), Dis::GetApplication(), 0);
   pduPtr->SetProtocolVersion(Dis::GetProtocolVersion());
   pduPtr->SetExerciseIdentifier(Dis::GetExercise());
   pduPtr->SetOriginatingEntity(originator);
   pduPtr->SetReceivingEntity(aPdu.GetOriginatingEntity());
   pduPtr->SetRequestId(aPdu.GetRequestId());

   {
      auto out = ut::log::info() << "WSF Weapon Server: Sending LAR Data.";
      out.AddNote() << "Originator: " << originator;
   }

   // Define the DisData parts
   DisDatumSpec datumSpec;
   // deprecatedDisFixedDatum    fixedDatum;
   DisVariableDatum variableDatum;

   // deprecatedDisUint32 numFixedRecords = 1;
   // deprecated//deprecatedDisUint32 datumId         = 10000;
   // deprecatedDisUint32 datum           = (DisUint32)mLarMsgTypeIdOut;

   // deprecatedunsigned char swapBuf[4];
   // deprecatedGenMemIO swapData(swapBuf, 4, GenBuf::BigEndian, 4);

   // deprecatedfixedDatum.SetId(datumId);
   // deprecatedswapData.Put(datum);
   // deprecatedfixedDatum.SetData((const unsigned char*)swapBuf);
   // deprecateddatumSpec.AddFixedDatum(fixedDatum);
   // deprecatedswapData.Reset();

   DisFloat64 vDatum;
   DisUint32  vU32Datum;
   DisUint16  vU16Datum;
   DisUint32  datumId = 0;
   vU16Datum          = 11;
   SetTheVariableData(datumId, vU16Datum, datumSpec);
   datumId = 1100;
   vDatum  = (DisFloat64)aLARData.rmax;
   SetTheVariableData(datumId, vDatum, datumSpec);
   datumId = 1101;
   vDatum  = (DisFloat64)aLARData.rmaxTOF;
   SetTheVariableData(datumId, vDatum, datumSpec);
   datumId = 1102;
   vDatum  = (DisFloat64)aLARData.rpi;
   SetTheVariableData(datumId, vDatum, datumSpec);
   datumId = 1103;
   vDatum  = (DisFloat64)aLARData.rpiTOF;
   SetTheVariableData(datumId, vDatum, datumSpec);
   datumId = 1104;
   vDatum  = (DisFloat64)aLARData.rNe;
   SetTheVariableData(datumId, vDatum, datumSpec);
   datumId = 1105;
   vDatum  = (DisFloat64)aLARData.rNeTOF;
   SetTheVariableData(datumId, vDatum, datumSpec);
   datumId = 1106;
   vDatum  = (DisFloat64)aLARData.rmin;
   SetTheVariableData(datumId, vDatum, datumSpec);
   datumId = 1107;
   vDatum  = (DisFloat64)aLARData.rminTOF;
   SetTheVariableData(datumId, vDatum, datumSpec);
   datumId   = 1108;
   vU32Datum = ut::safe_cast<DisUint32, int>(aLARData.DMC);
   SetTheVariableData(datumId, vU32Datum, datumSpec);

   pduPtr->SetDatumSpec(datumSpec);

   mDisInterfacePtr->ImmediatePutPdu(mSimulationPtr->GetSimTime(), *pduPtr);

   ut::log::info() << "WSF Weapon Server: Data PDU sent.";
}

//! A function that packs data into a variable record.
//! @param aDatumId      A 32 bit unsigned integer id variable.
//! @param aDatum        The 16 bit unsigned short data value.
//! @param aDataToSend   Output stream to place the data.
void WsfWeaponServer::SetTheVariableData(DisUint32 aDatumId, DisUint16 aDatum, DisDatumSpec& aDataToSend)
{
   DisVariableDatum pieceOfVData;

   unsigned char swapBuf[2];
   GenMemIO      swapData(swapBuf, 2, GenBuf::BigEndian, 2);

   pieceOfVData.SetId(aDatumId);
   swapData.Put(aDatum);
   pieceOfVData.SetData((const unsigned char*)swapBuf, 16);
   aDataToSend.AddVariableDatum(pieceOfVData);
   swapData.Reset();
}

//! A function that packs data into a variable record.
//! @param aDatumId      A 32 bit unsigned integer id variable.
//! @param aDatum        The 32 bit unsigned int data value.
//! @param aDataToSend   Output stream to place the data.
void WsfWeaponServer::SetTheVariableData(DisUint32 aDatumId, DisUint32 aDatum, DisDatumSpec& aDataToSend)
{
   DisVariableDatum pieceOfVData;

   unsigned char swapBuf[4];
   GenMemIO      swapData(swapBuf, 4, GenBuf::BigEndian, 4);

   pieceOfVData.SetId(aDatumId);
   swapData.Put(aDatum);
   pieceOfVData.SetData((const unsigned char*)swapBuf, 32);
   aDataToSend.AddVariableDatum(pieceOfVData);
   swapData.Reset();
}

//! A function that packs data into a variable record.
//! @param aDatumId      A 32 bit unsigned integer id variable.
//! @param aDatum        The 64 bit float data value.
//! @param aDataToSend   Output stream to place the data.
void WsfWeaponServer::SetTheVariableData(DisUint32 aDatumId, DisFloat64 aDatum, DisDatumSpec& aDataToSend)
{
   DisVariableDatum pieceOfVData;

   unsigned char swapBuf[8];
   GenMemIO      swapData(swapBuf, 8, GenBuf::BigEndian, 8);

   pieceOfVData.SetId(aDatumId);
   swapData.Put(aDatum);
   pieceOfVData.SetData((const unsigned char*)swapBuf, 64);
   aDataToSend.AddVariableDatum(pieceOfVData);
   swapData.Reset();
}

void WsfWeaponServer::Update(double /*aSimTime*/)
{
   UpdateAsServer();
}

void WsfWeaponServer::UpdateAsServer()
{
   for (unsigned int i = 0; i < mConnections.size(); ++i)
   {
      GenTCP_IO* lIO = mConnections[i];
      if (!lIO->IsConnected() && mServer->ConnectionCount() > 0)
      {
         mServer->OnDisconnect(lIO); // callback to disconnect
      }
   }
   GenTCP_IO* newConnection = mServer->Accept(0);
   if (newConnection != nullptr)
   {
      if (!newConnection->IsConnected())
      {
         if (mServer->ConnectionCount() > 0)
         {
            mServer->OnDisconnect(newConnection);
         }
      }
      else
      {
         auto i = find(mConnections.begin(), mConnections.end(), newConnection);
         if (i == mConnections.end())
         {
            mConnections.push_back(newConnection);
            auto out = ut::log::info() << "WSF Weapon Server: Accepted connection.";
            out.AddNote() << "Port: " << mPort;
         }
      }
   }
   else
   {
      // else check if any connections have been made, then read the packets.
      for (unsigned int i = 0; i < mConnections.size(); ++i)
      {
         GenTCP_IO* lIO = mConnections[i];
         if (!lIO->IsConnected())
         {
            if (mServer->ConnectionCount() > 0)
            {
               mServer->OnDisconnect(lIO); // callback on disconnect
            }
            continue;
         }

         bool keepGoing = false;
         do
         {
            keepGoing = false;

            lIO->ReceiveContinue(0);
            int bytesAvailable = lIO->GetRecvBuffer()->GetNumValidBytes();
            if (bytesAvailable >= 3)
            {
               // mTotalTimer.ResetClock();
               GenBuffer      buf((char*)lIO->GetRecvBuffer()->GetBuffer(), 1);
               unsigned char* positionEOS =
                  std::find(lIO->GetRecvBuffer()->GetBuffer(), (lIO->GetRecvBuffer()->GetBuffer() + bytesAvailable), '\0');
               int packet_length = (int)(positionEOS - lIO->GetRecvBuffer()->GetBuffer() + 1);
               // cout << "From TCP Buffer packet length = " << packet_length << endl;
               if (bytesAvailable >= packet_length)
               {
                  char* text = new char[packet_length];
                  memcpy(text, buf.GetBuffer(), packet_length);
                  lIO->GetRecvBuffer()->SetGetOffset(packet_length, GenBuf::FromBeg);
                  lIO->RemoveProcessedBufferData();
                  // Set up Send with the current sender for return message
                  mCurrentConnection = lIO;
                  if (ProcessCommand(std::string(text)) == true)
                  {
                     std::string okString = "OK ";
                     if (!this->GetLastWarning().empty())
                     {
                        okString += this->GetLastWarning();
                     }
                     okString += '\0';
                     // success, return "OK \0"  string
                     // lIO->Put("OK \0");
                     lIO->Put(okString.c_str());
                     lIO->Send();
                  }
                  else
                  {
                     // failure, return "FAIL <error string> \0"  string
                     // string errStr = this->GetLastError();  //mErrorString
                     std::ostringstream oss;
                     // oss << "FAIL " << errStr << '\0' << endl;
                     oss << "FAIL " << mErrorString << '\0' << std::endl;
                     lIO->Put(oss.str().c_str());
                     lIO->Send();
                  }
                  keepGoing = true; // see if another command is on the input buffer
               }
            }
         } while (keepGoing);
      }
   }
   UtSleep::SleepMinimum();
   // BARreturn Pause();   //let the thread rest for a second, it will continue after small sleep time
}


//! Execute the ASCIIUpdateEvent
WsfEvent::EventDisposition WsfWeaponServer::TCPUpdateEvent::Execute()
{
   double lSimTime = GetTime(); // Use the event's scheduled time
   mWeaponServerPtr->UpdateAsServer();

   // Set time for next Update
   SetTime(lSimTime + mUpdateTime); // was based on frame time

   return cRESCHEDULE;
}

/*
//WsfThread::FunctionType WsfWeaponServer::Available()
void WsfWeaponServer::Available()
{
   // Sleep for a little bit
   UtSleep::Sleep(GetSleepTime());

   //now go back to work
   AssignWork();
}
*/
WsfWeaponServerInput::WsfWeaponServerInput()
   : mWeaponNameMapTheirsToOurs()
   , mPlatformStationToOurWeaponName()
   , mWeaponEnumToWeaponType()
   , mTrackNumberToDisEntityIdMap()
   , mHostTrue_ClientFalse(true)
   , mWeaponServerIncluded(false)
   , mDebugEnabled(false)
   , mErrorString()
   , mWarningString()
   , mOutGoingHostName()
   , mOutGoingPort(17566)
   , mOutgoingWeaponTypes()
   , mPort(17566)
   , mHostName()
   , mTestRMax(90)
   , mTestRMaxTOF(120)
   , mTestRNe(60)
   , mTestRNeTOF(90)
   , mTestRMin(30)
   , mTestRMinTOF(60)
   , mRpiMultiplier(1.0)
   , mLarMsgTypeIdIn(0)
   , mLarMsgTypeIdOut(0)
   , mUseTestLARData(false)
   , mDebugSetData(false)
{
}
