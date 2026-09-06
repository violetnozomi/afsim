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

#include "DisEnvironmentalRecord.hpp"

#include "DisBoundingSphereRecord.hpp"
#include "DisCombicStateRecord.hpp"
#include "DisConeRecord.hpp"
#include "DisConeRecord2.hpp"
#include "DisEllipsoidRecord.hpp"
#include "DisEllipsoidRecord2.hpp"
#include "DisFlareStateRecord.hpp"
#include "DisGaussianPlumeRecord.hpp"
#include "DisGaussianPuffRecord.hpp"
#include "DisLineRecord1.hpp"
#include "DisLineRecord2.hpp"
#include "DisPduEnums.hpp"
#include "DisPointRecord.hpp"
#include "DisPointRecord2.hpp"
#include "DisRectangularRecord.hpp"
#include "DisRectangularRecord2.hpp"
#include "DisSphereRecord2.hpp"
#include "DisUniformGeometryRecord.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisEnvironmentalRecord::DisEnvironmentalRecord()
   : DisRecordHeader((DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::NONE)
   , mGeometryOrStateRecord()
{
}

DisEnvironmentalRecord::DisEnvironmentalRecord(DisEnum32 aRecordType)
   : DisRecordHeader(aRecordType)
   , mGeometryOrStateRecord()
{
}

DisEnvironmentalRecord::DisEnvironmentalRecord(const DisEnvironmentalRecord& aSrc)
   : DisRecordHeader(aSrc)
   , mIndex(aSrc.mIndex)
   , mGeometryOrStateRecord(std::unique_ptr<DisEnvironmentalSpaceDataRecord>(aSrc.mGeometryOrStateRecord->Clone()))
{
}

DisEnvironmentalRecord::DisEnvironmentalRecord(const DisEnvironmentalRecord& aSrc, GenI& aGenI)
   : DisRecordHeader(aSrc)
   , mIndex(aSrc.mIndex)
{
   // mGeometryOrStateRecord intentionally not initialized above but instead initialized when gotten off of the wire
   GetMemberData(aGenI);
}

DisEnvironmentalRecord::DisEnvironmentalRecord(GenI& aGenI)
   : DisRecordHeader(aGenI)
{
   // mGeometryOrStateRecord intentionally not initialized above but instead initialized when gotten off of the wire
   GetMemberData(aGenI);
}

void DisEnvironmentalRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisEnvironmentalRecord::GetLength()
{
   DisUint16 length = cBASE_LENGTH_IN_OCTETS;
   length += GetCorrectSubRecordLength();
   return length;
}

void DisEnvironmentalRecord::Put(GenO& aGenO) const
{
   DisRecordHeader::Put(aGenO);
   aGenO << mIndex;
   aGenO << mPadding;
   PutSubRecordType(aGenO);
}

bool DisEnvironmentalRecord::IsValid() const
{
   return DisRecordHeader::IsValid() && GetSubRecordIsValid();
}

DisEnvironmentalRecord* DisEnvironmentalRecord::Clone() const
{
   return new DisEnvironmentalRecord(*this);
}

DisUint16 DisEnvironmentalRecord::GetLengthBits() const
{
   return mLengthBits;
}

DisUint8 DisEnvironmentalRecord::GetIndex() const
{
   return mIndex;
}

DisEnvironmentalSpaceDataRecord& DisEnvironmentalRecord::GetGeometryOrStateRecord()
{
   return *mGeometryOrStateRecord;
}

void DisEnvironmentalRecord::SetIndex(DisUint8 aIndex)
{
   mIndex = aIndex;
}

void DisEnvironmentalRecord::SetGeometryOrStateRecord(std::unique_ptr<DisEnvironmentalSpaceDataRecord> aGeometryOrStateRecord)
{
   mGeometryOrStateRecord = std::move(aGeometryOrStateRecord);
}

void DisEnvironmentalRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisEnvironmentalRecord::ToString() const
{
   std::string returnString = "-------Environmental Record-------\n";
   returnString += DisRecordHeader::ToString();
   returnString += "Index: " + std::to_string(mIndex) + '\n';
   returnString += GetSubRecordToString() + '\n';
   returnString += "-----End Environmental Record-----\n";
   return returnString;
}

void DisEnvironmentalRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mIndex;
   aGenI >> mPadding;
   GetSubRecordType(aGenI);
}

bool DisEnvironmentalRecord::GetSubRecordType(GenI& aGenI)
{
   switch (DisRecordHeader::GetRecordType())
   {
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::POINT_RECORD_1:
   {
      // case 251
      mGeometryOrStateRecord = std::unique_ptr<DisPointRecord>(new DisPointRecord(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::POINT_RECORD_2:
   {
      // case 252
      mGeometryOrStateRecord = std::unique_ptr<DisPointRecord2>(new DisPointRecord2(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::LINE_RECORD_1:
   {
      // case 253
      mGeometryOrStateRecord = std::unique_ptr<DisLineRecord1>(new DisLineRecord1(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::LINE_RECORD_2:
   {
      // case 254
      mGeometryOrStateRecord = std::unique_ptr<DisLineRecord2>(new DisLineRecord2(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::BOUNDING_SPHERE_RECORD:
   {
      // case 255
      mGeometryOrStateRecord = std::unique_ptr<DisBoundingSphereRecord>(new DisBoundingSphereRecord(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::SPHERE_RECORD:
   {
      // case 256
      mGeometryOrStateRecord = std::unique_ptr<DisBoundingSphereRecord>(new DisBoundingSphereRecord(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::SPHERE_RECORD_2:
   {
      // case 257
      mGeometryOrStateRecord = std::unique_ptr<DisSphereRecord2>(new DisSphereRecord2(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::ELLIPSOID_RECORD_1:
   {
      // case 258
      mGeometryOrStateRecord = std::unique_ptr<DisEllipsoidRecord>(new DisEllipsoidRecord(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::ELLIPSOID_RECORD_2:
   {
      // case 259
      mGeometryOrStateRecord = std::unique_ptr<DisEllipsoidRecord2>(new DisEllipsoidRecord2(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::CONE_RECORD_1:
   {
      // case 260
      mGeometryOrStateRecord = std::unique_ptr<DisConeRecord>(new DisConeRecord(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::CONE_RECORD_2:
   {
      // case 261
      mGeometryOrStateRecord = std::unique_ptr<DisConeRecord2>(new DisConeRecord2(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_1:
   {
      // case 262
      mGeometryOrStateRecord = std::unique_ptr<DisRectangularRecord>(new DisRectangularRecord(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_2:
   {
      // case 263
      mGeometryOrStateRecord = std::unique_ptr<DisRectangularRecord2>(new DisRectangularRecord2(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::GAUSSIAN_PLUME_RECORD:
   {
      // case 264
      mGeometryOrStateRecord = std::unique_ptr<DisGaussianPlumeRecord>(new DisGaussianPlumeRecord(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::GAUSSIAN_PUFF_RECORD:
   {
      // case 265
      mGeometryOrStateRecord = std::unique_ptr<DisGaussianPuffRecord>(new DisGaussianPuffRecord(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::UNIFORM_GEOMETRY_RECORD:
   {
      // case 266
      mGeometryOrStateRecord = std::unique_ptr<DisUniformGeometryRecord>(new DisUniformGeometryRecord(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_3:
   {
      // case 267
      mGeometryOrStateRecord = std::unique_ptr<DisRectangularRecord>(new DisRectangularRecord(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::COMBIC_STATE:
   {
      // case 268
      mGeometryOrStateRecord = std::unique_ptr<DisCombicStateRecord>(new DisCombicStateRecord(aGenI));
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::FLARE_STATE:
   {
      // case 269
      mGeometryOrStateRecord = std::unique_ptr<DisFlareStateRecord>(new DisFlareStateRecord(aGenI));
      break;
   }
   default:
   {
      return false;
   }
   }
   return true;
}

DisUint16 DisEnvironmentalRecord::GetCorrectSubRecordLength() const
{
   switch (DisRecordHeader::GetRecordType())
   {
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::POINT_RECORD_1:
   {
      // case 251
      DisPointRecord* temp = dynamic_cast<DisPointRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::POINT_RECORD_2:
   {
      // case 252
      DisPointRecord2* temp = dynamic_cast<DisPointRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::LINE_RECORD_1:
   {
      // case 253
      DisLineRecord1* temp = dynamic_cast<DisLineRecord1*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::LINE_RECORD_2:
   {
      // case 254
      DisLineRecord2* temp = dynamic_cast<DisLineRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::BOUNDING_SPHERE_RECORD:
   {
      // case 255
      DisBoundingSphereRecord* temp = dynamic_cast<DisBoundingSphereRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::SPHERE_RECORD:
   {
      // case 256
      DisBoundingSphereRecord* temp = dynamic_cast<DisBoundingSphereRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::SPHERE_RECORD_2:
   {
      // case 257
      DisSphereRecord2* temp = dynamic_cast<DisSphereRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::ELLIPSOID_RECORD_1:
   {
      // case 258
      DisEllipsoidRecord* temp = dynamic_cast<DisEllipsoidRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::ELLIPSOID_RECORD_2:
   {
      // case 259
      DisEllipsoidRecord2* temp = dynamic_cast<DisEllipsoidRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::CONE_RECORD_1:
   {
      // case 260
      DisConeRecord* temp = dynamic_cast<DisConeRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::CONE_RECORD_2:
   {
      // case 261
      DisConeRecord2* temp = dynamic_cast<DisConeRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_1:
   {
      // case 262
      DisRectangularRecord* temp = dynamic_cast<DisRectangularRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_2:
   {
      // case 263
      DisRectangularRecord2* temp = dynamic_cast<DisRectangularRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::GAUSSIAN_PLUME_RECORD:
   {
      // case 264
      DisGaussianPlumeRecord* temp = dynamic_cast<DisGaussianPlumeRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::GAUSSIAN_PUFF_RECORD:
   {
      // case 265
      DisGaussianPuffRecord* temp = dynamic_cast<DisGaussianPuffRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::UNIFORM_GEOMETRY_RECORD:
   {
      // case 266
      DisUniformGeometryRecord* temp = dynamic_cast<DisUniformGeometryRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_3:
   {
      // case 267
      DisRectangularRecord* temp = dynamic_cast<DisRectangularRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::COMBIC_STATE:
   {
      // case 268
      DisCombicStateRecord* temp = dynamic_cast<DisCombicStateRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::FLARE_STATE:
   {
      // case 269
      DisFlareStateRecord* temp = dynamic_cast<DisFlareStateRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         return temp->GetLength();
      }
      break;
   }
   default:
   {
      break;
   }
   }
   return 0;
}

void DisEnvironmentalRecord::PutSubRecordType(GenO& aGenO) const
{
   switch (DisRecordHeader::GetRecordType())
   {
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::POINT_RECORD_1:
   {
      // case 251
      DisPointRecord* temp = dynamic_cast<DisPointRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::POINT_RECORD_2:
   {
      // case 252
      DisPointRecord2* temp = dynamic_cast<DisPointRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::LINE_RECORD_1:
   {
      // case 253
      DisLineRecord1* temp = dynamic_cast<DisLineRecord1*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::LINE_RECORD_2:
   {
      // case 254
      DisLineRecord2* temp = dynamic_cast<DisLineRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::BOUNDING_SPHERE_RECORD:
   {
      // case 255
      DisBoundingSphereRecord* temp = dynamic_cast<DisBoundingSphereRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::SPHERE_RECORD:
   {
      // case 256
      DisBoundingSphereRecord* temp = dynamic_cast<DisBoundingSphereRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::SPHERE_RECORD_2:
   {
      // case 257
      DisSphereRecord2* temp = dynamic_cast<DisSphereRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::ELLIPSOID_RECORD_1:
   {
      // case 258
      DisEllipsoidRecord* temp = dynamic_cast<DisEllipsoidRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::ELLIPSOID_RECORD_2:
   {
      // case 259
      DisEllipsoidRecord2* temp = dynamic_cast<DisEllipsoidRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::CONE_RECORD_1:
   {
      // case 260
      DisConeRecord* temp = dynamic_cast<DisConeRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::CONE_RECORD_2:
   {
      // case 261
      DisConeRecord2* temp = dynamic_cast<DisConeRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_1:
   {
      // case 262
      DisRectangularRecord* temp = dynamic_cast<DisRectangularRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_2:
   {
      // case 263
      DisRectangularRecord2* temp = dynamic_cast<DisRectangularRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::GAUSSIAN_PLUME_RECORD:
   {
      // case 264
      DisGaussianPlumeRecord* temp = dynamic_cast<DisGaussianPlumeRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::GAUSSIAN_PUFF_RECORD:
   {
      // case 265
      DisGaussianPuffRecord* temp = dynamic_cast<DisGaussianPuffRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::UNIFORM_GEOMETRY_RECORD:
   {
      // case 266
      DisUniformGeometryRecord* temp = dynamic_cast<DisUniformGeometryRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_3:
   {
      // case 267
      DisRectangularRecord* temp = dynamic_cast<DisRectangularRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::COMBIC_STATE:
   {
      // case 268
      DisCombicStateRecord* temp = dynamic_cast<DisCombicStateRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::FLARE_STATE:
   {
      // case 269
      DisFlareStateRecord* temp = dynamic_cast<DisFlareStateRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         temp->Put(aGenO);
      }
      break;
   }
   default:
   {
      break;
   }
   }
}

bool DisEnvironmentalRecord::GetSubRecordIsValid() const
{
   bool isValid = true;
   switch (DisRecordHeader::GetRecordType())
   {
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::POINT_RECORD_1:
   {
      // case 251
      DisPointRecord* temp = dynamic_cast<DisPointRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::POINT_RECORD_2:
   {
      // case 252
      DisPointRecord2* temp = dynamic_cast<DisPointRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::LINE_RECORD_1:
   {
      // case 253
      DisLineRecord1* temp = dynamic_cast<DisLineRecord1*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::LINE_RECORD_2:
   {
      // case 254
      DisLineRecord2* temp = dynamic_cast<DisLineRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::BOUNDING_SPHERE_RECORD:
   {
      // case 255
      DisBoundingSphereRecord* temp = dynamic_cast<DisBoundingSphereRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::SPHERE_RECORD:
   {
      // case 256
      DisBoundingSphereRecord* temp = dynamic_cast<DisBoundingSphereRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::SPHERE_RECORD_2:
   {
      // case 257
      DisSphereRecord2* temp = dynamic_cast<DisSphereRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::ELLIPSOID_RECORD_1:
   {
      // case 258
      DisEllipsoidRecord* temp = dynamic_cast<DisEllipsoidRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::ELLIPSOID_RECORD_2:
   {
      // case 259
      DisEllipsoidRecord2* temp = dynamic_cast<DisEllipsoidRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::CONE_RECORD_1:
   {
      // case 260
      DisConeRecord* temp = dynamic_cast<DisConeRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::CONE_RECORD_2:
   {
      // case 261
      DisConeRecord2* temp = dynamic_cast<DisConeRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_1:
   {
      // case 262
      DisRectangularRecord* temp = dynamic_cast<DisRectangularRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_2:
   {
      // case 263
      DisRectangularRecord2* temp = dynamic_cast<DisRectangularRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::GAUSSIAN_PLUME_RECORD:
   {
      // case 264
      DisGaussianPlumeRecord* temp = dynamic_cast<DisGaussianPlumeRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::GAUSSIAN_PUFF_RECORD:
   {
      // case 265
      DisGaussianPuffRecord* temp = dynamic_cast<DisGaussianPuffRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::UNIFORM_GEOMETRY_RECORD:
   {
      // case 266
      DisUniformGeometryRecord* temp = dynamic_cast<DisUniformGeometryRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_3:
   {
      // case 267
      DisRectangularRecord* temp = dynamic_cast<DisRectangularRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::COMBIC_STATE:
   {
      // case 268
      DisCombicStateRecord* temp = dynamic_cast<DisCombicStateRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::FLARE_STATE:
   {
      // case 269
      DisFlareStateRecord* temp = dynamic_cast<DisFlareStateRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         isValid = temp->IsValid();
      }
      break;
   }
   default:
   {
      break;
   }
   }
   return isValid;
}

std::string DisEnvironmentalRecord::GetSubRecordToString() const
{
   std::string returnString = "";
   switch (DisRecordHeader::GetRecordType())
   {
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::POINT_RECORD_1:
   {
      // case 251
      DisPointRecord* temp = dynamic_cast<DisPointRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::POINT_RECORD_2:
   {
      // case 252
      DisPointRecord2* temp = dynamic_cast<DisPointRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::LINE_RECORD_1:
   {
      // case 253
      DisLineRecord1* temp = dynamic_cast<DisLineRecord1*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::LINE_RECORD_2:
   {
      // case 254
      DisLineRecord2* temp = dynamic_cast<DisLineRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::BOUNDING_SPHERE_RECORD:
   {
      // case 255
      DisBoundingSphereRecord* temp = dynamic_cast<DisBoundingSphereRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::SPHERE_RECORD:
   {
      // case 256
      DisBoundingSphereRecord* temp = dynamic_cast<DisBoundingSphereRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::SPHERE_RECORD_2:
   {
      // case 257
      DisSphereRecord2* temp = dynamic_cast<DisSphereRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::ELLIPSOID_RECORD_1:
   {
      // case 258
      DisEllipsoidRecord* temp = dynamic_cast<DisEllipsoidRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::ELLIPSOID_RECORD_2:
   {
      // case 259
      DisEllipsoidRecord2* temp = dynamic_cast<DisEllipsoidRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::CONE_RECORD_1:
   {
      // case 260
      DisConeRecord* temp = dynamic_cast<DisConeRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::CONE_RECORD_2:
   {
      // case 261
      DisConeRecord2* temp = dynamic_cast<DisConeRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_1:
   {
      // case 262
      DisRectangularRecord* temp = dynamic_cast<DisRectangularRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_2:
   {
      // case 263
      DisRectangularRecord2* temp = dynamic_cast<DisRectangularRecord2*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::GAUSSIAN_PLUME_RECORD:
   {
      // case 264
      DisGaussianPlumeRecord* temp = dynamic_cast<DisGaussianPlumeRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::GAUSSIAN_PUFF_RECORD:
   {
      // case 265
      DisGaussianPuffRecord* temp = dynamic_cast<DisGaussianPuffRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::UNIFORM_GEOMETRY_RECORD:
   {
      // case 266
      DisUniformGeometryRecord* temp = dynamic_cast<DisUniformGeometryRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::RECTANGULAR_RECORD_3:
   {
      // case 267
      DisRectangularRecord* temp = dynamic_cast<DisRectangularRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::COMBIC_STATE:
   {
      // case 268
      DisCombicStateRecord* temp = dynamic_cast<DisCombicStateRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   case (DisEnum32)DisEnvironmentalAndGeometricRecordTypeEnum::FLARE_STATE:
   {
      // case 269
      DisFlareStateRecord* temp = dynamic_cast<DisFlareStateRecord*>(mGeometryOrStateRecord.get()->Clone());
      if (temp != nullptr)
      {
         returnString = temp->ToString();
      }
      break;
   }
   default:
   {
      break;
   }
   }
   return returnString;
}
