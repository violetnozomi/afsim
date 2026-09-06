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

#include "DisServiceRequest.hpp"

#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "DisSupplyRecord.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
const DisUint16 sDisServiceRequestSize = (48 + 48 + 8 + 8 + (16)) / 8;
}

DisServiceRequest::DisServiceRequest()
   : DisPdu()
   , mRequestingEntityId()
   , mServicingEntityId()
   , mServiceType(0)
   , mSupplyRecordCount(0)
{
   SetPduType(DisEnum::Pdu::Type::ServiceRequest);
   SetProtocolFamily(DisEnum::Pdu::Family::Logistics);
   GetLength();
}

DisServiceRequest::DisServiceRequest(const DisServiceRequest& aSrc)
   : DisPdu(aSrc)
   , mRequestingEntityId(aSrc.mRequestingEntityId)
   , mServicingEntityId(aSrc.mServicingEntityId)
   , mServiceType(aSrc.mServiceType)
   , mSupplyRecordCount(aSrc.mSupplyRecordCount)
   , mSupplyRecordList(aSrc.mSupplyRecordList)
{
   GetLength(); // Ensure consistent length
}

DisServiceRequest::DisServiceRequest(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisServiceRequest::DisServiceRequest(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisServiceRequest::~DisServiceRequest()
{
   RemoveAllSupplyRecords();
}

// virtual
DisPdu* DisServiceRequest::Clone() const
{
   return new DisServiceRequest(*this);
}

// virtual
const DisEntityId& DisServiceRequest::GetOriginatingEntity() const
{
   return mRequestingEntityId;
}

int DisServiceRequest::GetClass() const
{
   return (int)DisEnum::Pdu::Type::ServiceRequest;
}

// Virtual

void DisServiceRequest::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

void DisServiceRequest::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   DisUint16 pad16;

   RemoveAllSupplyRecords();

   aGenI >> mRequestingEntityId;
   aGenI >> mServicingEntityId;
   aGenI >> mServiceType;
   aGenI >> mSupplyRecordCount;

   aGenI >> pad16;

   if (mSupplyRecordCount != 0)
   {
      mSupplyRecordList.Reserve(mSupplyRecordCount);
      for (int i = 0; i < mSupplyRecordCount; ++i)
      {
         DisSupplyRecord* ptr = new DisSupplyRecord();
         aGenI >> *ptr;
         mSupplyRecordList.Add(ptr);
      }
   }

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

DisUint16 DisServiceRequest::GetLength()
{
   DisUint16 byteLength =
      DisPdu::GetBaseLength() + sDisServiceRequestSize + mSupplyRecordCount * DisSupplyRecord::GetLength();
   SetLength(byteLength);
   return byteLength;
}

void DisServiceRequest::Put(GenO& aGenO)
{
   DisUint16 pad16 = 0;

   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mRequestingEntityId;
   aGenO << mServicingEntityId;
   aGenO << mServiceType;
   aGenO << mSupplyRecordCount;
   aGenO << pad16;

   std::vector<DisSupplyRecord*>::const_iterator iter(mSupplyRecordList.GetBegin());
   while (iter != mSupplyRecordList.GetEnd())
   {
      aGenO << **iter++;
   }
}

bool DisServiceRequest::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mRequestingEntityId.IsValid())
      isValid = false;
   if (!mServicingEntityId.IsValid())
      isValid = false;

   std::vector<DisSupplyRecord*>::const_iterator iter(mSupplyRecordList.GetBegin());
   while (iter != mSupplyRecordList.GetEnd())
   {
      if (!(*iter++)->IsValid())
         isValid = false;
   }

   return isValid;
}

void DisServiceRequest::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);

   aStream << "      RequestEntityId = " << GetRequestingEntityId() << std::endl;
   aStream << "      ServicingEntity = " << GetServicingEntityId() << std::endl;

   aStream << "      => Record Count = " << static_cast<int>(mSupplyRecordCount) << std::endl;

   std::vector<DisSupplyRecord*>::const_iterator iter(mSupplyRecordList.GetBegin());
   while (iter != mSupplyRecordList.GetEnd())
   {
      (*iter++)->Stream(aStream);
      aStream << std::endl;
   }
   aStream << "      === End DisServiceRequest ===" << std::endl << std::endl;
}
