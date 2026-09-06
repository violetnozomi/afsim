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

#ifndef WSFRF_JAMMER_HPP
#define WSFRF_JAMMER_HPP

#include "wsf_mil_export.h"

#include <map>
#include <memory>
#include <mutex>

#include "WsfEM_Antenna.hpp"
class WsfEM_Interaction;
class WsfEM_Rcvr;
#include "WsfEM_Xmtr.hpp"
class WsfEW_Effect;
#include "WsfRF_Repeater.hpp"
class WsfScenario;
#include "WsfSensor.hpp"
#include "WsfTrackId.hpp"
#include "WsfWeapon.hpp"

//! @name WsfRF_Jammer Class
//! A jammer of Radio Frequency (RF) emissions (radar, comm).
//! @note The jammer frequencies for multi-beam and/or multi-spot jammers may be adjusted based on the frequency coverage
//!       of the jammer specified by the frequency/bandwidth pair or frequency_band.
class WSF_MIL_EXPORT WsfRF_Jammer : public WsfWeapon
{
public:
   class JammerBeam;
   class JammerMode;

   class WSF_MIL_EXPORT TargetingData
   {
   public:
      //! 'index' to the targeted platform.
      size_t mTargetIndex{0};

      //! Track Id of the targeted platform.
      WsfTrackId mTargetTrackId = WsfTrackId();

      //! The targeted location of this jammer transmitter.
      double mTargetedLocationWCS[3] = {0.0, 0.0, 0.0};

      //! 'true' if the WCS location is valid.
      bool mTargetedLocationValid{false};
   };

   using TargetingDataMap = std::map<int, TargetingData>;

   //! GroupPowerDistribution defines the intended power distribution of the transmitters in this group.
   enum PowerDistribution
   {
      cAVERAGE_POWER, //!< Average
      cCONSTANT_POWER //!< Constant
   };

   WsfRF_Jammer(const WsfScenario& aScenario);

   WsfRF_Jammer(const WsfRF_Jammer& aSrc);

   WsfRF_Jammer& operator=(const WsfRF_Jammer&) = delete;

   ~WsfRF_Jammer() override = default;

   //! @name Common infrastructure methods.
   //@{

   WsfWeapon* Clone() const override;

   bool Initialize(double aSimTime) override;

   bool ProcessInput(UtInput& aInput) override;
   //@}

   //! @name Component infrastructure methods
   //@{
   const int* GetComponentRoles() const override; //! Returns array of constants role type
   void*      QueryInterface(int aRole) override; //! @see WsfWeapon::QueryInterface
   //@}

   //! @name Status methods.
   //@{

   void TurnOff(double aSimTime) override;

   //! Turn the weapon on or return statement of whether jamming has already started.
   //! @param aSimTime  The current simulation time.
   //! @note Jamming assignment will start if the system has been initialized and is turned off.
   void TurnOn(double aSimTime) override;
   //@}

   //! @name Transmitter access methods.
   //@{
   //! These methods support accessing the underlying transmitter components.

   //! @return The number of transmitters associated with this device for the current mode.
   //! @note returns 0 if the jammer has not been initialized.
   size_t GetEM_XmtrCount() const override;

   //! Get the transmitter with a specified index.
   //! @param aIndex    The index of the transmitter whose attributes are being requested.
   //!                  This must be in the range [0, GetEM_XmtrCount() - 1].
   //! @note It is an error to call this method if the device does not have any transmitters
   //!       (i.e.: GetEM_XmtrCount() == 0).
   WsfEM_Xmtr& GetEM_Xmtr(size_t aIndex) override;

   //! @return The number of transmitters associated with this device at the specified mode index.
   size_t GetEM_XmtrCount(size_t aModeIndex) const override;

   //! Get the transmitter with a specified mode and index.
   //! @param aModeIndex   The mode index of the jammer whose pointer will be used.
   //!                     This must be in the range of [0, mJammerModeList.size() -1].
   //! @param aIndex       The index of the transmitter whose attributes are being requested.
   //!                     This must be in the range [0, GetEM_XmtrCount() - 1].
   //! @note It is an error to call this method if the device does not have any transmitters
   //!       (i.e.: GetEM_XmtrCount() == 0).
   WsfEM_Xmtr& GetEM_Xmtr(size_t aModeIndex, size_t aIndex) override;

   //! Get the attributes for the transmitter on the specified beam and spot index.
   //! @param aModeIndex   The index of the mode containing the beam with the transmitter.
   //! @param aBeamIndex   The index of the beam containing the transmitter.
   //! @param aSpotIndex   The index of the transmitter whose attributes are being requested.
   //!                     This must be in the range [0, GetEM_XmtrCount() - 1] for the specified beam.
   //! @note The jammer beam must have active spots before calling this method.
   //!       (i.e.: beamPtr->GetActiveSpots() == 0).
   WsfEM_Xmtr& GetEM_Xmtr(size_t aModeIndex, size_t aBeamIndex, size_t aSpotIndex) override;

   //! Get the master transmitter for the current mode
   //! @return Pointer to the Master transmitter for the current mode
   WsfEM_Xmtr& GetMasterXmtr();

   //! Get a transmitter using the supplied frequency, bandwidth, and track Id.
   //! @note Assumes current mode.
   //! @param aSimTime     The current simulation time.
   //! @param aFrequency   The assigned jammer transmitter frequency. (Hertz)
   //! @param aBandwidth   The assigned jammer transmitter bandwidth. (Hertz)
   //! @param aBeamNumber  The assigned jammer transmitter Beam Number. (0 to maximum number of beams)
   //! @param aTrackId     The assigned Track Id of the target.
   //! @return The first found jammer transmitter for the specified input values, if none exist
   //!         then the returned pointer is 0.
   WsfEM_Xmtr& GetJammerXmtr(double aSimTime, double aFrequency, double aBandwidth, size_t aBeamNumber, WsfTrackId aTrackId);
   //@}

   //! @name Min/Max frequency access methods.
   //! These methods support accessing the underlying master transmitter
   //! to get the current frequency limits, assumes current mode.
   //@{

   double GetMinimumFrequency() const;
   double GetMaximumFrequency() const;
   //@}

   //! @name Spot jammer start/stop methods.
   //! These methods support spot jamming but can also be used for barrage jamming.
   //@{

   //! Start a jamming assignment using the supplied frequency and bandwidth.
   //! @param aSimTime     The current simulation time.
   //! @param aFrequency   The assigned jammer transmitter frequency. (Hertz)
   //! @param aBandwidth   The assigned jammer transmitter bandwidth. (Hertz)
   //! @param aBeamNumber  The assigned jammer transmitter Beam Number. (0 to maximum number of beams)
   //! @param aTechniqueId The assigned technique Id of the EA function to use.
   //! @param aTrackId     The assigned Track Id of the target to be jammed.
   //! @param aTargetIndex The platform index of the target to be jammed.
   //! @return The request was successful.
   bool StartJamming(double      aSimTime,
                     double      aFrequency,
                     double      aBandwidth,
                     size_t      aBeamNumber,
                     WsfStringId aTechniqueId,
                     WsfTrackId  aTrackId,
                     size_t      aTargetIndex) override;

   //! Start a jamming assignment using the supplied frequency and bandwidth.
   //! @param aSimTime     The current simulation time.
   //! @param aTechniqueId The assigned technique Id of the EA function to use.
   //! @param aTrackPtr    The assigned Track Pointer of the target to be jammed.
   //! @return The request was successful.
   bool StartJammingTrack(double aSimTime, WsfStringId aTechniqueId, const WsfTrack* aTrackPtr) override;

   //! Stop a jamming assignment using the supplied frequency, bandwidth, and track Id.
   //! @note Assumes current mode.
   //! @param aSimTime     The current simulation time.
   //! @param aFrequency   The assigned jammer transmitter frequency. (Hertz)
   //! @param aBandwidth   The assigned jammer transmitter bandwidth. (Hertz)
   //! @param aBeamNumber  The assigned jammer transmitter Beam Number. (0 to maximum number of beams)
   //! @param aTrackId     The assigned Track Id of the target to be jammed.
   //! @return The request was successful.
   bool StopJamming(double aSimTime, double aFrequency, double aBandwidth, size_t aBeamNumber, WsfTrackId aTrackId) override;

   //! Update a jamming assignment using the supplied frequency and bandwidth.
   //! @note This method is primarily used to update a jammer transmitter via DIS or other
   //!       or other interface that is not going to Stop and Re-Start a jamming assignment
   //!       to capture changes in the assignment.
   //! @param aSimTime     The current simulation time.
   //! @param aXmtrIndex   The assigned jammer transmitter Beam Number. (0 to maximum number of beams)
   //! @param aFrequency   The assigned jammer transmitter frequency. (Hertz)
   //! @param aBandwidth   The assigned jammer transmitter bandwidth. (Hertz)
   //! @param aTechniqueId The assigned technique Id of the EA function to use.
   //! @param aTrackId     The assigned Track Id of the target to be jammed.
   //! @param aTargetIndex The platform index of the target to be jammed.
   //! @return The request was successful.
   bool UpdateJamming(double      aSimTime,
                      size_t      aXmtrIndex,
                      double      aFrequency,
                      double      aBandwidth,
                      WsfStringId aTechniqueId,
                      WsfTrackId  aTrackId,
                      size_t      aTargetIndex) override;
   //@}

   //! @name Message processing methods.
   //! These methods support messaging to and from the jammer as required.
   //@{
   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
   //@}

   //! @name Jammer targeting data methods.
   //@{

   //! Assign target to TargetDataMap.
   void SetTargetIndex(int aId, size_t aTargetIndex) override;

   //! Return Target Id's index.
   size_t GetTargetIndex(int aId) const override;

   //! Insert Target track into TargetDataMap, if it doesn't already exist.
   void SetTargetTrackId(int aId, const WsfTrackId& aTargetTrackId);

   //! Return trackId at aId location if it exists, otherwise return a nullptr.
   WsfTrackId GetTargetTrackId(int aId) const;

   //! Set the targeted location for the jammer spot.
   //! @param aId                  The Id of the jammer.
   //! @param aTargetedLocationWCS The targeted WCS location.
   //! @note Valid WCS location must be entered.
   void SetTargetedLocationWCS(int aId, const double aTargetedLocationWCS[3]) override;

   //! Get the targeted location for the jammer spot.
   //! @param aId                  The Id of the jammer.
   //! @param aTargetedLocationWCS The updated targeted WCS location.
   //! @note Check if the location is valid before calling using TargetLocationIsValid().
   void GetTargetedLocationWCS(int aId, double aTargetedLocationWCS[3]) const override;

   //! Preform a check of whether aId is in TargetMapData
   bool TargetedLocationValid(int aId) const override;
   //@}


   //! @name EA Techniques methods.
   //@{

   //! Select a jamming technique using the supplied criteria.
   //! @note Assumes current mode.
   //! @param aSimTime     The current simulation time.
   //! @param aFrequency   The assigned jammer transmitter frequency. (Hertz)
   //! @param aBandwidth   The assigned jammer transmitter bandwidth. (Hertz)
   //! @param aBeamNumber  The assigned jammer transmitter Beam Number. (0 to maximum number of beams)
   //! @param aTechniqueId The assigned technique Id of the EA function to use.
   //! @param aTrackId     The assigned Track Id of the target to be jammed.
   //! @return The request was successful.
   bool SelectEA_Technique(double      aSimTime,
                           double      aFrequency,
                           double      aBandwidth,
                           size_t      aBeamNumber,
                           WsfStringId aTechniqueId,
                           WsfTrackId  aTrackId) override;

   //! Select a jamming technique using the supplied criteria.
   //! @note Assumes current mode.
   //! @param aSimTime     The current simulation time.
   //! @param aFrequency   The assigned jammer transmitter frequency. (Hertz)
   //! @param aBandwidth   The assigned jammer transmitter bandwidth. (Hertz)
   //! @param aBeamNumber  The assigned jammer transmitter Beam Number. (0 to maximum number of beams)
   //! @param aTechniqueId The assigned technique Id of the EA function to use.
   //! @param aTrackId     The assigned Track Id of the target.
   //! @return The request was successful.
   bool DeselectEA_Technique(double      aSimTime,
                             double      aFrequency,
                             double      aBandwidth,
                             size_t      aBeamNumber,
                             WsfStringId aTechniqueId,
                             WsfTrackId  aTrackId) override;

   //! Set jamming power (delta gain) technique on the master transmitter for future use.
   //! @note Assumes current mode.
   //! @param aSimTime               The current simulation time.
   //! @param aTechniqueId           The assigned technique Id to set.
   //! @param aOnOff                 The default state of the technique.
   //! @param aEffectId              The assigned effect Id to set.
   //! @param aJammingDeltaGain_dB   The assigned jamming modulation value.
   //! @param aSystemTypeId          The assigned System Type Id of the target types this technique applies to.
   //! @param aFunctionTypeId        The assigned System Function Id of the target types this technique applies to.
   //! @return The request was successful.
   bool SetEA_DeltaGainTechnique(double      aSimTime,
                                 WsfStringId aTechniqueId,
                                 bool        aOnOff,
                                 WsfStringId aEffectId,
                                 double      aJammingDeltaGain_dB,
                                 WsfStringId aSystemTypeId,
                                 WsfStringId aFunctionTypeId) override;

   //! Set a jamming power(delta gain) technique on an active spot using the supplied criteria.
   //! @note Assumes current mode.
   //! @param aSimTime               The current simulation time.
   //! @param aTechniqueId           The assigned technique Id to set.
   //! @param aOnOff                 The default state of the technique, 'true' is default on, 'fasle' is default off.
   //! @param aEffectId              The assigned effect Id to set.
   //! @param aJammingDeltaGain_dB   The assigned jamming delta gain value.
   //! @param aSystemTypeId          The assigned System Type Id of the target types this technique applies to.
   //! @param aSystemFunctionId      The assigned System Function Id of the target types this technique applies to.
   //! @param aFrequency             The assigned jammer transmitter frequency. (Hertz)
   //! @param aBandwidth             The assigned jammer transmitter bandwidth. (Hertz)
   //! @param aBeamNumber            The assigned jammer transmitter Beam Number. (0 to maximum number of beams)
   //! @param aTrackId               The assigned Track Id of the target to be jammed.
   //! @return The request was successful.
   bool SetEA_DeltaGainTechnique(double      aSimTime,
                                 WsfStringId aTechniqueId,
                                 bool        aOnOff,
                                 WsfStringId aEffectId,
                                 double      aJammingDeltaGain_dB,
                                 WsfStringId aSystemTypeId,
                                 WsfStringId aSystemFunctionId,
                                 double      aFrequency,
                                 double      aBandwidth,
                                 size_t      aBeamNumber,
                                 WsfTrackId  aTrackId) override;
   //@}

   //! @name Jammer effect methods.
   //@{

   //! Compute the raw jammer power from the specified transmitter on the target interaction.
   //! @param aSimTime                The current simulation time.
   //! @param aJammerXmtrPtr          Pointer to the jammer transmitter.
   //! @param aTargetInteraction      The interaction object representing the interaction to be jammed (radio, radar,
   //! ESM, etc.).
   //!                                This is used only to get the pointer to the receiver and it's current pointing
   //!                                information.
   //! @param aJammerInteraction      [updated] The interaction between the receiver and jammer. On input the receiver
   //! and transmitter
   //!                                pointers are used. On output, the mNoiseJammerPower, mPulseJammerPower,
   //!                                mCoherentJammerPower, mEW_Effects and mEW_Error fields will be updated for this
   //!                                single interaction.
   //! @param aProcessingEW_EffectPtr [optional] The currently processing or called from EW effect's pointer.
   //! @return The jammer power of the interaction, returns the noise power if it is greater than 0, otherwise returns
   //! the
   //!         coherent power even if it is 0.
   static double ComputeJammerPower(double                   aSimTime,
                                    WsfEM_Xmtr*              aJammerXmtrPtr,
                                    const WsfEM_Interaction& aTargetInteraction,
                                    WsfEM_Interaction&       aJammerInteraction,
                                    WsfEW_Effect*            aProcessingEW_EffectPtr = nullptr);

   //! Compute the raw jammer power from the specified transmitter on the target interaction.
   //! @param aSimTime                The current simulation time.
   //! @param aJammerXmtrPtr          Pointer to the jammer transmitter.
   //! @param aTargetInteraction      The interaction object representing the interaction to be jammed (radio, radar, ESM, etc.).
   //!                                This is used only to get the pointer to the receiver and it's current pointing information.
   //! @param aProcessingEW_EffectPtr [optional] The currently processing or called from EW effect's pointer.
   //! @return The jammer power of the interaction, returns the noise power if it is greater than 0, otherwise returns the
   //!         coherent power even if it is 0.
   static double ComputeJammerPower(double                   aSimTime,
                                    WsfEM_Xmtr*              aJammerXmtrPtr,
                                    const WsfEM_Interaction& aTargetInteraction,
                                    WsfEW_Effect*            aProcessingEW_EffectPtr = nullptr);

   //! Compute the raw jammer power from the specified transmitter on the target interaction.
   //! @param aSimTime           [input] The current simulation time.
   //! @param aJammerXmtrPtr     [input] Pointer to the jammer transmitter.
   //! @param aTargetInteraction [input] The interaction object representing the interaction to be jammed (radio, radar,
   //! ESM, etc.).
   //!                           This is used only to get the pointer to the receiver and it's current pointing
   //!                           information.
   //! @param aJammerInteraction [updated] The interaction between the receiver and jammer. On input the receiver and
   //! transmitter
   //!                           pointers are used. On output, the mNoiseJammerPower, mPulseJammerPower,
   //!                           mCoherentJammerPower, mEW_Effects and mEW_Error fields will be updated for this single
   //!                           interaction.
   //! @param aJammerPowers      [updated] The jammer powers of the interaction to include: aJammerPowers[0] - noise
   //!                                                                                      aJammerPowers[1] - pulse
   //!                                                                                      aJammerPowers[2] - coherent
   //! @param aProcessingEW_EffectPtr [optional] The currently processing or called from EW effect's pointer.
   static void ComputeJammerPowers(double                   aSimTime,
                                   WsfEM_Xmtr*              aJammerXmtrPtr,
                                   const WsfEM_Interaction& aTargetInteraction,
                                   WsfEM_Interaction&       aJammerInteraction,
                                   double                   aJammerPowers[],
                                   WsfEW_Effect*            aProcessingEW_EffectPtr = nullptr);

   //! Compute the raw jammer power from the specified transmitter on the target interaction.
   //! @param aSimTime           [input] The current simulation time.
   //! @param aJammerXmtrPtr     [input] Pointer to the jammer transmitter.
   //! @param aTargetInteraction [input] The interaction object representing the interaction to be jammed (radio, radar,
   //! ESM, etc.).
   //!                           This is used only to get the pointer to the receiver and it's current pointing
   //!                           information.
   //! @param aJammerPowers      [updated] The jammer powers of the interaction to include: aJammerPowers[0] - noise
   //!                                                                                      aJammerPowers[1] - pulse
   //!                                                                                      aJammerPowers[2] - coherent
   //! @param aProcessingEW_EffectPtr [optional] The currently processing or called from EW effect's pointer.
   static void ComputeJammerPowers(double                   aSimTime,
                                   WsfEM_Xmtr*              aJammerXmtrPtr,
                                   const WsfEM_Interaction& aTargetInteraction,
                                   double                   aJammerPowers[],
                                   WsfEW_Effect*            aProcessingEW_EffectPtr = nullptr);

   //! Compute the effect of a single jammer on the receiver in the specified interaction.
   //! This is the main method that should be called to compute the effect of a single jammer on an interaction.
   //!   It determines the effect of the jammer on the given target interaction and computes the net result.
   //!   If aApplyEffects is 'true' the aTargetInteraction and aJammerInteraction powers are updated with the EW
   //!   effects. Upon entering it is also assumed that the aTargetInteraction jammer powers already have EW effects
   //!   applied if this flag is set to true.
   //! @param aSimTime           [input] The current simulation time.
   //! @param aJammerXmtrPtr     [input] The jammer pointer to compute the effects for the given interaction.
   //! @param aTargetInteraction [updated] The interaction for which jamming calculations are to be performed. On input
   //! the receiver
   //!                           beam position attributes are used. On output, the mNoiseJammerPower, mPulseJammerPower,
   //!                           mCoherentJammerPower, mEW_Effects and mEW_Error fields will be updated with current
   //!                           updated values.
   //! @param aJammerInteraction [updated] The interaction between the receiver and jammer. On input the receiver and
   //! transmitter
   //!                           pointers are used. On output, the mNoiseJammerPower, mPulseJammerPower,
   //!                           mCoherentJammerPower, mEW_Effects and mEW_Error fields will be updated for this single
   //!                           interaction.
   //! @param aApplyEW_Effects   [input] If true, will apply the electronic warfare effects that impinge upon the
   //! receiver to the
   //!                           jamming power variables.
   static void ComputeSingleJammerEffect(double             aSimTime,
                                         WsfEM_Xmtr*        aJammerXmtrPtr,
                                         WsfEM_Interaction& aTargetInteraction,
                                         WsfEM_Interaction& aJammerInteraction,
                                         bool               aApplyEW_Effects);

   //! Compute the total effect of the jammer on the receiver in the specified interaction.
   //! This is the main method that should be called to compute the effect of jammers on an interaction. It determines
   //! all of the
   //!   jammers that have an effect on the given target interaction and computes the net result.
   //! @param aSimTime           [input] The current simulation time.
   //! @param aTargetInteraction [updated] The interaction for which jamming calculations are to be performed. On input
   //! the receiver
   //!                           pointer and beam position attributes are used. On output, the mNoiseJammerPower,
   //!                           mEW_Effects and mEW_Error fields will be updated.
   //! @param aApplyEW_Effects   [input] If true, will apply the electronic warfare effects that impinge upon the
   //! receiver to the
   //!                           jamming power variables.
   static void ComputeTotalJammerEffects(double aSimTime, WsfEM_Interaction& aTargetInteraction, bool aApplyEW_Effects);

   //! Compute Jammer Receiver Gains from incident Radar Beam
   static double ComputeReceiverGains(WsfEM_Rcvr* aRcvrPtr);

   //@}

   //! @name Inventory methods.
   //! The definition of quantity depends on the type of weapon. For some weapons it might be
   //! 'rounds', for others it might be 'jamming spots/beams, etc'. In this case, the quantity
   //! returned refers to the number of spots for the current jammer mode.
   //@{

   unsigned int GetActiveRequestCount() const override;
   unsigned int GetMaximumRequestCount() const override;

   double GetQuantityRemaining() const override;

   size_t GetActiveBeamCount() const override;
   size_t GetMaximumBeamCount() const override;
   size_t GetActiveSpotCount(size_t aBeamNumber) const override;
   size_t GetMaximumSpotCount(size_t aBeamNumber) const override;

   //@}

   //! @name Field of view methods.
   //! Determines if the platform is within the weapons field of view.
   //@{

   bool WithinFieldOfView(double aSimTime, WsfPlatform* aPlatformPtr) override;
   bool WithinFieldOfView(double aSimTime, WsfTrack* aTrackPtr) override;

   //@}

   //! A specialized WsfEM_Xmtr class. Provides for handling of allowed interactions between
   //! a transmitter and receiver and specific targeted data to allow pointing of the jammer spots.
   class WSF_MIL_EXPORT JammerXmtr : public WsfEM_Xmtr
   {
      friend class WsfRF_Jammer::JammerBeam;
      friend class WsfRF_Jammer::JammerMode;

   public:
      //! Nested class WsfRF_Jammer::JammerXmtr. This class is to provide for handling
      //! of allowed interactions between a transmitter and receiver
      JammerXmtr(WsfEM_Antenna* aAntennaPtr);

      //! JammerXmtr Copy Constructor
      JammerXmtr(const JammerXmtr& aSrc, WsfEM_Antenna* aAntennaPtr);

      //! @name Common infrastructure methods.
      //@{
      bool Initialize(WsfSimulation& aSimulation) override;
      bool ProcessInput(UtInput& aInput) override;
      //@}

      //! @name Allowed interaction methods.
      //! Is this transmitter allowed to interact with (transmit radiation to) the specified
      //!   receiver.
      //! @param aRcvrPtr Pointer to the receiver that the allowed interaction is to be checked for.
      //! This method should answer the question, 'Should we allow this transmitter to
      //!   interact with the supplied receiver'.  This method must NOT consider geometry.
      bool AllowInteractionWith(WsfEM_Rcvr* aRcvrPtr) override;
      //@}

      //! @name Frequency band methods.
      //@{

      //! Get the frequency band, i.e. frequency limits.
      //! @param[out] aFrequencyBand: A tuple containing the lower and upper limits of the frequency band for the transmitter.
      void GetFrequencyBand(double aFrequencyBand[2]) const;

      //@}

   protected:
      //! Categories which this jammer should ignore.
      WsfCategoryList mIgnoredCategories;

      //! The sides (teams) which this jammer should ignore.
      std::vector<WsfStringId> mIgnoredSides;

      //! A bit mask of ignored domains.
      unsigned int mIgnoredDomains;

      //! 'true' if the jammer should not jam platforms on the same side.
      bool mIgnoreSameSide;

      //! Alternate input for defining the frequency and bandwidth of the jammer transmitter
      double mFrequencyBand[2];
   };

   // Search vector and map to find beam and spot indexes that meet a specified criteria.
   using SpotIndexVec     = std::vector<unsigned int>;
   using BeamSpotIndexMap = std::map<unsigned int, SpotIndexVec>;

   //! This class is for the spatial jamming beams that are part of each mode.
   class WSF_MIL_EXPORT JammerBeam
   {
   public:
      friend class WsfRF_Jammer::JammerMode;

      //! Type defines for managing of jammer spots.
      using SpotVec = std::vector<JammerXmtr*>;

      //! Type defines for managing of jammer spot track Ids.
      using SpotTrackIdVec = std::vector<WsfTrackId>;

      //! Construct a RF jammer beam
      JammerBeam() = delete;

      //! Copy construct a RF jammer mode
      JammerBeam(unsigned int aBeamNumber);

      JammerBeam(const JammerBeam& aSrc, unsigned int aBeamNumber);

      ~JammerBeam();

      //! @name Common infrastructure methods.
      //@{

      //! Initialize the jammer beam.
      //! @param aSimulation The simulation
      //! @param aXmtr The transmitter from which to clone all the available spots on the beam..
      //! @returns 'true' if successful or 'false' if some error was encountered.
      bool Initialize(WsfSimulation& aSimulation, JammerXmtr& aXmtr);

      //! Process input from a generic source.
      //! @param aInput The UtInput stream to process.
      //! @return Return success if input processed without errors
      bool ProcessInput(UtInput& aInput);
      //@}

      //! @name Beam methods.
      //@{

      //! Deselect the beam.
      //! @param aSimTime The current simulation time.
      void Deselect(double aSimTime);

      //! Select the beam.
      //! @param aSimTime The current simulation time.
      void Select(double aSimTime);

      //! Set the beam number.
      //! @param aBeamNumber The beam number to set.
      void SetBeamNumber(unsigned int aBeamNumber) { mBeamNumber = aBeamNumber; }

      //! Get the beam number.
      //! @return The beam number.
      unsigned int GetBeamNumber() const { return mBeamNumber; }
      //@}

      //! @name Spot methods.
      //@{

      //! Find the number of active spots on the beam.
      //! @return The number of active spots on the beam.
      unsigned int GetActiveSpotCount() const;

      //! Find the number of available spots on the beam.
      //! @return The number of available spots on the beam.
      unsigned int GetAvailableSpotCount() const;

      //! Returns true if a spot index is found at the supplied search
      //!   criteria (frequency, bandwidth, beam number, and/or target index).
      //! Passes by reference the spot index into mActiveSpotPtrs at which the supplied
      //!   search criteria is met, index also matches the mSpotTrackIds
      //!   as these are a maintained as matched pairs to mActiveSpotPtrs. If any of the
      //!   passed in search criteria variable are set to 0 it will not be used in the search.
      //! @param aFrequency    The adjusted jammer transmitter frequency. (Hertz)
      //! @param aBandwidth    The adjusted jammer transmitter bandwidth. (Hertz)
      //! @param aTrackId      The assigned Track Id of the target to be jammed.
      //! @param aRawTrackId   The assigned raw Track Id of the target to be jammed.
      //! @param aSpotIndexVec The spot index vector/list for which search criteria have been met.
      //! @return The request was successful.
      bool GetActiveSpotIndex(double        aFrequency,
                              double        aBandwidth,
                              WsfTrackId    aTrackId,
                              WsfTrackId    aRawTrackId,
                              SpotIndexVec& aSpotIndexVec) const;
      //@}

      //! Unique beam number for this mode.
      unsigned int mBeamNumber;

      //! Active jammer spot pointer list.
      SpotVec mActiveSpotPtrs;

      //! Active jammer spot pointer list.
      SpotVec mAvailableSpotPtrs;

      //! TrackId list for the active jammer spots.
      SpotTrackIdVec mSpotTrackIds;

      //! Maximum number of spots per beam for multiple beam jamming.
      //! Defaults to 1.
      unsigned int mMaxNumSpotsPerBeam;

      //! SpotPower distribution - average or constant - defaults to average.
      PowerDistribution mSpotPowerDistribution;
   };

   class WSF_MIL_EXPORT JammerMode : public WsfWeapon::WsfWeaponMode
   {
   public:
      //! map<BeamIndex, JammerBeamPointer>
      using BeamMap = std::map<size_t, JammerBeam*>;


      //! Need to track all of the active transmitters for DIS Emission PDU handling.
      using XmtrVec = std::vector<JammerXmtr*>;

      //! Construct RF Jammer
      JammerMode(const WsfScenario& aScenario);

      //! Copy Constructor
      JammerMode(const JammerMode& aSrc);

      //! Move operator
      JammerMode& operator=(const JammerMode&) = delete;

      ~JammerMode() override;

      //! @name Common infrastructure methods.
      //@{

      //! Clone this object.
      //! @return a pointer to the new object.
      WsfMode* Clone() const override;

      //! Initialize the jammer mode.
      //! @see WsfWeaponMode::!Initialize
      //! @param aSimTime   The current simulation time.
      //! @param aWeaponPtr The weapon associated to this mode
      //! @returns 'true' if successful or 'false' if some error was encountered.
      bool Initialize(double aSimTime, WsfWeapon* aWeaponPtr) override;

      //! Process input from a generic source.
      //! @see WsfWeaponMode::ProcessInput
      bool ProcessInput(UtInput& aInput) override;
      //@}

      //! Deselect the mode.
      //! @param aSimTime The current simulation time.
      void Deselect(double aSimTime) override;

      //! Select the mode.
      //! @param aSimTime The current simulation time.
      void Select(double aSimTime) override;

      //! Adjust the power for each active spot transmitter.
      //! The total power is stored in the master transmitter (mXmtr).
      //! @param aGroupPowerDistribution The power distribution of the groups (average, constant, etc.).
      //! @param aActiveGroupCount       The number of active groups in the jammer suite.
      void AdjustPowerDistribution(PowerDistribution aGroupPowerDistribution, unsigned int aActiveGroupCount);

      //! @name Beam methods and definitions
      //@{

      //! Find the number of active beams.
      //! @return The number of active beams.
      size_t GetActiveBeamCount() const;

      //! Find the number of available beams.
      //! @return The number of available beams.
      size_t GetAvailableBeamCount() const;

      //! Get the beam pointer for the specified beam number.
      //! @param aBeamNumber The assigned jammer transmitter Beam Number. (1 to maximum number of beams)
      //! @param aIsActive   Flag to provide the active status of the beam. Aids in determining which list the beam is in.
      //! @return Pointer to the beam, returns 0 if one is not found.
      JammerBeam* GetJammerBeam(size_t aBeamNumber, bool& aIsActive) const;

      //! Find and return beam indexes at referenced beam parameters.
      //! @param aFrequency        The assigned jammer transmitter frequency. (Hertz)
      //! @param aBandwidth        The assigned jammer transmitter bandwidth. (Hertz)
      //! @param aBeamNumber       The assigned jammer transmitter Beam Number. (0 to maximum number of beams)
      //! @param aTrackId          The assigned Track Id of the target to be jammed.
      //! @param aBeamSpotIndexMap The beam to spot index map for the active beams/spots list.
      //! @return The request was successful at the supplied search criteria (frequency,
      //!         bandwidth, beam number, and/or target index).
      bool GetActiveBeamSpotIndexes(double            aFrequency,
                                    double            aBandwidth,
                                    size_t            aBeamNumber,
                                    WsfTrackId        aTrackId,
                                    BeamSpotIndexMap& aBeamSpotIndexMap);

      //! Get the beam pointer for the specified beam number or select beam automatically (i.e. aBeamNumber = 0).
      //! @param aBeamNumber  The assigned jammer transmitter Beam Number. (1 to maximum number of beams)
      //! @param aIsActive    Flag to provide the active status of the beam. Aids in determining which list the beam is in.
      //! @return   Pointer to the beam, returns 0 if one is not found.
      JammerBeam* GetNextAssignableBeam(size_t aBeamNumber, bool& aIsActive) const;
      //@}

      //! @name Spot methods and definition for this mode
      //@{

      //! Get the total number of active spots for this mode
      //! @return Number of active spots for this mode.
      size_t GetActiveXmtrSpotCount() const { return mActiveXmtrSpotPtrs.size(); }

      //! Get the maximum number of available spots for this mode
      //! @return Number of available spots for this mode.
      unsigned int GetMaximumXmtrSpotCount() const { return mMaximumNumSpots; }

      //@}

      // TODO - make data private and protect copy constructor

      //! @name Master transmitter methods and definitions
      //@{

      //! Adjusts the input frequency and bandwidth if the jammer is not a barrage jammer and is in the frequency limits
      //!   of the master transmitter but the bandwidth is outside the frequency band of the master transmitter.
      //! @param aFrequency  The transmitter frequency that may need adjusted, modified by this method.
      //! @param aBandwidth  The transmitter bandwidth that may need adjusted, modified by this method.
      //! @return            Returns 'true' if the frequency and bandwidth required adjustment.
      bool AdjustFrequencyBandwidth(double& aFrequency, double& aBandwidth);

      //! Master Antenna, this mode.
      std::unique_ptr<WsfEM_Antenna> mAntenna;

      //! Master transmitter, this mode.
      std::unique_ptr<JammerXmtr> mXmtr;

      //! Master beam, this mode.
      std::unique_ptr<JammerBeam> mBeam;
      //@}

      //! @name Beam definitions
      //@{

      //! List of all active beams, this mode
      BeamMap mActiveBeamPtrs;

      //! List of all available beams, this mode
      BeamMap mAvailableBeamPtrs;

      //! Flag to indicate if a beam allocation change was made for the last [Start/Stop]Jamming call.
      bool mBeamAllocationChanged;

      //! Maximum number of beams for multi-beam jamming.
      //! Defaults to 1 (barrage jammer).
      unsigned int mMaxNumBeams;

      //! Beam power distribution - average or constant - defaults to average.
      PowerDistribution mBeamPowerDistribution;
      //@}

      //! @name Spot definitions
      //@{

      //! Maximum number of spots for this weapon.
      //! Includes all beams.
      unsigned int mMaximumNumSpots;

      //! Vector for the active jammer transmitters (i.e. spots).
      XmtrVec mActiveXmtrSpotPtrs;
      //@}

      //! @name Repeater definitions
      //@{
      //! Repeater logic, this mode
      WsfRF_Repeater mRepeater;

      //! Flag to specify if using a repeater, this mode.
      bool mUseRepeater;
      //@}
   };

protected:
   const TargetingDataMap& GetTargetDataMap() const { return mTargetDataMap; }

private:
   std::recursive_mutex& GetMutex() { return mMutex; }

   //! @name Private Jammer effect methods.
   //@{

   //! A internal routine to compute the effect of a given jammer on a given receiver.
   //! @param aSimTime                 [input]    The current simulation time.
   //! @param aXmtrPtr                 [input]    Pointer to the jammer transmitter.
   //! @param aTargetInteraction       [input]    The interaction object representing the interaction to be jammed
   //! (radio, radar, ESM, etc.).
   //!                                           This is used only to get the pointer to the receiver and it's current
   //!                                           pointing information.
   //! @param aJammerInteraction       [updated]  The interaction object to be used for the jamming interaction.
   //! @param aCalculateEW_Effects     [input]    If true, will calculate the electronic warfare effects that impinge
   //! upon the receiver.
   //! @param aIntermediateCalculation [input]    If true the interaction is an intermediate and certain message and
   //! calculations may be suppressed.
   //! @param aProcessingEW_EffectPtr  [optional] The currently processing or called from EW effect's pointer.
   static void ComputeJammerEffect(double                   aSimTime,
                                   WsfEM_Xmtr*              aXmtrPtr,
                                   const WsfEM_Interaction& aTargetInteraction,
                                   WsfEM_Interaction&       aJammerInteraction,
                                   bool                     aCalculateEW_Effects,
                                   bool                     aIntermediateCalculation,
                                   WsfEW_Effect*            aProcessingEW_EffectPtr = nullptr);
   //@}

   //! Selects the track or target and cues the jammer to the track or target.
   bool SelectTarget(double aSimTime, WsfEM_Xmtr* aXmtrPtr);

   //! Selects the target and cues the jammer to the target.
   bool ClearTarget(WsfEM_Xmtr* aXmtrPtr);

   //! Finds the target track by track Id from the available platform track processors (master and non-master).
   WsfLocalTrack* FindTargetTrack(const WsfTrackId& aTargetTrackId) const;

   //! @name Private repeater mode methods.
   //@{
   void SensorTrackDropped(double aSimTime, WsfTrackId aTrackId, size_t aTargetIndex);
   void SensorTrackInitiated(double aSimTime, const WsfTrack* aTrackPtr);
   void SensorTrackUpdated(double aSimTime, const WsfTrack* aTrackPtr);

   //! Checks for the use of a repeater then updates the repeater detection attempt and any other necessary.
   static bool UpdateRepeaterResult(double                   aSimTime,
                                    WsfEM_Xmtr*              aXmtrPtr,
                                    WsfSensorResult&         aResult,
                                    WsfEM_Interaction&       aJammerInteraction,
                                    const WsfEM_Interaction& aTargetInteraction);
   //@}

   //! @name Jammer group methods and definitions
   //@{

   //! Initialize the jammer groupings with other jammers on the same platform defined to be in same group.
   void InitializeJammerGroup();

   //! Returns the total number of active jammer weapons in group
   //! @note Since the return count includes this jammer it will always return at least 1.
   //! @return Returns the number of active jammer weapons in the group including current jammer.
   unsigned int GetActiveGroupCount() const;

   //! Pointers to other jammer on the same platform in the same group
   using JammerGroup = std::vector<WsfWeapon*>;

   //! List of all the jammers contained in the same group as this jammer
   JammerGroup mJammerGroup;

   //! The group to which this jammer belong; gets added as a category as well
   WsfStringId mJammerGroupNameId;

   //! Group power distribution - average or constant - defaults to average.
   PowerDistribution mGroupPowerDistribution;

   //@}

   //! The jammer-specific list of modes (not valid until Initialize is called)
   std::vector<JammerMode*> mJammerModeList;

   //! True after jammer is initialized. Required for DIS platform construction.
   bool mJammerInitialized;

   //! Map of the target data for maintaining target information for jamming assignments.
   TargetingDataMap mTargetDataMap;

   mutable std::recursive_mutex mMutex;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfRF_Jammer, cWSF_COMPONENT_RF_JAMMER)

#endif
