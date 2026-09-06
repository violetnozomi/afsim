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

#ifndef WSFEXTINTERFACE_HPP
#define WSFEXTINTERFACE_HPP

#include "wsf_export.h"

#include <bitset>
#include <cstdint>
#include <iosfwd>
#include <map>
#include <set>
#include <string>

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "UtRandom.hpp"
#include "WsfComm.hpp"
#include "WsfComponent.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"
#include "WsfStringId.hpp"
#include "ext/WsfExtEntityDeadReckon.hpp"
#include "ext/WsfExtEntityId.hpp"
#include "ext/WsfExtEntityType.hpp"
#include "ext/WsfExtRadioEntityType.hpp"

class UtInput;
class WsfExtEmission;
class WsfObject;
class WsfPlatform;
class WsfScenario;

class WsfExtData;

class WSF_EXPORT WsfExtInterfaceComponent : public WsfComponentT<WsfExtData>
{
public:
   virtual bool CompleteLoad(WsfScenario& aScenario) { return true; }
   virtual void PrintEmitterTypes(WsfScenario& aScenario, std::ostream& aStream) {}
};

//! Ext Interface configuration data to be populated by ProcessInput and copied
//! into WsfExtInterface upon creation
class WSF_EXPORT WsfExtData
{
public:
   using Component     = WsfExtInterfaceComponent;
   using ComponentList = WsfComponentListT<WsfExtInterfaceComponent>;

   WsfExtData(WsfScenario& aScenario);
   virtual ~WsfExtData();

   void                 AddComponent(Component* aComponentPtr) { mComponents.AddComponent(aComponentPtr); }
   const ComponentList& GetComponents() const { return mComponents; }

   virtual bool ProcessInput(WsfScenario& aScenario, UtInput& aInput);

   WsfExtEmission* GetEmission() const { return mEmissionPtr; }

   bool ProcessDeadReckonInput(UtInput& aInput);
   void ProcessEntityType(UtInput& aInput);

   void ProcessRadioEntityType(UtInput& aInput);
   void ProcessExternalEntity(UtInput& aInput);
   void ProcessExternalType(UtInput& aInput);
   // Allow other modules to set a specific entity id value during run-time if a new
   // entity is created after start time = 0 seconds.
   bool ProcessEntityId(std::string aName, int aEntityId);

   void ProcessEntityId(UtInput& aInput);

   void WriteEntityTypes(WsfScenario& aScenario, std::ostream& aStream);
   void WriteEmitterTypes(WsfScenario& aScenario, std::ostream& aStream);

   bool GetEntityType(WsfStringId aObjectTypeId, WsfExtEntityType& aEntityType) const;
   bool GetRadioEntityType(WsfStringId aCommTypeId, WsfExtRadioEntityType& aEntityType) const;

   bool GetEntityType(const WsfObject* aObjectPtr, WsfExtEntityType& aEntityType) const;
   int  GetForceId(WsfStringId aSideId) const;

   WsfStringId GetSideId(int aForceId) const;

   static void ReadEntityType(UtInput& aInput, WsfExtEntityType& aEntityType);
   static void ReadEntityId(UtInput& aInput, WsfExtEntityId& aEntityId);

   static void ReadRadioEntityType(UtInput& aInput, WsfExtRadioEntityType& aEntityType);

   static void ConvertInputToEntityId(std::string& aInput, WsfExtEntityId& aEntityId);


   static void ConvertInputToEntityType(std::string& aInput, WsfExtEntityType& aEntityType);

   static void ConvertInputToRadioEntityType(std::string& aInput, WsfExtRadioEntityType& aEntityType);

   void SelectEntityType(WsfPlatform* aPlatformPtr, WsfExtEntityType& aEntityType) const;

   WsfStringId GetStringId(const WsfExtEntityType& aEntityType) const;

   WsfStringId GetStringId(const WsfExtRadioEntityType& aEntityType) const;

   bool GetRadioEntityType(const wsf::comm::Comm* aCommPtr, WsfExtRadioEntityType& aEntityType) const;


   bool CompleteLoad(WsfScenario& aScenario);

   bool                  mAllPrivate;
   std::set<WsfStringId> mPrivateTypeId;
   std::set<WsfStringId> mPrivateNameId;
   WsfCategoryList       mPrivateCategoryId;
   uint16_t              mStartEntity;
   unsigned short        mSiteId;
   unsigned short        mApplicationId;
   double                mDeferredConnectionTime;
   //! Deferred time before any DIS PDUs can be sent
   double mDeferredPDUSendTime;
   bool   mDebugDetonationEnabled;

   std::map<WsfStringId, double> mTargetPriority;

   WsfStringId mUnknownPlatformTypeId;
   // The following are maps between WSF side and force ID
   std::map<WsfStringId, int> mSideIdToForceId;

   // The following maps a WSF platform type id to entity type.
   std::map<WsfStringId, WsfExtEntityType> mObjectTypeToEntityType;
   std::map<WsfExtEntityType, WsfStringId> mEntityTypeToObjectType;

   // The following maps a WSF Comm type id to entity type.
   std::map<WsfStringId, WsfExtRadioEntityType> mCommTypeToRadioEntityType;
   std::map<WsfExtRadioEntityType, WsfStringId> mRadioEntityTypeToCommType;

   WsfExtEntityDeadReckonSetup mDR_Setup;

   // A bit vector of entity IDs (number) that have been preassigned by the from entity_id.
   std::bitset<65536U> mPreassignedEntityIds;
   // Explicit mappings of a specific platform name to an entity Id (from the entity_id command).
   std::map<WsfStringId, uint16_t> mPlatformNameToEntityId;
   // Set of platforms which are externally moved
   std::set<WsfExtEntityId>   mExternalEntityIds;
   std::set<WsfExtEntityType> mExternalEntityTypes;

   WsfExtEmission* mEmissionPtr;

protected:
   ComponentList mComponents;
};

class WSF_EXPORT WsfExtInput : public WsfScenarioExtension
{
public:
   static WsfExtInput* Find(const WsfScenario& aScenario);

   WsfExtInput(WsfScenario& aScenario);
   ~WsfExtInput() override;

   void SimulationCreated(WsfSimulation& aSimulation) override;
   bool Complete() override;

   //! Process input from client input blocks (DIS, HLA).
   //!@note This method name is intentionally different from
   //! ProcessInput.  We do not want the scenario to process input
   //! for the Ext interface, as it could be at the global level.
   virtual bool    ProcessExtInput(UtInput& aInput) { return mData.ProcessInput(GetScenario(), aInput); }
   WsfExtEmission* GetEmission() { return mData.GetEmission(); }

   void AddedToScenario() override;

   WsfExtData mData;
};

//! The WSF Ext is code which is shared between common external interfaces like DIS and HLA.
class WSF_EXPORT WsfExtInterface : public WsfSimulationExtension, public WsfExtData
{
public:
   enum
   {
      cALL_SITES      = 0xFFFF,
      cALL_APPLIC     = 0xFFFF,
      cALL_ENTITIES   = 0xFFFF,
      cNO_SITES       = 0,
      cNO_APPLIC      = 0,
      cNO_ENTITY      = 0,
      cRQST_ASSIGN_ID = 0xFFFE
   };

   static WsfExtInterface* Find(const WsfSimulation& aSimulation);

   WsfExtInterface(WsfExtInput& aInput);
   ~WsfExtInterface() override;

   void SimulationComplete(double aSimTime);

   bool PrepareExtension() override;

   WsfStringId SelectPlatformType(const WsfExtEntityType& aEntityType);

   static void StripExternalPlatform(WsfPlatform* aPlatformPtr);

   WsfPlatform* CreateExternalPlatform(const std::string& aPlatformType);

   //! Return the type ID of the default platform
   WsfStringId GetDefaultPlatformTypeId() { return mDefaultPlatformTypeId; }
   //! Return the type ID of the platform mapped to unknown types
   WsfStringId GetUnknownPlatformTypeId() { return mUnknownPlatformTypeId; }

   void SelectEntityId(WsfPlatform* aPlatformPtr, uint16_t& aEntityId);
   void FreeEntityId(uint16_t aEntityId);

   bool IsPrivate(const WsfPlatform* aPlatformPtr) const;

   std::set<WsfStringId>& GetPrivateNameId() { return mPrivateNameId; }
   std::set<WsfStringId>& GetPrivateTypeId() { return mPrivateTypeId; }
   WsfCategoryList&       GetPrivateCategoryId() { return mPrivateCategoryId; }

   void AddPrivateType(WsfStringId aTypeId);

   unsigned short GetSiteId() { return mSiteId; }
   unsigned short GetApplicationId() { return mApplicationId; }

   std::map<WsfStringId, WsfExtEntityType>& GetObjectToEntityTypeMap() { return mObjectTypeToEntityType; }
   std::map<WsfExtEntityType, WsfStringId>& GetEntityTypeToObjectMap() { return mEntityTypeToObjectType; }


   bool IsEntityExternallyMoved(const WsfExtEntityId& aEntityId, const WsfExtEntityType& aEntityType);

   uint16_t GetNewEventId();

   //! Get the local random number generator.
   ut::Random& GetRandom() { return mRandom; }

   //! Return the time from the start of the simulation run that external interfaces will open connections.
   double GetDeferredConnectionTime() const { return mDeferredConnectionTime; }

   //! Return the time from the start of the simulation run that DIS PDUs can be sent across external interfaces.
   double GetDeferredPDUSendTime() const { return mDeferredPDUSendTime; }

   WsfExtEntityDeadReckon* GetEntityDeadReckon() const { return mDeadReckonPtr; }

   // Called at the point when external interfaces should be connected.
   // Either at the start of the simulation, or after the deferred connection time.
   UtCallbackListN<void(double)> ConnectionComplete;

   // Allow other modules to access the current entity number.
   uint16_t                         AssignNewEntity();
   std::bitset<65536U>*             PreassignedEntityIds() { return &mPreassignedEntityIds; }
   std::bitset<65536U>*             ActiveEntityIds() { return &mActiveEntityIds; }
   std::map<WsfStringId, uint16_t>* PlatformNameToEntityId() { return &mPlatformNameToEntityId; }

private:
   double Connected(double aSimTime);


   //! The random number generator used for determining the external interface timing.
   //! This is kept separate from the simulation RNG. Otherwise switching on an external
   //! interface on or off would perturb the random number stream.
   ut::Random mRandom;

   UtCallbackHolder mCallbacks;

   bool mIsInitialized;

   bool mShadowAll;

   WsfStringId mDefaultPlatformTypeId;

   WsfExtEntityDeadReckon* mDeadReckonPtr;

   uint16_t mNextEntity;

   uint16_t mNextEventId;

   // A bitset of entity IDs (number) that are currently in-use.
   std::bitset<65536U> mActiveEntityIds;
};

#endif
