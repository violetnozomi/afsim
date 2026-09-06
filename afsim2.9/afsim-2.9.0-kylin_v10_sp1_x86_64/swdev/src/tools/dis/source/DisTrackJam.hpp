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

// DIS Track/Jam Record

#ifndef DISTRACKJAM_HPP
#define DISTRACKJAM_HPP

#include "dis_export.h"

#include <iostream>

#include "DisEntityId.hpp"

class GenI;
class GenO;

//! Track/Jam Data record
/*!
 *  The Track/Jam Data record identifies an entity tracked or illuminated or an
 *  emitter beam targeted with jamming.
 */
class DIS_EXPORT DisTrackJam
{
public:
   //! Default Constructor
   DisTrackJam();
   //! Copy Constructor
   DisTrackJam(const DisTrackJam& aSrc);
   //! Initialized Constructor
   DisTrackJam(const DisEntityId& aEntity, DisUint8 aEmitter = 0, DisUint8 aBeam = 0);
   //! Destructor
   ~DisTrackJam() = default;

   // Accessors
   //! Entity ID
   /*!
    *  This field shall specify an entity by Simulation Address and Entity
    *  Number.
    */
   const DisEntityId& GetEntityId() const;
   //! Emitter Number
   /*!
    *  This field shall specify an emitter system associated with the entity.
    */
   DisUint8 GetEmitterNumber() const;
   //! Beam Number
   /*!
    *  This filed shall specify a beam associated with the emitter system.
    */
   DisUint8 GetBeamNumber() const;

   // Mutators
   void SetEntityId(const DisEntityId& aEntityId);
   void SetEmitterNumber(DisUint8 aNumber);
   void SetBeamNumber(DisUint8 aNumber);

   // Input/output
   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Operators
   DisTrackJam& operator=(const DisTrackJam& aRhs);
   bool         operator==(const DisTrackJam& aRhs) const;
   bool         operator<(const DisTrackJam& aRhs) const;

private:
   /*!
    * Specifies the entity
    */
   DisEntityId mEntityId;
   /*!
    * Specifies the emitter system associated with the entity
    */
   DisUint8 mEmitterNumber;
   /*!
    * Specifies a beam associated with the emitter system
    */
   DisUint8 mBeamNumber;
};

// Accessors
inline const DisEntityId& DisTrackJam::GetEntityId() const
{
   return mEntityId;
}
inline DisUint8 DisTrackJam::GetEmitterNumber() const
{
   return mEmitterNumber;
}
inline DisUint8 DisTrackJam::GetBeamNumber() const
{
   return mBeamNumber;
}
// Mutators

inline void DisTrackJam::SetEntityId(const DisEntityId& aEntityId)
{
   mEntityId = aEntityId;
}
inline void DisTrackJam::SetEmitterNumber(DisUint8 aNumber)
{
   mEmitterNumber = aNumber;
}
inline void DisTrackJam::SetBeamNumber(DisUint8 aNumber)
{
   mBeamNumber = aNumber;
}

// Operators

inline bool DisTrackJam::operator==(const DisTrackJam& aRhs) const
{
   return ((mEntityId == aRhs.mEntityId) && (mEmitterNumber == aRhs.mEmitterNumber) && (mBeamNumber == aRhs.mBeamNumber));
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisTrackJam& aTrackJam)
{
   aTrackJam.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisTrackJam& aTrackJam)
{
   aTrackJam.Get(aGenI);
   return aGenI;
}

#endif
