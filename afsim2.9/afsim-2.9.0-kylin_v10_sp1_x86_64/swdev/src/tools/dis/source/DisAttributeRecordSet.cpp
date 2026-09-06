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

#include "DisAttributeRecordSet.hpp"

#include "DisRecordHeader.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

// Concrete attribute record includes
#include "DisAngleDeceptionAttributeRecord.hpp"
#include "DisAttribute.hpp"
#include "DisBlankingSectorAttributeRecord.hpp"
#include "DisFalseTargetsAttributeRecord.hpp"
#include "DisFalseTargetsEnhancedAttributeRecord.hpp"

// Static
const DisUint16 DisAttributeRecordSet::sSizeOfFixedPortion = 8; // 64 bits - size of Attribute Record Set with no records

DisAttributeRecordSet::DisAttributeRecordSet()
   : mEntityId()
   , mAttributeRecordCount(0)
   , mLengthRead(0)
{
}

DisUint16 DisAttributeRecordSet::GetLength()
{
   // Update count
   UpdateAttributeRecordCount();

   DisUint16 octetCount = sSizeOfFixedPortion;

   std::vector<DisAttributeRecord*>::const_iterator iter(mAttributeRecordList.GetBegin());
   while (iter != mAttributeRecordList.GetEnd())
   {
      octetCount += (*iter++)->GetLength();
   }
   return octetCount;
}

void DisAttributeRecordSet::Get(GenI& aGenI)
{
   RemoveAllAttributeRecords();

   aGenI >> mEntityId;
   aGenI >> mAttributeRecordCount;
   mLengthRead = sSizeOfFixedPortion;

   if (mAttributeRecordCount != 0)
   {
      mAttributeRecordList.Reserve(mAttributeRecordCount);
      for (int i = 0; i < mAttributeRecordCount; ++i)
      {
         // Instantiate concrete attribute record if possible
         DisAttributeRecord* attributeRecordPtr = nullptr;

         DisAttributeRecord recordHeader(aGenI);
         DisEnum32          recordType = recordHeader.GetRecordType();

         if (recordType == DisAttributeRecord::ANGLE_DECEPTION_ATTRIBUTE_RECORD_TYPE)
         {
            attributeRecordPtr = new DisAngleDeceptionAttributeRecord(recordHeader, aGenI);
         }
         else if (recordType == DisAttributeRecord::BLANKING_SECTOR_ATTRIBUTE_RECORD_TYPE)
         {
            attributeRecordPtr = new DisBlankingSectorAttributeRecord(recordHeader, aGenI);
         }
         else if (recordType == DisAttributeRecord::FALSE_TARGETS_ATTRIBUTE_RECORD_TYPE)
         {
            attributeRecordPtr = new DisFalseTargetsAttributeRecord(recordHeader, aGenI);
         }
         else if (recordType == DisAttributeRecord::FALSE_TARGETS_ENHANCED_ATTRIBUTE_RECORD_TYPE)
         {
            attributeRecordPtr = new DisFalseTargetsEnhancedAttributeRecord(recordHeader, aGenI);
         }

         if (attributeRecordPtr != nullptr)
         {
            mAttributeRecordList.Add(attributeRecordPtr);   // List takes ownership
            mLengthRead += attributeRecordPtr->GetLength(); // This length uses the derived class to get total length
         }
         else
         {
            // Read past the unimplemented or invalid attribute record
            DisUint16 givenRecordLength  = recordHeader.GetRecordLength();
            DisUint16 sizeOfRecordHeader = recordHeader.GetLength();
            int       sizeOfBytesNotRead = givenRecordLength - sizeOfRecordHeader;

            while (sizeOfBytesNotRead > 0)
            {
               DisUint8 tempOctet;
               aGenI >> tempOctet;
               --sizeOfBytesNotRead;
            }
            mLengthRead += givenRecordLength;
         }
      }
   }
}

void DisAttributeRecordSet::Put(GenO& aGenO)
{
   aGenO << mEntityId;
   aGenO << mAttributeRecordCount;

   for (auto iter = mAttributeRecordList.GetBegin(); iter != mAttributeRecordList.GetEnd(); iter++)
   {
      (*iter)->Put(aGenO);
   }
}

void DisAttributeRecordSet::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisAttributeRecordSet::ToString() const
{
   using namespace std;

   std::ostringstream ss("");
   ss << "Entity ID:              " << GetEntityId().ToString() << '\n';
   // AttributeRecordCount and ...List are not sourced from the same
   // thing.  And as above the "Get" mutates the count.
   // As an example, this mutates a member variable:
   // ss << "Attribute Record Count: " << GetNumberOfAttributeRecords() << '\n';
   size_t record_count = 0;
   // Report count first, even though it requires a second iteration.
   for (vector<DisAttributeRecord*>::const_iterator it = GetAttributeRecordBegin(); it != GetAttributeRecordEnd(); ++it)
   {
      ++record_count;
   }
   ss << "Attribute Record Count: " << record_count << '\n';
   record_count = 0;
   for (vector<DisAttributeRecord*>::const_iterator it = GetAttributeRecordBegin(); it != GetAttributeRecordEnd(); ++it)
   {
      ss << "***** Attribute Record " << record_count << " Data   *****\n";
      if (*it)
      {
         ss << (**it).ToString();
      }
      else
      {
         ss << " null pointer (this is bad)\n";
      }
      ss << "***** End of Attribute Record " << record_count << " *****\n";
      ++record_count;
   }

   return ss.str();
}

bool DisAttributeRecordSet::IsValid() const
{
   // Perform data validation.
   if (!mEntityId.IsValid())
   {
      return false;
   }

   for (auto it = mAttributeRecordList.GetBegin(); it != mAttributeRecordList.GetEnd(); ++it)
   {
      if (!(**it).IsValid())
      {
         return false;
      }
   }

   return true;
}
