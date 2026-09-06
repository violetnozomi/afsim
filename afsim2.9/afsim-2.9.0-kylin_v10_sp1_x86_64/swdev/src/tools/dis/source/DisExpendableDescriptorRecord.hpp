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

#ifndef DIS_EXPENDABLEDESCRIPTORRECORD_HPP
#define DIS_EXPENDABLEDESCRIPTORRECORD_HPP

#include "dis_export.h"

#include <string>

#include "DisDescriptorBaseRecord.hpp"
#include "DisEntityType.hpp"
#include "DisTypes.hpp"


class GenI;
class GenO;

/**!
 * Defines a record containing information for a burst of chaff (expendable) or the
 * ignition or a flare.
 */
class DIS_EXPORT DisExpendableDescriptorRecord : public DisDescriptorBaseRecord
{
public:
   DisExpendableDescriptorRecord();
   DisExpendableDescriptorRecord(const DisExpendableDescriptorRecord& aSrc);
   explicit DisExpendableDescriptorRecord(GenI& aGenI);
   ~DisExpendableDescriptorRecord() = default;

   // Input/output
   DisUint16 GetLength() const override;
   void      Get(GenI& aGenI) override;
   void      Put(GenO& aGenO) const override;

   // Validate Data
   bool IsValid() const override;

   std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI) override;

   DisUint64 mPadding = 0;

   const DisUint16 cMIN_RECORD_LENGTH_OCTETS = 8;
};
#endif
