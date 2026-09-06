//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISAGGREGATETYPERECORD_HPP
#define DISAGGREGATETYPERECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Specifies the type and organization of an aggregate
 */
class DIS_EXPORT DisAggregateTypeRecord
{
public:
   DisAggregateTypeRecord() = default;
   DisAggregateTypeRecord(GenI& aGenI);
   DisAggregateTypeRecord(const DisAggregateTypeRecord& aSrc);
   ~DisAggregateTypeRecord() = default;

   // Input/output
   void      Get(GenI& aGenI);
   DisUint16 GetLength();
   void      Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   DisAggregateTypeRecord* Clone() const;

   void        Stream(std::ostream& aStream);
   std::string ToString() const;

   // Getters
   DisEnum8  GetAggregateKind() const;
   DisEnum8  GetDomain() const;
   DisEnum16 GetCountry() const;
   DisEnum8  GetCategory() const;
   DisEnum8  GetSubCategory() const;
   DisEnum8  GetSpecific() const;
   DisEnum8  GetExtra() const;

   // Setters
   void SetAggregateKind(const DisEnum8 aAggregateKind);
   void SetDomain(const DisEnum8 aDomain);
   void SetCountry(const DisEnum16 aCountry);
   void SetCategory(const DisEnum8 aCategory);
   void SetSubCategory(const DisEnum8 aSubCategory);
   void SetSpecific(const DisEnum8 aSpecific);
   void SetExtra(const DisEnum8 aExtra);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisAggregateTypeRecord& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   //! This field identifies the group criterion used to group the aggregate
   DisEnum8 mAggregateKind = 0;

   //! The domain in which the aggregate operates
   DisEnum8 mDomain = 0;

   //! The country with which the aggregate is associated
   DisEnum16 mCountry = 0;

   //! The category that describes the aggregate
   DisEnum8 mCategory = 0;

   //! The sub category to which the aggregate belongs based on the category field
   DisEnum8 mSubCategory = 0;

   //! The specific information about the aggregate based on the subcategory field
   DisEnum8 mSpecific = 0;

   //! Any extra information needed to describe the aggregate
   DisEnum8 mExtra = 0;

   //! Length of base record in octets
   static const DisUint16 cRECORD_BASE_LENGTH_IN_OCTETS = 8;
};
#endif
