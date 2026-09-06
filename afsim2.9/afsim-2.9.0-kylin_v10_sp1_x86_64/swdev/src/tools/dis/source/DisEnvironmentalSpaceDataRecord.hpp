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

#ifndef DISENVIRONMENTALSPACEDATARECORD_HPP
#define DISENVIRONMENTALSPACEDATARECORD_HPP

#include "dis_export.h"

#include <iostream>
#include <stdio.h>

#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Purely virtual base class which supports the DisEnvironmentalRecord class as a single type for
 * the 5th field (State or Geomtric records).
 * NOTE: There is no data that will impact the buffer transmission in any way
 */
class DIS_EXPORT DisEnvironmentalSpaceDataRecord
{
public:
   virtual ~DisEnvironmentalSpaceDataRecord() = default;
   // Input/output
   virtual void      Get(GenI& aGenI)       = 0;
   virtual DisUint16 GetLength()            = 0;
   virtual void      Put(GenO& aGenO) const = 0;

   // Data Validation
   virtual bool                             IsValid() const = 0;
   virtual DisEnvironmentalSpaceDataRecord* Clone() const   = 0;

   // Logging
   void                Stream(std::ostream& aStream) const;
   virtual std::string ToString() const = 0;

private:
   virtual void           GetMemberData(GenI& aGenI)    = 0;
   static const DisUint16 cRECORD_BASE_LENGTH_IN_OCTETS = 0;
};
#endif
