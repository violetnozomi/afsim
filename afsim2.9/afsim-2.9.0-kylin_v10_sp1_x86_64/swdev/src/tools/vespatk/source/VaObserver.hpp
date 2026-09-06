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

#ifndef VaObserver_hpp
#define VaObserver_hpp

#include "vespatk_export.h"

#include <string>

#include "VaCallback.hpp"
#include "VaDefs.hpp"

class UtColor;
class UtoMapProjection;

namespace vespa
{
class VaAttachment;
class VaEntity;
class VaObject;
class VaScenario;
class VaSelectionBox;
class VaViewer;

//! A collection of callback objects.
//!
//! This class maintains a set of callback objects to which simulation participants can
//! 'subscribe' to receive notification of significant events within the simulation. The
//! observer concept allows simulation developers to learn of certain events without having
//! to modify the core software that is issuing the event. Observers are typically used for
//! data collection and logging.
//!
//! Each variable represents a 'publisher' to which others can 'subscribe' using the
//! following idiom:
//!
//! \code
//! Class MyObserver
//! {
//!    public:
//!       void EntityAdded(double          aTime,
//!                        DrawableEntity* aEntityPtr,
//!                        Scenario*       aScenarioPtr);
//!    private:
//!       VaCallbackHolder   mCallbacks;
//! };
//!
//! // And then in the constructor.
//! mCallbacks.Add(VaObserver::EntityAdded.Connect(&MyObserver::EntityAdded, this));
//! \endcode
//!
//! One can register non-member method or a static method as an observer:
//!
//! \code
//! void EntityAdded(double          aTime,
//!                  DrawableEntity* aEntityPtr,
//!                  Scenario*       aScenarioPtr)
//! { ... }
//!
//! VaObserver::Add(EntityAdded);
//! \endcode

class VESPATK_EXPORT VaObserver
{
public:
   VaObserver();

   virtual ~VaObserver();

   virtual bool Initialize();

   virtual void Shutdown();

   //-----------------------------------------------------------------------
   // Create pre-defined group names
   //-----------------------------------------------------------------------
   static const std::string cTIME_MANAGER_GROUP;

   //-----------------------------------------------------------------------
   // Typedefs for the various callbacks used with the observer.
   //-----------------------------------------------------------------------

   // Application callbacks.

   typedef VaCallbackListN<void(VaObject*)> ResourceClaimedCallback;
   // Gui callbacks.

   typedef VaCallbackListN<void(VaObject*)> PopupDialogCommand;

   typedef VaCallbackListN<void(VaViewer*)> ViewerResizedCallback;

   typedef VaCallbackListN<void(VaViewer*)> ViewMatrixChangedCallback;

   typedef VaCallbackListN<bool(VaViewer*, int, int, int, int)> MouseButtonCallback;

   // Scenario callbacks.
   typedef VaCallbackListN<void(VaScenario*)> ScenarioAddedCallback;

   typedef VaCallbackListN<void(VaScenario*)> ScenarioRemovedCallback;

   typedef VaCallbackListN<void(VaScenario*)> ScenarioModifiedCallback;

   typedef VaCallbackListN<void(VaScenario*)> ScenarioCurrentChangedCallback;

   // Entity callbacks.
   typedef VaCallbackListN<void(VaEntity*)> EntityAddedCallback;

   typedef VaCallbackListN<void(VaEntity*)> EntityRemovedCallback;

   typedef VaCallbackListN<void(VaEntity*)> EntityDeletedCallback;

   typedef VaCallbackListN<void(VaViewer*, int, VaEntity*)> EntitySelectCommand;

   typedef VaCallbackListN<void(VaViewer*, int, VaEntity*)> EntitySelectedCallback;

   typedef VaCallbackListN<void(VaViewer*, int, const std::vector<VaEntity*>&)> EntitiesSelectedCallback;

   typedef VaCallbackListN<void()> SelectionChangedCallback;

   typedef VaCallbackListN<void(const std::string&, const std::string&, VaEntity*)> EntityNameChangedCallback;

   typedef VaCallbackListN<void(VaAttachment*)> AttachmentAddedCallback;

   typedef VaCallbackListN<void(VaAttachment*, const VaAttachmentSubId&)> AttachmentRemovedCallback;

   typedef VaCallbackListN<void(VaAttachment*, const VaAttachmentSubId&)> AttachmentDeletedCallback;

   typedef VaCallbackListN<void(VaViewer*, int, VaAttachment*, const VaAttachmentSubId&)> AttachmentSelectedCallback;

   typedef VaCallbackListN<void(VaEntity*, bool)> EntityMovedCallback;

   // Viewer initialized
   typedef VaCallbackListN<void(VaViewer*)> ViewerInitializedCallback;

   // Viewer is destroyed
   typedef VaCallbackListN<void(VaViewer*)> ViewerDestroyedCallback;

   // Camera callbacks.
   typedef VaCallbackListN<void(VaViewer*)> CameraUpdatedCallback;

   typedef VaCallbackListN<void(VaViewer*, int, int)> UpdateCursorPositionCallback;

   // Utility callbacks.

   typedef VaCallbackListN<void(int, int)> SetUnitFormatCommand;

   typedef VaCallbackListN<void(int, int)> ChangedUnitFormatCallback;

   typedef VaCallbackListN<void(double, double)> SymbolSizeChangedCallback;

   typedef VaCallbackListN<void(int, const std::string&)> UserMessageCommand;

   typedef VaCallbackListN<void()> MapLoadedMapCallback;

   typedef VaCallbackListN<void(unsigned int, const UtoMapProjection*)> MapProjectionChangedCallback;

   typedef VaCallbackListN<void(const float&)> SetIntensityCommand;

   typedef VaCallbackListN<void(const std::string&, const float&)> SetGroupIntensityCommand;

   typedef VaCallbackListN<void(const float&)> SetSaturationCommand;

   typedef VaCallbackListN<void(const std::string&, const float&)> SetGroupSaturationCommand;

   typedef VaCallbackListN<void(const float&)> SetContrastCommand;

   typedef VaCallbackListN<void(const std::string&, const float&)> SetOpacityCommand;

   // Time callbacks.

   typedef VaCallbackListN<void(double, double, double, double)> TimeDirtyCallback;

   typedef VaCallbackListN<void(double, double, double, double)> TimeUpdatedCallback;

   typedef VaCallbackListN<void(int)> TimeModeChangedCallback;

   typedef VaCallbackListN<void()> PreUpdateFrameCallback;
   typedef VaCallbackListN<void()> PostUpdateFrameCallback;
   // Viewer callbacks.
   typedef VaCallbackListN<void()> RedrawWindowCommand;

   // Camera callbacks.
   typedef VaCallbackListN<void(VaViewer*, double, double)> CameraSetLatLonCommand;

   typedef VaCallbackListN<void(VaViewer*, double, double, double)> CameraSetAzElDistCommand;
   typedef VaCallbackListN<void(VaViewer*, double, double, double)> CameraSetYawPitchDistCommand;

   typedef VaCallbackListN<void(int)> CameraSetProjectionCommand;

   typedef VaCallbackListN<void()> CameraSetCenterCommand;

   typedef VaCallbackListN<void(const std::string&, unsigned int, bool)> ExternalApplicationConnectedCallback;

   typedef VaCallbackListN<void(const std::string&, unsigned int, bool)> ExternalApplicationDisconnectedCallback;

   typedef VaCallbackListN<void(const std::string&, unsigned int, bool)> ExternalApplicationControlChangedCallback;

   typedef VaCallbackListN<void()> ExternalApplicationConnectCommand;

   typedef VaCallbackListN<void()> ExternalApplicationDisconnectCommand;

   typedef VaCallbackListN<void()> ExternalApplicationTakeControlCommand;

   typedef VaCallbackListN<void(bool)> ECI_ModeChangedCallback;

   typedef VaCallbackListN<void(VaViewer*)> SelectionClearedCallback;

   typedef VaCallbackListN<void(VaViewer*)> BeginSelectionChangeCallback;

   typedef VaCallbackListN<void(VaViewer*)> EndSelectionChangeCallback;

   typedef VaCallbackListN<void(VaViewer*, const VaSelectionBox&)> HandleSelectionBoxCallback;

   typedef VaCallbackListN<void(const std::string&, VaViewer*, const VaSelectionBox&)> HandleSelectionBoxResultCallback;

   typedef VaCallbackListN<void(const std::string&)> AddNewTeamCallback;

   typedef VaCallbackListN<void(const std::string&)> BeginUserStatCallback;
   typedef VaCallbackListN<void(const std::string&)> EndUserStatCallback;

   typedef VaCallbackListN<void()> ModelsReloadedCallback;

   typedef VaCallbackListN<void(float)> ModelScaleChangedCallback;

   //-----------------------------------------------------------------------
   // Static references to callbacks
   //-----------------------------------------------------------------------

   //! An object has claimed a named resource, the filter should be the resource name
   //! @param aObjectPtr the pointer to the object making the claim.  Existing claims should be renounced.
   static ResourceClaimedCallback ResourceClaimed;

   //! Application has requested a popup
   //! @param aObjectPtr The object associated with he dialog.
   static PopupDialogCommand PopupDialog;

   //! A viewer has been resized.
   //! @param aViewer The resized viewer.
   static ViewerResizedCallback ViewerResized;

   //! A viewer has changed its matrix
   //! @param aViewer The viewer affected
   static ViewMatrixChangedCallback ViewMatrixChanged;

   //! Mouse events.
   //! @param The viewer in which the button event occurred
   //! @param The button pressed/unpressed
   //! @param The x coordinate
   //! @param The y coordinate
   //! @param The state flags
   static MouseButtonCallback ButtonClick;
   static MouseButtonCallback ButtonDoubleClick;
   static MouseButtonCallback ButtonDown;
   static MouseButtonCallback ButtonDrag;
   static MouseButtonCallback ButtonUp;

   //! A scenario file was added to the environment.
   //! @param aScenarioPtr The scenario that was added.
   static ScenarioAddedCallback ScenarioAdded;

   //! A scenario file was removed to the environment.
   //! @param aScenarioPtr The scenario that was removed.
   static ScenarioRemovedCallback ScenarioRemoved;

   //! A scenario file was modified.
   //! @param aScenarioPtr The scenario that was modified.
   static ScenarioModifiedCallback ScenarioModified;

   //! The current scenario has changed.
   //! @param aScenarioPtr The new current scenario.
   static ScenarioCurrentChangedCallback ScenarioCurrentChanged;

   //! An entity was added to a scenario.
   //! @param aEntityPtr The entity that was added to the scenario.
   static EntityAddedCallback EntityAdded;

   //! An entity was removed from a scenario.  This is not due to a deletion, most likely
   //! and the entity could continue to exist in another scenario... when in doubt use
   //! EntityDeleted
   //! @param aEntityPtr The entity that was removed from the scenario.
   static EntityRemovedCallback EntityRemoved;

   //! An entity was deleted.
   //! @param aEntityPtr The entity that was deleted.
   static EntityDeletedCallback EntityDeleted;

   //! Select or deselect the specified entity.
   //! @param aViewer the viewer to be selected in
   //! @param aSelect 1 to select, 0 to deselect
   //! @param aEntityPtr The entity to select
   static EntitySelectCommand EntitySelect;

   //! An entity was selected or deselected
   //! @param aViewer the viewer the selection applies to
   //! @param aSelect 1 is selected, 0 is deselected
   //! @param aEntityPtr The entity that was selected or deselected
   static EntitySelectedCallback EntitySelected;

   //! A list of entity were selected or deselected
   //! @param aViewer the viewer the selection applies to
   //! @param aSelect 1 is selected, 0 is deselected
   //! @param aEntityPtrList a List of entity pointer that were selected or deselected
   static EntitiesSelectedCallback EntitiesSelected;

   //! Called when the selection is changed
   static SelectionChangedCallback SelectionChanged;

   //! An entity's name was changed.
   //! @param aOldName The old name.
   //! @param aNewName The new name.
   //! @param aEntityPtr The entity whose name was changed.
   static EntityNameChangedCallback EntityNameChanged;

   //! An attachment was added to an entity.
   //! @param aAttachmentPtr The attachment that was added to the entity.
   //! @param aAttachmentSubPartId An id containing any additional selection information, used for zonepoint sub-selection.
   static AttachmentAddedCallback AttachmentAdded;

   //! An attachment was removed from an entity.
   //! @param aAttachmentPtr The attachment that was removed from the entity.
   //! @param aAttachmentSubPartId An id containing any additional selection information, used for zonepoint sub-selection.
   static AttachmentRemovedCallback AttachmentRemoved;

   //! An attachment was deleted.
   //! @param aAttachmentPtr The attachment that was deleted.
   //! @param aAttachmentSubPartId An id containing any additional selection information, used for zonepoint sub-selection.
   static AttachmentDeletedCallback AttachmentDeleted;

   //! An attachment was selected.
   //! @param aViewer the viewer in which the selection occurred.
   //! @param aSelect 1 is selected, 0 is deselected
   //! @param aHitId  The attachment / part id
   static AttachmentSelectedCallback AttachmentSelected;

   //! An entity changed position
   //! @param aEntityPtr The entity who moved.
   //! @param aMoving Indicates if the entity is moving.
   // VA_DECLARE_CALLBACK(EntityMovedCallback, EntityMoved);
   static EntityMovedCallback EntityMoved;

   //! Indicated that a viewer has been initialized
   //! @param aViewerPtr the viewer
   static ViewerInitializedCallback ViewerInitialized;

   //! Indicates that a viewer is about to be deleted
   //! @param aViewerPtr the viewer
   static ViewerDestroyedCallback ViewerDestroyed;

   //! Indicated that the camera has been updated.
   //! @param aViewerPtr The viewer whose camera was updated.
   static CameraUpdatedCallback CameraUpdated;

   //! Indicates the mouse cursor has moved within a viewer
   //! @param aViewerPtr the viewer
   //! @param aMouseX mouse horizontal screen position, left to right
   //! @param aMouseY mouse vertical screen position, bottom to top
   static UpdateCursorPositionCallback UpdateCursorPosition;

   //! Set the units and/or precision to use for a given measurement type
   //! @param aMeasurementType The type of data to apply the unit to.
   //! @param aUnit The unit type to apply to the measurements of aMeasurementType
   static SetUnitFormatCommand SetUnitFormat;

   //! Indicates that the units and/or precision to use for a given measurement type have changed.
   //! @param aMeasurementType The type of data to apply the unit to.
   //! @param aUnit The unit type to apply to the measurements of aMeasurementType
   static ChangedUnitFormatCallback ChangedUnitFormat;

   //! Indicates an event of interest to the user has occurred.
   //! @param aLevel An integer indicating the critical level of the error.
   //           0 indicates a message of debug interest (i.e. scenario load time.)
   //           1 indicates an error or warning.
   //           2 indicates a message of debug interest and that the panel should be made visible.
   //           3 indicates an error or warning and that the panel should be made visible.
   //! @param aMessage A message associated with the error.
   static UserMessageCommand UserMessage;

   //! Indicate a different map has loaded
   static MapLoadedMapCallback MapLoadedMap;

   //! Indicate a change in map projection
   //! @param aViewerId Id of the viewer affected
   //! @param aProjectionIndex The projection
   static MapProjectionChangedCallback MapProjectionChanged;

   //! Indicates that map color intensity should be modified
   //! @param aValue The intensity value.
   static SetIntensityCommand SetIntensity;

   //! Indicates that map color saturation should be modified
   //! @param aValue The saturation value.
   static SetSaturationCommand SetSaturation;

   //! Indicates that map color contrast should be modified
   //! @param aValue the contrast value
   static SetContrastCommand SetContrast;

   //! Indicates that map color intensity (or group if specified) should be modified
   //! @param aGroupName
   //! @param aValue
   static SetGroupIntensityCommand SetGroupIntensity;

   //! Indicates that map (or group if name specified) color saturation should be modified
   //! @param aGroupName
   //! @param aValue
   static SetGroupSaturationCommand SetGroupSaturation;

   //! Indicates that map color opacity should be modified
   //! @param aValue The opacity value.
   static SetOpacityCommand SetOpacity;

   //! Indicates that the global symbol size was changed.
   //! @param aSymbolSize The new symbol size.
   //! @param aOldSymbolSize The old symbol size.
   static SymbolSizeChangedCallback SymbolSizeChanged;

   //! Indicates that one or more time values has changed.
   //! @param aTime
   //! @param aRate
   //! @param aMinTime
   //! @param aMaxTime
   static TimeDirtyCallback TimeDirty;

   //! Indicates that the time has been updated.
   //! @param aTime
   //! @param aRate
   //! @param aMinTime
   //! @param aMaxTime
   static TimeUpdatedCallback TimeUpdated;

   //! Indicates the play mode has changed (play, pause, reverse)
   //! @param aTimeMode The mode enumeration
   static TimeModeChangedCallback TimeModeChanged;

   //! Executes before a frame update
   static PreUpdateFrameCallback PreUpdateFrame;

   //! Executes after a frame update
   static PostUpdateFrameCallback PostUpdateFrame;

   static BeginUserStatCallback BeginUserStat;
   static EndUserStatCallback   EndUserStat;

   /////////////////////////////////////////////////////////////////////////////
   // Deprecated 'TimeManager' access.  Use VaTimeManager directly.
   /////////////////////////////////////////////////////////////////////////////
   //! Tells the time manager to update.
   static void TimeManagerUpdateFrame();

   //! Tells the time manager to start playing forwards.
   static void TimeManagerPlay();

   //! Tells the time manager to start playing backwards.
   static void TimeManagerRewind();

   //! Tells the time manager to pause.
   static void TimeManagerPause();

   //! Tells the time manger to reset.
   static void TimeManagerReset();

   //! Sets the time manager's current time.
   //! @param aTime The time to set.
   //! @param aForceReport Forces a TimeUpdateCallback to be sent
   static void TimeManagerSetTime(double aTime, bool aForceReport);

   //! Sets the time manager's current rate.
   //! @param aRate The rate to set.
   static void TimeManagerSetRate(double aRate);

   //! Sets the update rate of the simulation (this can effect live play rate, and window growth rate)
   //! @param aRate The rate to set
   static void TimeManagerSetSimulationRate(double aRate);
   static void TimeManagerSetSimulationPause();
   static void TimeManagerSetSimulationGo();

   //! Set the time manager's minimum time.
   //! @param aMinTime The minimum time to set.
   static void TimeManagerSetMinTime(double aMinTime);

   //! Set the time manager's maximum time.
   //! @param aMinTime The maximum time to set.
   static void TimeManagerSetMaxTime(double aMaxTime);

   //! Set whether the time manager should loop back to the
   //! beginning when the end is reached.
   //! @param aLoopEnabled
   static void TimeManagerSetLoopEnabled(bool aLoopEnabled);

   //! Set whether the max time should increase with realtime
   //! @param aIncreateMaxTimeWithRealtime
   static void TimeManagerSetRealtimeClock(bool aState);

   //! Set the time manager's current time using a number in the range [0, 1]
   //! @param aNormalizedTime The current time to set [0, 1].
   static void TimeManagerSetNormalizedTime(double aNormTime);


   //! Redraws recipients
   static RedrawWindowCommand RedrawWindow;

   //! Sets the camera's position to the provide latitude and longitude.
   //! @param aViewer     The viewer that the camera belongs to.
   //! @param aLat        The desired latitude in degrees.
   //! @param aLon        The desired longitude in degrees.
   static CameraSetLatLonCommand CameraSetLatLon;

   //! Sets the camera's position to the provide latitude and longitude.
   //! @param aViewer     The viewer that the camera belongs to.
   //! @param aAz         The desired azimuth in degrees.
   //! @param aEl         The desired elevation in degress.
   //! @param aDist       The distance in meters.
   static CameraSetAzElDistCommand CameraSetAzElDist;

   //! Sets the camera's orientation.
   //! @param aViewer     The viewer that the camera belongs to.
   //! @param aAz         The desired yaw in degrees.
   //! @param aEl         The desired pitch in degress.
   //! @param aDist       The distance in meters.
   static CameraSetYawPitchDistCommand CameraSetYawPitchDist;

   //! Sets the camera's projection
   //! @param aProjection 0 - perspective, 1 - orthographic
   static CameraSetProjectionCommand CameraSetProjection;

   //! Centers the camera on the currently selected entities.
   static CameraSetCenterCommand CameraSetCenter;

   //! Callback indicates an application has connected over ATIP
   //! @param aAppName The name of the external app.
   //! @param aId An integer id
   //! @param aCommand Indicates if the app insists on taking command
   static ExternalApplicationConnectedCallback ExternalApplicationConnected;

   //! Callback indicates an application has connected over ATIP
   //! @param aAppName The name of the external app.
   //! @param aId An integer id
   //! @param aCommand Indicates if the app had command
   static ExternalApplicationDisconnectedCallback ExternalApplicationDisconnected;

   //! Callback indicates an application has changed its role over ATIP
   //! @param aAppName The name of the external app.
   //! @param aId An integer id
   //! @param aCommand Indicates if the app insists on taking command
   static ExternalApplicationControlChangedCallback ExternalApplicationControlChanged;

   //! Callback to initiate an ATIP connection
   static ExternalApplicationConnectCommand ExternalApplicationConnect;

   //! Callback to end an ATIP connection
   static ExternalApplicationDisconnectCommand ExternalApplicationDisconnect;

   //! Callback to take control of an ATIP connection
   static ExternalApplicationTakeControlCommand ExternalApplicationTakeControl;

   //! Command the application to adjust drawing to use an ECI coordinate system
   //! @param aState true-ECI false-ECEF (default)
   static ECI_ModeChangedCallback ECI_ModeChanged;

   //! Action taken has cleared the selection list
   //! @param aViewerPtr the viewer in which the clear occurred
   static SelectionClearedCallback SelectionCleared;

   //! The view is beginning a selection change, if the application
   //! is concerned about relationships between the old and new selections
   //! it should start collecting the changes
   //! @param aViewerPtr the viewer in which the change is occuring
   static BeginSelectionChangeCallback BeginSelectionChange;


   //! The view is ending a selection change, if the application
   //! is concerned about relationships between the old and new selections
   //! it should examine the changes now
   //! @param aViewerPtr the viewer in which the change is occuring
   static EndSelectionChangeCallback EndSelectionChange;

   //! A selection box was just drawn. Use this callback to handle the response.
   //! @param aViewerPtr the associated viewer.
   //! @param aSelectionBox the VaSelectionBox object
   static HandleSelectionBoxCallback HandleSelectionBox;

   //! A selection box was just drawn. Use this callback to handle the response.
   //! @param aBoxOperation the box operation to perform
   //! @param aViewerPtr the associated viewer.
   //! @param aSelectionBox the VaSelectionBox object
   static HandleSelectionBoxResultCallback HandleSelectionBoxResult;

   //! A new team has been added to the scenario.  Allow GUIs to update for this
   //! @param aTeamName the name of the new team
   static AddNewTeamCallback AddNewTeam;

   //! Notifies subscribers that the model set has been reloaded.
   static ModelsReloadedCallback ModelsReloaded;

   //! Indicate a model has had a scale defined for it
   static ModelScaleChangedCallback ModelScaleChanged;

private:
   bool mInitialized;
};
} // end namespace vespa.

#endif
