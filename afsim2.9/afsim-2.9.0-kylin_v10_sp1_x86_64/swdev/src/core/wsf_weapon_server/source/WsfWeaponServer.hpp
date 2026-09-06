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

#ifndef WSFWEAPONSERVER_HPP
#define WSFWEAPONSERVER_HPP

#include "wsf_weapon_server_export.h"

#include <functional>
#include <iostream>
#include <list>
#include <map>

#include "DisEntityId.hpp"
#include "UtAtmosphere.hpp"
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfEvent.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"
#include "WsfStringId.hpp"
#include "WsfThread.hpp"

// class     GenUDP_IO;
class DisDatumSpec;
class GenTCP_IO;
class PakTCP_Server;
class WsfDisPlatform;
class WsfDisSetData;
class WsfPlatform;
class WsfTrack;
class WsfWeapon;
class WsfWeaponEngagement;
class UtInput;
class UtCallback;
class WsfDisInterface;

struct WSF_WEAPON_SERVER_EXPORT WsfWeaponServerInput
{
   WsfWeaponServerInput();

   // Mapping for weapon keyword to AFNES weapon type
   // Key  = user-defined keyword
   // Data = WSF weapon type
   std::map<WsfStringId, WsfStringId> mWeaponNameMapTheirsToOurs;

   // Mapping for weapon station to weapon type on station
   // Key  = ShooterNameId,Station number
   // Data = WSF weapon type
   std::map<std::pair<WsfStringId, WsfStringId>, WsfStringId> mPlatformStationToOurWeaponName;

   // Weapon mapping for LARs.
   // Key  = Weapon enumeration
   // Data = WSF weapon type
   std::map<int, WsfStringId> mWeaponEnumToWeaponType;

   // Map to match track number to external entities
   // Key  = track number
   // Data = DisEntityId
   std::map<unsigned int, DisEntityId> mTrackNumberToDisEntityIdMap;

   bool                   mHostTrue_ClientFalse; // configuration (is this server a client or a host)
   bool                   mWeaponServerIncluded; // weapon server enabled
   bool                   mDebugEnabled;
   std::string            mErrorString;
   std::string            mWarningString;
   std::string            mOutGoingHostName;
   unsigned int           mOutGoingPort;
   std::list<WsfStringId> mOutgoingWeaponTypes;
   unsigned int           mPort;     // listen for connections on this port
   std::string            mHostName; // listen for connections on this host

   // Test parameters for default return LAR data, used to test communication link only
   double       mTestRMax;
   double       mTestRMaxTOF;
   double       mTestRNe;
   double       mTestRNeTOF;
   double       mTestRMin;
   double       mTestRMinTOF;
   double       mRpiMultiplier;
   unsigned int mLarMsgTypeIdIn;
   unsigned int mLarMsgTypeIdOut;
   bool         mUseTestLARData;
   bool         mDebugSetData;
};

class WSF_WEAPON_SERVER_EXPORT WsfWeaponServerExtension : public WsfScenarioExtension, public WsfWeaponServerInput
{
public:
   WsfWeaponServerExtension();
   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;
};

//! Implementation of worker thread
// The weapon server is not thread-safe.  Taking out that functionality until it gets reworked.
// TODO: Make thread-safe public WsfThread
class WSF_WEAPON_SERVER_EXPORT WsfWeaponServer : public WsfWeaponServerInput, public WsfSimulationExtension
{
public:
   typedef UtCallbackListN<void(WsfPlatform*, unsigned int, double, int, bool)> PostLaunchCallback;
   static PostLaunchCallback                                                    PostLaunch;

   typedef UtCallbackListN<void(bool)> WeaponServerEnabledCallback;
   static WeaponServerEnabledCallback  WeaponServerEnabled;

   // useful as enum or bit mask
   enum IdentifierType
   {
      NO_IDENTIFIER  = 0x00,
      BY_NAME        = 0x01,
      BY_DIS_ID      = 0x02,
      BY_TAIL_NUMBER = 0x08,
      BY_LAT         = 0x10,
      BY_LON         = 0x20,
      BY_LOCATION    = 0x30,
   };

   enum ValidityType
   {
      cNO_IDENTIFIER = 0x00,
      cFREQ          = 0x01,
      cMID           = 0x02,
      cPOSN          = 0x04,
      cPOSE          = 0x08,
      cPOSD          = 0x10,
      cVELN          = 0x20,
      cVELE          = 0x40,
      cVELD          = 0x80
   };

   struct LARInputType
   {
      double ownshipLLA[3];
      double ownshipVelWCS[3];
      double targetLLA[3];
      double targetVelWCS[3];
      int    weaponEnum;
      int    testCase;
      int    DMC;
   };
   struct LAROutputType
   {
      double rmax;
      double rmaxTOF;
      double rpi;
      double rpiTOF;
      double rNe;
      double rNeTOF;
      double rmin;
      double rminTOF;
      int    DMC;
   };

   struct MissileIdType
   {
      unsigned int   mode;               // 0=init; 1=umbilical; 2=launch; 3=delete; 4=uplink; 5=update
      unsigned short missileIdentity[3]; // site; app; entity
   };

   struct InitVariablesType
   {
      MissileIdType missileIdentity;
      int           randomNumberSeed; //
      unsigned char launchStation[2]; // e.g. "2A"
      float         mountingAngle[3]; // yaw, pitch, roll; Radians
      float         timeThreshold;    // DR time setting [s]
      double        simTime;          // Simulation time; Seconds
   };

   struct UmbilicalVariablesType
   {
      MissileIdType   misslieIdentity;
      float           ejectVelocity[3];        // NED; Meters/second
      float           alignmentUncertainty[3]; // yaw, pitch, roll; Radians
      int             engagementOrderNumber;   // 0=visual launch, 1-4
      double          firstDatalinkTime;       // relative to launch; Seconds
      float           radarResolutions;        // range, range rate, azimuth, elevation; Radians
      unsigned short  uncertaintyTableType;    // 0=coupled; 1=decoupled; 2=decoupled/expanded
      float*          uncertaintyTable;        //
      unsigned short* uncertaintiyIndices;     // 1-n
      short           rcsSize;                 //
      short           physicalSize;            //
      double          cuePosition[3];          // NED relative to stake; Meters
      float           cueVelocity[3];          // NED; Meters/second
      unsigned short  missileNumber;           //
      double          freqA;                   // MHz
      double          freqB;                   // MHz
      unsigned short  enableFreqB;             // 0=disabled; 1=enabled
      short           launchMode;              //
      short           launchFlags[3];          //
   };

   struct LaunchVariablesType
   {
      MissileIdType missileId;
      short         targetIdentifiers[3];
      double        targetPosition[3]; // WGS84; Meters
      float         targetAngle[3];    // NED; Radians
      float         targetRCS[3];      // square meters
   };

   struct TelemetryVariablesType
   {
      MissileIdType missileId;
      short         missileMode;
      short         missileTerminationCode;
      float         pk; // <0-1>
      short         thrustPhase;
      float         tof;                  // Seconds
      short         targetIndex;          // <1-6>
      float         missileMach;          //
      float         missileAngleOfAttack; // Degrees;
      double        simTime;              // Missile simulation time; Seconds
      float         missileGs;
   };

   struct LaunchMessageType
   {
      MissileIdType missileId;
      int           launchType; // 0=air-to-air; 1=air-to-ground
      int           missileIdentifier;
      int           frequency;          // Mhz
      int           targetPositionType; // 0=NED; 1=LLA
      double        targetPosition[3];
      float         targetVelocity[3];     // NED; Meters/second
      int           controllerTrackNumber; //
      int           tailNumber[2];         // [0]=tailNumber; [1]=0 - do not use tail number is DIS PDUs
      //                    =1 - use tail number if Dis Pdu entity field
   };

   static WsfWeaponServer* Find(const WsfSimulation& aSimulation);

   static void GetWeaponTrackNumbers(WsfSimulation& aSimulation,
                                     size_t         aWeaponPlatformIndex,
                                     unsigned int&  aWeaponTrackNumber,
                                     unsigned int&  aControllerTrackNumber);

   //! Constructor and destructor
   WsfWeaponServer(WsfWeaponServerInput& aScenario, WsfSimulation* aSimulationPtr, WsfDisInterface* aDisInterfacePtr);

   ~WsfWeaponServer() override;

   // The weapon server is not thread-safe.
   // Implementation of abstract base class methods.
   // TODO: Make thread-safe virtual FunctionType DoWork();
   // TODO: Make thread-safe virtual FunctionType Pause() { return AVAILABLE; }
   // TODO: Make thread-safe virtual void         Available();   //not a pure virtual, overriding.

   bool Initialize() override;

   // Added to keep weapon server from running as thread until logic is made thread-safe
   void AddedToSimulation() override;

   // Added to keep weapon server from running as thread until logic is made thread-safe
   // Methods for TCP connection
   void Update(double aSimTime);
   // Added to keep weapon server from running as thread until logic is made thread-safe
   void UpdateAsServer();

   bool IsConnected();

   bool        ProcessCommand(const std::string& command);
   std::string GenerateCommand(const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTrackPtr);
   // should return a valid description string if ProcessCommand() returns false
   std::string GetLastError() { return mErrorString; }
   std::string GetLastWarning() { return mWarningString; }

   double GetCurrentFrequency() { return mCurrentFrequency; }
   int    GetCurrentId() { return mCurrentWpnId; }

   // Track data was insufficient, create a point out into space at which to aim
   WsfTrack* CreateATargetPoint(double aSimTime, WsfPlatform* aPlatformPtr, double aAtRangeNM = 40.0);

   // Track data was insufficient, create a point out into space at which to aim
   // from known n, e, d components.
   WsfTrack* CreateATargetPoint(double aSimTime, WsfPlatform* aPlatformPtr, double aPosNM, double aPosEM, double aPosDM);

   unsigned int DecimalToOctal(unsigned int aDecimalValue);

   // callbacks, the simulation hooks into the weapon server with these methods
   void SendToOtherWeaponServer(double                     aSimTime,
                                WsfPlatform*               aPlatformPtr,
                                const WsfWeaponEngagement* aEngagementPtr,
                                const WsfTrack*            aTrackPtr);
   bool GetWeaponServerNumbers(const size_t weaponIndex, unsigned int& weaponTnumber, unsigned int& controllerTnumber);

   void PlatformAdded(double aTime, WsfPlatform* aPlatformPtr);
   void PlatformDeleted(double aTime, WsfPlatform* aPlatformPtr);
   void PlatformInitialized(double aTime, WsfPlatform* aPlatformPtr);

   void RetrieveErrorMessage(std::string& aErrorString) { aErrorString = mErrorString; }

   bool GetWeaponServerIncluded() const { return mWeaponServerIncluded; }
   void SetWeaponServerIncluded(bool aBool) { mWeaponServerIncluded = aBool; }
   void InitiateCallbacks();

   class UniqueWeaponKey
   {
   public:
      UniqueWeaponKey(DisEntityId aShooter, unsigned int aControllerNum, unsigned int aWeaponNum)
         : mShooter(aShooter)
         , mControllerNum(aControllerNum)
         , mWeaponNum(aWeaponNum)
      {
      }

      bool operator<(const UniqueWeaponKey& aRhs) const
      {
         if (mShooter < aRhs.mShooter)
         {
            return true;
         }
         if (mShooter != aRhs.mShooter)
         {
            return false;
         } // its not less than & not equal, so its greater than
         if (mControllerNum < aRhs.mControllerNum)
         {
            return true;
         }
         if (mControllerNum > aRhs.mControllerNum)
         {
            return false;
         }
         if (mWeaponNum < aRhs.mWeaponNum)
         {
            return true;
         }
         return false;
      }

      DisEntityId  mShooter;
      unsigned int mControllerNum;
      unsigned int mWeaponNum;
   };

   std::map<UniqueWeaponKey, size_t>* GetPlatformIndexMap() { return &mPlatformIndexMap; }

   std::map<size_t, unsigned int>* GetWeaponTrackNumberMap() { return &mWeaponTrackNumberMap; }

   std::map<size_t, unsigned int>* GetControllerTrackNumberMap() { return &mControllerTrackNumberMap; }

   static std::function<unsigned int(WsfPlatform&)> GetControllerTrackNumber;

   // Returns pointer to weapon name map.  Maps an external application weapon name to one defined in the
   // WSF input files. Used with the wsf_weapon_server.
   std::map<WsfStringId, WsfStringId>* GetWeaponNameMap() { return &mWeaponNameMapTheirsToOurs; }

   std::map<std::pair<WsfStringId, WsfStringId>, WsfStringId>* GetPlatformStationToOurWeaponNameMap()
   {
      return &mPlatformStationToOurWeaponName;
   }

   // Be able to set the simulation pointer when the Register_wsf_weapon_server does not get called.
   // Will be the case when the ASCII Interface is being used and the 'wsf_weapon_server' block in not present in any input file.
   void           SetSimulation(WsfSimulation* aSimulationPtr) { mSimulationPtr = aSimulationPtr; }
   WsfSimulation& GetSimulation();

   //! Set commander on weapon, first look for the CTRL_TRACK_NUM platform, if not set, then use firing platform
   //! aPlatformPtr Weapon platform pointer
   //! returns 'true' if commander platform found
   bool SetCommander(WsfPlatform* aPlatformPtr);

private:
   // Write debug statements to console output to debug LAR interface
   bool DebugSetData() const { return mDebugSetData; }
   // Convert string DisEntityId to DisEntityId
   bool GetDisEntityId(const std::string& aStr, DisEntityId& aDisEntityId);

   // Returns LAR data based on own ship and target inputs
   bool GetLARData(LARInputType aLARInput, LAROutputType& aLAROutput);

   // Process any Set Data PDU coming across the DIS network.
   void HandleSetDataPDU(WsfDisInterface* aInterfacePtr, const WsfDisSetData& aPdu);

   void SendLARData(const WsfDisSetData& aPdu, LAROutputType aLARData);

   void SetTheVariableData(DisUint32 aDatumId, DisUint16 aDatum, DisDatumSpec& aDataToSend);

   void SetTheVariableData(DisUint32 aDatumId, DisUint32 aDatum, DisDatumSpec& aDataToSend);

   void SetTheVariableData(DisUint32 aDatumId, DisFloat64 aDatum, DisDatumSpec& aDataToSend);

   // Added to keep weapon server from running as thread until logic is made thread-safe
   //! TCPUpdateEvent is executed at a regular interval
   //! allow the update of the TCP socket when in non-realtime mode
   class TCPUpdateEvent : public WsfEvent
   {
   public:
      EventDisposition Execute() override;

      double           mUpdateTime;
      WsfWeaponServer* mWeaponServerPtr;
   };

   bool mWeaponServerRequest; // set if the release store command was issued because of a weapon server request.
   bool mWeaponDisidOverride; // set to 'true' if the user sent a new disid for the weapon
   bool mWeaponIdOverride;    // set to 'true' if the user sent a new id only for a weapon
   bool mUseWpnIdInPDUs;      // set to 'true' if the user sent a new id for a weapon and this id is to be used
   // by all associated DIS PDUs.
   bool mWarningStringSet; // set to 'true' if an error occurred during processing

   // properties for weapon server host
   PakTCP_Server*                    mServer; // listens for & initializes any connections to the weapon server host
   std::vector<GenTCP_IO*>           mConnections; // current connections (from other clients to this host)
   GenTCP_IO*                        mCurrentConnection;
   UtAtmosphere                      mAtm;
   std::map<size_t, DisEntityId>     mShooterIdMap;
   std::map<size_t, unsigned int>    mControllerTrackNumberMap;
   std::map<size_t, unsigned int>    mWeaponTrackNumberMap;
   std::map<UniqueWeaponKey, size_t> mPlatformIndexMap;

   DisEntityId  mCurrentShooterId;
   DisEntityId  mCurrentWeaponId;
   unsigned int mCurrentControllerTrackNumber;
   unsigned int mCurrentWeaponTrackNumber;
   double       mCurrentFrequency;
   int          mCurrentWpnId;

   // properties for weapon server client
   GenTCP_IO*   mOutgoingClientConnection;
   unsigned int mOutgoingWeaponTrackNumbers; // ever increasing counter for track number (tail number) IDs

   UtCallbackHolder mCallbacks;
   WsfDisInterface* mDisInterfacePtr;
   WsfSimulation*   mSimulationPtr;
   WsfPlatform*     mWeaponPlatformPtr;
};

#endif
