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
#ifndef L16_LINK16INTERFACE_J11_HPP
#define L16_LINK16INTERFACE_J11_HPP

#include <list>
#include <map>

// L16
#include "wsf_l16_export.h"

#include "Message11_0.hpp"
#include "MessagesPreDef.hpp"

// WSF
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfEvent.hpp"
#include "WsfStringId.hpp"

// Forward declarations
class DisEntityId;
class UtInput;
class WsfDisPlatform;
class WsfMessage;
class WsfPlatform;
class WsfTrack;
class WsfWeapon;
class WsfWeaponEngagement;

namespace WsfL16
{
class Interface;

class WSF_L16_EXPORT Interface_J11_Setup
{
public:
   Interface_J11_Setup();
   bool ProcessInput(UtInput& aInput);
   //! Set the debug state.
   //! @param aDebugEnabled New debug state
   void SetDebugEnabled(bool aDebugEnabled) { mDebugEnabled = aDebugEnabled; }

protected:
   void AddTypeOfNEW(WsfStringId aPlatformType, int aNEW_Type);
   ////! Process pending weapon track number changes
   // void UpdateWeaponTrackNumber(const DisEntityId& aSenderEntityId,
   //                              const unsigned int aSenderTrackNumber,
   //                              const unsigned int aAddresseeTrackNumber);

   bool mDebugEnabled; //! Debug flag
   bool mDebugPrintMessages;

   bool   mAllowAnyComm;               //! True if any comm device can be used
   bool   mUseTimeOfTargetMeasurement; //! True if using J11.1C3 TIME OF TARGET MEASUREMENT
   double mWIFT_Interval;              //! Weapon in-flight track report interval

   //! [Weapon Platform Type String ID, Type of new enum]
   typedef std::pair<WsfStringId, Messages::J11_0::WeaponNEW_TypeField::WeaponNEW_TypeType> TypeOfNEW_MapTypePair;
   typedef std::map<WsfStringId, Messages::J11_0::WeaponNEW_TypeField::WeaponNEW_TypeType>  TypeOfNEW_MapType;
   typedef std::map<WsfStringId, Messages::J11_0::WeaponNEW_TypeField::WeaponNEW_TypeType>::iterator TypeOfNEW_MapTypeIterator;

   TypeOfNEW_MapType mTypeOfNEW_Map;

   typedef std::map<unsigned int, WsfStringId> ControllerMapType;
   ControllerMapType                           mControllerMap;
};

//! WsfL16::Interface_J11 is an interface to DisSignal PDUs conveying J11 messages.
class WSF_L16_EXPORT Interface_J11 : public Interface_J11_Setup
{
public:
   enum J11_MessageType
   {
      cWEAPON_PESPONSE_STATUS, // J11.0
      cWEAPON_DIRECTIVE,       // J11.1
      cWEAPON_COORDINATION     // J11.2
   };

   //! Constructor
   Interface_J11(Interface* aInterfacePtr, Interface_J11_Setup& aScenario);

   //! Virtual destructor
   virtual ~Interface_J11();

   void Initialize();


   //! @name J11 Message Processing
   //@{

   void ProcessJ11Message(const DisEntityId& aSenderEntityId, const Messages::InitialBase& aMessage, int aSubLabel);
   void ProcessJ11_0Message(const DisEntityId& aSenderEntityId, const Messages::InitialBase& aMessage);
   void ProcessJ11_1Message(const DisEntityId& aSenderEntityId, const Messages::InitialBase& aMessage);
   void ProcessJ11_2Message(const DisEntityId& aSenderEntityId, const Messages::InitialBase& aMessage);

   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr);

   void WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);

   void HandoffControlChange(const DisEntityId&           aSenderEntityId,
                             const unsigned int           aSenderTrackNumber,
                             WsfPlatform*                 aWeaponPlatformPtr,
                             const Messages::InitialBase& aMessage);

   void InFlightTargetUpdate(const DisEntityId&           aSenderEntityId,
                             WsfPlatform*                 aWeaponPlatformPtr,
                             const Messages::InitialBase& aMessage);

   //! Event for weapon in-flight track reporting
   double ProcessWIFT_Event(double aSimTime, WsfPlatform* aWeaponPlatformPtr);

   void SendBombHitIndicationReport(WsfPlatform* aWeaponPlatformPtr, WsfPlatform* aTargetPlatformPtr);

   void SendWeaponSelfAbortReport(WsfPlatform* aWeaponPlatformPtr, const std::string& aExtendedResult);
   //@}

   WsfSimulation* GetSimulation() const;

   static UtCallbackListN<void(const DisEntityId&, const unsigned int, unsigned int&)> FindWeaponIndex;

   static UtCallbackListN<void(WsfWeapon*, WsfDisPlatform*&)> UpdateFiringEntity;

private:
   //! Weapon In-flight Track Report
   class WIFT_Event : public WsfEvent
   {
   public:
      WIFT_Event(double aSimTime, Interface_J11* aInterfacePtr, WsfPlatform* aPlatformPtr);

      EventDisposition Execute() override;

   private:
      Interface_J11* mInterfacePtr;
      size_t         mPlatformIndex;
   };

   //! Type definition for several maps in this class
   typedef std::pair<unsigned int, unsigned int>          NEW_MapTypePair;
   typedef std::map<unsigned int, unsigned int>           NEW_MapType;
   typedef std::map<unsigned int, unsigned int>::iterator NEW_MapTypeIterator;

   //! Fills the map with extended result termination strings and
   //! maps them to the J11.0 DetailedStatusInformation enum
   void CreateTerminationResultMap();

   //! Delete data from one of the maps
   bool DeleteNEW_MapData(NEW_MapType& aNEW_Map, const unsigned int aKey);

   //! Delete data from the director to weapon mappings
   bool DeleteDirectorToWeapon(const DisEntityId& aDisEntityId, unsigned int aWeaponTrackNumber);

   void PurgeWeaponRecords(size_t aWeaponPlatformIndex);

   //! Finds minimum-time intercept point; needed for the WIFT message 11.0 C2 word
   bool FindIntercept(const double aWpnLocWCS[3],
                      double       aWpnSpeed,
                      const double aTgtLocWCS[3],
                      const double aTgtVelWCS[3],
                      double       aIntLocWCS[3]);

   //! Fill the WsfTrack given the J11 message
   void FillTrack(const DisEntityId& aSenderEntityId, const Messages::InitialBase& aMessage, WsfTrack* aTrackPtr);

   //! Find a platform from it track number
   WsfPlatform* FindPlatform(unsigned int aTrackNumber);

   //! Convert the string to an enumeration
   Messages::J11_0::WeaponDetailedStatusField::WeaponDetailedStatusType
   GetDetailedStatusInformation(const std::string& aExtendedResult);

   //! Get data about the predicted intercept
   void GetInterceptData(WsfPlatform* aWeaponPlatformPtr, double& aTimeToGo, double& aRangeToGo);

   //! Get data from one of the maps
   bool GetNEW_MapData(NEW_MapType& aNEW_Map, const unsigned int aKey, unsigned int& aValue);

   //! Return the network-enabled weapon type given the platform type
   Messages::J11_0::WeaponNEW_TypeField::WeaponNEW_TypeType GetNEW_Type(WsfPlatform* aWeaponPlatformPtr);

   //! Given the firing platform index return the latest number of weapons fired
   //! This is just a count of the number of weapons fired
   unsigned int GetNumberOfWeaponsFired(unsigned int aFiringPlatformIndex);

   //! Return a pointer to the weapon platform
   WsfPlatform* GetWeapon(const DisEntityId& aSenderEntityId, unsigned int aWeaponTrackNumber);

   //! Return 'true' if the weapon platform has a JTIDS comm device
   bool IsJTIDS_EnabledWeapon(WsfPlatform* aWeaponPlatformPtr);

   //! Return 'true' if the weapon platform has a comm device; may not be able to receive though
   bool IsNetworkEnabledWeapon(WsfPlatform* aWeaponPlatformPtr);

   //! Return 'true' if any sensor on the weapon is on
   bool IsActive(WsfPlatform* aWeaponPlatformPtr);

   //! Return 'true' if any sensor on the weapon is tracking the target
   bool IsTracking(WsfPlatform* aWeaponPlatformPtr);

   //! For JTIDS parts
   void ProcessJMessage(WsfPlatform* aWeaponPlatformPtr, const Messages::InitialBase& aMessage);

   //! Receive the WsfMessage on the weapon platform
   bool ReceiveMessage(WsfPlatform* aWeaponPlatformPtr, WsfMessage& aMessage);

   //! Send the WsfMessage from the weapon platform
   bool SendMessage(WsfPlatform* aWeaponPlatformPtr, Messages::InitialBase* aMessage);

   //! Set a Director (CC or 3PS) to weapon mapping
   bool SetDirectorToWeapon(const DisEntityId& aDisEntityId, unsigned int aWeaponTrackNumber, size_t aWeaponPlatformIndex);

   //! DIS Entity ID to DIS Entity ID mapping
   typedef std::pair<DisEntityId, unsigned int>               EID_MapTypePair;
   typedef std::multimap<DisEntityId, unsigned int>           EID_MapType;
   typedef std::multimap<DisEntityId, unsigned int>::iterator EID_MapTypeIterator;

   ////! Set data in a DIS Entity ID to DIS Entity ID map
   // bool SetEntityIDMapData(EID_MapType&       aEID_Map,
   //                         const DisEntityId& aDisEntityId,
   //                         const unsigned int aValue);

   //! Set the weapon track number in any JTIDS device on the weapon platform
   void SetL16ComputerTrackNumber(WsfPlatform* aWeaponPlatformPtr, unsigned int aWeaponTrackNumber);

   //! Set data in a NEW_MapType
   bool SetNEW_MapData(NEW_MapType& aNEW_Map, const unsigned int aKey, const unsigned int aValue);

   //! Process pending director (CC or 3PS) changes
   void UpdateDirector(const DisEntityId& aSenderEntityId, const unsigned int aSenderTrackNumber);

   ////! Process pending weapon track number changes
   // void UpdateWeaponTrackNumber(const DisEntityId& aSenderEntityId,
   //                              const unsigned int aSenderTrackNumber,
   //                              const unsigned int aAddresseeTrackNumber);
   //
   //        bool             mDebugEnabled;            //! Debug flag
   //        bool             mDebugPrintMessages;
   //
   //        bool             mAllowAnyComm;            //! True if any comm device can be used
   //        bool             mUseTimeOfTargetMeasurement; //! True if using J11.1C3 TIME OF TARGET MEASUREMENT
   //        double           mWIFT_Interval;           //! Weapon in-flight track report interval
   //
   Interface* mInterfacePtr;
   //! [Firing platform index, number of weapons fired]
   NEW_MapType mFiringPlatformToWeaponCountMap;

   //! [Current Controller DIS Entity ID, NEW platform index]  one-to-many
   // EID_MapType mPendingControllerToNEWsMap;

   //! [NEW platform index, weapon firing number/track number]
   NEW_MapType mNEW_TrackNumber;

   //! [NEW platform index, current controller track number]
   NEW_MapType mNEW_ControllerTrackNumber;

   //! [Director (CC or 3PS) track number, NEW platform index]
   NEW_MapType mPendingDirectorTrackNumber;

   //! [[Director (CC or 3PS) Entity ID, weapon track number], weapon platform index]
   typedef std::pair<EID_MapTypePair, size_t>          D_NEW_MapTypePair;
   typedef std::map<EID_MapTypePair, size_t>           D_NEW_MapType;
   typedef std::map<EID_MapTypePair, size_t>::iterator D_NEW_MapTypeIterator;

   D_NEW_MapType mDirectorToWeaponMap;

   //! [Extended Result String ID, DetailedStatusInformation enum]
   typedef std::pair<WsfStringId, Messages::J11_0::WeaponDetailedStatusField::WeaponDetailedStatusType> Result_MapTypePair;
   typedef std::map<WsfStringId, Messages::J11_0::WeaponDetailedStatusField::WeaponDetailedStatusType> Result_MapType;
   typedef std::map<WsfStringId, Messages::J11_0::WeaponDetailedStatusField::WeaponDetailedStatusType>::iterator Result_MapTypeIterator;

   Result_MapType mResultMap;
};
} // namespace WsfL16

#endif
