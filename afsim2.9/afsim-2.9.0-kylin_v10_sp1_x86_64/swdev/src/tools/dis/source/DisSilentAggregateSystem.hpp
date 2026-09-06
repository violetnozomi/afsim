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

#ifndef DISSILENTAGGREGATESYSTEM_HPP
#define DISSILENTAGGREGATESYSTEM_HPP

#include "dis_export.h"

#include "DisAggregateTypeRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"

//! Wrapper class for containing all the data components of a Silent Aggregate System
class DIS_EXPORT DisSilentAggregateSystem
{
public:
   DisSilentAggregateSystem();
   DisSilentAggregateSystem(GenI& aGenI);
   DisSilentAggregateSystem(const DisSilentAggregateSystem& aSrc);
   ~DisSilentAggregateSystem() = default;

   // Input/output
   void      Get(GenI& aGenI);
   DisUint16 GetLength();
   void      Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   DisSilentAggregateSystem* Clone() const;

   void        Stream(std::ostream& aStream, const std::string& aSpacing);
   std::string ToString();

   // Getters
   DisUint16              GetNumberOfAggregates() const;
   DisAggregateTypeRecord GetAggregateSystem() const;

   // Setters
   void SetNumberOfAggregates(DisUint16 aNumberofAggregates);
   void SetAggregateSystem(const DisAggregateTypeRecord& aAggregateSytem);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisSilentAggregateSystem& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   //! The number of aggregates that have the type specified by mAggregateSystem
   DisUint16 mNumberOfAggregates = 0;

   //! Padding
   DisUint16 mPadding = 0;

   //! This field shall specify the aggregates common to this system list
   DisAggregateTypeRecord mAggregateSystem;

   //! Size of base record in octets
   static const DisUint16 cBASE_RECORD_LENGTH_IN_OCTETS = 4;
};
#endif
