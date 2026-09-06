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

#ifndef WSFSENSOR_HPP
#define WSFSENSOR_HPP

#include "wsf_export.h"

#include <memory>
#include <vector>

class UtInput;
#include "UtQuaternion.hpp"
#include "UtRandom.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfCategoryList.hpp"
#include "WsfComponentList.hpp"
class WsfEM_Rcvr;
class WsfEM_Xmtr;
#include "WsfEvent.hpp"
#include "WsfMeasurement.hpp"
class WsfMessage;
class WsfPlatform;
class WsfSensor;
#include "WsfSensorComponent.hpp"
class WsfSensorMode;
class WsfSensorModeList;
class WsfSensorResult;
class WsfSensorScheduler;
class WsfSensorTracker;
class WsfSingleSensorObserver;
class WsfTrack;
class WsfTrackId;
#include "WsfTrackListFwd.hpp"
#include "script/WsfScriptContext.hpp"

//! A base class for all types of sensing systems.
//! A sensor acts as the 'sensory organs' (eyes, ears, etc.) of a platform.
//!
//! A sensor is considered to be a collection of 'modes', where each mode is defined to be a
//! particular set of operating characteristics. A sensor is implemented by:
//! - Creating a subclass of WsfSensor. This is generally a very simple class that populates
//! a few base class members.
//! - Creating a subclass of WsfSensorMode. This is generally where everything happens. Here
//! one creates the WsfSensorMode::AttemptsToDetect method that performs the actual detection
//! chances.
//!
//! Note that ALL sensors are inherently 'multi-mode' even if they only support one mode.
//! WsfSensorModeList provides all of the basic support for defining modes from the input stream
//! and selecting and deselecting modes at run time.
//!
//! In addition, there are a a couple of support classes.
//! - WsfSensorScheduler which controls how detection chances are scheduled.
//! WsfDefaultSensorScheduler provides an implementation for that is applicable for
//! most all scanning and tracking sensors.
//! - WsfSensorTracker which controls the initiating, updating and deleting tracks based on
//! the results of sensor detection attempts. WsfDefaultSensorTracker provides an implementation
//! that is usable for most scanning and tracking sensors.
//!
//! The use of a scheduler and tracker are not required. For instance, an imaging sensor looks
//! at everything at once during the detection cycle and produces an image instead of a track.
//!
//! The common detection mechanism is implemented by the abstract virtual method WsfSensorMode::AttemptToDetect.
//! Every sensor mode implementation must provide this method and it should be implemented using the following pattern:
//!
//! a) Ensure the target is marked as 'concealed'.
//! b) Ensure the target is in range.
//! c) Ensure the target is not masked by the horizon.
//! d) Ensure the target is in the field of view.
//! e) Compute the probability of detection (sensor specific).
//! f) If detectable then ensure the target is not masked by the terrain.
//!
//! Note that the above pattern performs the simplest (least computationally expensive)
//! operations first. Subsequent steps should be bypassed if an operation fails. Note
//! that terrain masking is performed last as it is the most computationally intensive.
//!
//! @see WsfSensorMode
//! @see WsfSensorScheduler
//! @see WsfSensorTracker
//! @see WsfDefaultSensorScheduler
//! @see WsfDefaultSensorTracker
//! @see WsfSebsirModeList
class WSF_EXPORT WsfSensor : public WsfArticulatedPart
{
public:
   //! 'Result' used to be a nested class but it needed to be pulled out to support components.
   //! A typedef is used here so existing code will continue to compile.
   using Result = WsfSensorResult;

   //! An enumeration that defines the general classification of a sensor.
   //! Sensors are often classified by the spectrum in which they operate and if they are
   //! 'active' or 'passive'. It is the responsibility of the sensor constructor to define
   //! this field.
   //!
   //! The 'classification' of a sensor may be tested using the 'IsClass' methods.
   enum
   {
      //! @name Detection method.

      //! The sensor detects reflected signals that originate from this sensor.
      cACTIVE = 0x00000001,
      //! The sensor detects reflected signals that originate from another transmitter.
      cSEMI_ACTIVE = 0x00000002,
      //! The sensor detects signals that are emitted directly by the target or reflected from a
      //! natural source (such as the Sun).
      cPASSIVE = 0x00000004,

      //! @name Operating spectrum.
      //@{
      cRADIO    = 0x00000010, //!< The sensor operates in the radio frequency spectrum
      cINFRARED = 0x00000020, //!< The sensor operates in the infrared frequency spectrum
      cVISUAL   = 0x00000040, //!< The sensor operates in the visual frequency spectrum
      cACOUSTIC = 0x00000080, //!< The sensor operates in the acoustic frequency spectrum
                              //@}

      //! @name Other characteristics.
      //@{
      cIMAGING = 0x00001000 //!< The sensor produces an image
                            //@}
   };

   //! The sensor settings provided to the AttemptToDetect method.
   //! This class provides the sensor settings for the detection attempt.
   class WSF_EXPORT Settings
   {
   public:
      Settings()
         : mModeIndex(0)
         , mRequiredPd(0.0)
         , mLockonTime(-1.0)
      {
      }

      //! The index of the mode to be used for the current attempt.
      //! If not specified then the first mode will be used.
      size_t mModeIndex;

      //! The required probability-of-detection.
      //!
      //! This is an optional value that can allow significant optimizations to be made.
      //! For example, if the computed Pd is not sufficient then the expensive terrain
      //! masking calculation can be bypassed.
      //!
      //! The default value of 0 prevents any optimization.
      //!
      //! The value must be in the range [0 .. 1]. Typical values are:
      //!
      //! - A value of '1.0 - GetSimulation()->GetRandom().Uniform()'. This would be typical for use within
      //! a normal simulation environment.
      //! - A value of '0.5' would be used to generate a Pd=0.5 detectability contour.
      //! - A value of '0.0' means that the probability of detection is not considered in
      //! the detectability.
      double mRequiredPd;

      //! The time when the sensor 'locked-on' to the target in the current mode.
      //!
      //! This is an optional value that indicates when the sensor 'locked-on' to the target.
      //! It is typically used by sensors that model trackers, where the detection threshold
      //! is often reduced once the tracker has 'locked-on' to the target.
      //!
      //! The default value of '< 0.0' indicates the sensor is not 'locked-on' to the target.
      double mLockonTime;
   };

   WsfSensor(const WsfScenario& aScenario);
   ~WsfSensor() override;

   //! @name Component list support.
   //! A processor contains a list of components.
   //@{
   using ComponentList = WsfComponentListT<WsfSensorComponent>;
   ComponentList&       GetComponents() { return mComponents; }
   const ComponentList& GetComponents() const { return mComponents; }
   //@}

   //! @name Component infrastructure methods not provided by WsfPlatformPart.
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_SENSOR; }
   //@}

   bool AllowDetectionChancesFor(WsfPlatform* aPlatformPtr);
   void AddIgnoredCategory(const std::string& aCategory);

   virtual bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult);

   //! @name Common infrastructure methods.
   //@{
   const char* GetScriptClassName() const override;
   //! Clone this object.
   //! @return a pointer to the new object.
   WsfSensor*   Clone() const override = 0;
   bool         PreInitialize(double aSimTime) override;
   bool         Initialize(double aSimTime) override;
   bool         Initialize2(double aSimTime) override;
   void         PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) override;
   void         PlatformDeleted(WsfPlatform* aPlatformPtr) override;
   void         PreInput() override;
   bool         ProcessInput(UtInput& aInput) override;
   void         Update(double aSimTime) override;
   void         UpdatePosition(double aSimTime) override;
   virtual void PerformScheduledDetections(double aSimTime);
   //@}

   //! @name Thread safe methods.
   //@{
   //! Returns true if the sensor update is thread safe.
   bool ThreadSafe() const { return mThreadSafe; }
   void SetThreadSafe() { mThreadSafe = true; }
   void SetNotThreadSafe() { mThreadSafe = false; }
   //@}

   //! @name Calibration data methods.
   //@{
   //! Returns true if the sensor calibration should be output.
   bool ShowCalibrationData() const { return mShowCalibrationData; }
   void SetShowCalibrationData(bool aShowCalibrationData) { mShowCalibrationData = aShowCalibrationData; }
   //@}

   //! @name Methods to attach, delete and notify to sensor observers.
   //! Observers receive notification of sensor updates, detection attempts and deletion.
   //@{
   void AttachObserver(WsfSingleSensorObserver* aObserverPtr);
   void DetachObserver(WsfSingleSensorObserver* aObserverPtr);
   void NotifyDeleted();
   void NotifyTrackInitiated(double aSimTime, const WsfTrack& aTrack);
   void NotifyTrackUpdated(double aSimTime, const WsfTrack& aTrack);
   void NotifyTrackDropped(double aSimTime, const WsfTrack& aTrack);
   void NotifyTargetUpdated(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult);
   void NotifyTurnedOff(double aSimTime);
   void NotifyTurnedOn(double aSimTime);

   virtual void NotifySensorDetectionAttempted(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult);
   //@}

   //! @name Methods to query the class of a sensor.
   //! These methods should be only be used to determine the general way in which a sensor operates.
   //@{

   //! Returns the sensor class information
   int GetClass() const { return mClass; }

   //! Is the sensor an 'active' sensor?
   bool IsClassActive() const { return (mClass & cACTIVE) != 0; }

   //! Is the sensor a 'passive' sensor?
   bool IsClassPassive() const { return (mClass & cPASSIVE) != 0; }

   //! Is the sensor a 'semi-active' sensor.
   bool IsClassSemiActive() const { return (mClass & cSEMI_ACTIVE) != 0; }

   //! Is this an 'acoustic' sensor?
   bool IsClassAcoustic() const { return (mClass & cACOUSTIC) != 0; }

   //! Is this an 'infrared' sensor?
   bool IsClassInfrared() const { return (mClass & cINFRARED) != 0; }

   //! Is this a 'radio frequency' sensor?
   bool IsClassRadio() const { return (mClass & cRADIO) != 0; }

   //! Is this a 'visual' sensor?
   bool IsClassVisual() const { return (mClass & cVISUAL) != 0; }

   //! Does this sensor produce an image?
   bool IsClassImaging() const { return (mClass & cIMAGING) != 0; }

   //@}

   //! @name Status methods.
   //@{
   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;
   void SetNonOperational(double aSimTime) override;
   void SetOperational(double aSimTime) override;
   void PartBroken(double aSimTime) override;
   //@}

   //! Get the update interval for sensing chances.
   //! @return Update interval.
   double GetUpdateInterval() const override { return mUpdateInterval; }

   //! @name Transmitter/receiver access methods.
   //! These methods support accessing the underlying transmitter and receiver components.
   //@{
   virtual size_t      GetEM_RcvrCount() const;
   virtual WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const;
   virtual size_t      GetEM_RcvrCount(size_t aModeIndex) const;
   virtual WsfEM_Rcvr& GetEM_Rcvr(size_t aModeIndex, size_t aIndex) const;
   virtual size_t      GetEM_XmtrCount() const;
   virtual WsfEM_Xmtr& GetEM_Xmtr(size_t aIndex) const;
   virtual size_t      GetEM_XmtrCount(size_t aModeIndex) const;
   virtual WsfEM_Xmtr& GetEM_Xmtr(size_t aModeIndex, size_t aIndex) const;
   //@}

   //! @name Field of view methods.
   //! Determines if the platform is within the sensor field of view.
   //@{
   virtual bool WithinFieldOfView(double aSimTime, WsfPlatform* aPlatformPtr);
   virtual bool WithinFieldOfView(double aSimTime, WsfTrack* aTrackPtr);
   //@}

   //! @name Mode selection methods.
   //! These methods support the control of a multi-mode sensor.
   //! @note Not all implementations support multiple modes.
   //@{
   virtual WsfStringId    GetCurrentModeName() const;
   virtual WsfSensorMode* GetCurrentMode() const;
   virtual size_t         GetModeCount() const;
   virtual WsfSensorMode* GetModeEntry(size_t aModeIndex) const;
   virtual size_t         GetModeIndex(WsfStringId aModeNameId) const;
   virtual WsfStringId    GetModeName(size_t aModeIndex) const;
   virtual void           DeselectMode(double aSimTime, WsfStringId aModeNameId);
   virtual void           SelectMode(double aSimTime, WsfStringId aModeNameId);
   virtual void           ScheduleModeChange(double aSimTime, bool aSelectModeFlag, WsfStringId aModeNameId);
   virtual bool           IsModeChangeScheduled() const;
   virtual double         GetModeSelectDelay() const;

   //! Return a pointer to the mode list.
   //! @returns A pointer to the mode list. This may be zero if the sensor does not support mode lists
   //! @note This is not valid until Initialize has been called.
   //! @note This return value is non-const, but the list should only be modified with GREAT care.

   WsfSensorModeList* GetModeList() const { return mModeListPtr.get(); }
   //@}

   //! @name Track request methods
   //! These methods support the control of a cued sensor.
   //@{
   virtual int  GetActiveRequestCount() const;
   virtual int  GetActiveRequestCount(WsfStringId aModeNameId) const;
   virtual int  GetMaximumRequestCount() const;
   virtual int  GetMaximumRequestCount(WsfStringId aModeNameId) const;
   virtual bool HaveRequestFor(const WsfTrackId& aTrackId) const;
   virtual bool StartTracking(double aSimTime, const WsfTrack& aTrack, WsfStringId aModeNameId);
   virtual bool StopTracking(double aSimTime, const WsfTrackId& aRequestId);
   //@}

   //! @name Track status notification methods.
   //! These methods are called from sensor components to inform the simulation
   //! about the change in status of a track.
   //@{
   virtual int GetActiveTrackCount() const;

   virtual void GetActiveTrackList(WsfTrackList& aActiveTrackList) const;

   virtual int GetMaximumTrackCount() const;

   virtual void SendTrackCoastMessage(double aSimTime, const WsfTrack* aTrackPtr);

   virtual void SendTrackDropMessage(double aSimTime, const WsfTrack* aTrackPtr);

   virtual void SendTrackUpdateMessage(double aSimTime, const WsfTrack* aTrackPtr);

   virtual void SetMessageParameters(WsfStringId aModeNameId, WsfMessage& aMessage);

   virtual void TrackCoasted(double aSimTime, WsfTrack* aTrackPtr);

   virtual void TrackDropped(double aSimTime, WsfTrack* aTrackPtr);

   virtual void TrackUpdated(double aSimTime, WsfTrack* aTrackPtr);
   //@}

   //! @name Determine the 'required Pd for detection' for a detection event.
   //@{
   //! Get the 'required Pd for detection' to be used for a detection event.
   //!
   //! The value returned by this method will be one of the following:
   //!
   //! - If UseConstantPdRequired() is 'true' then it will return the value stored in the sensor mode.
   //! - If UseConstantPdRequired() is 'false' then it will return a random value for each call.
   //!   (Computed as 1.0 - GetSimulation()->GetRandom().Uniform());
   //!
   //! @note The 'required_pd' is meaningless for sensors that are binary detectors (i.e.: the Pd from a
   //! detection attempt is either 0 or 1).
   //!
   //! @note This only applies to detection attempts made via Update()/PerformScheduledDetections()
   //! (i.e.: during a standard simulation run). It does not apply for direct calls to AttemptToDetect().
   static double GetRequiredPd(const WsfSensorMode* aModePtr);

   //! @name Slave status methods.
   //! A 'slave sensor' is one whose detection opportunities are controlled by some other sensor that is
   //! defined to be the 'master'. The Update() method of a slave sensor will never be called by the simulation.
   //@{
   //! Get the current 'slave' status of a sensor.
   bool IsSlave() const { return mIsSlave; }

   void SetIsSlave(double aSimTime, bool aIsSlave);
   //@}

   //! @name Sensor scheduler and tracker maintenance.
   //! The 'scheduler' is responsible for determining when detection chances are to be performed
   //! to be performed against targets. The 'tracker' takes the detection results and creates tracks.
   //@{
   //! Return a pointer to the sensor scheduler.
   //! @returns The pointer to the sensor scheduler, which can be zero if there is no scheduler.
   WsfSensorScheduler* GetScheduler() const { return mSchedulerPtr.get(); }

   void                                SetScheduler(std::unique_ptr<WsfSensorScheduler> aSchedulerPtr);
   std::unique_ptr<WsfSensorScheduler> SwapScheduler(std::unique_ptr<WsfSensorScheduler> aSchedulerPtr);

   //! Return a pointer to the sensor tracker.
   //! @returns The pointer to the sensor tracker, which can be zero if there is no tracker.
   WsfSensorTracker* GetTracker() const { return mTrackerPtr.get(); }

   void                              SetTracker(std::unique_ptr<WsfSensorTracker> aTrackerPtr);
   std::unique_ptr<WsfSensorTracker> SwapTracker(std::unique_ptr<WsfSensorTracker> aTrackerPtr);
   //@}

   //! @name Methods for simulation control of the update interval.
   //! @note These methods are only intended for use by simulation executives (classes derived from WsfSimulation)
   //! and objects associated with controlling calls to Update(). Any other use is at your own risk!
   //{@
   //! Set the time interval between the periodic calls to Update().
   //! A value of less than or equal to zero indicates periodic updates are not to be performed.
   //! @param aUpdateInterval The new update interval.
   //! @note Users should call WsfSimulation::GetInstance().SetSensorUpdateInterval() to set the interval.
   void SetUpdateInterval(double aUpdateInterval) override { mUpdateInterval = aUpdateInterval; }

   //! Get the current update event 'epoch'.
   //! The event 'epoch' is used to recognize and ignore obsolete events.
   unsigned int GetUpdateEventEpoch() const { return mUpdateEventEpoch; }

   //! Increment the 'epoch' for the update event.
   //! This should be called whenever the update interval is changed or if the sensor is turned off or on.
   void IncrementUpdateEventEpoch() { ++mUpdateEventEpoch; }

   //! Is an 'Update' event active?
   bool UpdateEventActive() const { return mUpdateEventActive; }

   //! Indicate if the 'Update' event is active.
   void SetUpdateEventActive(bool aUpdateEventActive) { mUpdateEventActive = aUpdateEventActive; }

   void AdjustNextUpdateTime(double aNextUpdateTime);

   //! Get the next update time.
   //! @returns The next update time for the sensor.
   double GetNextUpdateTime() const { return mNextUpdateTime; }

   void SetNextUpdateTime(double aSimTime, double aNextUpdateTime);

   //@}

   //! Get the base level message parameters (may get overridden by the mode)
   void GetMessageParameters(int& aMessageLength, int& aMessagePriority) const
   {
      aMessageLength   = mMessageLength;
      aMessagePriority = mMessagePriority;
   }

   //! Get the zone attenuation modifier.
   WsfStringId GetZoneAttenuationModifier() const { return mZoneAttenuationModifier; }

   //! Return mSendDis flag
   bool SendDis() { return mSendDis; }

   //! Get the sensor's random number generator.
   ut::Random& GetRandom() { return mRandom; }

   virtual bool GetFilteredDoppler() const { return true; }

   //! If a script exists, allows it to impose further constraints on a sensor detection
   bool ScriptAllowDetection(double aSimTime, WsfPlatform* aTargetPtr, WsfSensorResult& aResult);

   //! This nested class 'ChangeMode' is necessary so that
   //! the data necessary for this event is separate from WsfSensorEvent.
   class ModeChangeEvent : public WsfEvent
   {
   public:
      enum EventType
      {
         cSELECT,
         cDESELECT
      };

      ModeChangeEvent();
      ModeChangeEvent(double aSimTime, EventType aEventType, WsfSensor* aSensorPtr, WsfStringId aModeNameId);

      EventDisposition Execute() override;

   private:
      size_t      mPlatformIndex; //!< The platform index of the source platform.
      EventType   mEventType;     //!< The type of mode change event.
      WsfSensor*  mSensorPtr;     //!< The sensor pointer.
      WsfStringId mModeNameId;    //!< The ID of the alternate frequency to select.
   };

protected:
   WsfSensor(const WsfSensor& aSrc);
   WsfSensor& operator=(const WsfSensor& aSrc) = delete;

   void SetClass(int aClass);
   void SetModeList(std::unique_ptr<WsfSensorModeList> aModeListPtr);

   UtScriptContext* GetScriptAccessibleContext() const override;
   void             SetPlatform(WsfPlatform* aPlatformPtr) override;

   //! Identifies the sensor as thread safe; sensor updates can be multi-threaded
   //! Certain types of derived sensors may have dependencies that may not make them thread-safe.
   bool mThreadSafe;

   std::unique_ptr<WsfSensorModeList>  mModeListPtr;
   std::unique_ptr<WsfSensorScheduler> mSchedulerPtr;
   std::unique_ptr<WsfSensorTracker>   mTrackerPtr;

   //! The time when the next call to Update() *SHOULD* occur
   double mNextUpdateTime;

   //! The logical length (in bits) of report messages that originate from this sensor.
   int mMessageLength;

   //! The priority of report messages that originate from this sensor.
   int mMessagePriority;

   //! If non-zero, the name of ID of the category to be used for zone-based attenuation.
   WsfStringId mZoneAttenuationModifier;

   //! Categories which this sensor should ignore
   WsfCategoryList mIgnoredCategories;

   //! The sides (teams) which this sensor should ignore.
   std::vector<WsfStringId> mIgnoredSides;

   //! A bit mask of ignored domains.
   unsigned int mIgnoredDomains;

   //! 'true' if the sensor should not detect platforms on the same side.
   bool mIgnoreSameSide;

private:
   ComponentList mComponents;

   //! The generic list of modes (not valid until Initialize is called)
   std::vector<WsfSensorMode*> mSensorModeList;

   //! The classification of the sensor.
   int mClass;

   //! The update interval is time between the periodic calls to Update() by the simulation executive.
   //! If less than or equal to zero then the simulation executive should not make the periodic calls.
   //!
   //! This should be the time between sensing chances.
   double mUpdateInterval;

   //! The current update event epoch.
   //! This is used to identify and reject obsolete 'update' events.
   unsigned int mUpdateEventEpoch;

   //! True if an event-driven periodic call to Update() is in progress.
   bool mUpdateEventActive;

   //! True if the sensor is a 'slave' sensor.
   bool mIsSlave;

   //! true if calibration data is do be displayed during initialization.
   bool mShowCalibrationData;

   //! By default send DIS Emissions PDU's for all sensors. At times may not want to.
   bool mSendDis;

   ut::Random mRandom;

   //! Allows for the use of scripts that are triggered when detection events occur
   WsfScriptContext mContext;
   UtScript*        mOnDetectionScriptPtr{nullptr};

   std::vector<WsfSingleSensorObserver*> mObservers;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfSensor, cWSF_COMPONENT_SENSOR)

#endif
