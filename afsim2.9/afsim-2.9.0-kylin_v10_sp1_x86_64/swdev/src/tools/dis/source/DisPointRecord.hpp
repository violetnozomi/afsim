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

#ifndef DISPOINTRECORD_HPP
#define DISPOINTRECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisGeometryRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

//! A point in space
class DIS_EXPORT DisPointRecord : public DisGeometryRecord
{
public:
   DisPointRecord();
   DisPointRecord(const DisPointRecord& aSrc);
   DisPointRecord(GenI& aGenI);

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool IsValid() const override;

   virtual DisPointRecord* Clone() const override;

   // Getters
   void GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const;

   // Setters
   void SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ);

   // Logging
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI) override;

   //! The location of the point
   DisFloat64 mLocation[3];

   //! 24 Octets - 3 groups of 64, no need for padding
   static const DisUint16 cMIN_LENGTH_OCTETS = 24;
};
#endif
