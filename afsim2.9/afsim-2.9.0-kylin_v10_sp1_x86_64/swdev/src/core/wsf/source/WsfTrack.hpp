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

#ifndef WSFTRACK_HPP
#define WSFTRACK_HPP

#include "wsf_export.h"

#include <memory>
#include <vector>

class UtInput;
class UtInputBlock;
#include "UtMatrix.hpp"
#include "UtReferenceTracked.hpp"
#include "UtScriptAccessible.hpp"
#include "UtVec3.hpp"
#include "WsfAuxDataEnabled.hpp"
class WsfGeoPoint;
#include "WsfKinematicStateExtrapolation.hpp"
#include "WsfMeasurement.hpp"
class WsfObject;
class WsfPlatform;
class WsfScenario;
class WsfSimulation;
#include "WsfStringId.hpp"
class WsfTrack;
#include "WsfTrackId.hpp"
#include "WsfTypes.hpp"

//! A representation of a detected signal.
class WSF_EXPORT WsfTrack_Signal
{
public:
   WsfTrack_Signal();
   WsfTrack_Signal(double aFrequency);
   WsfTrack_Signal(double aLowerFreq, double aUpperFreq);
   WsfTrack_Signal(double aLowerFreq, double aUpperFreq, double aPRI, double aPW);

   //! Check to see if two signals are equal.
   //! They are equal if their signal parameters match.
   bool operator==(const WsfTrack_Signal& aRhs) const;

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mLowerFrequency& mUpperFrequency& mPulseRepetitionInterval& mPulseWidth;
   }

   double      mLowerFrequency;
   double      mUpperFrequency;
   double      mPulseRepetitionInterval;
   double      mPulseWidth;
   WsfStringId mEmitterTruthId;
   WsfStringId mEmitterDerivedId;
};

//! A nested class for maintain a list of detected signals.
struct WsfTrack_SignalList
{
   std::vector<WsfTrack_Signal> mSignalList;

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mSignalList;
   }
};

/**
   An object that represents an 'track' (perception) of something.

   A track can be:
     - the product of a sensor
     - the product of a processor.
     - 'pre-briefed' (an 'initial' track)

   Tracks are passed from sensors to processors internally within a platform
   and are passed between platforms via communications links.  A processor can
   pass a sensor track through unmodified or it can perform fusion, etc. and
   create new tracks that then become a product of the processor.

   The creator/maintainer of a track should create an instance of the track
   using the proper constructor to identify its source (sensor or processor)
   The instance is then associated ONLY with that sensor or processor.

   When a track is started Initialize() should be called to set the start time
   and track ID and then proceed with the normal track update process
   described below (including calling Update()!).

   When a track is updated, Update() should be called and then the necessary
   fields should be updated.

   A reporter must declare which fields in the track are 'valid'.  The
   validity of a field is set with the 'Set\<field>Valid()' methods and queried
   with the '\<field>Valid()'.  The validity of all fields is initially set to
   'false' (invalid) by the constructor.  The field validity and value are
   modified ONLY by the 'Set' methods and are NOT modified by Initialize() or
   Update().

   An "auxiliary data" service is available with WsfTrack for storing
   arbitrary data of interest (see WsfAttributeContainer for
   more information).  The auxiliary data are accessed through the
   WsfTrack::GetAuxData method.  The aux data can be added through
   user input (see the "aux_data" keyword in the WSF User Input Guide).
   (see WsfAttributeContainer and UtAttributeContainer).
*/

class WSF_EXPORT WsfTrack : public UtScriptAccessible, public UtReferenceTracked, public WsfMeasurement, public WsfAuxDataEnabled
{
public:
   using Signal = WsfTrack_Signal;

   //! The source type of the track data.
   enum TrackType
   {
      //! The track is the result of a direct sensor measurement.
      cUNFILTERED_SENSOR,
      //! The track is the result of filtered or processed sensor measurements.
      cFILTERED_SENSOR,
      //! The track is 'pre-defined' or 'pre-briefed' (typically read from an input file).
      cPREDEFINED,
      //! The track is the product of some 'process' (filtering, fusion, intel reports, etc.)
      cPROCESSED,
      //! The track is an extract from a static image (e.g.: spot SAR or photograph).
      //! This is similar to a 'FILTERED_SENSOR' track, but a different type must be used because the mechanisms
      //! for using it in fusion processes may be different.
      cSTATIC_IMAGE,
      //! The track is a 'pseudo-sensor' track.
      //! These exist only for visualization purposes and are produced by WsfEOIR_Sensor and WsfSAR_Sensor
      //! during the formation of an image. These tracks are only presented via the observer interface
      //! (never sent over internal/external links). In general, an observer will typically ignore these
      //! tracks.
      cPSEUDO_SENSOR
   };

   //! An enumeration for IFF
   enum IFF_Status
   {
      //! No IFF status is being reported.
      cIFF_UNKNOWN,
      //! The reporter cannot determine if the object is a friend or a foe.
      cIFF_AMBIGUOUS,
      //! The object has been determined to be a foe,
      cIFF_FOE,
      //! The object has been determined to be a friend
      cIFF_FRIEND,
      //! The object has been determined to be neutral
      cIFF_NEUTRAL
   };

   //! A representation of a detected emitter type.
   class EmitterTypeData
   {
   public:
      EmitterTypeData()
         : mEmitterTruthId()
         , mEmitterDerivedId()
         , mSignalIndices()
      {
      }

      EmitterTypeData(WsfStringId aTruthEmitterTypeId, WsfStringId aEmitterTypeId)
         : mEmitterTruthId(aTruthEmitterTypeId)
         , mEmitterDerivedId(aEmitterTypeId)
         , mSignalIndices()
      {
      }

      int GetSignalIndexEntry(unsigned int aIndex)
      {
         int signalIndex(0);
         if ((aIndex < mSignalIndices.size()))
         {
            signalIndex = mSignalIndices[aIndex];
         }
         return signalIndex;
      }

      //! For XIO (de)serialization.
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mEmitterTruthId& mEmitterDerivedId& mSignalIndices;
      }

      WsfStringId      mEmitterTruthId;
      WsfStringId      mEmitterDerivedId;
      std::vector<int> mSignalIndices;
   };

   //! A nested class to maintain a list of emitter type IDs.
   struct EmitterTypeIdList
   {
      std::vector<EmitterTypeData> mEmitterTypeIdList;

      //! For XIO (de)serialization.
      template<typename T>
      void Serialize(T& aBuff)
      {
         aBuff& mEmitterTypeIdList;
      }
   };

   //! An enumeration that is used to determine how 'CopyMemberData' operates.
   enum CopyCaller
   {
      //! Called from a copy constructor.
      cCC_COPY_CONSTRUCTOR,

      //! Called from an assignment operator.
      cCC_ASSIGNMENT,

      //! Called from the replacement update function.
      //! This track retains its ID.
      cCC_REPLACEMENT_UPDATE
   };

   WsfTrack();
   WsfTrack(WsfStringId aOriginatorNameId, WsfStringId aOriginatorTypeId);
   WsfTrack(TrackType aTrackType, WsfObject* aOriginatingProcessorOrSensor, WsfPlatform* aOriginatingPlatform);
   WsfTrack(const WsfTrack& aSrc);

   ~WsfTrack() override;

   WsfTrack*    Clone() const override;
   virtual void Initialize(double aSimTime, const WsfTrackId& aTrackId, WsfSimulation& aSimulation);
   void         SetSimulation(WsfSimulation* aSimulationPtr);
   void         Update(double aSimTime) override;
   void         KinematicUpdate(double aSimTime) override;
   virtual void ReplacementUpdate(const WsfTrack& aSource);
   virtual void UpdateFromMeasurement(double aSimTime, const WsfTrack& aMeasurement);

   const char* GetScriptClassName() const override;

   //! Return that this is a track.
   bool IsTrack() const override { return true; }

   void SetPlatformOriginator(WsfPlatform* aPlatformPtr);
   void SetProcessorOriginator(WsfObject* aProcessorPtr, WsfPlatform* aPlatformPtr);
   void SetSensorOriginator(WsfObject* aSensorPtr, WsfPlatform* aPlatformPtr);
   void SetSensorOriginator(WsfStringId aSensorNameId, WsfStringId aSensorTypeId);

   //! Get the track ID.
   const WsfTrackId& GetTrackId() const { return mTrackId; }

   //! Set the track ID.
   void SetTrackId(const WsfTrackId& aTrackId) { mTrackId = aTrackId; }

   //! Get the 'track type'.
   TrackType GetTrackType() const { return mTrackType; }

   //! Set the 'track type'.
   //! @note This only allows switching between filtered and unfiltered sensor types.
   void SetTrackType(TrackType aTrackType);

   //! Is the track a predefined (pre-briefed) track?
   //! This method is retained for compatibility.
   bool IsPredefinedTrack() const { return (mTrackType == cPREDEFINED); }

   //! Get the time when this track was started.
   //! If this is the first update then StartTime = UpdateTime.
   double GetStartTime() const { return mStartTime; }

   //! Set the time when this track was started
   void SetStartTime(double aStartTime) { mStartTime = aStartTime; }

   //! Get the time when this track was last updated.
   double GetUpdateTime() const { return mUpdateTime.Get(); }

   //! Get the last time interval between updates.
   double GetLastUpdateInterval() const { return (mLastUpdateTime.Get() - mUpdateTime.Get()); }

   //! Define the time when this track was last updated.
   // void SetUpdateTime(double aUpdateTime)               { mUpdateTime = aUpdateTime; }

   //! Get the number of updates to this track.
   int GetUpdateCount() const { return mUpdateCount; }

   //! Set the number of updates to this track.
   void SetUpdateCount(int aUpdateCount) { mUpdateCount = aUpdateCount; }

   //! The 'data Id' to be used when the track is attached to a message.
   double GetMessageDataTag() const { return mMessageDataTag; }

   //! Set the 'data Id' to be used when the track is attached to a message.
   void SetMessageDataTag(double aMessageDataTag) { mMessageDataTag = aMessageDataTag; }

   //! Get the platform name ID of the originator.
   WsfStringId GetOriginatorNameId() const { return mOriginatorNameId; }

   //! Get the platform index of the originator.
   size_t GetOriginatorIndex() const { return mOriginatorPlatformIndex; }

   //! The platform type ID of the originator.
   WsfStringId GetOriginatorTypeId() const { return mOriginatorTypeId; }

   //! Get the processor name Id of the reporting processor.
   //! @note this will be 0 if the track type is either a 'cFILTERED_SENSOR' or 'cUNFILTER_SENSOR'.
   WsfStringId GetProcessorNameId() const { return mProcessorNameId; }

   //! Get the processor Type Id of the reporting processor.
   //! @note this will be 0 if the track type is either a 'cFILTERED_SENSOR' or 'cUNFILTER_SENSOR'.
   WsfStringId GetProcessorTypeId() const { return mProcessorTypeId; }

   //! Get the sensor name Id of the reporting sensor.
   //! @note this will be 0 if the track type is not either a 'cFILTERED_SENSOR' or 'cUNFILTER_SENSOR'.
   WsfStringId GetSensorNameId() const { return mSensorNameId; }

   //! Get the sensor Type Id of the reporting sensor.
   //! @note this will be 0 if the track type is not either a 'cFILTERED_SENSOR' or 'cUNFILTER_SENSOR'.
   WsfStringId GetSensorTypeId() const { return mSensorTypeId; }

   //! Get the sensor mode ID in use at the time of the sensor report.
   //! @note this will be 0 if the track type is not either a 'cFILTERED_SENSOR' or 'cUNFILTER_SENSOR'.
   WsfStringId GetSensorModeId() const { return mSensorModeId; }

   //! Set the sensor mode ID in use at the time of the sensor report.
   //! This should not be called for non-sensor tracks.
   void SetSensorModeId(WsfStringId aSensorModeId) { mSensorModeId = aSensorModeId; }

   //! Is the track a fusible track.
   bool IsFusible() const { return mIsFusible; }

   //! Define whether the track is a fusible track.
   void SetFusible(bool aIsFusible) { mIsFusible = aIsFusible; }

   //! Set whether this track can be purged.
   virtual void SetPurgeable(bool aIsPurgeable) { mIsPurgeable = aIsPurgeable; }

   //! Return the current state of whether this track is purgeable.
   //!@note A result of true indicates that the track can be purged eventually.
   //! In the case of a WsfLocalTrack, it may have a nonzero use
   //! count and will not be purged until the use count goes to zero.
   bool IsPurgeable() const { return mIsPurgeable; }

   //! Are the data in the track 'stale.'
   //! @note Track data are considered stale if it should have been purged, but was retained instead
   //! due to an existing lock on it.
   bool IsStale() const { return mIsStale; }

   //! Define whether the track data are to be considered 'stale.'
   void SetStale(bool aIsStale) { mIsStale = aIsStale; }

   //! @name Reporting responsibility methods.
   //! This is used to declare if the local track manager has reporting responsibility for this track.
   //@{

   //! Is the track reportable by the local track manager?
   //! @returns 'true' if the local track manager has reporting responsibility for the track or 'false' if not.
   bool IsReportable() const { return mIsReportable; }

   void IsReportable(bool aIsReportable);

   bool Is3D() const;

   //! Set whether to consider this track as having a full 3D solution (see WsfTrack::Is3D).
   void Set3D(bool aIs3D) { mIs3D = aIs3D; }

   //! Declare the local reporting responsibility of the track.
   //! @param aIsReportable 'true' if the local track manager is to have reporting responsibility for the track
   //! or 'false' if it should not report the track.
   void SetReportable(bool aIsReportable) { mIsReportable = aIsReportable; }

   using History = std::vector<std::unique_ptr<WsfMeasurement>>; // track history type

   bool             HasHistory() const;
   virtual History& GetHistory() const;

   //! @name Identify Friend or Foe (IFF).
   //! Get/Set the IFF status.
   //! A value of cIFF_UNKNOWN indicates there is no report of IFF status;
   //! This is the same as IFF_StatusValid() == false.
   //@{

   bool IFF_StatusValid() const { return mIFF_Status.IsValid(); }

   IFF_Status GetIFF_Status() const { return (const IFF_Status)mIFF_Status.Get(); }

   //! Set the IFF status of the target.
   void SetIFF_Status(IFF_Status aIFF_Status)
   {
      mIFF_Status.Set(aIFF_Status);
      mIFF_Status.SetValid(true);
   }

   //@}f


   //! @name Track quality.
   //! Get/Set the track quality.
   //!
   //! Track quality is expressed as a real value in the range [0..1] with 0 being the
   //! least quality and 1 being the highest quality. A quality of 1 should be reserved
   //! for high quality tracks that actions can be taken on.
   //!
   //! There is no validity flag for this field.  All tracks have 'quality'. It is the
   //! responsibility of the producer to assign track quality.
   //@{

   //! Get the track quality.
   double GetTrackQuality() const;

   //! Get the track quality at a future time.
   double GetTrackQuality(double aSimTime) const;

   void SetTrackQuality(double aTrackQuality);

   //@}

   //! @name Quality and Expected Error
   //! Get the track quality and associated expected error.
   //!
   //! Track quality is expressed as a real value in the range [0..1] with 0 being the
   //! least quality and 1 being the highest quality. A quality of 1 should be reserved
   //! for highest quality tracks.
   //!
   //! There is no validity flag for this field.  All tracks have 'quality'.
   //! This field always returns quantitative track quality when available from
   //! covariances.  Otherwise, it returns the default (0.5) or value provided
   //! using the "quality" keyword.  The expected error calculation returns the
   //! expected RMS error in meters based on the track quality. See
   //! UtMeasurementUtil for a description of the track quality / error algorithms.
   //!
   //! The expected error
   //@{

   //! Get the track quality.
   double GetQuality() const;

   //! Get the track quality at a future time.
   double GetQuality(double aSimTime) const;

   //! Get the expected RMS location error associated with the track.
   double GetExpectedLocationError() const;

   //! Get the expected RMS error associated with the track at a given time.
   double GetExpectedLocationError(double aSimTime) const;

   //@}
   //! @name Detected signal data
   //! Get/Set the detected signal and validity flags
   //@{

   using SignalList = WsfTrack_SignalList;
   const SignalList* GetSignalList() const { return mSignalListPtr.Get(); }

   void         AddSignal(const Signal& aSignal);
   unsigned int GetSignalCount() const;
   void         GetSignalEntry(unsigned int aIndex, Signal& aSignal) const;
   void         MergeSignalLists(const WsfTrack& aOtherTrack);
   void         SetSignalList(const std::vector<Signal>& aSignalList);

   double GetFrequency() const;
   void   SetFrequency(double aFrequency);

   bool FrequencyValid() const;
   void SetFrequencyValid(bool aFrequencyValid);

   //@}

   //! @name Emitter Type ID data
   //! Get/Set the emitter type and validity flags
   //@{

   void         AddEmitterTypeId(const EmitterTypeData& aEmitterTypeData);
   void         AddEmitterTypeId(WsfStringId aTruthEmitterTypeId, WsfStringId aEmitterTypeId);
   unsigned int GetEmitterTypeIdCount() const;
   void         GetEmitterTypeIdEntry(unsigned int aIndex, EmitterTypeData& aEmitterTypeData) const;
   void         MergeEmitterTypeIdLists(const WsfTrack& aOtherTrack);
   void         SetEmitterTypeIdList(const std::vector<EmitterTypeData>& aEmitterTypeIdList);


   //! Is the emitter type ID attribute valid?
   bool EmitterTypeIdValid() const { return mEmitterTypeIdListPtr.IsValid(); }
   //! Define whether or not the emitter type ID attribute is valid.
   void SetEmitterTypeIdValid(bool aEmitterTypeIdValid) { mEmitterTypeIdListPtr.SetValid(aEmitterTypeIdValid); }

   //@}
   //! @name Signal to noise.
   //! Get/Set the signal-to-noise ratio (in dB) and validity flags
   //@{

   //! Get the signal to noise from the sensor.
   double GetSignalToNoise() const { return mSignalToNoise.Get(); }
   //! Set the signal to noise from the sensor.
   void SetSignalToNoise(double aSignalToNoise) { mSignalToNoise.Set(aSignalToNoise); }

   //! Is the signal-to-noise attribute valid?
   bool SignalToNoiseValid() const { return mSignalToNoise.IsValid(); }
   //! Define whether or not the signal-to-noise attribute is valid.
   void SetSignalToNoiseValid(bool aSignalToNoiseValid) { mSignalToNoise.SetValid(aSignalToNoiseValid); }

   //@}

   //! Pixel count methods (for tracks produced from images)
   //@{
   //! Get the number of pixels occupied by the object.
   //! This value will be zero if not from an image.
   double GetPixelCount() const { return mPixelCount.Get(); }
   //! Set the number of pixels occupied by the object.
   void SetPixelCount(double aPixelCount) { mPixelCount.Set(aPixelCount); }

   //@}

   //! @name Residual Covariance matrix.
   //!@{
   //! Get/Set the track residual covariance
   //!
   // Set as a UtMatrix.
   const UtMatrixd* GetResidualCovarianceMatrix() const;

   // Set as a covariance object.
   const UtCovariance* GetResidualCovariance() const { return mResidualCovariancePtr.Get(); }
   void                SetResidualCovariance(const UtCovariance& aResidualCovarianceMatrix);
   void                SetResidualCovariance(UtCovariance* aResidualCovarianceMatrixPtr);
   bool                ResidualCovarianceValid() const;
   void                SetResidualCovarianceValid(bool aStateCovarianceValid);

   bool ProcessNoiseVariancesValid() const;
   void GetProcessNoiseVariancesECS(double aProcessNoiseVariancesECS[3]) const;
   void SetProcessNoiseVariancesECS(double aProcessNoiseVariancesECS[3]);
   int  GetProcessNoiseModel() const;
   //@}

   //! @name Candidate methods
   //! Get/Set the "candidate" status of the track.
   //@{

   //! Set the "candidate" status of a track.
   void SetIsCandidate(bool aIsCandidate) { mIsCandidate = aIsCandidate; }

   //! Return whether the track is marked as a "candidate"
   //! (i.e., perhaps, not to be used for decision making due to its immaturity).
   bool IsCandidate() const { return mIsCandidate; }

   //@}

   //! @name False target methods.
   //! A track may represent a false target.
   //@{

   //! Set the flag indicating this is a spurious target not corresponding to a real platform.
   //! @param aIsFalseTarget 'True' = Is a false target (default = not a false target).
   void SetIsFalseTarget(bool aIsFalseTarget) { mIsFalseTarget = aIsFalseTarget; }

   //! Get the flag indicating this is a spurious target not corresponding to a real platform.
   //! @return 'True' = Is a false target.
   bool IsFalseTarget() const { return mIsFalseTarget; }
   //@}

   //! @name Strobe methods.
   //! A track may represent a strobe track, possibly from interference.
   //@{

   //! Set the flag indicating this is a strobe detection track.
   //! @param aIsStrobe 'True' = Is a strobe track ('false' (default) = not a strobe track).
   void SetIsStrobe(bool aIsStrobe) { mIsStrobe = aIsStrobe; }

   //! Get the flag indicating this is a strobe detection track.
   //! @return 'True' = Is a strobe track.
   bool IsStrobe() const { return mIsStrobe; }
   //@}

   //! @name Truth methods.
   //! The 'Target Platform' is not really part of the 'track' in that it
   //! represents truth data.  In general this should not be used for
   //! simulation decisions, unless it is absolutely necessary to 'cheat'.
   //
   //! NOTE: GetTargetIndex() can return zero which means the track is probably
   //!       not explicitly tied to a specific platform.
   //@{

   //! Set the platform index, name, and type of the target.
   //! @param aTarget: The target to be associated with this track.
   void SetTarget(const WsfPlatform& aTarget);

   //! Set the platform index of the target.
   //! @param aTargetIndex The platform index of the target.
   void SetTargetIndex(size_t aTargetIndex) { mTargetIndex = aTargetIndex; }

   //! Return the platform index of the target.
   size_t GetTargetIndex() const;

   //! Set the string id of that corresponds to the target platform or false-target blip name.
   //! @param aTargetName The string id corresponds to the target platform name.
   void SetTargetName(WsfStringId aTargetName) { mTargetName = aTargetName; }

   //! Return the string id that corresponds to the target platform or false-target blip name.
   WsfStringId GetTargetName() const { return mTargetName; }

   //! Set the id corresponding with the target platform's type
   //! @param aTargetType The string id corresponding with the target platform's type.
   void SetTargetType(WsfStringId aTargetType) { mTargetType = aTargetType; }

   //! Return the string id corresponding with the target platform's type.
   WsfStringId GetTargetType() const;
   //@}

   //! @name Methods to support WsfTrackProcessor circular report detection.
   //@{
   //! Set the platform name (ID) of the platform that last provided data to update this track.
   //! This should be the value from WsfTrackMessage::GetSenderId.
   //! @note This is 'const' even though it modifies the track. This attribute is 'mutable'.
   void SetLastSourceName(WsfStringId aSourceName) const { mLastSourceName = aSourceName; }

   //! Set the platform name (ID) of the platform that last provided data to update this track.
   //! This should be the value from WsfTrackMessage::GetSenderId.
   WsfStringId GetLastSourceName() const { return mLastSourceName; }
   //@}

   //! @name Input methods.
   //@{

   bool ProcessInput(UtInput& aInput) override;
   void ProcessInputBlock(const WsfScenario& aScenario, UtInputBlock& aInputBlock);
   static bool LoadTrackInstance(const WsfScenario& aScenario, UtInput& aInput, WsfPlatform* aPlatformPtr, WsfTrack*& aTrackPtr);
   //@}

   WsfTrack& operator=(const WsfTrack& aRhs);

   //! @name Global Track Allocation Number (GTAN) methods
   //! A track, if required, may be given a global track identification number to
   //! associate to a specific track instance.
   //@{
   void SetGlobalTrackNumber(int aNumber) { mGlobalTrackNumber = aNumber; }
   int  GetGlobalTrackNumber() const { return mGlobalTrackNumber; }
   //@}

   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

   //! Fetch relative line of sight rates, from the Observer's perspective,
   //! (the Observer does not have to be the originator of the track)
   //! for Intercept guidance.  Will return true if the results are valid.
   bool LineOfSightRatesFrom(double aSimTime, WsfPlatform* aObserver, double& aLOSR_Az, double& aLOSR_El);

   // ================================= WARNING: New member variables ===========================
   //
   //    This class is transmitted over the network. If you add a new member variable, it is
   //    YOUR RESPONSIBILITY to verify that the member is serialized correctly.
   //    Please see the implementation of WsfTrack::SerializeP in xio/WsfXIO_SerializeTypes.cpp
   //
   // ===========================================================================================

   //! For XIO (de)serialization.
   template<typename T>
   void SerializeP(T& aBuff);

protected:
   bool UseQuantitativeTrackQuality() const;

   WsfTrackId          mTrackId;
   TrackType           mTrackType;
   WsfStringId         mOriginatorNameId;
   size_t              mOriginatorPlatformIndex;
   WsfStringId         mOriginatorTypeId;
   WsfStringId         mProcessorNameId;
   WsfStringId         mProcessorTypeId;
   WsfStringId         mSensorNameId;
   WsfStringId         mSensorTypeId;
   WsfStringId         mSensorModeId;
   int                 mUpdateCount;
   double              mStartTime;
   UtDoubleInformation mLastUpdateTime;
   int                 mGlobalTrackNumber;

   // The 'data tag' to be attached to a message containing this track.
   double mMessageDataTag;

   // Target track data are all UtAttribute-based.
   // Actual location of the originator at the time of the report.
   // The following pointers are always valid.
   // I am intentionally excepting the standard that pointer variable names end in "Ptr"
   using IFF_StatusInformation       = UtInformation<int>;
   using SignalListPtrInformation    = UtInformation<SignalList*>;
   using EmitterTypeIdPtrInformation = UtInformation<EmitterTypeIdList*>;

   UtDoubleInformation         mTrackQuality;
   UtDoubleInformation         mSignalToNoise;
   UtDoubleInformation         mPixelCount;
   IFF_StatusInformation       mIFF_Status;
   SignalListPtrInformation    mSignalListPtr;
   EmitterTypeIdPtrInformation mEmitterTypeIdListPtr;
   UtCovariancePtrInformation  mResidualCovariancePtr;

   union
   {
      struct
      {
         bool mIsFusible : 1;
         bool mIsPurgeable : 1;
         bool mIsStale : 1;
         bool mIs3D : 1;
         bool mIsReportable : 1;
         bool mIsCandidate : 1;
         bool mIsStrobe : 1;      // The track is a strobe detection from an interference possibly.
         bool mIsFalseTarget : 1; // The track is a spurious target, w/no real target platform.
      };

      unsigned int mFlags;
   };

   int mFalseTargetId; // Implementation-defined unique ID for this false target.

   //! The name of the platform that last provided data that updated this track.
   mutable WsfStringId mLastSourceName;

   mutable History mHistory; // optional track history (possibly to be maintained by the tracker itself).

private:
   void ConstructInformation();
   void InitializeMemberData();
   void CopyMemberData(const WsfTrack& aSrc, CopyCaller aCaller);

   // The following are deprecated and should remain private.
   void SetResidualCovarianceMatrix(const UtMatrixd& aResidualCovarianceMatrix);

   // These values may need to be computed when requested, accessors should be used to get values
   mutable size_t      mTargetIndex; // Platform index of the target platform
   WsfStringId         mTargetName;
   mutable WsfStringId mTargetType;
};

#endif
