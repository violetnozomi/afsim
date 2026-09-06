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

#ifndef DISGEOMETRYRECORD_HPP
#define DISGEOMETRYRECORD_HPP

#include "dis_export.h"

#include "DisEnvironmentalSpaceDataRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Base class for all Geometry State Records found specifically in version 21 of the
 * SISO-REF-010-2015-Ref_Enums-Version-21.pdf
 */
class DIS_EXPORT DisGeometryRecord : public DisEnvironmentalSpaceDataRecord
{
public:
   // Input/output
   virtual void      Get(GenI& aGenI) override       = 0;
   virtual DisUint16 GetLength() override            = 0;
   virtual void      Put(GenO& aGenO) const override = 0;

   // Data Validation
   virtual bool IsValid() const override = 0;

   virtual DisGeometryRecord* Clone() const override = 0;

   // Logging
   void                Stream(std::ostream& aStream) const;
   virtual std::string ToString() const override = 0;

private:
   virtual void GetMemberData(GenI& aGenI) override = 0;
};
#endif
