// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DIS_DESCRIPTORBASERECORD_HPP
#define DIS_DESCRIPTORBASERECORD_HPP

#include "dis_export.h"

#include <string>

#include "DisEntityType.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

/**!
 * Defines the base class for all descriptor records
 */
class DIS_EXPORT DisDescriptorBaseRecord
{
public:
   DisDescriptorBaseRecord() = default;
   DisDescriptorBaseRecord(const DisDescriptorBaseRecord& aSrc);
   explicit DisDescriptorBaseRecord(GenI& aGenI);
   ~DisDescriptorBaseRecord() = default;

   // Input/output
   virtual DisUint16 GetLength() const;
   virtual void      Get(GenI& aGenI);
   virtual void      Put(GenO& aGenO) const;

   // Validate Data
   virtual bool IsValid() const;

   // Operators
   DisDescriptorBaseRecord& operator=(const DisDescriptorBaseRecord& aRhs);
   bool                     operator==(const DisDescriptorBaseRecord& aRhs) const;

   // Accessors
   const DisEntityType& GetEntityType() const;

   // Mutators
   void SetEntityType(const DisEntityType& aEntityType);

   virtual std::string ToString() const;
   virtual void        Stream(std::ostream& aStream);

private:
   virtual void GetMemberData(GenI& aGenI);
   //! Weapon type
   DisEntityType mEntityType;
   //! Base Record length
   const DisUint16 cBASE_RECORD_LENGTH = 8;
};

// Accessors
inline const DisEntityType& DisDescriptorBaseRecord::GetEntityType() const
{
   return mEntityType;
}

// Mutators
inline void DisDescriptorBaseRecord::SetEntityType(const DisEntityType& aEntityType)
{
   mEntityType = aEntityType;
}

// Define GenIO insertion and extraction operators
inline GenO& operator<<(GenO& aGenO, const DisDescriptorBaseRecord& aBaseDescriptorRecord)
{
   aBaseDescriptorRecord.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisDescriptorBaseRecord& aBaseDescriptorRecord)
{
   aBaseDescriptorRecord.Get(aGenI);
   return aGenI;
}

inline bool DisDescriptorBaseRecord::operator==(const DisDescriptorBaseRecord& aRhs) const
{
   return (mEntityType == aRhs.mEntityType);
}
#endif
