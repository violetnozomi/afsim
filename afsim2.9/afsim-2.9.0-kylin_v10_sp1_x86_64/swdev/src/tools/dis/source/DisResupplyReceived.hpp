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

#ifndef DISRESUPPLYRECEIVED_HPP
#define DISRESUPPLYRECEIVED_HPP

#include "dis_export.h"

#include <ostream>

#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"
#include "DisSupplyRecord.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisResupplyReceived : public DisPdu
{
public:
   DisResupplyReceived();
   DisResupplyReceived(const DisResupplyReceived& aSrc);
   DisResupplyReceived(const DisPdu& aPdu, GenI& aGenI);
   DisResupplyReceived(GenI& aGenI);
   ~DisResupplyReceived() override;

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

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void Stream(std::ostream& aStream) const override;

   DisUint16 GetLength() override;
   // Data Validation
   bool IsValid() const override;

   // Supply Record list maintenance

   // WARNING: AddSupplyRecord takes ownership of the pointed to object.
   void AddSupplyRecord(DisSupplyRecord* aRecord);

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

   DisEntityId                      mRequestingEntityId;
   DisEntityId                      mServicingEntityId;
   DisUint8                         mSupplyRecordCount;
   DisPtrContainer<DisSupplyRecord> mSupplyRecordList;
};

// Accessors

inline const DisEntityId& DisResupplyReceived::GetRequestingEntityId() const
{
   return mRequestingEntityId;
}
inline const DisEntityId& DisResupplyReceived::GetServicingEntityId() const
{
   return mServicingEntityId;
}
inline DisUint8 DisResupplyReceived::GetSupplyRecordCount() const
{
   return mSupplyRecordCount;
}

// Mutators

inline void DisResupplyReceived::SetRequestingEntityId(const DisEntityId& aEntityId)
{
   mRequestingEntityId = aEntityId;
}
inline void DisResupplyReceived::SetServicingEntityId(const DisEntityId& aEntityId)
{
   mServicingEntityId = aEntityId;
}

// SupplyRecordList Functions

inline void DisResupplyReceived::AddSupplyRecord(DisSupplyRecord* aRecord)
{
   mSupplyRecordList.Add(aRecord);
   mSupplyRecordCount = static_cast<DisUint8>(mSupplyRecordList.GetSize());
}

inline void DisResupplyReceived::RemoveAllSupplyRecords()
{
   mSupplyRecordList.RemoveAll();
   mSupplyRecordCount = 0;
}

inline void DisResupplyReceived::RemoveSupplyRecord(DisSupplyRecord* aRecord)
{
   mSupplyRecordList.Remove(aRecord);
   mSupplyRecordCount = static_cast<DisUint8>(mSupplyRecordList.GetSize());
}

inline std::vector<DisSupplyRecord*>::iterator DisResupplyReceived::GetSupplyRecordBegin()
{
   return mSupplyRecordList.GetBegin();
}

inline std::vector<DisSupplyRecord*>::const_iterator DisResupplyReceived::GetSupplyRecordBegin() const
{
   return mSupplyRecordList.GetBegin();
}

inline std::vector<DisSupplyRecord*>::iterator DisResupplyReceived::GetSupplyRecordEnd()
{
   return mSupplyRecordList.GetEnd();
}

inline std::vector<DisSupplyRecord*>::const_iterator DisResupplyReceived::GetSupplyRecordEnd() const
{
   return mSupplyRecordList.GetEnd();
}
#endif
