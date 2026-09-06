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

#ifndef WSFEM_INTERACTION_HPP
#define WSFEM_INTERACTION_HPP

#include "wsf_export.h"

#include <iosfwd>
#include <ostream>

#include "UtLog.hpp"
#include "UtScriptAccessible.hpp"
#include "WsfComponentList.hpp"
class WsfEM_Antenna;
class WsfEM_Interaction;
class WsfEM_Rcvr;
class WsfEM_Xmtr;
class WsfEM_XmtrRcvr;
class WsfEnvironment;
class WsfPlatform;
#include "WsfStringId.hpp"


//! A support class for electromagnetic interactions.
//!
//! This class is used to provide a common mechanism for performing electro-
//! magnetic interactions (i.e.: interactions involving electromagnetic
//! radiation). These interactions can be communications, sensing, interference
//! or direct energy.
//!
//! Most of the class data is public and represents the results of various
//! calculations performed by the class. Much data is maintained so interested
//! parties can gather data about an interaction without having to modify
//! the underlying code.
//!
//! Not all data members are applicable to all types of interactions. Because
//! this class is used as the base class for things like WsfComm::Result and
//! WsfSensorResult, it must be necessarily complex to support the just
//! about any type of electromagnetic interaction.
class WSF_EXPORT WsfEM_InteractionComponent : public WsfComponentT<WsfEM_Interaction>
{
   friend class WsfEM_Interaction;

public:
   virtual void Print(ut::log::MessageStream& aMsgStream, WsfEM_Interaction& aEM_Interaction) {}
   virtual void PrintStatus(ut::log::MessageStream& aMsgStream, WsfEM_Interaction& aEM_Interaction) {}
   virtual void Reset() {}

   // These are called from WsfEventOutput::PrintEM_Interaction.
   // It is a bit of a hack in that only addresses event_output in a very specific way.
   virtual bool PrintEventOutput(std::ostream& aStream) { return false; }
   virtual void PrintEventOutput_Powers(std::ostream& aStream) {}

   virtual bool PrintCSV_EventOutput(std::ostream& aStream) { return false; }
   virtual bool PrintCSV_EventOutput_Powers(std::ostream& aStream) { return false; }
};

class WSF_EXPORT WsfEM_Interaction : public UtScriptAccessible
{
public:
   using Component     = WsfEM_InteractionComponent;
   using ComponentList = WsfComponentListT<WsfEM_InteractionComponent>;

   //! Bit mask values for mCheckedStatus/mFailedStatus.
   //! The base class uses bits 0-10 and reserves bit 11-15 for future use.
   //! Derived classes MUST start at bit 16 or higher.
   enum
   {
      cRCVR_RANGE_LIMITS    = 0x00000001, //!< Receiver range limits checked/failed
      cRCVR_ALTITUDE_LIMITS = 0x00000002, //!< Receiver altitude limits checked/failed.
      cRCVR_ANGLE_LIMITS    = 0x00000004, //!< Receiver angle limits checked/failed
      cRCVR_HORIZON_MASKING = 0x00000008, //!< Receiver horizon masking checked/failed
      cRCVR_TERRAIN_MASKING = 0x00000010, //!< Receiver terrain masking checked/failed
      cXMTR_RANGE_LIMITS    = 0x00000020, //!< Transmitter range limits checked/failed.
      cXMTR_ALTITUDE_LIMITS = 0x00000040, //!< Transmitter altitude limits checked/failed
      cXMTR_ANGLE_LIMITS    = 0x00000080, //!< Transmitter angle limits checked/failed
      cXMTR_HORIZON_MASKING = 0x00000100, //!< Transmitter horizon masking checked/failed
      cXMTR_TERRAIN_MASKING = 0x00000200, //!< Transmitter terrain masking checked/failed
      cSIGNAL_LEVEL         = 0x00000400, //!< Signal level checked/failed
      cSTATUS_MASK          = 0x0000FFFF  //!< Bit mask of all base-class status bits
   };

   enum Geometry
   {
      cXMTR_TO_TARGET,
      cTARGET_TO_RCVR,
      cXMTR_TO_RCVR
   };

   //! Represents the location of a platform or device.
   struct LocationData
   {
      double mLat{0.0};                 //!< Latitude (decimal degrees)
      double mLon{0.0};                 //!< Longitude (decimal degrees)
      double mAlt{0.0};                 //!< Altitude (meters)
      double mLocWCS[3]{0.0, 0.0, 0.0}; //!< WCS locations (meters)
      bool   mIsValid{false};           //!< Data is valid only if this is true
   };

   //! Represents the relative location of one object with respect to another.
   //! @note The azimuth and elevation angle are with respect to the CUED orientation of the source object.
   //!
   //! @note This data may not be valid if the Begin<>Interaction methods returned early due to a failed
   //! constraint check (they exit as soon as possible to eliminate unnecessary computations). Observers
   //! using this data (e.g.: WsfEventOutput) should call ComputeUndefinedGeometry prior to using this data.
   struct RelativeData
   {
      double mRange{-1.0}; //!< Range to the other object (meters).

      double mTrueUnitVecWCS[3]{0.0, 0.0, 0.0}; //!< True WCS unit vector pointing to the other object.
      double mTrueAz{0.0};                      //!< The azimuth of the other object (radians)
      double mTrueEl{0.0};                      //!< The elevation of the other object (radians)

      double mUnitVecWCS[3]{0.0, 0.0, 0.0}; //!< Apparent WCS unit vector pointing to the other object.
      double mAz{0.0};                      //!< The apparent azimuth of the other object (radians)
      double mEl{0.0};                      //!< The apparent elevation of the other object (radians)
   };

   //! Represents the antenna gain lookup.
   struct BeamData
   {
      //! Azimuth of the target with respect to the beam (radians).
      //! This is valid only if mGain >= 0.0.
      double mAz{0.0};

      //! Elevation of the target with respect to the beam (radians).
      //! This is valid only if mGain >= 0.0.
      double mEl{0.0};

      //! Gain at the specified az/el (absolute, not dB).
      //! This is valid only if it is >= 0.0.  The special value of 0 indicates that
      //! the gain has not yet been defined, but that the az/el values are valid.
      double mGain{-1.0};

      //! The azimuth of the beam with respect to the antenna face (radians).
      //! This is valid only if mGain >= 0.0.
      double mEBS_Az{0.0};

      //! The elevation of the beam with respect to the antenna face (radians).
      //! This is valid only if mGain >= 0.0.
      double mEBS_El{0.0};

      //! The transformation matrix that defines the pointing angle of the beam.
      //! The rows of the matrix define the unit vectors of beam coordinate system
      //! in the WCS frame.
      double mWCS_ToBeamTransform[3][3]{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
   };

   WsfEM_Interaction()           = default;
   ~WsfEM_Interaction() override = default;

   virtual unsigned int BeginOneWayInteraction(WsfEM_Rcvr* aRcvrPtr, WsfPlatform* aTgtPtr);

   virtual unsigned int BeginOneWayInteraction(WsfEM_Xmtr* aXmtrPtr,
                                               WsfEM_Rcvr* aRcvrPtr,
                                               bool        aCheckXmtrLimits,
                                               bool        aCheckRcvrLimits,
                                               bool        aCheckMaskingFactor = true);

   virtual unsigned int BeginTwoWayInteraction(WsfEM_Xmtr* aXmtrPtr, WsfPlatform* aTgtPtr, WsfEM_Rcvr* aRcvrPtr);

   virtual unsigned int BeginTwoWayOTH_Interaction(WsfEM_Xmtr*  aXmtrPtr,
                                                   WsfPlatform* aTgtPtr,
                                                   WsfEM_Rcvr*  aRcvrPtr,
                                                   double       aReflectionLocWCS[3]);

   virtual unsigned int BeginGenericInteraction(WsfEM_Xmtr* aXmtrPtr, WsfPlatform* aTgtPtr, WsfEM_Rcvr* aRcvrPtr);

   double ComputeAttenuationFactor(Geometry aGeometry);

   void ComputeMaskingFactor();

   void ComputeInfraredSigAzEl();

   void ComputeOpticalSigAzEl();

   void ComputeRadarSigAzEl();

   double ComputeRF_OneWayPower();

   double ComputeRF_TwoWayPower(double aCrossSection);

   double ComputeRF_ReceivedPower(double aPowerDensity);

   double ComputeRF_TransmittedPower();

   virtual void ComputeUndefinedGeometry();

   double ComputeTargetDopplerFrequency(bool aFilterOwnshipFromDoppler);

   double ComputeTargetDopplerSpeed(bool aFilterOwnshipFromDoppler);

   //! Return the pointer to the receiver.
   //!
   //! @returns The pointer to the receiver.
   WsfEM_Rcvr* GetReceiver() const { return mRcvrPtr; }

   //! Return the pointer to the transmitter.
   //!
   //! @returns The pointer to the transmitter or 0 if the interaction does not
   //!          involve a transmitter.
   WsfEM_Xmtr* GetTransmitter() const { return mXmtrPtr; }

   //! Return the pointer to the target platform.
   //!
   //! @returns The pointer to the target platform or 0 if the interaction is
   //!          one that doesn't involve a target platform.
   WsfPlatform* GetTarget() const { return mTgtPtr; }

   static bool MaskedByHorizon(const WsfEM_Xmtr* aXmtrPtr, const WsfEM_Rcvr* aRcvrPtr);

   static bool MaskedByHorizon(const WsfEM_XmtrRcvr* aXmtrRcvrPtr, WsfPlatform* aPlatformPtr, double aEarthRadiusScale);

   static bool MaskedByHorizon(const WsfEM_XmtrRcvr* aXmtrRcvrPtr,
                               WsfPlatform*          aPlatformPtr,
                               double                aReflectionPointWCS[3],
                               double                aEarthRadiusScale);

   bool MaskedByTerrain();

   virtual void Print(ut::log::MessageStream& aMsgStream);

   virtual void PrintGeometry(ut::log::MessageStream& aMsgStream);

   //! PrintStatus adds a "Status" note and formats it properly if the status mFailedStatus is 0.
   virtual void PrintStatus(ut::log::MessageStream& aMsgStream) const;
   //! Adds a list of notes to aMsgStream with the status.
   virtual void PrintStatusP(ut::log::MessageStream& aMsgStream) const;

   double PrintableAngle(double aAngle) const;

   virtual void Reset();

   //! Get the name of the script class associated with this class.
   //! This is necessary for proper downcasts in the scripting language.
   const char* GetScriptClassName() const override { return "WsfEM_Interaction"; }

   void SetReceiverBeamPosition();

   void SetReceiverBeamPosition(const BeamData& aBeamData);

   void SetTransmitterBeamPosition();

   void SetTransmitterBeamPosition(const BeamData& aBeamData);

   bool WithinFieldOfView(WsfEM_Antenna* aAntennaPtr,
                          LocationData&  aSrcLoc,
                          LocationData&  aTgtLoc,
                          RelativeData&  aSrcToTgt,
                          RelativeData&  aTgtToSrc,
                          bool           aIgnoreLimits = false);

   void SetCategory(WsfStringId aCategory) { mCategoryId = aCategory; }
   bool CategoryIsSet() const { return !mCategoryId.Empty(); }

   void AddComponent(Component* aComponentPtr) { mComponents.AddComponent(aComponentPtr); }

   const ComponentList& GetComponents() const { return mComponents; }

   //! A bit mask indicating which limits have been checked.
   //! If this value is zero then no limits have been checked,
   //! and the interaction should be considered 'failed'.
   unsigned int mCheckedStatus{0};

   //! A bit mask indicating which limit check failed.
   //! This is valid only if 'mCheckedStatus' is non-zero (i.e: one can't
   //! have a failure if one hasn't checked anything.)
   unsigned int mFailedStatus{0};

   //! 'true' if a two-way interaction is 'bistatic'.
   //! (i.e.: the transmitter and receiver are not at the same location)
   bool mBistatic{false};

   //! Location of the transmitter.
   //! @note This is valid only if the interaction involves a transmitter.
   LocationData mXmtrLoc;

   //! Location of the receiver.
   //! @note This is valid only if the interaction involves a receiver.
   LocationData mRcvrLoc;

   //! Location of the target.
   //! @note This is valid only if the interaction involves a target platform.
   LocationData mTgtLoc;

   //! Receiver-to-target relative data.
   //! @note This is valid only if the interaction involves a target platform.
   RelativeData mRcvrToTgt;

   //! Target-to-receiver relative data.
   //! @note This is valid only if the interaction involves a target platform.
   RelativeData mTgtToRcvr;

   //! Transmitter-to-target relative data.
   //! @note This is valid only if the interaction involves a transmitter and a
   //! target platform.
   RelativeData mXmtrToTgt;

   //! Target-to-transmitter relative data.
   //! @note This is valid only if the interaction involves a transmitter and a
   //! target platform.
   RelativeData mTgtToXmtr;

   //! Transmitter-to-receiver relative data.
   //! @note This is only valid for one-way transmitter-receiver interactions.
   RelativeData mXmtrToRcvr;

   //! Receiver-to-transmitter relative data.
   //! @note This is only valid for one-way transmitter-receiver interactions.
   RelativeData mRcvrToXmtr;

   //! Antenna gain lookup data for the receiver.
   BeamData mRcvrBeam;

   //! Antenna gain lookup data for the transmitter.
   BeamData mXmtrBeam;

   //! The masking factor.
   //! A factor in the range [0..1] that accounts for blockage or attenuation by structure.
   //! A value of 1 indicates there no blockage or attenuation by structure while a value of
   //! zero indicates blockage or attenuation.
   //!
   //! @note This includes the effects of both the transmitter and receiver sides.
   double mMaskingFactor{1.0};

   //! Infrared signature (radiant intensity) (watts/steradian)
   //! @note If the value is less than or equal to zero then it hasn't been determined.
   double mInfraredSig{-1.0};

   //! Azimuth used for infrared signature determination (radians).
   //! @note This is valid only if mInfraredSig > 0.
   double mInfraredSigAz{0.0};

   //! Elevation used for infrared signature determination (radians).
   //! @note This is valid only if mInfraredSig > 0.
   double mInfraredSigEl{0.0};

   //! Radar signature (cross section) (m^2)
   //! @note If the value is less than or equal to zero then it hasn't been determined.
   double mRadarSig{-1.0};

   //! Azimuth used for radar signature determination (radians).
   //! @note This is valid only if mRadarSig > 0.
   double mRadarSigAz{0.0};

   //! Elevation used for radar signature determination (radians).
   //! @note This is valid only if mRadarSig > 0.
   double mRadarSigEl{0.0};

   //! Optical signature (cross section) (m^2).
   //! @note If the value is less than or equal to zero then it hasn't been determined.
   double mOpticalSig{-1.0};

   //! Azimuth used for optical signature determination (radians).
   //! @note This is valid only if mOpticalSig > 0.
   double mOpticalSigAz{0.0};

   //! Elevation used for optical signature determination (radians).
   //! @note This is valid only if mOpticalSig > 0.
   double mOpticalSigEl{0.0};

   //! Optical reflectivity Factor (non-dimensional (0 .. 1]).
   //! @note If the value is less than or equal to zero then it hasn't been determined.
   double mOpticalReflectivity{-1.0};

   //! Transmitted power (watts).
   //! The transmitted power at the antenna face.
   //! @note If the value is less than or equal to zero then it hasn't been determined.
   double mXmtdPower{0.0};

   //! Power density at the target (Watts/meter^2)
   //! This is the power density at the target body (for a two-way interaction) or at the
   //! face of the receiver antenna (one-way interaction).
   //! @note If the value is less than or equal to zero then it hasn't been determined.
   double mPowerDensityAtTarget{0.0};

   //! Received power (watts).
   //! The received power at the input to the detector.
   //! @note If the value is less than or equal to zero then it hasn't been determined.
   double mRcvdPower{0.0};

   //! Receiver noise power (watts)
   //! The internal receiver noise.
   //! @note If the value is less than or equal to zero then it hasn't been determined.
   double mRcvrNoisePower{0.0};

   //! Clutter power (watts)
   //! @note If the value is less than or equal to zero then it hasn't been determined.
   double mClutterPower{0.0};

   //! Interference power (watts)
   //! @note This is valid only if it is greater than zero.
   double mInterferencePower{0.0};

   //! The interference effect factor [0..1]
   //! A value of 0 indicates there was no effects-based interference.
   //! A value of 1 indicates the signal was total blocked by effects-based interference.
   //! The default value is 0.0 (no interference).
   double mInterferenceFactor{0.0};

   //! Absolute signal to noise ratio (not dB).
   //! This should probably be called signal-to-interference ratio, where the interference
   //! is the sum of the receiver noise, clutter power and other interfering effects.
   //! @note If the value is less than or equal to zero then it hasn't been determined.
   double mSignalToNoise{0.0};

   //! The pattern propagation factor.
   //! This is typically called 'F40' and represents the gains and losses due to things
   //! like multipath interference.
   //! @note This value is invalid unless > 0.0.
   double mPropagationFactor{0.0};

   //! The atmospheric absorption factor (0-1).
   //! Typically called the atmospheric attenuation or transmittance, this represents the
   //! fraction of the transmitted signal that is NOT blocked by the atmosphere.
   //! A value of 1 indicates the atmosphere did not absorb (attenuate) the signal while a
   //! a value of 0 indicates the atmosphere absorbed ALL the signal.
   //! @note This value is invalid unless > 0.0.
   double mAbsorptionFactor{0.0};

   //! Infrared background radiant intensity (watts/steradian)
   //! @note This value is invalid unless mSignalToNoise > 0.0.
   double mBackgroundRadiantIntensity{0.0};

   //! Infrared contrast radiant intensity (watts/steradian)
   //! @note This value is invalid unless mSignalToNoise > 0.0.
   double mContrastRadiantIntensity{0.0};

   //! The nominal detection threshold.
   //! The signal-to-noise ratio above which a detection is nominally declared. Note that even though the
   //! signal-to-noise may exceed this value, a detection may not be declared because of probabilistic effects.
   //! @note If the value is less than or equal to zero then it hasn't been determined.
   double mDetectionThreshold{0.0};

   //! The approximate number of pixels the target occupies in the image.
   //! (For imaging sensors only)
   //! @note If the value is zero then it hasn't been determined.
   double mPixelCount{0.0};

   //! The scale factor used to determine the EFFECTS of atmospheric refraction.
   //! This is typically 4/3 for RF and 1 for optical.
   double mEarthRadiusScale{0.0};

   //! If non-zero, the name of ID of the category to be used for zone-based attenuation.
   WsfStringId mCategoryId{nullptr};

   //! The computed zone-based attenuation factor.
   double mZoneAttenuationValue{0.0};

private:
   void ComputeRF_PropagationFactor();

   void ComputeReceiverBeamAspect();

   void ComputeTransmitterBeamAspect();

   void ComputeZoneAttenuation(WsfPlatform* aFromPtr, WsfPlatform* aTargetPtr, bool aIsTwoWay);

   void PrintAbsTargetData(ut::log::MessageStream& aMsgStream,
                           const char*             aPrefix,
                           WsfPlatform*            aTgtPtr,
                           const LocationData&     aLocData);

   void PrintRelTargetData(ut::log::MessageStream& aMsgStream,
                           const char*             aPrefix,
                           WsfPlatform*            aTgtPtr,
                           const RelativeData&     aRelData);

   void PrintAbsXmtrRcvrData(ut::log::MessageStream& aMsgStream,
                             const char*             aPrefix,
                             WsfEM_XmtrRcvr*         aXmtrRcvrPtr,
                             const LocationData&     aLocData);

   void PrintRelXmtrRcvrData(ut::log::MessageStream& aMsgStream,
                             const char*             aPrefix,
                             WsfEM_XmtrRcvr*         aXmtrRcvrPtr,
                             const RelativeData&     aRelData);

   void PrintBeamData(ut::log::MessageStream& aMsgStream,
                      const char*             aPrefix,
                      WsfEM_XmtrRcvr*         aXmtrRcvrPtr,
                      const BeamData&         aBeamData);

   void PrintLocationData(ut::log::MessageStream& aMsgStream,
                          const char*             aPrefix,
                          WsfPlatform*            aPlatformPtr,
                          const LocationData&     aLocationData) const;

   void PrintRange(ut::log::MessageStream& aMsgStream, double aRange) const;


   WsfEM_Xmtr*  mXmtrPtr{nullptr};
   WsfEM_Rcvr*  mRcvrPtr{nullptr};
   WsfPlatform* mTgtPtr{nullptr};

   ComponentList mComponents;
};

#endif
