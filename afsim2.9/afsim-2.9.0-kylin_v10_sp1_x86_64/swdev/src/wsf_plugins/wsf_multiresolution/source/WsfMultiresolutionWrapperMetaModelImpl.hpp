// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <algorithm>
#include <iterator>
#include <type_traits>

#include "ComponentNameHelper.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfComponentRoles.hpp"
#include "WsfMultiresolutionWrapperMetaModel.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

template<typename DerivedComponent>
WsfObject* WsfMultiresolutionWrapperMetaModel<DerivedComponent>::Clone() const
{
   return new WsfMultiresolutionWrapperMetaModel<DerivedComponent>(*this);
}

template<typename DerivedComponent>
WsfComponent* WsfMultiresolutionWrapperMetaModel<DerivedComponent>::CloneComponent() const
{
   return new WsfMultiresolutionWrapperMetaModel<DerivedComponent>(*this);
}

template<typename DerivedComponent>
bool WsfMultiresolutionWrapperMetaModel<DerivedComponent>::ProcessInput(UtInput& aInput)
{
   const std::string command(aInput.GetCommand());
   if (command == "model")
   {
      UtInputBlock inputBlock(aInput);
      return ProcessModel(inputBlock, ProcessModelName(inputBlock));
   }
   else if (command == "add" || command == "edit")
   {
      // Ensure next command is model, then process
      std::string nextCommand;
      aInput.ReadCommand(nextCommand);
      if (nextCommand != "model")
      {
         throw UtInput::BadValue(aInput, std::string("Unexpected command ") + nextCommand + ". Expected model");
      }
      UtInputBlock inputBlock(aInput);
      return ProcessModel(inputBlock, ProcessModelName(inputBlock, command == "edit"));
   }
   else if (command == "common")
   {
      return ProcessCommonParameters(aInput);
   }
   else
   {
      return WsfMultiresolutionPlatformComponent<DerivedComponent>::ProcessInput(aInput);
   }
}

template<typename DerivedComponent>
bool WsfMultiresolutionWrapperMetaModel<DerivedComponent>::ProcessModel(
   UtInputBlock&                                                              aInputBlock,
   const typename WsfMultiresolutionWrapperMetaModel<DerivedComponent>::Index aModelIndex)
{
   namespace wm = wsf::multiresolution;
   assert(aModelIndex < mComponentModels.size());
   ModelWithFidelity& model = mComponentModels[aModelIndex];
   std::string        nextCommand;
   while (aInputBlock.ReadCommand(nextCommand))
   {
      if (nextCommand == "fidelity_range")
      {
         model.mFidelityRange = wm::ProcessFidelityRange(aInputBlock.GetInput());
         AffirmDisjointWithFidelityRanges(aInputBlock.GetInput(), model);
      }
      else if (nextCommand == wm::BaseComponentName<DerivedComponent>())
      {
         model.mComponent = ProcessComponent(aInputBlock.GetInput(), std::move(model.mComponent));
      }
      else if (nextCommand == "default")
      {
         mDefaultComponentIndex = aModelIndex;
      }
      else
      {
         throw UtInput::BadValue(aInputBlock.GetInput(),
                                 std::string("Unexpected command ") + nextCommand +
                                    ". Expected either fidelity_range, default, or " +
                                    std::string(wm::BaseComponentName<DerivedComponent>()));
      }
   }
   FinalizeModelProcessing(aInputBlock.GetInput(), model);
   return true;
}

template<typename DerivedComponent>
std::unique_ptr<DerivedComponent>
WsfMultiresolutionWrapperMetaModel<DerivedComponent>::ProcessComponent(UtInput&                          aInput,
                                                                       std::unique_ptr<DerivedComponent> aExistingComponent)
{
   UtInputBlock                      componentBlock(aInput);
   std::unique_ptr<DerivedComponent> component =
      aExistingComponent ? std::move(aExistingComponent) : ProcessComponentType(aInput);
   assert(component); // ProcessComponentType throws if it can't read the type
   bool        processedInput = true;
   std::string nextCommand;
   while (componentBlock.ReadCommand(nextCommand) && processedInput)
   {
      processedInput &= component->ProcessInput(aInput);
   }
   if (!processedInput)
   {
      throw UtInput::BadValue(aInput, "Model couldn't process its input. Model type: " + component->GetType());
   }
   return component;
}

template<typename DerivedComponent>
typename WsfMultiresolutionWrapperMetaModel<DerivedComponent>::Index
WsfMultiresolutionWrapperMetaModel<DerivedComponent>::ProcessModelName(UtInputBlock&            aInputBlock,
                                                                       const ut::optional<bool> aExpectExists)
{
   std::string modelName;
   aInputBlock.GetInput().ReadValue(modelName);
   ut::optional<Index> modelIndex = ModelIndexWithName(modelName);
   // Check errors
   if (aExpectExists)
   {
      if (modelIndex.has_value() && !aExpectExists.value())
      {
         throw UtInput::BadValue(aInputBlock.GetInput(), "Expected no existing model with name " + modelName);
      }
      else if (!modelIndex.has_value() && aExpectExists.value())
      {
         throw UtInput::BadValue(aInputBlock.GetInput(), "Expected existing model with name " + modelName);
      }
   }
   // Insert new model if necessary
   if (!modelIndex.has_value())
   {
      mComponentModels.emplace_back();
      mComponentModels.back().mName = std::move(modelName);
      modelIndex                    = mComponentModels.size() - 1;
   }
   return *modelIndex;
}

template<typename DerivedComponent>
std::unique_ptr<DerivedComponent> WsfMultiresolutionWrapperMetaModel<DerivedComponent>::ProcessComponentType(UtInput& aInput)
{
   std::string baseType;
   aInput.ReadValue(baseType);
   WsfObjectTypeList<DerivedComponent>& componentTypeList =
      wsf::multiresolution::GetObjectTypeList<DerivedComponent>(aInput);
   auto component = std::unique_ptr<DerivedComponent>(componentTypeList.Clone(baseType));
   if (!component)
   {
      // A failed call to Clone invokes deferred loading, so a second call should be successful if the type exists.
      component.reset(componentTypeList.Clone(baseType));
   }
   if (!component)
   {
      throw UtInput::BadValue(aInput, "Could not load type " + baseType);
   }
   return component;
}

template<typename DerivedComponent>
bool WsfMultiresolutionWrapperMetaModel<DerivedComponent>::ProcessCommonParameters(UtInput& aInput)
{
   auto commonInputLocation = aInput.StoreLocation();
   bool inputProcessed      = true;
   if (mComponentModels.empty())
   {
      throw UtInput::BadValue(aInput, "Expected common input block after definition of one or more models.");
   }
   for (auto& component : mComponentModels)
   {
      aInput.RestoreLocation(commonInputLocation);
      UtInputBlock inputBlock(aInput);
      std::string  nextCommand;
      while (inputBlock.ReadCommand(nextCommand))
      {
         inputProcessed &= component.mComponent->ProcessInput(aInput);
      }
   }
   return inputProcessed;
}

template<typename DerivedComponent>
void WsfMultiresolutionWrapperMetaModel<DerivedComponent>::FinalizeModelProcessing(
   UtInput&                                                                                aInput,
   const typename WsfMultiresolutionWrapperMetaModel<DerivedComponent>::ModelWithFidelity& aModelWithFidelity)
{
   if (aModelWithFidelity.mComponent)
   {
      // If this isn't directly read, check that the default doesn't overlap.
      AffirmDisjointWithFidelityRanges(aInput, aModelWithFidelity);
   }
   else
   {
      // This should be unreachable due to other error checking, but this serves as a final check that a valid
      // model exists.
      throw UtInput::BadValue(aInput,
                              "Expected a model specification of type " +
                                 std::string(wsf::multiresolution::BaseComponentName<DerivedComponent>()));
   }
}

template<typename DerivedComponent>
void WsfMultiresolutionWrapperMetaModel<DerivedComponent>::AffirmDisjointWithFidelityRanges(
   UtInput&                                                                                aInput,
   const typename WsfMultiresolutionWrapperMetaModel<DerivedComponent>::ModelWithFidelity& aModelWithFidelity) const
{
   const auto result = std::find_if(mComponentModels.cbegin(),
                                    mComponentModels.cend(),
                                    [&aModelWithFidelity](const ModelWithFidelity& model)
                                    {
                                       return aModelWithFidelity.mComponent.get() != model.mComponent.get() &&
                                              aModelWithFidelity.mFidelityRange.Overlaps(model.mFidelityRange);
                                    });
   if (result != mComponentModels.end())
   {
      throw UtInput::BadValue(aInput,
                              "Expected no overlap between any models' fidelity_range. Found overlap between " +
                                 aModelWithFidelity.mName + " and " + result->mName);
   }
}

template<typename DerivedComponent>
ut::optional<typename WsfMultiresolutionWrapperMetaModel<DerivedComponent>::Index>
WsfMultiresolutionWrapperMetaModel<DerivedComponent>::ModelIndexForFidelity(const double aFidelity) const
{
   assert(!mComponentModels.empty());
   assert(aFidelity >= 0.0);
   assert(aFidelity <= 1.0);
   const auto result =
      std::find_if(mComponentModels.cbegin(),
                   mComponentModels.cend(),
                   [aFidelity](const ModelWithFidelity& model) { return model.mFidelityRange.Contains(aFidelity); });
   if (result != mComponentModels.cend())
   {
      return std::distance(mComponentModels.cbegin(), result);
   }
   ut::log::warning() << "No matching model found in " << wsf::multiresolution::CommandName<DerivedComponent>()
                      << " on platform " << this->GetComponentParent()->GetName() << ". Using default model.";
   if (!mDefaultComponentIndex)
   {
      ut::log::error() << "No default model in " << wsf::multiresolution::CommandName<DerivedComponent>()
                       << " on platform " << this->GetComponentParent()->GetName();
   }
   return mDefaultComponentIndex;
}

template<typename DerivedComponent>
typename ut::optional<typename WsfMultiresolutionWrapperMetaModel<DerivedComponent>::Index>
WsfMultiresolutionWrapperMetaModel<DerivedComponent>::ModelIndexWithName(const std::string& aTypeName) const
{
   const auto result = std::find_if(mComponentModels.cbegin(),
                                    mComponentModels.cend(),
                                    [&aTypeName](const ModelWithFidelity& model) { return model.mName == aTypeName; });
   if (result != mComponentModels.cend())
   {
      return std::distance(mComponentModels.cbegin(), result);
   }
   else
   {
      return ut::nullopt;
   }
}

template<typename DerivedComponent>
std::unique_ptr<DerivedComponent> WsfMultiresolutionWrapperMetaModel<DerivedComponent>::GetModel() const
{
   const ut::optional<Index> componentIndex = ModelIndexForFidelity(this->GetFidelity());
   if (componentIndex.has_value())
   {
      return std::unique_ptr<DerivedComponent>{mComponentModels[*componentIndex].mComponent->Clone()};
   }
   else
   {
      return nullptr;
   }
}
