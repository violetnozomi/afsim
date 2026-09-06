//*****************************************************************************
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

#ifndef DIS_DISRECORDHEADER_HPP
#define DIS_DISRECORDHEADER_HPP

#include "dis_export.h"

#include <ostream>

#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"
#include "UtLog.hpp"
#include "UtScriptAccessible.hpp"

class GenI;
class GenO;
class UtScriptContext;

//! Base class for all records used by PDUs
class DIS_EXPORT DisRecordHeader : public UtScriptAccessible
{
public:
   DisRecordHeader() = default;
   explicit DisRecordHeader(DisEnum32 aRecordType);
   explicit DisRecordHeader(GenI& aGenI);
   ~DisRecordHeader() override = default;

   virtual DisRecordHeader* Clone() const;

   //----------------------- Script accessibility methods -----------------------
   const char* GetScriptClassName() const override;
   //--------------------- End script accessibility methods ---------------------

   // Accessors
   virtual DisEnum32 GetRecordType() const;
   virtual DisUint16 GetRecordLength() const;

   // Mutators
   void SetRecordType(DisEnum32 aRecordType);
   void SetRecordLength(DisUint16 aRecordLength);

   // Input/output
   virtual DisUint16 GetLength();
   virtual void      Get(GenI& aGenI);
   virtual void      Put(GenO& aGenO) const;

   virtual void        Stream(std::ostream& aStream) const;
   virtual std::string ToString() const;

   // Data Validation
   virtual bool IsValid() const;
   void         HandleReadError(ut::log::MessageStream& aOut);
   virtual bool operator==(const DisRecordHeader& aOther) const;

private:
   // These virtual methods must be implemented by derived Record classes
   virtual void      GetDerivedClass(GenI& aGenI);
   virtual void      PutDerivedClass(GenO& aGenO) const;
   virtual DisUint16 GetDerivedClassLength();

protected:
   DisEnum32         mRecordType   = 0;
   mutable DisUint16 mRecordLength = 6;
};

// Accessors

inline DisRecordHeader* DisRecordHeader::Clone() const
{
   return new DisRecordHeader(*this);
}

inline const char* DisRecordHeader::GetScriptClassName() const
{
   return "DisBaseRecord";
}

inline DisEnum32 DisRecordHeader::GetRecordType() const
{
   return mRecordType;
}

inline DisUint16 DisRecordHeader::GetRecordLength() const
{
   return mRecordLength;
}

// Mutators

inline void DisRecordHeader::SetRecordType(DisEnum32 aRecordType)
{
   mRecordType = aRecordType;
}

inline void DisRecordHeader::SetRecordLength(DisUint16 aRecordLength)
{
   mRecordLength = aRecordLength;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisRecordHeader& aRecordHeader)
{
   aRecordHeader.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisRecordHeader& aRecordHeader)
{
   aRecordHeader.Get(aGenI);
   return aGenI;
}

#endif
