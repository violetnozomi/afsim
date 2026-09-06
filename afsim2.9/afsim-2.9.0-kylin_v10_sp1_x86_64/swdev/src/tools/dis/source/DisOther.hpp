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

#ifndef DISOTHER_HPP
#define DISOTHER_HPP

#include "dis_export.h"

// This class represents all other PDU's that aren't represented as
// another specific PDU class.  The data portion (i.e.: the stuff
// beyond the normal DIS header) is stored as an array of bytes. A
// user of this class can use the GetData() and SetData() methods
// to manage the data.

#include "DisPdu.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisOther : public DisPdu
{
public:
   DisOther();
   DisOther(const DisOther& aSrc);
   DisOther(DisPdu& aPdu, GenI& aGenI);
   DisOther(DisUint8 aPduType, DisUint8 aProtocolFamily);
   ~DisOther() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // GetData returns a pointer to the data portion and the number
   // of bytes in the data portion.  The caller can use a GenMemI
   // object to interpret the data in a platform independent manner.

   void GetData(const unsigned char*& aDataPtr, unsigned int& aDataLen);

   // SetData allows the caller to replace the data portion of the
   // PDU.  The memory pointed to the supplied pointer becomes the
   // property of the object and will be deleted when the objected
   // is deleted or if the pointer is later replaced.

   void SetData(unsigned char* aDataPtr, DisUint16 aDataLen);

   // SetUserData is like SetData except that the memory pointed to
   // by the supplied pointer remains the property of the caller
   // and is NOT deleted when the object is deleted.

   void SetUserData(unsigned char* aDataPtr, DisUint16 aDataLen);

   // Input/Output

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void Stream(std::ostream& aStream) const override;
   bool IsValid() const override;

   // Operators

   DisOther& operator=(const DisOther& aRhs);

private:
   void Copy(const DisOther& aRhs);
   void DeleteClassData();
   void GetMemberData(GenI& aGenI);

   unsigned char* mDataPtr;
   bool           mDataIsClassOwned; // true if the data is owned by the
                                     // class and not the user
};

#endif
