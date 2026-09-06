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

#include "ext/WsfExtInterface.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfComm.hpp"
#include "WsfCommandChain.hpp"
#include "WsfException.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfProcessor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorTypes.hpp"
#include "WsfSimulation.hpp"
#include "ext/WsfExtEmission.hpp"
#include "ext/WsfExtEntityDeadReckon.hpp"
#include "ext/WsfExtEntityType.hpp"
#include "ext/WsfExtRadioEntityType.hpp"
#include "ext/WsfExtScriptExtensions.hpp"

class WsfExtApplicationExtension : public WsfApplicationExtension
{
public:
   void AddedToApplication(WsfApplication& aApplication) override
   {
      // Register new script classes and add new methods to existing script classes.
      UtScriptTypes* scriptTypesPtr = aApplication.GetScriptTypes();
      scriptTypesPtr->Register(WsfExtEntityType::CreateScriptClass("EntityType", scriptTypesPtr));
      wsf::ext::ScriptExtensions::ExtendScriptClasses(*scriptTypesPtr);
   }

   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<WsfExtInput>(aScenario));
   }
};

void WSF_EXPORT Register_ext_interface(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("ext_interface"))
   {
      aApplication.RegisterExtension("ext_interface", ut::make_unique<WsfExtApplicationExtension>());
   }
}

namespace
{
// Helper method for ReadEntityId and ReadEntityType.
// It ensures that the input stream is valid and nothing remains except trailing whitespace.
bool InputIsValid(std::istream& aIn)
{
   bool isValid = false;
   if (aIn)
   {
      // make sure there is nothing but trailing white space
      isValid = true;
      int nextChar;
      while ((nextChar = aIn.get()) != EOF)
      {
         if (!isspace(nextChar))
         {
            isValid = false;
            break;
         }
      }
   }
   return isValid;
}
} // namespace

WsfExtData::WsfExtData(WsfScenario& aScenario)
   : mAllPrivate(false)
   , mStartEntity(1)
   , mSiteId(1)
   , mApplicationId(1)
   , mDeferredConnectionTime(0.0)
   , mDeferredPDUSendTime(0.0)
   , mDebugDetonationEnabled(false)
   , mUnknownPlatformTypeId()
   , mEmissionPtr(new WsfExtEmission(aScenario))
{
   mStartEntity            = 1;
   mDeferredConnectionTime = 0.0;
   mDebugDetonationEnabled = false;
}

WsfExtData::~WsfExtData()
{
   delete mEmissionPtr;
}

bool WsfExtData::ProcessInput(WsfScenario& aScenario, UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "entity_id")
   {
      ProcessEntityId(aInput);
   }
   else if (command == "entity_type")
   {
      ProcessEntityType(aInput);
   }
   else if (command == "radio_entity_type")
   {
      ProcessRadioEntityType(aInput);
   }
   else if (command == "unknown_platform_type")
   {
      std::string platformType;
      aInput.ReadValue(platformType);
      mUnknownPlatformTypeId = platformType;
   }
   else if (command == "force")
   {
      std::string side;
      int         forceId;
      aInput.ReadValue(side);
      aInput.ReadValue(forceId);
      aInput.ValueInClosedRange(forceId, 0, 255);
      mSideIdToForceId[side] = forceId;
   }
   else if (command == "enumerate")
   {
      std::string what;
      std::string to;
      std::string fileName;
      aInput.ReadValue(what);
      aInput.ReadValue(to);
      aInput.StringEqual(to, "to");
      aInput.ReadValueQuoted(fileName);
      fileName                = aInput.SubstitutePathVariables(fileName);
      std::ostream* targetPtr = &std::cout;
      std::ofstream outFile;
      if (fileName != "STDOUT")
      {
         outFile.open(fileName.c_str());
         if (!outFile.is_open())
         {
            throw UtInput::BadValue(aInput, "Cannot open file: " + fileName);
         }
         targetPtr = &outFile;
      }

      if (what == "entity_types")
      {
         WriteEntityTypes(aScenario, *targetPtr);
      }
      else if (what == "emitter_types")
      {
         WriteEmitterTypes(aScenario, *targetPtr);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown enumeration type: " + what);
      }
   }
   else if (command == "private")
   {
      // look for qualifier
      std::string qualifier;
      aInput.ReadValue(qualifier);
      if (qualifier == "type")
      {
         std::string type;
         aInput.ReadValue(type);
         mPrivateTypeId.insert(type);
      }
      else if (qualifier == "name")
      {
         std::string name;
         aInput.ReadValue(name);
         mPrivateNameId.insert(name);
      }
      else if (qualifier == "category")
      {
         std::string category;
         aInput.ReadValue(category);
         mPrivateCategoryId.JoinCategory(category);
      }
      else if (qualifier == "all")
      {
         mAllPrivate = true;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Expected 'type', 'name', or 'all'");
      }
   }
   else if (command == "site")
   {
      int siteId;
      aInput.ReadValue(siteId);
      aInput.ValueInClosedRange(siteId, 0, 65535);
      mSiteId = (unsigned short)siteId;
   }
   else if (command == "application")
   {
      int applicationId;
      aInput.ReadValue(applicationId);
      aInput.ValueInClosedRange(applicationId, 0, 65535);
      mApplicationId = (unsigned short)applicationId;
   }
   else if (command == "map_external_entity")
   {
      ProcessExternalEntity(aInput);
   }
   else if (command == "map_external_type")
   {
      ProcessExternalType(aInput);
   }
   else if (command == "deferred_connection_time")
   {
      aInput.ReadValueOfType(mDeferredConnectionTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mDeferredConnectionTime, 1.0);
   }
   else if (command == "deferred_pdu_send_time")
   {
      aInput.ReadValueOfType(mDeferredPDUSendTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mDeferredPDUSendTime, 0.0);
   }
   else if (command == "start_entity")
   {
      int startEntity;
      aInput.ReadValue(startEntity);
      mStartEntity = static_cast<unsigned short>(startEntity);
      aInput.ValueInClosedRange<unsigned short>(mStartEntity, 1U, 65534U);
   }
   else if (command == "target_priority")
   {
      std::string platformType;
      double      priority;
      aInput.ReadValue(platformType);
      aInput.ReadValue(priority);
      aInput.ValueGreater(priority, 0.0);
      mTargetPriority[platformType] = priority;
   }
   else if (ProcessDeadReckonInput(aInput))
   {
   }
   else if (mEmissionPtr->ProcessInput(aInput))
   {
   }
   else if (mComponents.ProcessComponentInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Return the force ID that corresponds to a given WSF side ID.
int WsfExtData::GetForceId(WsfStringId aSideId) const
{
   int  forceId = 0;
   auto iter    = mSideIdToForceId.find(aSideId);
   if (iter != mSideIdToForceId.end())
   {
      forceId = iter->second;
   }
   return forceId;
}

//! Return the WSF side ID that corresponds to the given force ID.
WsfStringId WsfExtData::GetSideId(int aForceId) const
{
   // We don't maintain a separate force ID to side ID map because the user
   // may have mapped multiple sides to one force.  Therefore, we'll just use the
   // side ID to force ID map and use the first entry where the force ID's match
   // and use the side ID from it.

   WsfStringId                                sideId = "green";
   std::map<WsfStringId, int>::const_iterator iter;
   for (iter = mSideIdToForceId.begin(); iter != mSideIdToForceId.end(); ++iter)
   {
      if (aForceId == iter->second)
      {
         sideId = iter->first;
         break;
      }
   }
   return sideId;
}


// private
void WsfExtData::ProcessEntityType(UtInput& aInput)
{
   std::string typeString;
   aInput.ReadValue(typeString);

   WsfExtEntityType entityType;
   ReadEntityType(aInput, entityType);

   WsfStringId typeId                  = typeString;
   mObjectTypeToEntityType[typeId]     = entityType;
   mEntityTypeToObjectType[entityType] = typeId;
}

// private
void WsfExtData::ProcessRadioEntityType(UtInput& aInput)
{
   std::string typeString;
   aInput.ReadValue(typeString);

   WsfExtRadioEntityType entityType;
   ReadRadioEntityType(aInput, entityType);

   WsfStringId typeId                     = typeString;
   mCommTypeToRadioEntityType[typeId]     = entityType;
   mRadioEntityTypeToCommType[entityType] = typeId;
}

//! Read a Entity Type from an input stream.
//! @param aInput      The input stream
//! @param aEntityType The value read from the stream.
// static
void WsfExtData::ReadEntityType(UtInput& aInput, WsfExtEntityType& aEntityType)
{
   std::string entityTypeString;
   aInput.ReadValue(entityTypeString);

   WsfExtInterface::ConvertInputToEntityType(entityTypeString, aEntityType);
}

// static
void WsfExtData::ReadEntityId(UtInput& aInput, WsfExtEntityId& aEntityId)
{
   std::string entityIdString;
   aInput.ReadValue(entityIdString);
   WsfExtInterface::ConvertInputToEntityId(entityIdString, aEntityId);
}

//! Read a  Radio Entity Type from an input stream.
//! @param aInput           The input stream
//! @param aRadioEntityType The value read from the stream.
// static
void WsfExtData::ReadRadioEntityType(UtInput& aInput, WsfExtRadioEntityType& aRadioEntityType)
{
   std::string radioEntityTypeString;
   aInput.ReadValue(radioEntityTypeString);

   WsfExtInterface::ConvertInputToRadioEntityType(radioEntityTypeString, aRadioEntityType);
}

void WsfExtData::ConvertInputToEntityType(std::string& aInput, WsfExtEntityType& aEntityType)
{
   std::istringstream iss(aInput.c_str());
   unsigned int       field[7] = {0, 0, 0, 0, 0, 0, 0};
   char               fieldSep[6];
   iss >> field[0] >> fieldSep[0] >> field[1] >> fieldSep[1] >> field[2] >> fieldSep[2] >> field[3] >> fieldSep[3] >>
      field[4] >> fieldSep[4] >> field[5] >> fieldSep[5] >> field[6];
   if (InputIsValid(iss) && (field[0] <= 0xff) && ((fieldSep[0] == ':') || (fieldSep[0] == '.') || (fieldSep[0] == '-')) &&
       (field[1] <= 0xff) && (fieldSep[1] == fieldSep[0]) && (field[2] <= 0xffff) && (fieldSep[2] == fieldSep[0]) &&
       (field[3] <= 0xff) && (fieldSep[3] == fieldSep[0]) && (field[4] <= 0xff) && (fieldSep[4] == fieldSep[0]) &&
       (field[5] <= 0xff) && (fieldSep[5] == fieldSep[0]) && (field[6] <= 0xff))
   {
      aEntityType.mEntityKind  = static_cast<uint8_t>(field[0]);
      aEntityType.mDomain      = static_cast<uint8_t>(field[1]);
      aEntityType.mCountry     = static_cast<uint16_t>(field[2]);
      aEntityType.mCategory    = static_cast<uint8_t>(field[3]);
      aEntityType.mSubcategory = static_cast<uint8_t>(field[4]);
      aEntityType.mSpecific    = static_cast<uint8_t>(field[5]);
      aEntityType.mExtra       = static_cast<uint8_t>(field[6]);
   }
   else
   {
      throw UtException("Bad Entity Type: '" + aInput + "'");
   }
}

void WsfExtData::ConvertInputToRadioEntityType(std::string& aInput, WsfExtRadioEntityType& aEntityType)
{
   std::istringstream iss(aInput.c_str());
   unsigned int       field[6] = {0, 0, 0, 0, 0, 0};
   char               fieldSep[5];
   iss >> field[0] >> fieldSep[0] >> field[1] >> fieldSep[1] >> field[2] >> fieldSep[2] >> field[3] >> fieldSep[3] >>
      field[4] >> fieldSep[4] >> field[5];
   if (InputIsValid(iss) && (field[0] <= 0xff) &&
       ((fieldSep[0] == ':') || (fieldSep[0] == '.') || (fieldSep[0] == '-')) && (field[1] <= 0xff) &&
       (fieldSep[1] == fieldSep[0]) && (field[2] <= 0xffff) && (fieldSep[2] == fieldSep[0]) && (field[3] <= 0xff) &&
       (fieldSep[3] == fieldSep[0]) && (field[4] <= 0xff) && (fieldSep[4] == fieldSep[0]) && (field[5] <= 0xffff))
   {
      aEntityType.mEntityKind          = static_cast<uint8_t>(field[0]);
      aEntityType.mDomain              = static_cast<uint8_t>(field[1]);
      aEntityType.mCountry             = static_cast<uint16_t>(field[2]);
      aEntityType.mCategory            = static_cast<uint8_t>(field[3]);
      aEntityType.mNomenclatureVersion = static_cast<uint8_t>(field[4]);
      aEntityType.mNomenclature        = static_cast<uint16_t>(field[5]);
   }
   else
   {
      throw UtException("Bad Radio Entity Type: '" + aInput + "'");
   }
}

bool WsfExtData::GetEntityType(WsfStringId aObjectTypeId, WsfExtEntityType& aEntityType) const
{
   bool found = false;
   auto iter  = mObjectTypeToEntityType.find(aObjectTypeId);
   if (iter != mObjectTypeToEntityType.end())
   {
      aEntityType = iter->second;
      found       = true;
   }
   return found;
}

//! Given an object, select a entity type.
bool WsfExtData::GetEntityType(const WsfObject* aObjectPtr, WsfExtEntityType& aEntityType) const
{
   bool found = false;

   // Iterate over the type-hierarchy of the object from most-to-least specific to see if an entity type has been defined.
   // The entity type will be that from the first object type for which an associated entity type has been defined.

   const WsfObject::TypeList& typeList = aObjectPtr->GetTypeList();
   for (auto tli : typeList)
   {
      auto iter = mObjectTypeToEntityType.find(tli);
      if (iter != mObjectTypeToEntityType.end())
      {
         found       = true;
         aEntityType = iter->second;
         break;
      }
   }
   return found;
}

bool WsfExtData::GetRadioEntityType(WsfStringId aCommTypeId, WsfExtRadioEntityType& aEntityType) const
{
   bool found = false;
   auto iter  = mCommTypeToRadioEntityType.find(aCommTypeId);
   if (iter != mCommTypeToRadioEntityType.end())
   {
      aEntityType = iter->second;
      found       = true;
   }
   return found;
}
//! Given an object, select a entity type.
bool WsfExtData::GetRadioEntityType(const wsf::comm::Comm* aCommPtr, WsfExtRadioEntityType& aEntityType) const
{
   bool found = false;

   // Iterate over the type-hierarchy of the object from most-to-least specific to see if an entity type has been defined.
   // The entity type will be that from the first object type for which an associated entity type has been defined.

   const WsfObject::TypeList& typeList = aCommPtr->GetTypeList();
   for (auto tli : typeList)
   {
      auto iter = mCommTypeToRadioEntityType.find(tli);
      if (iter != mCommTypeToRadioEntityType.end())
      {
         found       = true;
         aEntityType = iter->second;
         break;
      }
   }
   return found;
}

WsfStringId WsfExtData::GetStringId(const WsfExtEntityType& aEntityType) const
{
   auto iter = mEntityTypeToObjectType.find(aEntityType);
   if (iter != mEntityTypeToObjectType.end())
   {
      return iter->second;
   }
   return nullptr;
}

WsfStringId WsfExtData::GetStringId(const WsfExtRadioEntityType& aEntityType) const
{
   auto iter = mRadioEntityTypeToCommType.find(aEntityType);
   if (iter != mRadioEntityTypeToCommType.end())
   {
      return iter->second;
   }
   return nullptr;
}

//! Prints entity type mapping for each platform type
void WsfExtData::WriteEntityTypes(WsfScenario& aScenario, std::ostream& aStream)
{
   WsfPlatformTypes&                 platformTypeList = WsfPlatformTypes::Get(aScenario);
   WsfObjectTypeListBase::TypeIdList platformTypes;
   platformTypeList.GetTypeIds(platformTypes);
   aStream << "Entity Types:\n";
   std::string divider(75, '-');
   aStream << divider << '\n';
   for (auto& i : platformTypes)
   {
      const std::string& platformType = i;
      WsfExtEntityType   entityType;
      WsfPlatform*       platformPtr = platformTypeList.Find(platformType);
      // Platform name is the type.  Use GetEntityType() for sub-types.
      // WsfExtInterface::GetInstance().GetObjectToEntityTypeMap()
      WsfExtEntityType extEntityType;
      if (!GetEntityType((WsfStringId)platformPtr->GetNameId(), extEntityType))
      {
         GetEntityType(platformPtr, entityType);
      }
      aStream << std::setw(30) << platformType << std::setw(20) << entityType << " (" << GetStringId(entityType) << ")\n";
   }
   aStream << divider << '\n';
}

//! Prints entity type mapping for each platform type
void WsfExtData::WriteEmitterTypes(WsfScenario& aScenario, std::ostream& aStream)
{
   WsfObjectTypeListBase::TypeIdList sensorTypes;
   WsfSensorTypes::Get(aScenario).GetTypeIds(sensorTypes);
   aStream << "Emitter Types:\n";
   std::string divider(75, '-');
   aStream << divider << '\n';
   int typeCount = static_cast<int>(sensorTypes.size());
   for (int i = 0; i < typeCount; ++i)
   {
      std::string typeName;
      int         index = i;
      WsfStringId partType;
      partType             = sensorTypes[index];
      typeName             = partType;
      WsfSensor* sensorPtr = WsfSensorTypes::Get(aScenario).Find(typeName);
      bool       isEmitter = (sensorPtr->IsClassRadio() && sensorPtr->IsClassActive());
      if (isEmitter)
      {
         uint16_t    emitterType = mEmissionPtr->GetEmitterType(partType);
         WsfStringId partTypeId  = mEmissionPtr->GetPartTypeId(emitterType);
         aStream << std::setw(30) << typeName << std::setw(20) << emitterType << " (" << partTypeId << ")\n";
      }
   }
   for (auto component : mComponents)
   {
      component->PrintEmitterTypes(aScenario, aStream);
   }
   aStream << divider << '\n';
}

//! Given a platform, select an entity type.
void WsfExtData::SelectEntityType(WsfPlatform* aPlatformPtr, WsfExtEntityType& aEntityType) const
{
   // Try first with the platform type and then with the icon.
   if (!GetEntityType(aPlatformPtr, aEntityType))
   {
      auto iter = mObjectTypeToEntityType.find(aPlatformPtr->GetIconId());
      if (iter != mObjectTypeToEntityType.end())
      {
         aEntityType = iter->second;
      }
      else
      {
         aEntityType = WsfExtEntityType();
      }
   }
}

// private
void WsfExtData::ProcessEntityId(UtInput& aInput)
{
   std::string name;
   int         entityId;

   aInput.ReadValue(name);
   aInput.ReadValue(entityId);
   bool ok = ProcessEntityId(name, entityId);
   if (!ok)
   {
      throw UtInput::BadValue(aInput, "Invalid or preassigned entity ID specified");
   }
}

// public
bool WsfExtData::ProcessEntityId(std::string aName, int aEntityId)
{
   bool ok = true;
   if ((aEntityId >= 0) && (aEntityId <= 0xFFFF))
   {
      // Make sure the specified ID is not a reserved value or one of the already preassigned values.
      if ((aEntityId == WsfExtInterface::cNO_ENTITY) || (aEntityId == WsfExtInterface::cALL_ENTITIES) ||
          (aEntityId == WsfExtInterface::cRQST_ASSIGN_ID))
      {
         ok = false;
      }
      if (ok)
      {
         WsfStringId nameId = aName;

         auto iter = mPlatformNameToEntityId.find(nameId);
         if (iter != mPlatformNameToEntityId.end())
         {
            // Name is already mapped. Remove the existing mapping so it can be reassigned.
            mPreassignedEntityIds.reset(iter->second);
            mPlatformNameToEntityId.erase(iter);
         }

         // Make sure the specified entity has not already been reserved.
         if (!mPreassignedEntityIds.test(aEntityId))
         {
            mPlatformNameToEntityId.insert(std::make_pair(nameId, aEntityId));
            mPreassignedEntityIds.set(aEntityId);
         }
         else
         {
            return false;
         }
      }
   }
   else
   {
      ok = false;
   }
   return ok;
}

bool WsfExtData::ProcessDeadReckonInput(UtInput& aInput)
{
   std::string command = aInput.GetCommand();
   if (command == "entity_orientation_threshold")
   {
      aInput.ReadValueOfType(mDR_Setup.mEntityOrientationThreshold, UtInput::cANGLE);
      aInput.ValueGreater(mDR_Setup.mEntityOrientationThreshold, 0.0);
   }
   else if (command == "entity_position_threshold")
   {
      aInput.ReadValueOfType(mDR_Setup.mEntityPositionThreshold, UtInput::cLENGTH);
      aInput.ValueGreater(mDR_Setup.mEntityPositionThreshold, 0.0);
   }
   else if (command == "heartbeat_timer")
   {
      aInput.ReadValueOfType(mDR_Setup.mHeartbeatTimer, UtInput::cTIME);
      aInput.ValueGreater(mDR_Setup.mHeartbeatTimer, 0.0);
   }
   else if (command == "mover_update_timer")
   {
      aInput.ReadValueOfType(mDR_Setup.mMoverUpdateTimer, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mDR_Setup.mMoverUpdateTimer, 0.0);
   }
   else if (command == "initial_distribution_interval")
   {
      aInput.ReadValueOfType(mDR_Setup.mInitialDistributionInterval, UtInput::cTIME);
      aInput.ValueGreater(mDR_Setup.mInitialDistributionInterval, 0.0);
   }
   else if (command == "test_dead_reckon")
   {
      // allow this to be requested for testing without an external interface
      mDR_Setup.mDR_ServiceRequested = true;
   }
   else
   {
      return false;
   }
   return true;
}

void WsfExtData::ConvertInputToEntityId(std::string& aInput, WsfExtEntityId& aEntityId)
{
   const std::string& entityIdString = aInput;
   std::istringstream iss(entityIdString.c_str());
   unsigned int       field[3] = {0, 0, 0};
   char               fieldSep[2];
   iss >> field[0] >> fieldSep[0] >> field[1] >> fieldSep[1] >> field[2];
   if (InputIsValid(iss) && (field[0] <= 0xffff) && ((fieldSep[0] == ':') || (fieldSep[0] == '.') || (fieldSep[0] == '-')) &&
       (field[1] <= 0xffff) && (fieldSep[1] == fieldSep[0]) && (field[2] <= 0xffff))
   {
      aEntityId.SetSite(field[0]);
      aEntityId.SetApplication(field[1]);
      aEntityId.SetEntity(field[2]);
   }
   else
   {
      throw UtException("Bad Entity ID: '" + entityIdString + "'");
   }
}

// private
void WsfExtData::ProcessExternalEntity(UtInput& aInput)
{
   WsfExtEntityId entityId;
   ReadEntityId(aInput, entityId);
   mExternalEntityIds.insert(entityId);
}

void WsfExtData::ProcessExternalType(UtInput& aInput)
{
   WsfExtEntityType entityType;
   ReadEntityType(aInput, entityType);
   mExternalEntityTypes.insert(entityType);
}

bool WsfExtData::CompleteLoad(WsfScenario& aScenario)
{
   for (auto component : mComponents)
   {
      if (!component->CompleteLoad(aScenario))
      {
         return false;
      }
   }
   return true;
}


WsfExtInput::WsfExtInput(WsfScenario& aScenario)
   : mData(aScenario)
{
}

WsfExtInput::~WsfExtInput() {}

void WsfExtInput::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfExtInterface>(*this));
}

bool WsfExtInput::Complete()
{
   return mData.CompleteLoad(GetScenario());
}

void WsfExtInput::AddedToScenario() {}

WsfExtInput* WsfExtInput::Find(const WsfScenario& aScenario)
{
   return static_cast<WsfExtInput*>(aScenario.FindExtension("ext_interface"));
}

WsfExtInterface::WsfExtInterface(WsfExtInput& aInput)
   : WsfExtData(aInput.mData)
   , mDefaultPlatformTypeId("WSF_PLATFORM")
   , mNextEntity(1)
   , mNextEventId(0)
{
   mEmissionPtr   = new WsfExtEmission(*mEmissionPtr);
   mIsInitialized = false;
   // Mark reserved entity ID's as preassigned.
   mPreassignedEntityIds.set(cNO_ENTITY);
   mPreassignedEntityIds.set(cRQST_ASSIGN_ID);
   mPreassignedEntityIds.set(cALL_ENTITIES);

   mDeadReckonPtr = new WsfExtEntityDeadReckon(this);
}

WsfExtInterface::~WsfExtInterface()
{
   delete mDeadReckonPtr;
}


bool WsfExtInterface::PrepareExtension()
{
   if (mIsInitialized)
   {
      return true;
   }

   mNextEntity = mStartEntity;

   mDeadReckonPtr->Initialize();
   mRandom.SetSeed(GetSimulation().GetRandom().GetSeed()); // Set local random number seed to simulation seed

   mActiveEntityIds.reset();

   // If not defined, initialize the WSF Side ID to Force ID mapping with some default values.
   if (mSideIdToForceId.empty())
   {
      mSideIdToForceId["blue"]  = 1;
      mSideIdToForceId["red"]   = 2;
      mSideIdToForceId["green"] = 3;
   }

   double connectionTime = mDeferredConnectionTime + 0.1;
   GetSimulation().AddEvent(
      ut::make_unique<WsfOneShotEvent>(connectionTime, [=]() { ConnectionComplete(connectionTime); }));

   mIsInitialized = true;
   return true;
}

// Strip out the 'non-essential' components from the new platform.
// These are:
//    - non-mover platform parts
//    - command chains
void WsfExtInterface::StripExternalPlatform(WsfPlatform* aPlatformPtr)
{
   std::vector<WsfPlatformComponent*> componentsToDelete;

   const WsfPlatformComponentList& components(*aPlatformPtr);
   for (auto componentPtr : components)
   {
      if (componentPtr->ComponentHasRole(cWSF_COMPONENT_COMMAND_CHAIN))
      {
         componentsToDelete.push_back(componentPtr);
      }
      else if (componentPtr->ComponentHasRole(cWSF_COMPONENT_PLATFORM_PART) &&
               (!componentPtr->ComponentHasRole(cWSF_COMPONENT_MOVER)))
      {
         componentsToDelete.push_back(componentPtr);
      }
   }


   for (WsfComponent* componentPtr : componentsToDelete)
   {
      WsfStringId name(componentPtr->GetComponentName());
      int         role(componentPtr->GetComponentRoles()[0]); // The primary role under which it is registered.
      aPlatformPtr->DeleteComponent(name, role);
   }
}

WsfPlatform* WsfExtInterface::CreateExternalPlatform(const std::string& aPlatformType)
{
   auto&        platformTypes = WsfPlatformTypes::Get(GetScenario());
   WsfPlatform* platformPtr   = platformTypes.Clone(aPlatformType);
   if (platformPtr == nullptr)
   {
      // The platform type doesn't exist so clone one using the default type (WSF_PLATFORM).
      platformPtr = platformTypes.Clone(mDefaultPlatformTypeId);
      if (platformPtr == nullptr)
      {
         auto out = ut::log::warning() << "Unable to clone platform.";
         out.AddNote() << "Type: " << aPlatformType;
      }
   }

   // If a platform could not be created then simply create a default platform.
   if (platformPtr == nullptr)
   {
      platformPtr = new WsfPlatform(GetScenario());
   }

   return platformPtr;
}

//! Given an entity type, select the object type to be used.
WsfStringId WsfExtInterface::SelectPlatformType(const WsfExtEntityType& aEntityType)
{
   WsfExtEntityType entityType(aEntityType);
   auto&            platformTypes = WsfPlatformTypes::Get(GetScenario());

   std::map<WsfExtEntityType, WsfStringId>::iterator mapIter;

   // Try a complete match...
   mapIter = mEntityTypeToObjectType.find(entityType);
   if (mapIter != mEntityTypeToObjectType.end())
   {
      WsfStringId objectTypeId = (*mapIter).second;

      if (platformTypes.Find(objectTypeId) != nullptr)
      {
         return (*mapIter).second;
      }
   }

   // Eliminate 'extra'.
   entityType.mExtra = 0;
   mapIter           = mEntityTypeToObjectType.find(entityType);
   if (mapIter != mEntityTypeToObjectType.end())
   {
      WsfStringId objectTypeId = (*mapIter).second;
      if (platformTypes.Find(objectTypeId) != nullptr)
      {
         return (*mapIter).second;
      }
   }

   // Eliminate 'specific'.
   entityType.mSpecific = 0;
   mapIter              = mEntityTypeToObjectType.find(entityType);
   if (mapIter != mEntityTypeToObjectType.end())
   {
      WsfStringId objectTypeId = (*mapIter).second;
      if (platformTypes.Find(objectTypeId) != nullptr)
      {
         return (*mapIter).second;
      }
   }

   // Eliminate 'subcategory'
   entityType.mSubcategory = 0;
   mapIter                 = mEntityTypeToObjectType.find(entityType);
   if (mapIter != mEntityTypeToObjectType.end())
   {
      WsfStringId objectTypeId = (*mapIter).second;
      if (platformTypes.Find(objectTypeId) != nullptr)
      {
         return (*mapIter).second;
      }
   }

   // If nothing has mapped, use the unknown specified by the user.
   if (!mUnknownPlatformTypeId.IsNull())
   {
      return mUnknownPlatformTypeId;
   }

   // Nothing has mapped and the user didn't specify an unknown type,
   // so use the local default (WSF_PLATFORM).
   return mDefaultPlatformTypeId;
}

// Assigns the next sequential entity number.  Site and Application
// are not modified.
uint16_t WsfExtInterface::AssignNewEntity()
{
   while ((mNextEntity == cNO_ENTITY) || (mNextEntity == cALL_ENTITIES) || (mNextEntity == cRQST_ASSIGN_ID))
   {
      ++mNextEntity;
   }
   return mNextEntity++;
}

//! Release an entity ID to be used by another entity
void WsfExtInterface::FreeEntityId(uint16_t aEntityId)
{
   mActiveEntityIds.reset(aEntityId);
}

//! Returns 'true' if aPlatform is a private platform which should not be transmitted to another simulation
bool WsfExtInterface::IsPrivate(const WsfPlatform* aPlatformPtr) const
{
   return aPlatformPtr && ((mAllPrivate) || (mPrivateNameId.find(aPlatformPtr->GetNameId()) != mPrivateNameId.end()) ||
                           (mPrivateTypeId.find(aPlatformPtr->GetTypeId()) != mPrivateTypeId.end()) ||
                           (mPrivateCategoryId.Intersects(aPlatformPtr->GetCategories())));
}

//! Given a platform, select an entity id.
//! Call FreeEntityId() when the ID is no longer assigned to the platform
void WsfExtInterface::SelectEntityId(WsfPlatform* aPlatformPtr, uint16_t& aEntityId)
{
   uint16_t entity = 0;

   // Determine if an ID has been preassigned to this platform using the 'entity_id' command
   auto iter = mPlatformNameToEntityId.find(aPlatformPtr->GetNameId());
   if (iter != mPlatformNameToEntityId.end())
   {
      // Using the 'entity_id' command.
      entity = iter->second;
      // This shouldn't occur... but just in case it does...
      if (mActiveEntityIds.test(entity))
      {
         auto out = ut::log::warning() << "Preassigned entity ID already in use.";
         out.AddNote() << "Platform: " << aPlatformPtr->GetName();
         out.AddNote() << "Entity ID: " << entity;
         out.AddNote() << "An available ID will be used instead.";
         entity = 0;
      }
   }

   // If an entity ID has not been defined then assign one dynamically.

   if (entity == 0)
   {
      int count = 0;
      while ((entity == 0) && (count < 65536))
      {
         ++count;
         entity = AssignNewEntity();
         if (mPreassignedEntityIds.test(entity) || mActiveEntityIds.test(entity))
         {
            entity = 0;
         }
      }
      if (entity == 0)
      {
         std::string errorMessage{"All Entity ID's have been used."};
         ut::log::error() << errorMessage;
         throw WsfException(errorMessage);
      }
   }

   aEntityId = entity;
   mActiveEntityIds.set(entity);
}

bool WsfExtInterface::IsEntityExternallyMoved(const WsfExtEntityId& aEntityId, const WsfExtEntityType& aEntityType)
{
   return (mExternalEntityIds.find(aEntityId) != mExternalEntityIds.end() ||
           mExternalEntityTypes.find(aEntityType) != mExternalEntityTypes.end());
}

uint16_t WsfExtInterface::GetNewEventId()
{
   while ((mNextEventId == 0x0000) || (mNextEventId == 0xFFFF))
   {
      ++mNextEventId;
   }
   uint16_t number = mNextEventId;
   ++mNextEventId;
   return number;
}

double WsfExtInterface::Connected(double aSimTime)
{
   ConnectionComplete(aSimTime);
   return -1.0;
}

void WsfExtInterface::AddPrivateType(WsfStringId aTypeId)
{
   mPrivateTypeId.insert(aTypeId);
}

WsfExtInterface* WsfExtInterface::Find(const WsfSimulation& aSimulation)
{
   return static_cast<WsfExtInterface*>(aSimulation.FindExtension("ext_interface"));
}
