// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISGRIDDATAREPRESENTATIONTYPE_HPP
#define DISGRIDDATAREPRESENTATIONTYPE_HPP

#include "dis_export.h"

#include <iostream>

#include "DisSampleTypeEnum.hpp"
#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/*!
 * This is the base class for all DisGridDataRepresentationTypes
 */
class DIS_EXPORT DisGridDataRepresentationType
{
public:
   DisGridDataRepresentationType() = default;
   DisGridDataRepresentationType(DisUint16 aNumberOfValues);
   DisGridDataRepresentationType(GenI& aGenI);
   virtual ~DisGridDataRepresentationType() = default;

   // Input/output
   virtual void      Get(GenI& aGenI);
   virtual DisUint16 GetLength() const;
   virtual void      Put(GenO& aGenO) const;

   DisUint16 GetNumberOfValues() const;
   void      SetNumberOfValues(DisUint16 aNumberOfValues);

   // Data Validation
   virtual bool                           IsValid() const = 0;
   virtual DisGridDataRepresentationType* Clone() const   = 0;

   void                Stream(std::ostream& aStream) const;
   virtual std::string ToString() const;

protected:
   virtual void Copy(const DisGridDataRepresentationType& aSrc);
   //! The number of octets in this record
   DisUint16 mNumberOfValues = 0;

private:
   virtual DisUint16 CalculateNumberOfPaddingOctets() const;
   virtual void      GetMemberData(GenI& aGenI) = 0;

   static const DisUint16 cBASE_LENGTH_IN_OCTETS = 2;
};
#endif
