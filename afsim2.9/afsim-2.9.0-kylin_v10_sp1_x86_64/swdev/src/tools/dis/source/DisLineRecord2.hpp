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

#ifndef DISLINERECORD2_HPP
#define DISLINERECORD2_HPP

#include "dis_export.h"

#include <iostream>

#include "DisGeometryRecord.hpp"
#include "DisPointRecord2.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines a straight line between two points with negligible width and height with
 * both points moving
 */
class DIS_EXPORT DisLineRecord2 : public DisGeometryRecord
{
public:
   DisLineRecord2();
   DisLineRecord2(const DisLineRecord2& aSrc);
   DisLineRecord2(GenI& aGenI);
   ~DisLineRecord2() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool IsValid() const override;

   virtual DisLineRecord2* Clone() const override;

   // Getters
   DisPointRecord2& GetStartPointLocation();
   DisPointRecord2& GetEndPointLocation();
   void             GetStartPointVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   void             GetEndPointVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;

   // Setters
   void SetStartPointLocation(const DisPointRecord2& aStartPointLocation);
   void SetEndPointLocation(const DisPointRecord2& aEndPointLocation);
   void SetStartPointVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);
   void SetEndPointVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);

   // Logging
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI) override;
   //! Line start point
   DisPointRecord2 mStartPointLocation;
   //! Line end point
   DisPointRecord2 mEndPointLocation;
   DisFloat32      mStartPointVelocity[3];
   DisFloat32      mEndPointVelocity[3];
   //! 72 Octets - 9 64 bit sectiions, no need for padding
   static const DisUint16 cLENGTH_IN_OCTETS = 72;
};
#endif
