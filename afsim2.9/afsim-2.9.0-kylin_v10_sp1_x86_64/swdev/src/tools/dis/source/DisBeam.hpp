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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

// DIS Emitter Beam

#ifndef DISBEAM_HPP
#define DISBEAM_HPP

#include "dis_export.h"

#include <deque>
#include <iomanip>
#include <list>
#include <set>
#include <string>
#include <vector>

#include "DisBeamDataRecord.hpp"
#include "DisJammingTechniqueRecord.hpp"
#include "DisPtrContainer.hpp"
#include "DisTrackJam.hpp"
#include "DisTypes.hpp"

class DisEmission;
class DisSystem;

class GenI;
class GenO;

/*!
 * This class encapsulates all the data for an active beam in a DisEmission PDU.
 *
 * NOTE: This class encapsulates a DisBeamDataRecord class and is not a formal "type" in the specification
 *       but the necessary information about the information contained in this class can be found at page
 *       113 of the "IEEE 1278.1-2012.pdf" document
 */
class DIS_EXPORT DisBeam
{
public:
   //! Default Constructor
   DisBeam();
   //! Copy Constructor
   DisBeam(const DisBeam& aSrc);
   //! Destructor
   ~DisBeam() = default;

   // Accessors

   //! Beam Data Length (# of 32-bit words)
   /*
    *  If the length of this beam's data (including the Beam Data Length
    *  field and track/jam information) does not exceed 1020 octets (255,
    *  32-bit words), then this field shall specify the length of this
    *  beam's data in 32-bit words.  Otherwise, the size of the beam is
    *  considered large and this field shall be set to zero.
    *
    *  /note It is possible for a beam to have more than 1020 octets!
    *        The maximum size of a beam is 2092 octets.
    *        (52 octets for the beam + 255 Targets * 8 octets / target)
    *  /note The Beam Data Length will be 0 and the System that contains this
    *        beam will have a Data Length of 0 as well.  The number of Targets
    *        should be limited by EE_HIGH_DENSITY_THRSH.  The maximum
    *        PDU size of MAX_PDU_SIZE_OCTETS.
    *  /note EE_HIGH_DENSITY_THRSH is determined by exercise agreement, but
    *        defaults to 10.
    *  /note MAX_PDU_SIZE_OCTETS is determined by exercise agreement, but
    *        defaults to 8192.
    */
   DisUint8 GetDataLength() const;
   //! Reported Beam Data Length (# of 32-bit words)
   /*!
    *  If this DisBeam was read in from a buffer, this function returns
    *  the DataLength reported within that buffer.  If this DisBeam was
    *  not read from a buffer, then the value will be either 0 or the value
    *  from the previous read from a buffer.
    *  \note
    *     This value is not the actual DataLength that would be sent if packed
    *     into a PDU.  If you desire the actual value, use GetDataLength()
    *     instead.  If you want an unbounded length of this Beam (not 0 if
    *     the beam is larger than 1020 octets, call GetLengthOctets()
    */
   DisUint8 GetReportedDataLength() const;
   //! Returns the actual size of this beam in octets
   /*!
    *  \return the actual number of octets this beam will use when packing into
    *          a GenO buffer.
    */
   DisUint16 GetLengthOctets() const;

   //! Beam Number (unique ID)
   /*
    *  This field shall specify a unique number assigned to differentiate
    *  between otherwise similar or identical emitter beams within an emitter
    *  system.
    *
    *  \note No beam shall be assigned a number containing NO_BEAM or ALL_BEAMS.
    *        Beam numbers for active beams shall not be changed during an
    *        exercise.
    */
   DisUint8 GetNumber() const;
   //! Beam Parameter Index (part of database key)
   /*!
    *  Used in conjunction with the Emitter Name field as a database primary
    *  key, this field shall specify a number by which receiving entities
    *  reference stored database parameters required to regenerate the beam.
    */
   DisUint16 GetParameterIndex() const;

   // EE Fundamental Parameter Data Record
   //    The EE Fundamental Parameter Data Record contains electromagnetic
   //    emission regeneration parameters that are variable throughout a
   //    scenario dependent on the actions of the participants in the
   //    simulation.  This record also provides basic parametric data that
   //    may be used to support low-fidelity simulations that do not have the
   //    processing capability to model a high-fidelity regeneration of emission
   //    beams.

   //! Frequency (Hertz)
   /*!
    *  For non-frequency agile emissions, this field shall specify the
    *  average center frequency in Hertz.  For a frequency-agile emission,
    *  this field shall specify the middle of the range of center
    *  frequencies:
    *     F = (fu + fl) / 2
    *  where:
    *     F   is the Frequency field value
    *     fu  is the upper limit of center frequencies
    *     fl  is the lower limit of center frequencies
    *  Frequency modulation for a particular emitter beam and mode shall be
    *  derived from database parameters stored in the receiving entity.
    *
    *  \note  If this beam is inactive, a frequency of 0.0 Hz will be reported.
    */
   DisFloat32 GetFrequency() const;
   //! Frequency Range (Hertz)
   /*!
    *  This field shall specify the difference between the upper and lower
    *  limit of center frequencies corresponding to the Frequency Field.
    *  Thus, for non-frequency-agile emissionis, the frequency range shall be
    *  zero.  For frequency-agile emissions, it shall indicate the range of
    *  agile radio frequencies:
    *     R = fu - fl
    *  where:
    *     R   is the Frequency Range field value
    *     fu  is the upper limit of center frequencies
    *     fl  is the lower limit of center frequencies
    *  The actual list of center frequencies for a particular emitter beam
    *  and mode shall be derived from database parameters stored in the
    *  receiving entity.
    *
    *  \note  If this beam is inactive, a frequency range of 0.0 Hz will be
    *         reported.
    */
   DisFloat32 GetFrequencyRange() const;
   //! Effective Radiated Power (ERP) (dBm)
   /*!
    *  This field shall specify the average effective radiated power for the
    *  emission in dBm.  For a radar or jammer, this field shall indicate the
    *  peak of the transmitted power.  Thus, it includes peak transmitter
    *  power, transmission line losses, and peak of the antenna gain.
    *  Effective radiated Power modulation for a particular emitter beam and
    *  mode shall be derived from database parameters stored in the receiving
    *  entity.
    *
    *  \note  If this beam is inactive, an ERP of 0.0 dBm will be reported.
    */
   DisFloat32 GetEffectiveRadiatedPower() const;
   //! Pulse Repetition Frequency (PRF) (Hertz)
   /*!
    *  This field shall specify the average pulse repetition frequency of the
    *  emission in Hertz.  Pulse Repetition Frequency modulation for a
    *  particular emitter beam and mode shall be derived from database
    *  parameters stored in the receiving entity.
    *
    *  \note  If this beam is inactive, a PRF of 0.0 Hz will be reported.
    *
    *  \note  Continuous wave emissions have a 0.0 for this value and for Pulse
    *         Width.
    */
   DisFloat32 GetPulseRepetitionFrequency() const;
   //! Pulse Width (microseconds)
   /*!
    *  This field shall specify the average pulse width of the emission in
    *  microseconds.  Pulse modulation for a particular emitter beam and mode
    *  shall be derived from database parameters stored in the receiving
    *  entity.
    *
    *  \note  If this beam is inactive, a pulse width 0 usec will be reported.
    *
    *  \note  Continuous wave emissions have a 0.0 for this value and for Pulse
    *         Repetition Frequency
    */
   DisFloat32 GetPulseWidth() const;

   // Beam Data Record
   //    The specification of beam-specific data necessary to describe the scan
   //    volume of an emitter beam.
   const DisBeamDataRecord& GetBeamDataRecord() const;

   //! Pass through to mBeamDataRecord.GetAzimuthCenter()
   DisFloat32 GetAzimuthCenter() const;
   //! Pass through to mBeamDataRecord.GetAzimuthSweep()
   DisFloat32 GetAzimuthSweep() const;
   //! Pass through to mBeamDataRecord.GetElevationCenter()
   DisFloat32 GetElevationCenter() const;
   //! Pass through to mBeamDataRecord.GetElevationSweep()
   DisFloat32 GetElevationSweep() const;
   //! Pass through to mBeamDataRecord.GetSweepSync()
   DisFloat32 GetSweepSync() const;

   //! Beam Function (enumeration)
   /*!
    *  This field shall specify the intended use of a particular beam.
    *  Typical functions include search, acquisition, tracking, illumination,
    *  jamming, and so on.  This field is intended to help receiving entities
    *  determine the emission mode represented by the beam.
    */
   DisEnum8 GetFunction() const;
   //! Number of Targets
   /*!
    *  This field, in conjunction with the High-Density Track/Jam field,
    *  shall identify, for the current PDU and emitter beam, the number of
    *  entities tracked or under illumination (as appropriate for an emitter
    *  beam's function) or the number of targeted emitter beams (for
    *  jammers) that will be sent when this PDU is packed.
    *  \note This value is the number of targets to be published.
    *        The value will be 0 if the beam is inactive, High Density Track/Jam
    *        is selected, or the number of targets exceeds
    *        EE_HIGH_DENSITY_THRSH.
    *  \note If you wish to know the number of targets stored in this class,
    *        use GetTarget[Deque|List|Set|Vector]().size();
    */
   DisUint8 GetNumberOfTargets() const;
   //! Reported Number of Targets
   /*!
    *  If this DisBeam was read in from a buffer, this function returns
    *  the Number of Targets reported within that buffer.  If this DisBeam
    *  was not read from a buffer, then the value will be either 0 or the
    *  value from the previous read from a buffer.
    *  \note
    *     This value is not the actual Number of Targets, if you desire the
    *     actual value, use GetNumberOfTargets() instead
    */
   DisUint8 GetReportedNumberOfTargets() const;
   //! High-Density Track/Jam (enum)
   /*!
    *  This field shall be used to indicate that receiving simulation
    *  applications can assume that all viable targets in the field of regard
    *  specified by the beam data are being tracked or jammed.
    *  \note This is automatically set to "Selected" if the number of targets
    *        exceeds EE_HIGH_DENSITY_THRSH.  If the number of targets drops
    *        below the threshold, the value will revert to "Not-Selected" unless
    *        manually set to "Selected".
    */
   DisEnum8 GetHighDensityTrackJam() const;
   //! Beam Status Record (enumerated bits)
   /*!
    *  This field shall be used to indicate the status of the beam (e.g. the
    *  beam is active or deactivated).
    *  |-------------|--------|-------------------|
    *  | Field Name  |   Bit  |    Data Type      |
    *  |-------------|--------|-------------------|
    *  | Beam State  |    0   | 1-bit enumeration |
    *  |-------------|--------|-------------------|
    *  | Padding     | 1 to 7 | 7 bits unused     |
    *  |-------------|--------|-------------------|
    *
    */
   DisEnum8 GetStatus() const;
   //! Helper function to determine if this beam is active or not.
   bool IsActive() const;
   //! Jamming Technique Record
   /*!
    *  This field shall be used to identify the jamming method or methods.
    */
   const DisJammingTechniqueRecord& GetJammingTechniqueRecord() const;

   // Mutators

   void SetNumber(DisUint8 aNumber);
   void SetParameterIndex(DisUint16 aIndex);
   void SetFrequency(DisFloat32 aFrequency);
   void SetFrequencyRange(DisFloat32 aFrequencyRange);
   void SetEffectiveRadiatedPower(DisFloat32 aEffectiveRadiatedPower);
   void SetPulseRepetitionFrequency(DisFloat32 aPulseRepetitionFreqency);
   void SetPulseWidth(DisFloat32 aPulseWidth);
   void SetBeamDataRecord(const DisBeamDataRecord& aBdr);
   void SetAzimuthCenter(DisFloat32 aAzimuthCenter);
   void SetAzimuthSweep(DisFloat32 aAzimuthSweep);
   void SetElevationCenter(DisFloat32 aElevationCenter);
   void SetElevationSweep(DisFloat32 aElevationSweep);
   void SetSweepSync(DisFloat32 aSweepSync);
   void SetFunction(DisEnum8 aFunction);
   //! Set HighDensityTrackJam status.
   /*!
    *  \param[in] aHighDensityTrackJam Set new default value
    *  \note If this function sets the state to "Selected", then targets in the
    *        Track/Jam list will never be published.  If this method is used to
    *        set the value to "Not-Selected", the value of the
    *        HighDensityTrackJam flag will be automatically determined by the
    *        state of the beam (Active|Inactive) and the number of targets in
    *        the Track/Jam list compared to EE_HIGH_DENSITY_THRSH.
    */
   void SetHighDensityTrackJam(DisEnum8 aHighDensityTrackJam);
   //! Set Beam Status
   /*!
    *  \param[in] aStatus New Beam Status (See GetStatus for valid values)
    *  \note If aStatus sets this beam inactive, the TargetList will be flushed.
    */
   void SetStatus(DisEnum8 aStatus);
   void SetJammingTechniqueRecord(const DisJammingTechniqueRecord& aJtr);

   // Input/output
   void        Get(GenI& aGenI);
   void        Put(GenO& aGenO) const;
   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Data Validation
   bool IsValid() const;

   // The following gets the length read in octets for this beam as,
   // read by the Get() method.  The length includes the base Beam size
   // plus the contents of the Track/Jam fields
   DisUint16 GetLengthRead() const;

   // Track/Jam list methods
   //! Return a deque of all current Targets of this beam
   /*!
    *  \note Thread Safe
    */
   std::deque<DisTrackJam> GetTargetDeque() const;

   //! Return a list of all current Targets of this beam
   /*!
    *  \note Thread Safe
    */
   std::list<DisTrackJam> GetTargetList() const;

   //! Return a set of all current Targets of this beam
   /*!
    *  \note Thread Safe
    */
   std::set<DisTrackJam> GetTargetSet() const;

   //! Return a vector of all current Targets of this beam
   /*!
    *  \note Thread Safe
    */
   std::vector<DisTrackJam> GetTargetVector() const;

   //! Return a set of all current Targets of this beam (Non-Thread Safe)
   /*!
    *  This method is effectively the same as GetTargetSet, but returns a
    *  const reference to the Set instead of a copy of the set.  This method
    *  should only be used if the caller can guarantee no other thread will be
    *  modifying the set. (Multiple threads only reading will be safe).
    */
   const std::set<DisTrackJam>& GetTargetSetNTS() const;

   //! Return a set of all current Targets of this beam (Non-Thread Safe)
   /*!
    *  This method is effectively the same as GetTargetSet, but returns a
    *  reference to the Set instead of a copy of the set.  This method
    *  should only be used if the caller can guarantee no other thread will be
    *  accessing the set (reading or writing).
    */
   std::set<DisTrackJam>& GetTargetSetNTS();

   //! Adds a target to the Track/Jam List
   /*!
    *  \note The target will not be sent if the target list grows above
    *        EE_HIGH_DENSITY_THRSH, but will be stored in case other targets
    *        are removed and take the total count to within the threshold.
    *  \note If the beam status is set to Inactive, AddTarget will do nothing.
    */
   void AddTarget(const DisTrackJam& aTarget);

   //! Removes a target from the Track/Jam List
   void RemoveTarget(const DisTrackJam& aTarget);

   //! Clears out all targets in the Track/Jam List
   void RemoveAllTargets();

   //! Evaluates if another Track/Jam may be added to this beam.
   /*!
    *  The size limits considered are:
    *  o  Will adding another Track/Jam increase the count to greater than
    *     EE_HIGH_DENSITY_THRSH.
    *  o  Will adding another Track/Jam to this beam exceed the size
    *     MAX_PDU_SIZE_OCTETS.
    *     o  If this beam is contained within a System, and that System is
    *        contained within an Emissions PDU, then the total size of all
    *        systems and beams are used in computing the total size.
    *     o  If this beam is contained within a System, but that System is not
    *        contained within an Emissions PDU, then the total size of the
    *        system plus all of it's existing beams is used, with the assumption
    *        that this system will be the only system in the Emissions PDU.
    *  \param[in] aSystemPtr    If not null, overrides this beam's parent system
    *                           with aSystemPtr, and assumes this beam is not
    *                           within aSystemPtr.
    *  \param[in] aEmissionPtr  If not null, overrides this beam's emission pdu
    *                           with aEmissionPtr, and assumes this beam's
    *                           system is not within aEmissionPdu.
    */
   bool CanAddTrackJamEntry(const DisSystem* aSystemPtr = nullptr, const DisEmission* aEmissionPtr = nullptr) const;

   //! Set a pointer to a parent system that holds this beam.
   void SetParentSystem(const DisSystem* aSystem);

private:
   // Disallow assignment
   DisBeam& operator=(const DisBeam& aRhs);

   /*!
    * Specifies the length of emitter system data
    */
   DisUint8 mReportedDataLength;
   /*!
    * Specifies the number of beams described in the emitter system data section
    */
   DisUint8 mNumber;
   /*!
    * Indicates which receivers reference detailed local database parameters
    */
   DisUint16 mParameterIndex;
   // The following parameters are for the fundamental parametric data
   /*!
    * For non-frequency-agile emissions: Specifies the average center frequency in Hz
    * For frequency-agile emissions: Specifies the middle of the range of center frequencies
    */
   DisFloat32 mFrequency;
   /*!
    * Specifies the difference between upper and lower limit of center frequencies corresponding
    * to the frequency field.
    *
    * For non-frequency-agile emissions: frequency range is zero
    * For frequency-agile emissions: indicates the range of agile radio frequencies
    */
   DisFloat32 mFrequencyRange;
   /*!
    * Specifies the average effective radiated power for the emission in dBm.
    */
   DisFloat32 mEffectiveRadiatedPower;
   /*!
    * Specifies the average pulse repetition frequency of the emission in Hz. Pulse
    * repetition frequency and modulation for a particular emitter beam and mode are
    * derived from database parameters receiving entity.
    */
   DisFloat32 mPulseRepetitionFrequency;
   /*!
    * Specifies the average pulse width of the emission in microseconds. Pulse width
    * modulation for a particular emitter beam and mode is derived from database parameters
    * stored in the receiving entity.
    */
   DisFloat32 mPulseWidth;
   /*!
    * Specifies the parameters of the beam data.
    */
   DisBeamDataRecord mBeamDataRecord;
   /*!
    * Specifies the intended use of a particular beam. Typical functions include
    * search, acquisition, tracking, illumination, jamming, etc. This field is
    * intended to help receiving entities determine the emissions mode represented
    * by the beam.
    */
   DisEnum8 mFunction;
   /*!
    * Identifies (in conjunction with the High-Density Track/Jam field) for the current
    * emitter beam and PDU the number of entities tracked or under illumination (as
    * appropriate for an emitter's beam function) or the number of targeted emitter beams
    * for jammers.
    */
   DisUint8 mReportedNumberOfTargets;
   /*!
    * Indicates that receiving simulation applications can assume that all viable targets in the
    * field of regard specified by the beam data are being tracked or jammed.
    */
   DisEnum8 mHighDensityTrackJam;
   /*!
    * Indicates the status of the beam (active or deactivated)
    */
   DisEnum8 mStatus;
   /*!
    * Identifies the jamming method or methods
    */
   DisJammingTechniqueRecord mJammingTechniqueRecord;
   /*!
    * Set of targets associated with this object
    */
   std::set<DisTrackJam>        mTargetSet;
   DisPtrContainer<DisTrackJam> mTrackJamList; //!< Don't use this

   DisUint16        mLengthRead;
   const DisSystem* mParentSystem;
};

// Accessors

inline DisUint8 DisBeam::GetReportedDataLength() const
{
   return mReportedDataLength;
}
inline DisUint8 DisBeam::GetNumber() const
{
   return mNumber;
}
inline DisUint16 DisBeam::GetParameterIndex() const
{
   return mParameterIndex;
}
inline DisFloat32 DisBeam::GetFrequency() const
{
   return mFrequency;
}
inline DisFloat32 DisBeam::GetFrequencyRange() const
{
   return mFrequencyRange;
}
inline DisFloat32 DisBeam::GetEffectiveRadiatedPower() const
{
   return mEffectiveRadiatedPower;
}
inline DisFloat32 DisBeam::GetPulseRepetitionFrequency() const
{
   return mPulseRepetitionFrequency;
}
inline DisFloat32 DisBeam::GetPulseWidth() const
{
   return mPulseWidth;
}
inline const DisBeamDataRecord& DisBeam::GetBeamDataRecord() const
{
   return mBeamDataRecord;
}
inline DisFloat32 DisBeam::GetAzimuthCenter() const
{
   return mBeamDataRecord.GetAzimuthCenter();
}
inline DisFloat32 DisBeam::GetAzimuthSweep() const
{
   return mBeamDataRecord.GetAzimuthSweep();
}
inline DisFloat32 DisBeam::GetElevationCenter() const
{
   return mBeamDataRecord.GetElevationCenter();
}
inline DisFloat32 DisBeam::GetElevationSweep() const
{
   return mBeamDataRecord.GetElevationSweep();
}
inline DisFloat32 DisBeam::GetSweepSync() const
{
   return mBeamDataRecord.GetSweepSync();
}
inline DisEnum8 DisBeam::GetFunction() const
{
   return mFunction;
}
inline DisUint8 DisBeam::GetReportedNumberOfTargets() const
{
   return mReportedNumberOfTargets;
}
inline DisEnum8 DisBeam::GetStatus() const
{
   return mStatus;
}
inline const DisJammingTechniqueRecord& DisBeam::GetJammingTechniqueRecord() const
{
   return mJammingTechniqueRecord;
}

inline DisUint16 DisBeam::GetLengthRead() const
{
   return mLengthRead;
}

inline std::deque<DisTrackJam> DisBeam::GetTargetDeque() const
{
   return std::deque<DisTrackJam>(mTargetSet.begin(), mTargetSet.end());
}

inline std::list<DisTrackJam> DisBeam::GetTargetList() const
{
   return std::list<DisTrackJam>(mTargetSet.begin(), mTargetSet.end());
}

inline std::set<DisTrackJam> DisBeam::GetTargetSet() const
{
   return mTargetSet;
}

inline std::vector<DisTrackJam> DisBeam::GetTargetVector() const
{
   return std::vector<DisTrackJam>(mTargetSet.begin(), mTargetSet.end());
}

inline const std::set<DisTrackJam>& DisBeam::GetTargetSetNTS() const
{
   return mTargetSet;
}

inline std::set<DisTrackJam>& DisBeam::GetTargetSetNTS()
{
   return mTargetSet;
}

// Mutators
inline void DisBeam::SetNumber(DisUint8 aNumber)
{
   mNumber = aNumber;
}
inline void DisBeam::SetParameterIndex(DisUint16 aIndex)
{
   mParameterIndex = aIndex;
}
inline void DisBeam::SetFrequency(DisFloat32 aFrequency)
{
   mFrequency = aFrequency;
}
inline void DisBeam::SetFrequencyRange(DisFloat32 aFrequencyRange)
{
   mFrequencyRange = aFrequencyRange;
}
inline void DisBeam::SetEffectiveRadiatedPower(DisFloat32 aERP)
{
   mEffectiveRadiatedPower = aERP;
}
inline void DisBeam::SetPulseRepetitionFrequency(DisFloat32 aPRF)
{
   mPulseRepetitionFrequency = aPRF;
}
inline void DisBeam::SetPulseWidth(DisFloat32 aPulseWidth)
{
   mPulseWidth = aPulseWidth;
}
inline void DisBeam::SetBeamDataRecord(const DisBeamDataRecord& aBdr)
{
   mBeamDataRecord = aBdr;
}
inline void DisBeam::SetAzimuthCenter(DisFloat32 aAzimuthCenter)
{
   mBeamDataRecord.SetAzimuthCenter(aAzimuthCenter);
}
inline void DisBeam::SetAzimuthSweep(DisFloat32 aAzimuthSweep)
{
   mBeamDataRecord.SetAzimuthSweep(aAzimuthSweep);
}
inline void DisBeam::SetElevationCenter(DisFloat32 aElevationCenter)
{
   mBeamDataRecord.SetElevationCenter(aElevationCenter);
}
inline void DisBeam::SetElevationSweep(DisFloat32 aElevationSweep)
{
   mBeamDataRecord.SetElevationSweep(aElevationSweep);
}
inline void DisBeam::SetSweepSync(DisFloat32 aSweepSync)
{
   mBeamDataRecord.SetSweepSync(aSweepSync);
}
inline void DisBeam::SetFunction(DisEnum8 aFunction)
{
   mFunction = aFunction;
}
inline void DisBeam::SetHighDensityTrackJam(DisEnum8 aHighDensityTrackJam)
{
   mHighDensityTrackJam = aHighDensityTrackJam;
}
inline void DisBeam::SetJammingTechniqueRecord(const DisJammingTechniqueRecord& aJtr)
{
   mJammingTechniqueRecord = aJtr;
}
inline void DisBeam::SetParentSystem(const DisSystem* aSystem)
{
   mParentSystem = aSystem;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, DisBeam& aBeam)
{
   aBeam.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisBeam& aBeam)
{
   aBeam.Get(aGenI);
   return aGenI;
}

inline void DisBeam::AddTarget(const DisTrackJam& aTarget)
{
   if (IsActive())
   {
      mTargetSet.insert(aTarget);
   }
}

inline void DisBeam::RemoveTarget(const DisTrackJam& aTarget)
{
   mTargetSet.erase(aTarget);
}

inline void DisBeam::RemoveAllTargets()
{
   mTargetSet.clear();
}

#endif
