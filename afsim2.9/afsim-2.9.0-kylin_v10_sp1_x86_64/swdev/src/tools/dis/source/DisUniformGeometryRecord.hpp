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

#ifndef DISUNIFORMGEOMETRYRECORD_HPP
#define DISUNIFORMGEOMETRYRECORD_HPP

#include "dis_export.h"

#include "DisGeometryRecord.hpp"
#include "DisPointRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines a geometry that extends in all directions without limit.
 */
class DIS_EXPORT DisUniformGeometryRecord : public DisGeometryRecord
{
public:
   DisUniformGeometryRecord() = default;
   DisUniformGeometryRecord(const DisUniformGeometryRecord& aSrc);
   DisUniformGeometryRecord(GenI& aGenI);
   ~DisUniformGeometryRecord() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool IsValid() const override;

   virtual DisUniformGeometryRecord* Clone() const override;

   // Getters
   DisUint8 GetField();
   // Setters
   void SetField(DisUint8 aField);

   // Logging

   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI) override;
   //! Should be set to zero initially
   DisUint8  mField    = 0;
   DisUint32 mPadding1 = 0;
   DisUint16 mPadding2 = 0;
   DisUint8  mPadding3 = 0;

   //! 1 Octets - no, 64 bit sections, needs padding
   static const DisUint16 cMIN_LENGTH_OCTETS = 8;
};
#endif
