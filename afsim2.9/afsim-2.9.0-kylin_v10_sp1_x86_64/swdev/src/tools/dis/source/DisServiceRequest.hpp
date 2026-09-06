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

#ifndef DISSERVICEREQUEST_HPP
#define DISSERVICEREQUEST_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"
#include "DisSupplyRecord.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisServiceRequest : public DisPdu
{
public:
   DisServiceRequest();
   DisServiceRequest(const DisPdu& aPdu, GenI& aGenI);
   DisServiceRequest(const DisServiceRequest& aSrc);
   DisServiceRequest(GenI& aGenI);
   ~DisServiceRequest() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetRequestingEntityId() const;
   const DisEntityId& GetServicingEntityId() const;

   const DisEntityId& GetOriginatingEntity() const override;

   DisUint8 GetServiceType() const;
   DisUint8 GetSupplyRecordCount() const;

   // Mutators

   void SetRequestingEntityId(const DisEntityId& aEntityId);
   void SetServicingEntityId(const DisEntityId& aEntityId);
   void SetServiceType(DisUint8 aType);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;
   void      Stream(std::ostream& aStream) const override;

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

   // define the type of service requests
   enum
   {
      Other    = 0,
      Resupply = 1,
      Repair   = 2
   };

private:
   void GetMemberData(GenI& aGenI);

   // Disallow assignment
   DisServiceRequest& operator=(const DisServiceRequest& aRhs);

   DisEntityId                      mRequestingEntityId;
   DisEntityId                      mServicingEntityId;
   DisUint8                         mServiceType;
   DisUint8                         mSupplyRecordCount;
   DisPtrContainer<DisSupplyRecord> mSupplyRecordList;
};

// Accessors

inline const DisEntityId& DisServiceRequest::GetRequestingEntityId() const
{
   return mRequestingEntityId;
}
inline const DisEntityId& DisServiceRequest::GetServicingEntityId() const
{
   return mServicingEntityId;
}
inline DisUint8 DisServiceRequest::GetServiceType() const
{
   return mServiceType;
}
inline DisUint8 DisServiceRequest::GetSupplyRecordCount() const
{
   return mSupplyRecordCount;
}

// Mutators

inline void DisServiceRequest::SetRequestingEntityId(const DisEntityId& aEntityId)
{
   mRequestingEntityId = aEntityId;
}
inline void DisServiceRequest::SetServicingEntityId(const DisEntityId& aEntityId)
{
   mServicingEntityId = aEntityId;
}
inline void DisServiceRequest::SetServiceType(DisUint8 aType)
{
   mServiceType = aType;
}

// SupplyRecordList Functions

inline void DisServiceRequest::AddSupplyRecord(DisSupplyRecord* aRecord)
{
   mSupplyRecordList.Add(aRecord);
   mSupplyRecordCount = static_cast<DisUint8>(mSupplyRecordList.GetSize());
}

inline void DisServiceRequest::RemoveAllSupplyRecords()
{
   mSupplyRecordList.RemoveAll();
   mSupplyRecordCount = 0;
}

inline void DisServiceRequest::RemoveSupplyRecord(DisSupplyRecord* aRecord)
{
   mSupplyRecordList.Remove(aRecord);
   mSupplyRecordCount = static_cast<DisUint8>(mSupplyRecordList.GetSize());
}

inline std::vector<DisSupplyRecord*>::iterator DisServiceRequest::GetSupplyRecordBegin()
{
   return mSupplyRecordList.GetBegin();
}

inline std::vector<DisSupplyRecord*>::const_iterator DisServiceRequest::GetSupplyRecordBegin() const
{
   return mSupplyRecordList.GetBegin();
}

inline std::vector<DisSupplyRecord*>::iterator DisServiceRequest::GetSupplyRecordEnd()
{
   return mSupplyRecordList.GetEnd();
}

inline std::vector<DisSupplyRecord*>::const_iterator DisServiceRequest::GetSupplyRecordEnd() const
{
   return mSupplyRecordList.GetEnd();
}
#endif
