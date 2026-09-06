//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#ifndef DISSILENTENTITYSYSTEMRECORD_HPP
#define DISSILENTENTITYSYSTEMRECORD_HPP

#include "dis_export.h"

#include <iostream>
#include <vector>

#include "DisEntityType.hpp"

//! Record for entities not producing Entity State PDUs.
class DIS_EXPORT DisSilentEntitySystemRecord
{
public:
   DisSilentEntitySystemRecord();
   DisSilentEntitySystemRecord(GenI& aGenI);
   DisSilentEntitySystemRecord(const DisSilentEntitySystemRecord& aSrc);
   ~DisSilentEntitySystemRecord() = default;

   // Input/output
   void      Get(GenI& aGenI);
   DisUint16 GetLength();
   void      Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   DisSilentEntitySystemRecord* Clone() const;

   void        Stream(std::ostream& aStream);
   std::string ToString();

   // Getters
   DisUint16              GetNumberOfEntities() const;
   DisUint16              GetNumberOfAppearanceRecords() const;
   const DisEntityType&   GetEntityType() const;
   std::vector<DisUint32> GetAppearanceRecords() const;
   const bool             GetAppearanceRecordAtIndex(DisUint16 aIndex, DisUint32& aReturnValue) const;

   // Setters
   void SetNumberOfEntities(DisUint16 aNumberOfEntities);
   void SetNumberOfAppearanceRecords(DisUint16 aNumberOfAppearanceRecords);
   void SetEntityType(const DisEntityType& aEntityType);
   void SetAppearanceRecords(const std::vector<DisUint32>& aAppearanceRecords);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisSilentEntitySystemRecord& aRhs) const;

private:
   //! Get the member data from the wire and stores the information in the correct variables
   void GetMemberData(GenI& aGenI);

   //! The number of entities that have the type specified by the entity type Field
   DisUint16 mNumberOfEntities = 0;

   //! The number of entity appearance records that deviate from the default entity appearance (IEEE 1278.1-2012 section 5.9.2.2.3)
   DisUint16 mNumberOfAppearanceRecords = 0;

   //! Specifies the entity type common to the entities in this system list
   DisEntityType mEntityType;

   //! The entities appearances of aggregating entities that deviate from the default appearance.
   //! Length of this vector is mNumberOfAppearanceRecords
   //! Value of each appearance record is determined by the entity it is referencing (see IEEE 1278.1-2012 6.2.26 and
   //! SISO-REF-010-2019 Enumerations v26 UID [31-43])
   std::vector<DisUint32> mAppearanceRecords;

   //! Size of base record in octets
   static const DisUint16 cRECORD_BASE_LENGTH_IN_OCTETS = 12;
};
#endif
