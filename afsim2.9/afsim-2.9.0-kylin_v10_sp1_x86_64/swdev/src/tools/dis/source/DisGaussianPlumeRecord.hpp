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

#ifndef DISGAUSSIANPLUMERECORD_HPP
#define DISGAUSSIANPLUMERECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisPointRecord.hpp"

//! A point in space
class DIS_EXPORT DisGaussianPlumeRecord : public DisPointRecord
{
public:
   DisGaussianPlumeRecord();
   DisGaussianPlumeRecord(const DisGaussianPlumeRecord& aSrc);
   DisGaussianPlumeRecord(GenI& aGenI);
   ~DisGaussianPlumeRecord() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool IsValid() const override;

   virtual DisGaussianPlumeRecord* Clone() const override;

   // Getters
   void       GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const;
   DisFloat32 GetPlumeLength() const;
   DisFloat32 GetPlumeWidth() const;
   DisFloat32 GetPlumeHeight() const;
   DisFloat32 GetDeltaPlumeLength() const;
   DisFloat32 GetDeltaPlumeWidth() const;
   DisFloat32 GetDeltaPlumeHeight() const;
   DisFloat32 GetLeadingEdgeCentroidHeight() const;
   void       GetLeadingEdgeVelocity(DisFloat32& aXComponent, DisFloat32& aYComponent, DisFloat32& aZComponent) const;
   // Setters
   void SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi);
   void SetPlumeLength(DisFloat32 aPlumeLength);
   void SetPlumeWidth(DisFloat32 aPlumeWidth);
   void SetPlumeHeight(DisFloat32 aPlumeHeight);
   void SetDeltaPlumeLength(DisFloat32 aDeltaPlumeLength);
   void SetDeltaPlumeWidth(DisFloat32 aDeltaPlumeWidth);
   void SetDeltaPlumeHeight(DisFloat32 aDeltaPlumeHeight);
   void SetLeadingEdgeCentroidHeight(DisFloat32 aLeadingEdgeControlType);
   void SetLeadingEdgeVelocity(DisFloat32 aXComponent, DisFloat32 aYComponent, DisFloat32 aZComponent);

   // Logging
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const;

private:
   void       GetMemberData(GenI& aGenI) override;
   DisFloat32 mOrientation[3];
   DisFloat32 mPlumeLength               = 0;
   DisFloat32 mPlumeWidth                = 0;
   DisFloat32 mPlumeHeight               = 0;
   DisFloat32 mDeltaPlumeLength          = 0;
   DisFloat32 mDeltaPlumeWidth           = 0;
   DisFloat32 mDeltaPlumeHeight          = 0;
   DisFloat32 mLeadingEdgeCentroidHeight = 0;
   DisFloat32 mLeadingEdgeVelocity[3];
   DisUint32  mPadding = 0;
   //! Record length is 76 Octets which is 9.5 groups of 64, thus 32 bits of padding are required
   static const DisUint16 cMIN_LENGTH_OCTETS = 56;
};
#endif
