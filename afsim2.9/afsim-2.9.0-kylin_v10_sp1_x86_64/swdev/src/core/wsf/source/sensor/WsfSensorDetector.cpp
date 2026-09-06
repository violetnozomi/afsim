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

#include "WsfSensorDetector.hpp"

#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfSensorResult.hpp"

namespace wsf
{

SensorDetector::SensorDetector()
   : mDebugEnabled(false)
{
}

// ================================================================================================
// virtual
bool SensorDetector::Initialize(double aSimTime, WsfSensorMode* aModePtr, size_t aBeamIndex /* = 0*/)
{
   return true;
}

// ================================================================================================
// virtual
bool SensorDetector::ProcessInput(UtInput& aInput)
{
   return false;
}


// ================================================================================================
// Start of static methods for supporting definition of sensor detector types and instances.
// ================================================================================================

SensorDetectorTypes::ObjectFactoryList SensorDetectorTypes::mObjectFactoryList;


// =================================================================================================
//! Process error_model type input.
//! @param aInput             The current input stream.
//! @param aSensorDetectorPtr The pointer to the sensor detector.
//! @return 'true' if the current command was recognized and processed, 'false' if not recognized.
bool SensorDetectorTypes::LoadInstance(UtInput& aInput, std::unique_ptr<SensorDetector>& aSensorDetectorPtr)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "detector")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      std::string  baseTypeName;
      aInput.ReadValue(baseTypeName);
      aSensorDetectorPtr = std::unique_ptr<SensorDetector>(CreateInstance(baseTypeName));
      if (aSensorDetectorPtr != nullptr)
      {
         inputBlock.ProcessInput(aSensorDetectorPtr.get());
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown detector type: " + baseTypeName);
      }
   }
   return myCommand;
}

// =================================================================================================
// private
SensorDetector* SensorDetectorTypes::CreateInstance(const std::string& aTypeName)
{
   SensorDetector* instancePtr = nullptr;

   ObjectFactoryList::iterator fli = mObjectFactoryList.begin();
   while ((fli != mObjectFactoryList.end()) && (instancePtr == nullptr))
   {
      FactoryPtr factoryPtr = *fli;
      instancePtr           = (*factoryPtr)(aTypeName);
      ++fli;
   }
   return instancePtr;
}

// ================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void SensorDetectorTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}

} // namespace wsf
