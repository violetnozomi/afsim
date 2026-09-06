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

// DIS Attribute Record Set PDU

#ifndef VWC_DIS_DIS_ATTRIBUTE_RECORD_SET_HPP
#define VWC_DIS_DIS_ATTRIBUTE_RECORD_SET_HPP

#include "dis_export.h"

#include "DisAttributeRecord.hpp"
#include "DisEntityId.hpp"
#include "DisPtrContainer.hpp"

class GenI;
class GenO;

//! Encapsulates a set of attribute records which pertain only to the entity ID found in this class
class DIS_EXPORT DisAttributeRecordSet
{
public:
   DisAttributeRecordSet();
   virtual ~DisAttributeRecordSet() = default;

   // Allow compiler to build copy constructor and operator=
   // DisAttributeRecordSet(const DisAttributeRecordSet& aSrc);
   // DisAttributeRecordSet& operator=(const DisAttributeRecordSet& aRhs);

   // Accessors

   const DisEntityId& GetEntityId() const;
   const DisEnum16    GetNumberOfAttributeRecords() const;

   // Mutators

   void SetEntityId(const DisEntityId& aEntityId);

   // Input/Output

   virtual DisUint16 GetLength();
   virtual void      Get(GenI& aGenI);
   virtual void      Put(GenO& aGenO);

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Data Validation
   bool IsValid() const;

   // The following gets the length as read from the header of
   // individual attribute records, not the length computed by adding the
   // length of the base data plus the length required for the specified
   // number of attribute records.
   DisUint16 GetLengthRead() const;

   // AttributeRecord list maintenance...

   // WARNING: AddAttributeRecord takes ownership of the passed object.
   void AddAttributeRecord(DisAttributeRecord* aAttributeRecord);

   // Return iterators to DisAttributeRecord vector.
   std::vector<DisAttributeRecord*>::iterator       GetAttributeRecordBegin();
   std::vector<DisAttributeRecord*>::const_iterator GetAttributeRecordBegin() const;
   std::vector<DisAttributeRecord*>::iterator       GetAttributeRecordEnd();
   std::vector<DisAttributeRecord*>::const_iterator GetAttributeRecordEnd() const;

   // Remove and delete a DisAttributeRecord object from the list.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first AttributeRecord on the list.
   unsigned int RemoveAttributeRecord(DisAttributeRecord* aAttributeRecord);

   // Remove and delete all DisAttributeRecord objects from the list.
   void RemoveAllAttributeRecords();

   // Remove DisAttributeRecord object from the list, but DO NOT DELETE the object.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first AttributeRecord on the list.
   unsigned int RemoveAttributeRecordWithNoDelete(DisAttributeRecord* aAttributeRecord);

   // Remove all DisAttributeRecord objects from the list, but DO NOT DELETE them.
   void RemoveAllAttributeRecordsWithNoDelete();

protected:
   static const DisUint16 sSizeOfFixedPortion;

   void UpdateAttributeRecordCount() const;

   /// Caller takes ownership of returned pointer
   DisAttributeRecord* CreateNewAttributeRecord(GenI& aGenI);

   /// The mEntityId shall indicate the entity identifier or object identifier to
   /// which all attribute records in the record set apply.
   /*!
    * The entity Id that all the records to which this set pertain
    */
   DisEntityId mEntityId;
   /*!
    * The number of attribute records in this set
    */
   mutable DisUint16 mAttributeRecordCount;
   /*!
    * The list of attribute records
    */
   DisPtrContainer<DisAttributeRecord> mAttributeRecordList;
   /*!
    * The length of data to read
    */
   DisUint16 mLengthRead;
};

inline void DisAttributeRecordSet::UpdateAttributeRecordCount() const
{
   mAttributeRecordCount = static_cast<DisUint16>(mAttributeRecordList.GetSize());
}

inline DisUint16 DisAttributeRecordSet::GetLengthRead() const
{
   return mLengthRead;
}

// Accessors

inline const DisEntityId& DisAttributeRecordSet::GetEntityId() const
{
   return mEntityId;
}

inline const DisUint16 DisAttributeRecordSet::GetNumberOfAttributeRecords() const
{
   UpdateAttributeRecordCount();
   return mAttributeRecordCount;
}

// Mutators

inline void DisAttributeRecordSet::SetEntityId(const DisEntityId& aEntityId)
{
   mEntityId = aEntityId;
}

// Attribute Record list methods

inline void DisAttributeRecordSet::AddAttributeRecord(DisAttributeRecord* aAttributeRecord)
{
   mAttributeRecordList.Add(aAttributeRecord);
}

inline std::vector<DisAttributeRecord*>::iterator DisAttributeRecordSet::GetAttributeRecordBegin()
{
   return mAttributeRecordList.GetBegin();
}

inline std::vector<DisAttributeRecord*>::const_iterator DisAttributeRecordSet::GetAttributeRecordBegin() const
{
   return mAttributeRecordList.GetBegin();
}

inline std::vector<DisAttributeRecord*>::iterator DisAttributeRecordSet::GetAttributeRecordEnd()
{
   return mAttributeRecordList.GetEnd();
}

inline std::vector<DisAttributeRecord*>::const_iterator DisAttributeRecordSet::GetAttributeRecordEnd() const
{
   return mAttributeRecordList.GetEnd();
}

inline unsigned int DisAttributeRecordSet::RemoveAttributeRecord(DisAttributeRecord* aAttributeRecord)
{
   return mAttributeRecordList.Remove(aAttributeRecord);
}

inline void DisAttributeRecordSet::RemoveAllAttributeRecords()
{
   mAttributeRecordList.RemoveAll();
}

inline unsigned int DisAttributeRecordSet::RemoveAttributeRecordWithNoDelete(DisAttributeRecord* aAttributeRecord)
{
   return mAttributeRecordList.RemoveWithNoDelete(aAttributeRecord);
}

inline void DisAttributeRecordSet::RemoveAllAttributeRecordsWithNoDelete()
{
   mAttributeRecordList.RemoveAllWithNoDelete();
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, DisAttributeRecordSet& aAttributeRecordSet)
{
   aAttributeRecordSet.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisAttributeRecordSet& aAttributeRecordSet)
{
   aAttributeRecordSet.Get(aGenI);
   return aGenI;
}

#endif
