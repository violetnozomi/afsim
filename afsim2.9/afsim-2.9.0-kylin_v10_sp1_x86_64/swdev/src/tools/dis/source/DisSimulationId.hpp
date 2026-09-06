// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISSIMULATIONID_HPP
#define DISSIMULATIONID_HPP

#include "dis_export.h"

// An overloaded operator<<(ostream& aOut, const DisEntityId) function to write
// the contents of this class to an ostream can be found in DisIO.hpp.  This
// function is not defined here due to portability issues dealing with the
// old and new I/O stream libraries (i.e.: one typically can't use <iostream.h>
// and <iostream> in the same application).  See DisIO.hpp for more details.

#include <cstddef>
#include <iosfwd>
#include <iostream>
#include <string>

#include "DisEntityId.hpp"
#include "DisSimulationAddressRecord.hpp"
#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"
#include "Filterable.hpp"
#include "UtScriptAccessible.hpp"

class GenI;
class GenO;

//! DIS Entity Identification Record
class DIS_EXPORT DisSimulationId : public DisSimulationAddressRecord
{
public:
   DisSimulationId();
   DisSimulationId(DisUint16 aSiteNumber, DisUint16 aApplicationNumber, DisUint16 aReferenceNumber);
   DisSimulationId(DisUint16 aSiteNumber, DisUint16 aApplicationNumber);
   DisSimulationId(const DisSimulationId& aSrc);
   DisSimulationId(const DisSimulationAddressRecord& aNumber);
   ~DisSimulationId() = default;

   DisSimulationId* Clone() const;
   DisEntityId*     ToEntityId() const;

   // Accessors
   DisUint16 GetReferenceNumber() const;

   // Mutators
   void SetReferenceNumber(DisUint16 aReferenceNumber);

   // Input/output

   void      Get(GenI& aGenI);
   DisUint16 GetLength();
   void      Put(GenO& aGenO) const;

   // Operators
   DisSimulationId& operator=(const DisSimulationId& aRhs);

   // Data Validation
   bool IsValid() const;

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisSimulationId& aRhs) const;
   bool operator!=(const DisSimulationId& aRhs) const;
   bool operator<(const DisSimulationId& aRhs) const;

private:
   /*!
    * According to the current spec (IEE 1278.1-2012) this variable must be included but is
    * currently not applicable for any use case provided.
    * HOWEVER, one use case implied is this value is used when converting between a SimulationId and EntityId
    */
   DisUint16 mReferenceNumber = 0;

   //! The size of this object (simulation address record + reference number) in octets
   static const int cSIZE_IN_OCTETS = 2;
};

// Accessors

inline DisUint16 DisSimulationId::GetReferenceNumber() const
{
   return mReferenceNumber;
}

// Mutators

inline void DisSimulationId::SetReferenceNumber(DisUint16 aReferenceNumber)
{
   mReferenceNumber = aReferenceNumber;
}

// Inline Operators
inline bool DisSimulationId::operator==(const DisSimulationId& aRhs) const
{
   return ((DisSimulationAddressRecord::GetApplicationNumber() == aRhs.GetApplicationNumber()) &&
           (DisSimulationAddressRecord::GetSiteNumber() == aRhs.GetSiteNumber()));
}

inline bool DisSimulationId::operator!=(const DisSimulationId& aRhs) const
{
   return ((DisSimulationAddressRecord::GetApplicationNumber() != aRhs.GetApplicationNumber()) ||
           (DisSimulationAddressRecord::GetSiteNumber() != aRhs.GetSiteNumber()));
}
#endif
