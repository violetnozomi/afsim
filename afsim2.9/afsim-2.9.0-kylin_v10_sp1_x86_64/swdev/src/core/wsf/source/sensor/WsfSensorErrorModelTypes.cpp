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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfSensorErrorModelTypes.hpp"

#include <memory>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfAbsoluteSensorErrorModel.hpp"
#include "WsfRadarSensorErrorModel.hpp"
#include "WsfScenario.hpp"
#include "WsfStandardSensorErrorModel.hpp"

class WsfNullSensorErrorModel : public wsf::SensorErrorModelBase
{
public:
   wsf::SensorErrorModelBase* Clone() const override { return new WsfNullSensorErrorModel(); }
   void                       ApplyMeasurementErrors(WsfSensorResult& aResult) override
   {
      // The null sensor error model does not modify the sensor result.
   }
   bool IsNullModel() const override { return true; }
};

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfSensorErrorModelTypes& WsfSensorErrorModelTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetSensorErrorModelTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfSensorErrorModelTypes& WsfSensorErrorModelTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetSensorErrorModelTypes();
}

// =================================================================================================
WsfSensorErrorModelTypes::WsfSensorErrorModelTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<wsf::SensorErrorModelBase>(aScenario, cREDEFINITION_ALLOWED, "error_model")
{
   mUniqueId = 0;
   mObjectFactoryList.push_back(wsf::StandardSensorErrorModel::ObjectFactory);
   mObjectFactoryList.push_back(wsf::RadarSensorErrorModel::ObjectFactory);
   mObjectFactoryList.push_back(wsf::AbsoluteSensorErrorModel::ObjectFactory);
}

// =================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void WsfSensorErrorModelTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}

// =================================================================================================
//! Process error_model type input.
//! @param aInput                [input] The current input stream.
//! @param aSensorErrorModelPtr  [output] The sensor error model pointer for the loaded instance.
//! @return 'true' if the current command was recognized and processed, 'false' if not recognized.
bool WsfSensorErrorModelTypes::LoadInstance(UtInput& aInput, std::unique_ptr<wsf::SensorErrorModelBase>& aSensorErrorModelPtr)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "error_model")
   {
      myCommand = true;
      std::string blockTerminator("end_" + command);
      std::string baseTypeName;
      aInput.ReadValue(baseTypeName);
      if (baseTypeName == "none")
      {
         aSensorErrorModelPtr = nullptr;
         // 'end_error_model' is optional after 'none', but it must be the next command.
         aInput.ReadValue(command);
         if (command != blockTerminator)
         {
            aInput.PushBack(command);
         }
      }
      else
      {
         if (Find(baseTypeName))
         {
            aSensorErrorModelPtr = std::unique_ptr<wsf::SensorErrorModelBase>(Clone(baseTypeName));
            // 'end_error_model' is optional, but it must be the next command.
            aInput.ReadValue(command);
            if (command != blockTerminator)
            {
               aInput.PushBack(command);
            }
         }
         else
         {
            UtInputBlock inputBlock(aInput, blockTerminator);
            aSensorErrorModelPtr = std::unique_ptr<wsf::SensorErrorModelBase>(CreateInstance(baseTypeName));

            if (aSensorErrorModelPtr != nullptr)
            {
               inputBlock.ProcessInput(aSensorErrorModelPtr.get());
            }
            else
            {
               throw UtInput::BadValue(aInput, "Unknown error_model type: " + baseTypeName);
            }
         }
      }
   }
   return myCommand;
}

// =================================================================================================
//! Process error_model type input.
//! @param aInput The current input stream.
//! @return 'true' if the current command was recognized and processed
//! 'false' if not recognized.
WsfSensorErrorModelTypes::LoadResult WsfSensorErrorModelTypes::LoadType(UtInput& aInput)
{
   LoadResult  result;
   std::string command(aInput.GetCommand());
   if (command == "error_model")
   {
      std::string blockTerminator("end_" + command);
      std::string userTypeName;
      std::string baseTypeName;
      aInput.ReadValue(userTypeName);
      aInput.ReadValue(baseTypeName);

      if (baseTypeName == "none")
      {
         // 'end_error[_model]' is optional after 'none', but it must be the next command.
         aInput.ReadValue(command);
         if (command != blockTerminator)
         {
            aInput.PushBack(command);
         }
         std::unique_ptr<wsf::SensorErrorModelBase> userTypePtr = ut::make_unique<WsfNullSensorErrorModel>();
         result.mObjectTypePtr                                  = userTypePtr.get();
         if (!Add(userTypeName, std::move(userTypePtr)))
         {
            throw UtInput::BadValue(aInput, "Unable to register error_model: " + userTypeName);
         }
         result.mIsCommandProcessed = true;
      }
      else
      {
         UtInputBlock                               inputBlock(aInput, blockTerminator);
         std::unique_ptr<wsf::SensorErrorModelBase> userTypePtr(CreateInstance(baseTypeName));
         if (userTypePtr == nullptr)
         {
            throw UtInput::BadValue(aInput, "Unknown error_model type: " + baseTypeName);
         }
         inputBlock.ProcessInput(userTypePtr.get());
         result.mObjectTypePtr = userTypePtr.get();
         if (!Add(userTypeName, std::move(userTypePtr)))
         {
            throw UtInput::BadValue(aInput, "Unable to register error_model: " + userTypeName);
         }
         result.mIsCommandProcessed = true;
      }
   }

   return result;
}

// =================================================================================================
// private
std::unique_ptr<wsf::SensorErrorModelBase> WsfSensorErrorModelTypes::CreateInstance(const std::string& aTypeName)
{
   for (const auto& factoryPtr : mObjectFactoryList)
   {
      auto instancePtr = factoryPtr(aTypeName);
      if (instancePtr)
      {
         return instancePtr;
      }
   }
   return nullptr;
}
