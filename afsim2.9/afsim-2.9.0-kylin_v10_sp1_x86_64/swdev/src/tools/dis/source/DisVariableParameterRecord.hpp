// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#ifndef DISVARIABLEPARAMETERRECORD_HPP
#define DISVARIABLEPARAMETERRECORD_HPP

#include "dis_export.h"

#include <string>

#include "DisEntityType.hpp"

class GenI;
class GenO;

/**!
 * Defines a record which contains information associated with an entity or detonation,
 * not otherwise accounted for in a PDU. Information in this record can contain info on:
 *    - Articulated Parts
 *    - Attached Parts
 *    - Other data associated with an entity or detonation
 */
class DIS_EXPORT DisVariableParameterRecord
{
public:
   DisVariableParameterRecord() = default;
   DisVariableParameterRecord(GenI& aGenI);
   DisVariableParameterRecord(const DisVariableParameterRecord& aSrc);
   virtual ~DisVariableParameterRecord() = default;

   virtual DisVariableParameterRecord* Clone() const;
   // Accessors

   DisEnum8 GetTypeDesignator() const;

   // Mutators

   void SetTypeDesignator(DisEnum8 aTypeDesignator);

   // Input/output

   virtual void Get(GenI& aGenI);
   DisUint16    GetLength();
   virtual void Put(GenO& aGenO) const;

   // DisVariableParameterRecord&
   // operator=(const DisVariableParameterRecord& aRhs);
   virtual bool operator==(const DisVariableParameterRecord& aRhs);

   // Data Validation
   bool                IsValid() const;
   virtual std::string ToString() const;
   virtual void        Stream(std::ostream& aStream) const;

private:
   virtual void                GetMemberData(GenI& aGenI);
   DisVariableParameterRecord& operator=(const DisVariableParameterRecord& aRhs) = delete;

   //! The type ID of this record - defaulted to 5 for NONE
   DisEnum8               mTypeDesignator    = 5;
   static const DisUint16 cMIN_LENGTH_OCTETS = 16;
};

inline void DisVariableParameterRecord::SetTypeDesignator(DisEnum8 aTypeDesignator)
{
   mTypeDesignator = aTypeDesignator;
}

inline DisEnum8 DisVariableParameterRecord::GetTypeDesignator() const
{
   return mTypeDesignator;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisVariableParameterRecord& aPart)
{
   aPart.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisVariableParameterRecord& aPart)
{
   aPart.Get(aGenI);
   return aGenI;
}

#endif
