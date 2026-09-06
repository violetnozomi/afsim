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

#ifndef DISCOMMENT_HPP
#define DISCOMMENT_HPP

#include "dis_export.h"

#include <vector>

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisComment : public DisPdu
{
public:
   struct VariableRecord
   {
      DisUint32 mVariableDatumId;
      DisUint32 mVariableDatumLength;
      char*     mVariableDatumValue;
      DisUint32 mVariablePaddingLength;
   };

   DisComment();
   DisComment(const DisComment& aSrc);
   DisComment(DisPdu& aPdu, GenI& aGenI);
   DisComment(GenI& aGenI);
   ~DisComment() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetReceivingEntity() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   DisUint32   GetNumFixedRecords() const;
   DisUint32   GetNumVariableRecords() const;
   const char* GetData(const unsigned int aIndex, unsigned int& aDataLength) const;

   // Mutators

   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);

   void AddVariableRecord(unsigned char* aDataPtr, unsigned int aDataLength);

   const VariableRecord* GetVariableRecord(const unsigned int i) const;

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;
   void      Stream(std::ostream& aStream) const override;

   // Data Validation
   bool IsValid() const override;

   // Operators
   DisComment& operator=(const DisComment& aRhs);

private:
   void Copy(const DisComment& aRhs);
   void DeleteClassData();
   void GetMemberData(GenI& aGenI);

   DisEntityId mOriginatingEntity;
   DisEntityId mReceivingEntity;
   DisUint32   mNumFixedRecords;
   DisUint32   mNumVariableRecords;

   std::vector<VariableRecord> mVariableRecords;
};

// Accessors

inline DisUint32 DisComment::GetNumFixedRecords() const
{
   return mNumFixedRecords;
}

inline DisUint32 DisComment::GetNumVariableRecords() const
{
   return mNumVariableRecords;
}

#endif
