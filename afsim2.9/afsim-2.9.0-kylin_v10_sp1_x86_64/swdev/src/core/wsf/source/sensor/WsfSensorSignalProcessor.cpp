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

#include "WsfSensorSignalProcessor.hpp"

#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfSensorResult.hpp"

WsfSensorSignalProcessor::ObjectFactoryList WsfSensorSignalProcessor::sObjectFactoryList;

// ================================================================================================
// virtual
bool WsfSensorSignalProcessor::Initialize(double         aSimTime,
                                          WsfSensor*     aSensorPtr,
                                          WsfSensorMode* aModePtr,
                                          size_t         aBeamIndex /*= 0*/)
{
   return true;
}


// ================================================================================================
// virtual
bool WsfSensorSignalProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "debug")
   {
      mDebug = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// ================================================================================================
// Start of nested class WsfSensorSignalProcessor::List
// ================================================================================================

// ================================================================================================
bool WsfSensorSignalProcessor::List::Initialize(double         aSimTime,
                                                WsfSensor*     aSensorPtr,
                                                WsfSensorMode* aModePtr,
                                                size_t         aBeamIndex /* = 0*/)
{
   bool ok = true;
   for (auto& processorPtr : mProcessorPtrs)
   {
      if (!processorPtr->Initialize(aSimTime, aSensorPtr, aModePtr))

      // ================================================================================================
      {
         auto out = ut::log::error() << "Initialization failed for 'signal_processor'.";
         out.AddNote() << "Type: " << processorPtr->GetType();
         ok = false;
      }
   }
   return ok;
}

// ================================================================================================
bool WsfSensorSignalProcessor::List::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "signal_processor")
   {
      // If a processor of the requested type has already been defined then the input is used to
      // edit the existing definition. Otherwise, a new signal processor is added.
      std::string typeName;
      aInput.ReadValue(typeName);
      auto it = std::find_if(mProcessorPtrs.begin(),
                             mProcessorPtrs.end(),
                             [&typeName](const ListType::value_type& aProc) { return (aProc->GetType() == typeName); });

      if (it == mProcessorPtrs.end())
      {
         // Adding a new signal processor.
         std::unique_ptr<WsfSensorSignalProcessor> newProcessorPtr(CreateInstance(typeName));
         if (newProcessorPtr == nullptr)
         {
            throw UtInput::BadValue(aInput, "Unknown signal_processor type: " + typeName);
         }
         UtInputBlock inputBlock(aInput);
         while (inputBlock.ReadCommand())
         {
            if (!newProcessorPtr->ProcessInput(inputBlock.GetInput()))
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
         mProcessorPtrs.emplace_back(std::move(newProcessorPtr));
      }
      else
      {
         // Editing an existing signal processor
         UtInputBlock inputBlock(aInput);
         while (inputBlock.ReadCommand())
         {
            if (!(*it)->ProcessInput(inputBlock.GetInput()))
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
      }
   }
   else if (command == "delete_signal_processor")
   {
      std::string typeName;
      aInput.ReadValue(typeName);
      mProcessorPtrs.erase(std::remove_if(mProcessorPtrs.begin(),
                                          mProcessorPtrs.end(),
                                          [&typeName](const ListType::value_type& aProc)
                                          { return (aProc->GetType() == typeName); }),
                           mProcessorPtrs.end());
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// ================================================================================================
void WsfSensorSignalProcessor::List::Execute(double aSimTime, WsfSensorResult& aResult)
{
   for (auto& processorPtr : mProcessorPtrs)
   {
      processorPtr->Execute(aSimTime, aResult);
   }
}

// ================================================================================================
// Start of simple pre-defined signal processors.
//
// These are some very simple signal processors that are included in the baseline that can be used
// as examples.
// ================================================================================================

namespace
{
// =============================================================================================
class ConstantClutterSuppression : public WsfSensorSignalProcessor
{
public:
   ConstantClutterSuppression()
      : mSuppressionFactor(1.0)
   {
   }
   WsfSensorSignalProcessor* Clone() const override { return new ConstantClutterSuppression(*this); }
   bool                      ProcessInput(UtInput& aInput) override;
   void                      Execute(double aSimTime, WsfSensorResult& aResult) override;

private:
   double mSuppressionFactor;
};

bool ConstantClutterSuppression::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (aInput.GetCommand() == "suppression_factor")
   {
      aInput.ReadValueOfType(mSuppressionFactor, UtInput::cRATIO);
      aInput.ValueGreater(mSuppressionFactor, 0.0);
   }
   else
   {
      myCommand = WsfSensorSignalProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

void ConstantClutterSuppression::Execute(double aSimTime, WsfSensorResult& aResult)
{
   aResult.mClutterPower *= mSuppressionFactor;
}

// =============================================================================================
class ScaleFactor : public WsfSensorSignalProcessor
{
public:
   ScaleFactor()
      : mScaleFactor(1.0)
   {
   }
   WsfSensorSignalProcessor* Clone() const override { return new ScaleFactor(*this); }
   bool                      ProcessInput(UtInput& aInput) override;
   void                      Execute(double aSimTime, WsfSensorResult& aResult) override;

private:
   double mScaleFactor;
};

bool ScaleFactor::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (aInput.GetCommand() == "constant")
   {
      aInput.ReadValue(mScaleFactor);
      aInput.ValueGreater(mScaleFactor, 0.0);
   }
   else
   {
      myCommand = WsfSensorSignalProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

void ScaleFactor::Execute(double aSimTime, WsfSensorResult& aResult)
{
   aResult.mRcvdPower *= mScaleFactor;
}
} // namespace

// ================================================================================================
// Start of static methods for supporting definition of signal processor types and instances.
// ================================================================================================

// ================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void WsfSensorSignalProcessor::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   // Add the factory for the predefined object types if not already added.
   if (sObjectFactoryList.empty())
   {
      sObjectFactoryList.push_back(WsfSensorSignalProcessor::PredefinedObjectFactory);
   }
   sObjectFactoryList.push_back(aFactoryPtr);
}

// ================================================================================================
//! Add an object factory for removing an instance from a fundamental type.
//! The factory should be a static method and this should be for adding duplicate factories.
// static
void WsfSensorSignalProcessor::RemoveObjectFactory(FactoryPtr aFactoryPtr)
{
   // Remove the factory for the predefined object types if not already removed.
   sObjectFactoryList.remove(aFactoryPtr);
}

// ================================================================================================
// static
WsfSensorSignalProcessor* WsfSensorSignalProcessor::CreateInstance(const std::string& aTypeName)
{
   // Add the factory for the predefined object types if not already added.
   if (sObjectFactoryList.empty())
   {
      sObjectFactoryList.push_back(WsfSensorSignalProcessor::PredefinedObjectFactory);
   }

   // Invoke each object factory to see if it can create an instance of the requested type.
   for (const auto& factory : sObjectFactoryList)
   {
      auto instancePtr = (*factory)(aTypeName);
      if (instancePtr)
      {
         instancePtr->SetType(aTypeName);
         instancePtr->SetName(aTypeName);
         return instancePtr;
      }
   }

   return nullptr;
}

// static
void WsfSensorSignalProcessor::ClearTypes()
{
   sObjectFactoryList.clear();
}

// ================================================================================================
//! The object factory for predefined signal processor types.
// private static
WsfSensorSignalProcessor* WsfSensorSignalProcessor::PredefinedObjectFactory(const std::string& aTypeName)
{
   WsfSensorSignalProcessor* instancePtr = nullptr;
   if (aTypeName == "constant_clutter_suppression")
   {
      instancePtr = new ConstantClutterSuppression();
   }
   else if (aTypeName == "scale_factor")
   {
      instancePtr = new ScaleFactor();
   }
   return instancePtr;
}
