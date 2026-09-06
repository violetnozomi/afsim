// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaObserver.hpp"

#include "UtoTypes.hpp"
#include "VaEnvironment.hpp"
#include "VaPosition.hpp"
#include "VaTimeManager.hpp"

namespace vespa
{
VaObserver::VaObserver()
   : mInitialized(false)
{
}

VaObserver::~VaObserver()
{
   Shutdown();
}

#define VA_SET_TYPE_CALLBACK(CALLBACK_TYPE, CALLBACK_NAME) \
   CALLBACK_NAME.SetCallbackType(#CALLBACK_TYPE);          \
   CALLBACK_NAME.SetName(#CALLBACK_NAME);

// virtual
bool VaObserver::Initialize()
{
   if (!mInitialized)
   {
      mInitialized = true;

      VaPosition::InitializeStaticSubscriptions();

      VA_SET_TYPE_CALLBACK(VaObserver::ResourceClaimedCallback, VaObserver::ResourceClaimed);
      VA_SET_TYPE_CALLBACK(VaObserver::PopupDialogCommand, VaObserver::PopupDialog);
      VA_SET_TYPE_CALLBACK(VaObserver::ViewerResizedCallback, VaObserver::ViewerResized);
      VA_SET_TYPE_CALLBACK(VaObserver::ViewMatrixChangedCallback, VaObserver::ViewMatrixChanged);
      VA_SET_TYPE_CALLBACK(VaObserver::MouseButtonCallback, VaObserver::ButtonClick);
      VA_SET_TYPE_CALLBACK(VaObserver::MouseButtonCallback, VaObserver::ButtonDoubleClick);
      VA_SET_TYPE_CALLBACK(VaObserver::MouseButtonCallback, VaObserver::ButtonDown);
      VA_SET_TYPE_CALLBACK(VaObserver::MouseButtonCallback, VaObserver::ButtonDrag);
      VA_SET_TYPE_CALLBACK(VaObserver::MouseButtonCallback, VaObserver::ButtonUp);
      VA_SET_TYPE_CALLBACK(VaObserver::ScenarioAddedCallback, VaObserver::ScenarioAdded);
      VA_SET_TYPE_CALLBACK(VaObserver::ScenarioRemovedCallback, VaObserver::ScenarioRemoved);
      VA_SET_TYPE_CALLBACK(VaObserver::ScenarioModifiedCallback, VaObserver::ScenarioModified);
      VA_SET_TYPE_CALLBACK(VaObserver::ScenarioCurrentChangedCallback, VaObserver::ScenarioCurrentChanged);
      VA_SET_TYPE_CALLBACK(VaObserver::EntityAddedCallback, VaObserver::EntityAdded);
      VA_SET_TYPE_CALLBACK(VaObserver::AttachmentAddedCallback, VaObserver::AttachmentAdded);
      VA_SET_TYPE_CALLBACK(VaObserver::AttachmentRemovedCallback, VaObserver::AttachmentRemoved);
      VA_SET_TYPE_CALLBACK(VaObserver::EntityRemovedCallback, VaObserver::EntityRemoved);
      VA_SET_TYPE_CALLBACK(VaObserver::EntityDeletedCallback, VaObserver::EntityDeleted);
      VA_SET_TYPE_CALLBACK(VaObserver::EntitySelectCommand, VaObserver::EntitySelect);
      VA_SET_TYPE_CALLBACK(VaObserver::EntitySelectedCallback, VaObserver::EntitySelected);
      VA_SET_TYPE_CALLBACK(VaObserver::EntitiesSelectedCallback, VaObserver::EntitiesSelected);
      VA_SET_TYPE_CALLBACK(VaObserver::SelectionChangedCallback, VaObserver::SelectionChanged);
      VA_SET_TYPE_CALLBACK(VaObserver::EntityNameChangedCallback, VaObserver::EntityNameChanged);
      VA_SET_TYPE_CALLBACK(VaObserver::AttachmentDeletedCallback, VaObserver::AttachmentDeleted);
      VA_SET_TYPE_CALLBACK(VaObserver::AttachmentSelectedCallback, VaObserver::AttachmentSelected);
      VA_SET_TYPE_CALLBACK(VaObserver::EntityMovedCallback, VaObserver::EntityMoved);
      VA_SET_TYPE_CALLBACK(VaObserver::ViewerInitializedCallback, VaObserver::ViewerInitialized);
      VA_SET_TYPE_CALLBACK(VaObserver::ViewerDestroyedCallback, VaObserver::ViewerDestroyed);
      VA_SET_TYPE_CALLBACK(VaObserver::CameraUpdatedCallback, VaObserver::CameraUpdated);
      VA_SET_TYPE_CALLBACK(VaObserver::UpdateCursorPositionCallback, VaObserver::UpdateCursorPosition);
      VA_SET_TYPE_CALLBACK(VaObserver::SetUnitFormatCommand, VaObserver::SetUnitFormat);
      VA_SET_TYPE_CALLBACK(VaObserver::ChangedUnitFormatCallback, VaObserver::ChangedUnitFormat);
      VA_SET_TYPE_CALLBACK(VaObserver::UserMessageCommand, VaObserver::UserMessage);
      VA_SET_TYPE_CALLBACK(VaObserver::MapLoadedMapCallback, VaObserver::MapLoadedMap);
      VA_SET_TYPE_CALLBACK(VaObserver::MapProjectionChangedCallback, VaObserver::MapProjectionChanged);
      VA_SET_TYPE_CALLBACK(VaObserver::SetIntensityCommand, VaObserver::SetIntensity);
      VA_SET_TYPE_CALLBACK(VaObserver::SetGroupIntensityCommand, VaObserver::SetGroupIntensity);
      VA_SET_TYPE_CALLBACK(VaObserver::SetSaturationCommand, VaObserver::SetSaturation);
      VA_SET_TYPE_CALLBACK(VaObserver::SetContrastCommand, VaObserver::SetContrast);
      VA_SET_TYPE_CALLBACK(VaObserver::SetGroupSaturationCommand, VaObserver::SetGroupSaturation);
      VA_SET_TYPE_CALLBACK(VaObserver::SetOpacityCommand, VaObserver::SetOpacity);
      VA_SET_TYPE_CALLBACK(VaObserver::SymbolSizeChangedCallback, VaObserver::SymbolSizeChanged);
      VA_SET_TYPE_CALLBACK(VaObserver::TimeDirtyCallback, VaObserver::TimeDirty);
      VA_SET_TYPE_CALLBACK(VaObserver::TimeUpdatedCallback, VaObserver::TimeUpdated);
      VA_SET_TYPE_CALLBACK(VaObserver::TimeModeChangedCallback, VaObserver::TimeModeChanged);
      VA_SET_TYPE_CALLBACK(VaObserver::PreUpdateFrameCallback, VaObserver::PreUpdateFrame);
      VA_SET_TYPE_CALLBACK(VaObserver::PostUpdateFrameCallback, VaObserver::PostUpdateFrame);
      VA_SET_TYPE_CALLBACK(VaObserver::RedrawWindowCommand, VaObserver::RedrawWindow);
      VA_SET_TYPE_CALLBACK(VaObserver::CameraSetLatLonCommand, VaObserver::CameraSetLatLon);
      VA_SET_TYPE_CALLBACK(VaObserver::CameraSetAzElDistCommand, VaObserver::CameraSetAzElDist);
      VA_SET_TYPE_CALLBACK(VaObserver::CameraSetYawPitchDistCommand, VaObserver::CameraSetYawPitchDist);
      VA_SET_TYPE_CALLBACK(VaObserver::CameraSetProjectionCommand, VaObserver::CameraSetProjection);
      VA_SET_TYPE_CALLBACK(VaObserver::CameraSetCenterCommand, VaObserver::CameraSetCenter);
      VA_SET_TYPE_CALLBACK(VaObserver::ExternalApplicationConnectedCallback, VaObserver::ExternalApplicationConnected);
      VA_SET_TYPE_CALLBACK(VaObserver::ExternalApplicationDisconnectedCallback,
                           VaObserver::ExternalApplicationDisconnected);
      VA_SET_TYPE_CALLBACK(VaObserver::ExternalApplicationControlChangedCallback,
                           VaObserver::ExternalApplicationControlChanged);
      VA_SET_TYPE_CALLBACK(VaObserver::ExternalApplicationConnectCommand, VaObserver::ExternalApplicationConnect);
      VA_SET_TYPE_CALLBACK(VaObserver::ExternalApplicationDisconnectCommand, VaObserver::ExternalApplicationDisconnect);
      VA_SET_TYPE_CALLBACK(VaObserver::ExternalApplicationTakeControlCommand, VaObserver::ExternalApplicationTakeControl);
      VA_SET_TYPE_CALLBACK(VaObserver::ECI_ModeChangedCallback, VaObserver::ECI_ModeChanged);
      VA_SET_TYPE_CALLBACK(VaObserver::SelectionClearedCallback, VaObserver::SelectionCleared);
      VA_SET_TYPE_CALLBACK(VaObserver::BeginSelectionChangeCallback, VaObserver::BeginSelectionChange);
      VA_SET_TYPE_CALLBACK(VaObserver::EndSelectionChangeCallback, VaObserver::EndSelectionChange);
      VA_SET_TYPE_CALLBACK(VaObserver::HandleSelectionBoxCallback, VaObserver::HandleSelectionBox);
      VA_SET_TYPE_CALLBACK(VaObserver::HandleSelectionBoxResultCallback, VaObserver::HandleSelectionBoxResult);
      VA_SET_TYPE_CALLBACK(VaObserver::AddNewTeamCallback, VaObserver::AddNewTeam);
      VA_SET_TYPE_CALLBACK(VaObserver::ModelScaleChangedCallback, VaObserver::ModelScaleChanged);
   }
   return mInitialized;
}

void VaObserver::Shutdown()
{
   VaPosition::Shutdown();
}

void VaObserver::TimeManagerSetNormalizedTime(double aNormTime)
{
   VaEnvironment::Instance().GetTime()->SetNormalizedTime(aNormTime);
}

void VaObserver::TimeManagerSetLoopEnabled(bool aLoopEnabled)
{
   VaEnvironment::Instance().GetTime()->SetLoopEnabled(aLoopEnabled);
}

void VaObserver::TimeManagerSetMaxTime(double aMaxTime)
{
   VaEnvironment::Instance().GetTime()->SetMaxTime(aMaxTime);
}

void VaObserver::TimeManagerSetMinTime(double aMinTime)
{
   VaEnvironment::Instance().GetTime()->SetMinTime(aMinTime);
}

void VaObserver::TimeManagerSetRate(double aRate)
{
   VaEnvironment::Instance().GetTime()->SetRate(aRate);
}

void VaObserver::TimeManagerSetSimulationRate(double aRate)
{
   VaEnvironment::Instance().GetTime()->SetSimulationRate(aRate);
}

void VaObserver::TimeManagerSetSimulationPause()
{
   VaEnvironment::Instance().GetTime()->SetSimulationPause();
}

void VaObserver::TimeManagerSetSimulationGo()
{
   VaEnvironment::Instance().GetTime()->SetSimulationGo();
}

void VaObserver::TimeManagerSetTime(double aTime, bool aForceReport)
{
   VaEnvironment::Instance().GetTime()->SetTime(aTime, aForceReport);
}

void VaObserver::TimeManagerSetRealtimeClock(bool aState)
{
   VaEnvironment::Instance().GetTime()->SetRealtimeClock(aState);
}

void VaObserver::TimeManagerReset()
{
   VaEnvironment::Instance().GetTime()->Reset();
}

void VaObserver::TimeManagerPause()
{
   VaEnvironment::Instance().GetTime()->Pause();
}

void VaObserver::TimeManagerRewind()
{
   VaEnvironment::Instance().GetTime()->Rewind();
}

void VaObserver::TimeManagerPlay()
{
   VaEnvironment::Instance().GetTime()->Play();
}

void VaObserver::TimeManagerUpdateFrame()
{
   VaEnvironment::Instance().GetTime()->UpdateFrame();
}


const std::string VaObserver::cTIME_MANAGER_GROUP("TIME_MANAGER_GROUP");

// Don't set callback type and name here with static instantiation. Dictionary is not initialized yet.
// Defer to Initialize() with VA_SET_TYPE_CALLBACK macro
#define VA_INITIALIZE_CALLBACK(CALLBACK_TYPE, CALLBACK_NAME) CALLBACK_TYPE CALLBACK_NAME

// Static instantiations
VA_INITIALIZE_CALLBACK(VaObserver::ResourceClaimedCallback, VaObserver::ResourceClaimed);
VA_INITIALIZE_CALLBACK(VaObserver::PopupDialogCommand, VaObserver::PopupDialog);
VA_INITIALIZE_CALLBACK(VaObserver::ViewerResizedCallback, VaObserver::ViewerResized);
VA_INITIALIZE_CALLBACK(VaObserver::ViewMatrixChangedCallback, VaObserver::ViewMatrixChanged);
VA_INITIALIZE_CALLBACK(VaObserver::MouseButtonCallback, VaObserver::ButtonClick);
VA_INITIALIZE_CALLBACK(VaObserver::MouseButtonCallback, VaObserver::ButtonDoubleClick);
VA_INITIALIZE_CALLBACK(VaObserver::MouseButtonCallback, VaObserver::ButtonDown);
VA_INITIALIZE_CALLBACK(VaObserver::MouseButtonCallback, VaObserver::ButtonDrag);
VA_INITIALIZE_CALLBACK(VaObserver::MouseButtonCallback, VaObserver::ButtonUp);
VA_INITIALIZE_CALLBACK(VaObserver::ScenarioAddedCallback, VaObserver::ScenarioAdded);
VA_INITIALIZE_CALLBACK(VaObserver::ScenarioRemovedCallback, VaObserver::ScenarioRemoved);
VA_INITIALIZE_CALLBACK(VaObserver::ScenarioModifiedCallback, VaObserver::ScenarioModified);
VA_INITIALIZE_CALLBACK(VaObserver::ScenarioCurrentChangedCallback, VaObserver::ScenarioCurrentChanged);
VA_INITIALIZE_CALLBACK(VaObserver::EntityAddedCallback, VaObserver::EntityAdded);
VA_INITIALIZE_CALLBACK(VaObserver::AttachmentAddedCallback, VaObserver::AttachmentAdded);
VA_INITIALIZE_CALLBACK(VaObserver::AttachmentRemovedCallback, VaObserver::AttachmentRemoved);
VA_INITIALIZE_CALLBACK(VaObserver::EntityRemovedCallback, VaObserver::EntityRemoved);
VA_INITIALIZE_CALLBACK(VaObserver::EntityDeletedCallback, VaObserver::EntityDeleted);
VA_INITIALIZE_CALLBACK(VaObserver::EntitySelectCommand, VaObserver::EntitySelect);
VA_INITIALIZE_CALLBACK(VaObserver::EntitySelectedCallback, VaObserver::EntitySelected);
VA_INITIALIZE_CALLBACK(VaObserver::EntitiesSelectedCallback, VaObserver::EntitiesSelected);
VA_INITIALIZE_CALLBACK(VaObserver::SelectionChangedCallback, VaObserver::SelectionChanged);
VA_INITIALIZE_CALLBACK(VaObserver::EntityNameChangedCallback, VaObserver::EntityNameChanged);
VA_INITIALIZE_CALLBACK(VaObserver::AttachmentDeletedCallback, VaObserver::AttachmentDeleted);
VA_INITIALIZE_CALLBACK(VaObserver::AttachmentSelectedCallback, VaObserver::AttachmentSelected);
VA_INITIALIZE_CALLBACK(VaObserver::EntityMovedCallback, VaObserver::EntityMoved);
VA_INITIALIZE_CALLBACK(VaObserver::ViewerInitializedCallback, VaObserver::ViewerInitialized);
VA_INITIALIZE_CALLBACK(VaObserver::ViewerDestroyedCallback, VaObserver::ViewerDestroyed);
VA_INITIALIZE_CALLBACK(VaObserver::CameraUpdatedCallback, VaObserver::CameraUpdated);
VA_INITIALIZE_CALLBACK(VaObserver::UpdateCursorPositionCallback, VaObserver::UpdateCursorPosition);
VA_INITIALIZE_CALLBACK(VaObserver::SetUnitFormatCommand, VaObserver::SetUnitFormat);
VA_INITIALIZE_CALLBACK(VaObserver::ChangedUnitFormatCallback, VaObserver::ChangedUnitFormat);
VA_INITIALIZE_CALLBACK(VaObserver::UserMessageCommand, VaObserver::UserMessage);
VA_INITIALIZE_CALLBACK(VaObserver::MapLoadedMapCallback, VaObserver::MapLoadedMap);
VA_INITIALIZE_CALLBACK(VaObserver::MapProjectionChangedCallback, VaObserver::MapProjectionChanged);
VA_INITIALIZE_CALLBACK(VaObserver::SetIntensityCommand, VaObserver::SetIntensity);
VA_INITIALIZE_CALLBACK(VaObserver::SetGroupIntensityCommand, VaObserver::SetGroupIntensity);
VA_INITIALIZE_CALLBACK(VaObserver::SetSaturationCommand, VaObserver::SetSaturation);
VA_INITIALIZE_CALLBACK(VaObserver::SetContrastCommand, VaObserver::SetContrast);
VA_INITIALIZE_CALLBACK(VaObserver::SetGroupSaturationCommand, VaObserver::SetGroupSaturation);
VA_INITIALIZE_CALLBACK(VaObserver::SetOpacityCommand, VaObserver::SetOpacity);
VA_INITIALIZE_CALLBACK(VaObserver::SymbolSizeChangedCallback, VaObserver::SymbolSizeChanged);
VA_INITIALIZE_CALLBACK(VaObserver::TimeDirtyCallback, VaObserver::TimeDirty);
VA_INITIALIZE_CALLBACK(VaObserver::TimeUpdatedCallback, VaObserver::TimeUpdated);
VA_INITIALIZE_CALLBACK(VaObserver::TimeModeChangedCallback, VaObserver::TimeModeChanged);
VA_INITIALIZE_CALLBACK(VaObserver::PreUpdateFrameCallback, VaObserver::PreUpdateFrame);
VA_INITIALIZE_CALLBACK(VaObserver::PostUpdateFrameCallback, VaObserver::PostUpdateFrame);
VA_INITIALIZE_CALLBACK(VaObserver::RedrawWindowCommand, VaObserver::RedrawWindow);
VA_INITIALIZE_CALLBACK(VaObserver::CameraSetLatLonCommand, VaObserver::CameraSetLatLon);
VA_INITIALIZE_CALLBACK(VaObserver::CameraSetAzElDistCommand, VaObserver::CameraSetAzElDist);
VA_INITIALIZE_CALLBACK(VaObserver::CameraSetYawPitchDistCommand, VaObserver::CameraSetYawPitchDist);
VA_INITIALIZE_CALLBACK(VaObserver::CameraSetProjectionCommand, VaObserver::CameraSetProjection);
VA_INITIALIZE_CALLBACK(VaObserver::CameraSetCenterCommand, VaObserver::CameraSetCenter);
VA_INITIALIZE_CALLBACK(VaObserver::ExternalApplicationConnectedCallback, VaObserver::ExternalApplicationConnected);
VA_INITIALIZE_CALLBACK(VaObserver::ExternalApplicationDisconnectedCallback, VaObserver::ExternalApplicationDisconnected);
VA_INITIALIZE_CALLBACK(VaObserver::ExternalApplicationControlChangedCallback,
                       VaObserver::ExternalApplicationControlChanged);
VA_INITIALIZE_CALLBACK(VaObserver::ExternalApplicationConnectCommand, VaObserver::ExternalApplicationConnect);
VA_INITIALIZE_CALLBACK(VaObserver::ExternalApplicationDisconnectCommand, VaObserver::ExternalApplicationDisconnect);
VA_INITIALIZE_CALLBACK(VaObserver::ExternalApplicationTakeControlCommand, VaObserver::ExternalApplicationTakeControl);
VA_INITIALIZE_CALLBACK(VaObserver::ECI_ModeChangedCallback, VaObserver::ECI_ModeChanged);
VA_INITIALIZE_CALLBACK(VaObserver::SelectionClearedCallback, VaObserver::SelectionCleared);
VA_INITIALIZE_CALLBACK(VaObserver::BeginSelectionChangeCallback, VaObserver::BeginSelectionChange);
VA_INITIALIZE_CALLBACK(VaObserver::EndSelectionChangeCallback, VaObserver::EndSelectionChange);
VA_INITIALIZE_CALLBACK(VaObserver::HandleSelectionBoxCallback, VaObserver::HandleSelectionBox);
VA_INITIALIZE_CALLBACK(VaObserver::HandleSelectionBoxResultCallback, VaObserver::HandleSelectionBoxResult);
VA_INITIALIZE_CALLBACK(VaObserver::AddNewTeamCallback, VaObserver::AddNewTeam);
VA_INITIALIZE_CALLBACK(VaObserver::BeginUserStatCallback, VaObserver::BeginUserStat);
VA_INITIALIZE_CALLBACK(VaObserver::EndUserStatCallback, VaObserver::EndUserStat);
VA_INITIALIZE_CALLBACK(VaObserver::ModelsReloadedCallback, VaObserver::ModelsReloaded);
VA_INITIALIZE_CALLBACK(VaObserver::ModelScaleChangedCallback, VaObserver::ModelScaleChanged);

} // end namespace vespa.
