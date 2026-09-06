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

#ifndef WSFEM_XMTRRCVR_HPP
#define WSFEM_XMTRRCVR_HPP

#include "wsf_export.h"

#include <array>
#include <vector>

class UtInput;
#include "UtScriptAccessible.hpp"
class WsfAntennaPattern;
class WsfArticulatedPart;
#include "WsfAuxDataEnabled.hpp"
class WsfEM_Antenna;
class WsfEM_Attenuation;
class WsfEM_Manager;
class WsfEM_Propagation;
#include "WsfEM_Types.hpp"
class WsfScenario;
class WsfMode;
class WsfPlatform;
class WsfPlatformPart;
class WsfSimulation;
#include "WsfStringId.hpp"
#include "WsfUniqueId.hpp"

//! This is the base class for WsfEM_Xmtr and WsfEM_Rcvr. It encapsulates the
//! common attributes of a transmitter or a receiver (antenna position, gain
//! patterns, etc).
class WSF_EXPORT WsfEM_XmtrRcvr : public UtScriptAccessible, public WsfUniqueId, public WsfAuxDataEnabled
{
public:
   //! Vector of pairs of antenna pattern pointer and associated lower frequency bounds
   using AntennaPatternList = std::vector<std::pair<WsfAntennaPattern*, double>>;

   WsfEM_XmtrRcvr(WsfEM_Antenna* aAntennaPtr = nullptr);
   WsfEM_XmtrRcvr(const WsfEM_XmtrRcvr& aSrc, WsfEM_Antenna* aAntennaPtr);
   virtual ~WsfEM_XmtrRcvr();

   WsfPlatform* GetPlatform() const;

   WsfArticulatedPart* GetArticulatedPart() const;

   virtual bool Initialize(WsfSimulation& aSimulation);

   virtual bool ProcessInput(UtInput& aInput);

   virtual void UpdatePosition(double aSimTime);

   const char* GetScriptClassName() const override { return "WsfEM_XmtrRcvr"; }

   //! Get the pointer to the attenuation model (if one is defined).
   //! @note Only valid after initialization.
   WsfEM_Attenuation* GetAttenuationModel() const { return mAttenuationPtr; }

   void SetAttenuationModel(WsfEM_Attenuation* aAttenuationPtr);

   //! Return the type of the attenuation model (if one is defined).
   WsfStringId GetAttenuationModelType() const { return mAttenuationType; }

   //! Set the type of attenuation model to be used.
   //! @note Only valid before initialization.
   void SetAttenuationModelType(WsfStringId aAttenuationType) { mAttenuationType = aAttenuationType; }

   //! Get the pointer to the propagation model (if one is defined).
   //! @note Only valid after initialization.
   WsfEM_Propagation* GetPropagationModel() const { return mPropagationPtr; }

   void SetPropagationModel(WsfEM_Propagation* aPropagationPtr);

   //! Return the type of the propagation model (if one is defined).
   WsfStringId GetPropagationModelType() const { return mPropagationType; }

   //! Set the type of propagation model to be used.
   //! @note Only valid before initialization.
   void SetPropagationModelType(WsfStringId aPropagationType) { mPropagationType = aPropagationType; }

   //! @name Antenna methods.
   //@{

   //! Get the pointer associated geometry object.
   WsfEM_Antenna* GetAntenna() const { return mAntennaPtr; }

   //! Set the pointer to the associated antenna object.
   //! @param aAntennaPtr Pointer to the associated geometry object.
   //! @note The caller retains ownership of the geometry object as it may be shared with
   //! multiple transmitters and receivers.
   void SetAntenna(WsfEM_Antenna* aAntennaPtr) { mAntennaPtr = aAntennaPtr; }

   WsfAntennaPattern* GetAntennaPattern(WsfEM_Types::Polarization aPolarization, double aFrequency) const;

   bool SetAntennaPattern(WsfAntennaPattern* aAntennaPatternPtr, WsfEM_Types::Polarization aPolarization, double aFrequency);

   bool AddAntennaPattern(WsfAntennaPattern* aAntennaPatternPtr, WsfEM_Types::Polarization aPolarization, double aFrequency);

   const AntennaPatternList* GetAntennaPatternList(WsfEM_Types::Polarization aPolarization) const;

   //! Get the angle of the boresight of the antenna gain pattern above/below (+/-) the antenna boresight.
   //! Note that this is used primarily to model stacked beam radar systems.
   double GetBeamTilt() const { return mBeamTilt; }

   //! Set the angle of the boresight of the antenna gain pattern above/below (+/-) the antenna boresight.
   void SetBeamTilt(double aBeamTilt) { mBeamTilt = aBeamTilt; }

   virtual double GetAntennaGain(WsfEM_Types::Polarization aPolarization,
                                 double                    aFrequency,
                                 double                    aTargetAz,
                                 double                    aTargetEl,
                                 double                    aEBS_Az,
                                 double                    aEBS_El);

   WSF_DEPRECATED virtual double GetAzimuthBeamwidth() const final { return GetAzimuthBeamwidth(0.0, 0.0); }

   virtual double GetAzimuthBeamwidth(double aEBS_Azimuth, double aEBS_Elevation) const;

   WSF_DEPRECATED virtual double GetElevationBeamwidth() const final { return GetElevationBeamwidth(0.0, 0.0); }

   virtual double GetElevationBeamwidth(double aEBS_Azimuth, double aEBS_Elevation) const;

   virtual double GetPeakAntennaGain() const;

   virtual double GetMinimumAntennaGain() const;

   virtual void SetAntennaBeamCount(unsigned int aBeamCount);

   //@}

   //! @name Debugging methods
   //@{

   //! Determine if debugging is enabled.
   bool DebugEnabled() const { return mDebugEnabled; }

   //! Set the debug state.
   //! @param aDebugEnabled New debug state
   void SetDebugEnabled(bool aDebugEnabled) { mDebugEnabled = aDebugEnabled; }

   //@}

   static void AssociateSystems(WsfEM_XmtrRcvr* aSystem1Ptr, WsfEM_XmtrRcvr* aSystem2Ptr);

   //! determine if horizon masking is to be checked
   bool IsHorizonMaskingEnabled() const;

   //! determine if terrain masking is to be checked
   bool IsTerrainMaskingEnabled() const;

   //! Gets the masking mode
   std::string MaskingMode() const;

   //! Set the type of masking to perform
   void SetMaskingMode(const std::string& aTerrainMaskingMode);

   //! determine if horizon and terrain masking is to be checked
   bool CheckMasking() const { return mCheckMasking; }

   //! enable/disable checking of horizon and terrain masking
   void SetCheckMasking(bool aCheckMasking) { mCheckMasking = aCheckMasking; }

   //! disables the checking of horizon and terrain masking (retained for compatibility)
   void DisableMaskingCheck() { mCheckMasking = false; }

   //! @name Mode Association and Index methods.
   //!
   //! The use of these methods is completely optional. Complex systems may use these to identify the
   //! context in which a a specific xmtr/rcvr exists (e.g.: a particular xmtr/rcvr may represent 'beam x'
   //! in 'mode y'). Some observers (the DIS observer in particular) make use of the information if it
   //! is available.
   //!
   //! @note These are purely optional! Users of the 'Get' methods should be careful!

   //@{
   //! Get the mode with which this xmtr/rcvr is associated.
   //! @note This may be zero (association is optional).
   //! @note This is valid only after initialization.
   WsfMode* GetMode() const { return mModePtr; }

   //! Set the mode with which this xmtr/rcvr is associated.
   //! @note The caller retains ownership of the mode.
   //! @note The value is not propagate across copy construction.
   void SetMode(WsfMode* aModePtr) { mModePtr = aModePtr; }

   //! Get the index of the xmtr/rcvr in the complex assembly.
   size_t GetIndex() const { return mIndex; }

   //! Set the index of the xmtr/rcvr in the complex assembly.
   void SetIndex(size_t aIndex) { mIndex = aIndex; }
   //@}

   WsfSimulation* GetSimulation() const;

   WsfEM_Manager* GetEM_Manager() const;

   //! Get the earth radius multiplier (to account for diffraction).
   //! @return The earth radius multiplier (default 1)
   //! @note This is only for passive systems where there is not a corresponding transmitter object.
   double GetEarthRadiusMultiplier() const { return mEarthRadiusMultiplier; }

   //! Get the EM operating frequency.
   //! @return The current operating frequency (Hz).
   double GetFrequency() const { return mFrequency; }

   double GetWavelength() const;

   //! Get the EM operating bandwidth.
   //! @return The operating bandwidth (Hz).
   double GetBandwidth() const { return mBandwidth; }

   //! Return the source of the bandwidth
   //! @return true if bandwidth was defined in the input file
   bool IsBandwidthExplicit() const { return mExplicitBandwidth; }

   //! Get the internal loss.
   //! @return Internal loss (absolute ratio, NOT dB)
   //! @note This is a LOSS FACTOR (which is 1/GAIN). Therefore it should be greater than 1.
   double GetInternalLoss() const { return mInternalLoss; }

   //! Get the polarization of the EM system.
   WsfEM_Types::Polarization GetPolarization() const { return mPolarization; }

   //! Set the earth radius multiplier (to account for diffraction).
   //! @param aMultiplier The new earth radius multiplier.
   //! @note This is only for passive systems where there is not a corresponding transmitter object.
   void SetEarthRadiusMultiplier(double aMultiplier) { mEarthRadiusMultiplier = aMultiplier; }

   virtual void SetFrequency(double aFrequency);

   virtual void SetBandwidth(double aBandwidth);

   //! Set the internal loss.
   //! @param aInternalLoss Internal loss (absolute ratio, NOT dB)
   //! @note This is a LOSS FACTOR (which is 1/GAIN). Therefore it should be greater than 1.
   void SetInternalLoss(double aInternalLoss) { mInternalLoss = aInternalLoss; }

   virtual void SetPolarization(WsfEM_Types::Polarization aPolarization);

   //! @name Special functions for receivers.
   //! By default receivers disable the use of the attenuation_model and propagation_model, as interactions
   //! that involve a transmitter use models stored on the transmitter. For interactions where the
   //! signal source is not a transmitter, it is still
   //! useful to have these objects. Models that want to enabled them may do so...
   //@{
   //! Enable or disable 'attenuation_model' input.
   void SetAttenuationInputEnabled(bool aEnabled) { mAttenuationInputEnabled = aEnabled; }
   //! Enable or disable 'propagation_model' input
   void SetPropagationInputEnabled(bool aEnabled) { mPropagationInputEnabled = aEnabled; }
   //@}

protected:
   //! The type of the atmospheric attenuation model to be used.
   WsfStringId mAttenuationType;

   //! The type of the propagation model to be used.
   WsfStringId mPropagationType;

   //! Pointer to the atmospheric attenuation model.
   WsfEM_Attenuation* mAttenuationPtr;

   //! Pointer to the propagation object.
   WsfEM_Propagation* mPropagationPtr;

   //! The mode with which the xmtr/rcvr is associated.
   //! This may be zero. (association is optional).
   WsfMode* mModePtr;

   //! Pointer to the 'geometry' object for this transmitter/receiver
   WsfEM_Antenna* mAntennaPtr;

   //! Array of the polarization possibilities each containing a vector of Antenna pattern
   //! pointer/frequency pairs. If vector is empty, then there is no gain pattern and
   //! any gain is assumed to be 1.0
   std::array<AntennaPatternList, WsfEM_Types::cPOL_COUNT> mPatternTables{};

   //! Pointer to the transmitter/receiver which is associated to this receiver/transmitter.
   //! If two systems are linked then some unassigned values in one system will be implied from the
   //! other system. This makes user configuration a little easier.
   WsfEM_XmtrRcvr* mAssociatePtr;

   //! The tilt angle of the boresight of the antenna gain pattern above/below (+/-) the antenna boresight.
   //! Note that this is primarily intended for modeling stack-beam radars. Other uses have not been examined.
   double mBeamTilt;

   double                    mEarthRadiusMultiplier;
   double                    mFrequency;
   double                    mBandwidth;
   double                    mInternalLoss;
   WsfEM_Types::Polarization mPolarization;

   //! The index of this xmtr/rcvr in a system which may be composed of multiple xmtr/rcvrs.
   //! See GetIndex/SetIndex for more information.
   size_t mIndex;

   //! Array used to keep track of which explicitly declared patterns were loaded in the
   //! mPatternTables array
   std::array<bool, WsfEM_Types::cPOL_COUNT> mUsedExplicitPolarization{{false}};

   bool mDebugEnabled;

   unsigned int mTerrainMaskingMode; //!< Settings used for masking checks.
   bool         mCheckMasking;       //!< true if horizon and terrain masking enabled.
   bool         mExplicitBandwidth;  //!< true if explicit 'bandwidth' specified

private:
   void ClearAntennaPatterns();

   //! If true, the object will accept 'attenuation_model' and 'propagation_model' inputs.
   //! The default is true for transmitters and false for receivers. Some receivers will set
   //! this to true because the signal source is not a WsfEM_Xmtr.
   bool mAttenuationInputEnabled;
   bool mPropagationInputEnabled;
};

#endif
