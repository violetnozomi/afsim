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

// DIS Emitter System

#ifndef DISSYSTEM_HPP
#define DISSYSTEM_HPP

#include "dis_export.h"

#include "DisBeam.hpp"
#include "DisPtrContainer.hpp"
#include "DisTypes.hpp"

class DisEmission;

class GenI;
class GenO;

//! Enapsulates information for an emitter system: The lenth, number of beams, beam parameters etc.
class DIS_EXPORT DisSystem
{
public:
   //! Default Constructor
   DisSystem();
   //! Copy Constructor
   DisSystem(const DisSystem& aSrc);
   //! Destructor
   virtual ~DisSystem() {}

   // Accessors
   //! System Data Length (# of 32-bit words)
   /*!
    *  If the length of this emitter system's data (including the System Data
    *  Length field and beam and track/jam information) does not exceed 1020
    *  octets (255, 32-bit words), then this field shall specify the length of
    *  this emitter system's data in 32-bit words.  Otherwise, the size of the
    *  emitter system is considered large and this field shall be set to zero.
    *
    *  /note It is possible for a system to have more than 1020 octets!
    *        The theoretical maximum size of a system is bounded by the number
    *        of beams with the number of track/jam targets.  255 beams with 255
    *        targets each would yield a system size of 533,480 octets.
    *        (20 octets for the base system, plus 2092 octets per beam * 255)
    *  /note The actual maximum System Data Length is bounded by the exercise
    *        agreed upon limit of Targets per beam (EE_HIGH_DENSITY_THRSH), and
    *        the maximum size of a PDU (MAX_PDU_SIZE_OCTETS)
    *  /note EE_MAX_BEAMS_THRSH is determined by exercise agreement, but
    *        defaults to 255.
    *  /note EE_HIGH_DENSITY_THRSH is determined by exercise agreement, but
    *        defaults to 10.
    *  /note MAX_PDU_SIZE_OCTETS is determined by exercise agreement, but
    *        defaults to 8192.
    */
   DisUint8 GetDataLength() const;
   //! Reported System Data Length (# of 32-bit words)
   /*!
    *  If this DisSystem was read in from a buffer, this function returns
    *  the DataLength reported within that buffer.  If this DisBeam was
    *  not read from a buffer, then the value will be either 0 or the value
    *  from the previous read from a buffer.
    *  \note
    *     This value is not the actual DataLength, if you desire the actual
    *     value, use GetDataLength() instead
    */
   DisUint8 GetReportedDataLength() const;
   //! Returns the actual size of this system in octets
   /*!
    *  \return the actual number of octets this beam will use when packing into
    *          a GenO buffer.
    */
   DisUint16 GetLengthOctets() const;

   //! Returns the number of beams associated with this system.
   /*!
    *  This field shall specify the number of beams being described in the
    *  current PDU for the emitter system being described.
    */
   DisUint8 GetNumberOfBeams() const;
   //! Reported Number of Beams
   /*!
    *  If this DisSystem was read in from a buffer, this function returns
    *  the Number of Beams reported within that buffer.  If this DisSystem
    *  was not read from a buffer, then the value will be either 0 or the
    *  value from the previous read from a buffer.
    *  \note
    *     This value is not the actual Number of Beams, if you desire the
    *     actual value, use GetNumberOfBeams() instead
    */
   DisUint8 GetReportedNumberOfBeams() const;

   // Emitter System Record
   /*
    * Information about a particular emitter shall be represented using an
    * emitter system record.
    */

   //! Emitter Name (enumeration)
   /*!
    *  This field shall specify the nomenclature for a particular emitter.
    *
    *  \note Once established for an exercise, the Emitter Name for each emitter
    *        system shall not be changed during that exercise
    */
   DisEnum16 GetName() const;
   //! Emitter Function (enumeration)
   /*!
    *  This field shall specify the general function associated with a
    *  particular emitter.  Typical functions include airborne fire control,
    *  ground surveillance radar, and so on.  This field is intended to help
    *  receiving entities determine whether the EE PDU is of interest to the
    *  systems simulated by that entity.
    *
    *  \note Once established for an exercise, the Emitter Function for each
    *        emitter system shall not be changed during that exercise.
    */
   DisUint8 GetFunction() const;
   //! Emitter Number (unique ID)
   /*!
    *  This field shall specify the specific emitter system associated with an
    *  entity.  This field allows the differentiation of multiple emitter
    *  systems on an entity even if in some instances two or more of the
    *  emitters are of identical emitter types.
    *
    *  \note No emitter system shall be assigned a number containing NO_EMITTER
    *        or ALL_EMITTERS.  Once established for an exercise, the Emitter
    *        Number for each emitter system shall not be changed during that
    *        exercise.  Each Emitter Number shall be unique for a given entity.
    */
   DisUint8 GetNumber() const;
   //! Emitter Installation Location (Entity Coordinate Vector - meters)
   /*!
    *  This field shall specify the location of the antenna beam source with
    *  respect to the emitting entity's coordinate system.  This location shall
    *  be the origin of the emitter coordinate system that shall have the same
    *  orientation as the entity coordinate system.
    */
   void GetLocation(DisFloat32& aLocationX, DisFloat32& aLocationY, DisFloat32& aLocationZ) const;

   // Mutators
   void SetName(DisEnum16 aName);
   void SetFunction(DisUint8 aSystemFunction);
   void SetNumber(DisUint8 aNumber);
   void SetLocation(DisFloat32 aLocationX, DisFloat32 aLocationY, DisFloat32 aLocationZ);

   // Input/output
   void        Get(GenI& aGenI);
   void        Put(GenO& aGenO) const;
   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Data Validation
   bool IsValid() const;

   // The following gets the length as read from the header, not the
   // length computed by adding the length of the base data plus the
   // length required for the specified number of Track/Jam entries.
   DisUint16 GetLengthRead() const;

   // Beam list maintenance...

   //! Evaluates if another beam may be added to this beam.
   /*!
    *  The size limits considered are:
    *  o  Will adding another Beam to this emitter system exceed the size
    *     MAX_PDU_SIZE_OCTETS.
    *     o  If this emitter system is contained within an Emissions PDU, then
    *        the total size of all systems and beams are used in computing the
    *        total size.
    *     o  If this emitter system is not contained within an Emissions PDU,
    *        then the total size of the system plus all of it's existing beams
    *        is used, with the assumption that this emitter system will be the
    *        only system within an Emissions PDU.
    *  \param[in] aBeamPtr      A beam to consider adding.  If null, a best case
    *                           assumption of a beam with 0 targets will be
    *                           made.
    *  \param[in] aEmissionPtr  If not null, overrides this emitter system's
    *                           Emission PDU with aEmissionPtr, assuming this
    *                           emitter system is not already within
    *                           aEmissionPtr.
    */
   bool CanAddBeam(const DisBeam* aBeamPtr, const DisEmission* aEmissionPtr = nullptr) const;

   //! Add a Beam to this emitter system (if possible)
   /*!
    *  If room is available, add the beam to the beam list.
    *  If the PDU has no room, the beam will not be added.
    *
    *  \return true if the beam was added.
    *
    *  \warn AddBeam takes ownership of the passed object.
    */
   bool AddBeam(DisBeam* aBeam);

   // Return iterators to DisBeam vector.
   std::vector<DisBeam*>::iterator       GetBeamBegin();
   std::vector<DisBeam*>::const_iterator GetBeamBegin() const;
   std::vector<DisBeam*>::iterator       GetBeamEnd();
   std::vector<DisBeam*>::const_iterator GetBeamEnd() const;

   // Remove and delete a DisBeam object from the list.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first Beam on the list.
   unsigned int RemoveBeam(DisBeam* aBeam);

   // Remove and delete all DisBeam objects from the list.
   void RemoveAllBeams();

   // Remove DisBeam object from the list, but DO NOT DELETE the object.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first Beam on the list.
   unsigned int RemoveBeamWithNoDelete(DisBeam* aBeam);

   // Remove all DisBeam objects from the list, but DO NOT DELETE them.
   void RemoveAllBeamsWithNoDelete();

   //! Returns a pointer to the Emissions PDU that holds this emitter system.
   const DisEmission* GetParentEmission() const;

   //! Set a pointer to a parent Emission PDU that holds this emitter system.
   void SetParentEmission(const DisEmission* aEmission);

private:
   // Disallow assignment
   DisSystem& operator=(const DisSystem& aRhs);

   /*!
    * The length of the emitter system's data
    */
   DisUint8 mReportedDataLength;
   /*!
    * The number of beams being described in the current (owning) PDU
    */
   DisUint8 mReportedNumberOfBeams;
   // The following paramters are for a Emitter system record
   /*!
    * Specifies the nomenclature for a particular emitter
    */
   DisEnum16 mName;
   /*!
    * Specifies the general function associated with the particular emitter.
    * Typical functions include: airborne fire control, ground surveillance radar, etc.
    * This field is intended to help receiving entities determine whether the EE PDU is
    * of interest to the systems simulated by that entity.
    */
   DisEnum8 mFunction;
   /*!
    * Specifies the emitter system associated with an entity. This field allows the
    * differentiation of multiple emitter systems on an entity even if in some instances
    * two or more of the emitters are of identical emitter types.
    */
   DisUint8 mNumber;
   /*!
    * Specifies the location of the antenna beam source with respect to the emitting entity's
    * coordinate system. This location is the origin of the emitter coordinate system and has
    * orientation as the entity's coordinate system.
    */
   DisFloat32 mLocation[3];
   /*!
    * Container for all the beam objects
    */
   DisPtrContainer<DisBeam> mBeamList;

   DisUint16          mLengthRead;
   const DisEmission* mParentEmission;
};

// Accessors

inline DisUint8 DisSystem::GetReportedDataLength() const
{
   return mReportedDataLength;
}
inline DisUint8 DisSystem::GetNumberOfBeams() const
{
   return static_cast<DisUint8>(mBeamList.GetSize());
}
inline DisUint8 DisSystem::GetReportedNumberOfBeams() const
{
   return mReportedNumberOfBeams;
}
inline DisEnum16 DisSystem::GetName() const
{
   return mName;
}
inline DisUint8 DisSystem::GetFunction() const
{
   return mFunction;
}
inline DisUint8 DisSystem::GetNumber() const
{
   return mNumber;
}
inline void DisSystem::GetLocation(DisFloat32& aLocationX, DisFloat32& aLocationY, DisFloat32& aLocationZ) const
{
   aLocationX = mLocation[0];
   aLocationY = mLocation[1];
   aLocationZ = mLocation[2];
}
inline DisUint16 DisSystem::GetLengthRead() const
{
   return mLengthRead;
}

// Mutators

inline void DisSystem::SetName(DisEnum16 aName)
{
   mName = aName;
}
inline void DisSystem::SetFunction(DisUint8 aFunction)
{
   mFunction = aFunction;
}
inline void DisSystem::SetNumber(DisUint8 aNumber)
{
   mNumber = aNumber;
}
inline void DisSystem::SetLocation(DisFloat32 aLocationX, DisFloat32 aLocationY, DisFloat32 aLocationZ)
{
   mLocation[0] = aLocationX;
   mLocation[1] = aLocationY;
   mLocation[2] = aLocationZ;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, DisSystem& aSystem)
{
   aSystem.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisSystem& aSystem)
{
   aSystem.Get(aGenI);
   return aGenI;
}


// Beam list methods

inline bool DisSystem::AddBeam(DisBeam* aBeam)
{
   if (CanAddBeam(aBeam))
   {
      mBeamList.Add(aBeam);
      aBeam->SetParentSystem(this);
      return true;
   }
   return false;
}

inline std::vector<DisBeam*>::iterator DisSystem::GetBeamBegin()
{
   return mBeamList.GetBegin();
}

inline std::vector<DisBeam*>::const_iterator DisSystem::GetBeamBegin() const
{
   return mBeamList.GetBegin();
}

inline std::vector<DisBeam*>::iterator DisSystem::GetBeamEnd()
{
   return mBeamList.GetEnd();
}

inline std::vector<DisBeam*>::const_iterator DisSystem::GetBeamEnd() const
{
   return mBeamList.GetEnd();
}

inline unsigned int DisSystem::RemoveBeam(DisBeam* aBeam)
{
   return mBeamList.Remove(aBeam);
}

inline void DisSystem::RemoveAllBeams()
{
   mBeamList.RemoveAll();
}

inline const DisEmission* DisSystem::GetParentEmission() const
{
   return mParentEmission;
}

inline void DisSystem::SetParentEmission(const DisEmission* aEmission)
{
   mParentEmission = aEmission;
}

#endif
