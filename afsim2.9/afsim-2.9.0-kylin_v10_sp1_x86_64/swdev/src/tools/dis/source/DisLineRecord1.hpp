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

#ifndef DISLINERECORD1_HPP
#define DISLINERECORD1_HPP

#include "dis_export.h"

#include <iostream>

#include "DisGeometryRecord.hpp"
#include "DisPointRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines a straight line between two points with negligible width and height
 */
class DIS_EXPORT DisLineRecord1 : public DisGeometryRecord
{
public:
   DisLineRecord1() = default;
   DisLineRecord1(const DisLineRecord1& aSrc);
   DisLineRecord1(GenI& aGenI);
   ~DisLineRecord1() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool IsValid() const override;

   virtual DisLineRecord1* Clone() const override;

   // Getters
   DisPointRecord& GetStartPointLocation();
   DisPointRecord& GetEndPointLocation();

   // Setters
   void SetStartPointLocation(const DisPointRecord& aStartPointLocation);
   void SetEndPointLocation(const DisPointRecord& aEndPointLocation);

   // Logging
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI) override;
   //! Line start point
   DisPointRecord mStartPointLocation;
   //! Line end point
   DisPointRecord mEndPointLocation;
   //! 48 Octets - 6, 64 bit sections, no need to pad this PDU
   static const DisUint16 cLENGTH_IN_OCTETS = 48;
};
#endif
