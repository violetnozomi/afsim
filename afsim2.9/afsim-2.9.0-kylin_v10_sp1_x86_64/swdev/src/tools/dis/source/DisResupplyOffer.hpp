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

#ifndef DISRESUPPLYOFFER_HPP
#define DISRESUPPLYOFFER_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"
#include "DisSupplyRecord.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisResupplyOffer : public DisPdu
{
public:
   DisResupplyOffer();
   DisResupplyOffer(const DisResupplyOffer& aSrc);
   DisResupplyOffer(const DisPdu& aPdu, GenI& aGenI);
   DisResupplyOffer(GenI& aGenI);
   ~DisResupplyOffer() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetRequestingEntityId() const;
   const DisEntityId& GetServicingEntityId() const;

   const DisEntityId& GetOriginatingEntity() const override;

   DisUint8 GetSupplyRecordCount() const;

   // Mutators

   void SetRequestingEntityId(const DisEntityId& aEntityId);
   void SetServicingEntityId(const DisEntityId& aEntityId);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool IsValid() const override;

   // Supply Record list maintenance

   // WARNING: AddSupplyRecord takes ownership of the pointed to object.
   void AddSupplyRecord(DisSupplyRecord* aRecord);

   void Stream(std::ostream& aStream) const override;
   // WARNING: ReomoveEntityPart will delete the object pointer to.
   void RemoveSupplyRecord(DisSupplyRecord* aRecord);
   void RemoveAllSupplyRecords();

   // Return iterators to DisSupplyRecord vector.
   std::vector<DisSupplyRecord*>::iterator       GetSupplyRecordBegin();
   std::vector<DisSupplyRecord*>::const_iterator GetSupplyRecordBegin() const;
   std::vector<DisSupplyRecord*>::iterator       GetSupplyRecordEnd();
   std::vector<DisSupplyRecord*>::const_iterator GetSupplyRecordEnd() const;

private:
   void GetMemberData(GenI& aGenI);

   // Disallow assignment
   DisResupplyOffer& operator=(const DisResupplyOffer& aRhs);

   DisEntityId                      mRequestingEntityId;
   DisEntityId                      mServicingEntityId;
   DisUint8                         mSupplyRecordCount;
   DisPtrContainer<DisSupplyRecord> mSupplyRecordList;
};

// Accessors

inline const DisEntityId& DisResupplyOffer::GetRequestingEntityId() const
{
   return mRequestingEntityId;
}
inline const DisEntityId& DisResupplyOffer::GetServicingEntityId() const
{
   return mServicingEntityId;
}
inline DisUint8 DisResupplyOffer::GetSupplyRecordCount() const
{
   return mSupplyRecordCount;
}

// Mutators

inline void DisResupplyOffer::SetRequestingEntityId(const DisEntityId& aEntityId)
{
   mRequestingEntityId = aEntityId;
}
inline void DisResupplyOffer::SetServicingEntityId(const DisEntityId& aEntityId)
{
   mServicingEntityId = aEntityId;
}

// SupplyRecordList Functions

inline void DisResupplyOffer::AddSupplyRecord(DisSupplyRecord* aRecord)
{
   mSupplyRecordList.Add(aRecord);
   mSupplyRecordCount = static_cast<DisUint8>(mSupplyRecordList.GetSize());
}

inline void DisResupplyOffer::RemoveAllSupplyRecords()
{
   mSupplyRecordList.RemoveAll();
   mSupplyRecordCount = 0;
}

inline void DisResupplyOffer::RemoveSupplyRecord(DisSupplyRecord* aRecord)
{
   mSupplyRecordList.Remove(aRecord);
   mSupplyRecordCount = static_cast<DisUint8>(mSupplyRecordList.GetSize());
}

inline std::vector<DisSupplyRecord*>::iterator DisResupplyOffer::GetSupplyRecordBegin()
{
   return mSupplyRecordList.GetBegin();
}

inline std::vector<DisSupplyRecord*>::const_iterator DisResupplyOffer::GetSupplyRecordBegin() const
{
   return mSupplyRecordList.GetBegin();
}

inline std::vector<DisSupplyRecord*>::iterator DisResupplyOffer::GetSupplyRecordEnd()
{
   return mSupplyRecordList.GetEnd();
}

inline std::vector<DisSupplyRecord*>::const_iterator DisResupplyOffer::GetSupplyRecordEnd() const
{
   return mSupplyRecordList.GetEnd();
}

#endif
