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

#include "DisOther.hpp"

#include <cassert>
#include <cstring> // for memcpy
#include <iostream>

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtLog.hpp"

DisOther::DisOther()
   : DisPdu()
   , mDataPtr(nullptr)
   , mDataIsClassOwned(false)
{
}

DisOther::DisOther(const DisOther& aSrc)
   : DisPdu(aSrc)
   , mDataPtr(nullptr)
   , mDataIsClassOwned(false)
{
   Copy(aSrc);
}

DisOther::DisOther(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mDataPtr(nullptr)
   , mDataIsClassOwned(false)
{
   GetMemberData(aGenI);
}

DisOther::DisOther(DisUint8 aPduType, DisUint8 aProtocolFamily)
   : DisPdu()
   , mDataPtr(nullptr)
   , mDataIsClassOwned(false)
{
   SetPduType(aPduType);
   SetProtocolFamily(aProtocolFamily);
}

// virtual
DisOther::~DisOther()
{
   DeleteClassData();
}

// virtual
DisPdu* DisOther::Clone() const
{
   return new DisOther(*this);
}

// virtual
int DisOther::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Other;
}

// Copy the data portion from one DisOther to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

void DisOther::Copy(const DisOther& aSrc)
{
   unsigned int dataLen = aSrc.GetLengthToRead() - 12;
   if (dataLen != 0)
   {
      if (aSrc.mDataPtr != nullptr)
      {
         mDataIsClassOwned = true;
         mDataPtr          = new unsigned char[dataLen];
         memcpy(mDataPtr, aSrc.mDataPtr, dataLen);
      }
      else
      {
         ut::log::warning() << "DisOther::Copy - input data set is empty.";
         assert(aSrc.mDataPtr != nullptr);
      }
   }
}

// DeleteClassData deletes the data portion if the memory
// pointed to by the data pointer is owned by the class.

void DisOther::DeleteClassData()
{
   if (mDataPtr != nullptr)
   {
      if (mDataIsClassOwned)
      {
         delete[] mDataPtr;
      }
      mDataPtr = nullptr;
   }
   mDataIsClassOwned = false;
}

// GetData returns a pointer to the data portion and the number
// of bytes in the data portion.  The caller can use a GenMemIO
// object to interpret the data in a platform independent manner.

void DisOther::GetData(const unsigned char*& aDataPtr, unsigned int& aDataLen)
{
   aDataPtr = mDataPtr;
   aDataLen = GetLength() - 12;
}

// SetData allows the caller to replace the data portion of the
// PDU.  The memory pointed to the supplied pointer becomes the
// property of the object and will be deleted when the objected
// is deleted or if the pointer is later replaced.

void DisOther::SetData(unsigned char* aDataPtr, DisUint16 aDataLen)
{
   DeleteClassData();        // Release existing class-owned data
   mDataIsClassOwned = true; // Data owned by the class
   mDataPtr          = aDataPtr;
   DisPdu::SetLength(aDataLen + 12);
}

void DisOther::SetUserData(unsigned char* aDataPtr, DisUint16 aDataLen)
{
   DeleteClassData();         // Release existing class-owned data
   mDataIsClassOwned = false; // Data owned by the user
   mDataPtr          = aDataPtr;
   DisPdu::SetLength(aDataLen + 12);
}

// virtual
void DisOther::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisOther::GetMemberData(aGenI);
}

void DisOther::GetMemberData(GenI& aGenI)
{
   DeleteClassData(); // Release existing class-owned data
   int octetCount = GetLength() - 12;
   if (octetCount > 0)
   {
      mDataIsClassOwned = true; // Data is owned by the class
      mDataPtr          = new unsigned char[octetCount];
      assert(mDataPtr != nullptr);
      aGenI.GetRaw(mDataPtr, octetCount);
   }
}

// virtual
void DisOther::Put(GenO& aGenO)
{
   DisPdu::Put(aGenO);
   int octetCount = GetLength() - 12;
   if (octetCount > 0)
   {
      assert(mDataPtr != nullptr);
      aGenO.PutRaw(mDataPtr, octetCount);
   }
}

DisOther& DisOther::operator=(const DisOther& aRhs)
{
   if (this != &aRhs)
   {
      DisPdu::operator=(aRhs);
      DeleteClassData();
      Copy(aRhs);
   }
   return *this;
}

void DisOther::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "     === End Dis Other ===" << std::endl << std::endl;
}

// virtual
bool DisOther::IsValid() const
{
   bool isValid = true;
   if (!DisPdu::IsValid())
      isValid = false;
   return isValid;
}
