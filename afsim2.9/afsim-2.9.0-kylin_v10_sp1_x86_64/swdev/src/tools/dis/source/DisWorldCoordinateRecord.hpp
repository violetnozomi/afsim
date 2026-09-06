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

#ifndef DISWORLDCOORDINATERECORD_HPP
#define DISWORLDCOORDINATERECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

//! A point in space
class DIS_EXPORT DisWorldCoordinateRecord
{
public:
   DisWorldCoordinateRecord();
   DisWorldCoordinateRecord(const DisWorldCoordinateRecord& aSrc);
   DisWorldCoordinateRecord(GenI& aGenI);
   ~DisWorldCoordinateRecord() = default;

   // Input/output
   void      Get(GenI& aGenI);
   DisUint16 GetLength();
   void      Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   DisWorldCoordinateRecord* Clone() const;

   // Getters
   void GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const;
   // Setters
   void SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ);

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

private:
   void GetMemberData(GenI& aGenI);

   //! The location of the point
   DisFloat64 mLocation[3];

   //! 24 Octets - 3 groups of 64, no need for padding
   static const DisUint16 cBASE_LENGTH_IN_OCTETS = 24;
};

#endif
