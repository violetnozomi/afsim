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

#ifndef DISVECTORINGNOZZLESYSTEMDATARECORD_HPP
#define DISVECTORINGNOZZLESYSTEMDATARECORD_HPP

#include "DisRecordHeader.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**
 * Record specifying Vectoring Nozzle deflection system data
 */
class DIS_EXPORT DisVectoringNozzleSystemDataRecord
{
public:
   DisVectoringNozzleSystemDataRecord() = default;
   DisVectoringNozzleSystemDataRecord(GenI& aGenI);
   DisVectoringNozzleSystemDataRecord(const DisVectoringNozzleSystemDataRecord& aSrc);
   virtual ~DisVectoringNozzleSystemDataRecord() = default;

   DisVectoringNozzleSystemDataRecord* Clone() const;

   // Getters
   DisFloat32 GetHorizontalDeflectionAngle() const;
   DisFloat32 GetVerticalDeflectionAngle() const;
   DisUint16  GetLength();
   // Setters

   void SetHorizontalDeflectionAngle(DisFloat32 aHorizontalDeflectionAngleValue);
   void SetVerticalDeflectionAngle(DisFloat32 aVerticalDeflectionAngleValue);

   std::string ToString() const;
   void        Stream(std::ostream& aStream) const;

   // Input / Output
   virtual void Get(GenI& aGenI);
   virtual void Put(GenO& aGenO);

   bool IsValid() const;

private:
   void GetMemberData(GenI& aGenI);

   //! Nozzle deflection angle (in degrees) in the horizontal axis
   DisFloat32 mHorizontalDeflectionAngle = 0;

   //! nozzle deflection angle (in degrees) in the vertical axis
   DisFloat32 mVerticalDeflectionAngle = 0;
   DisUint16  cLENGTH                  = 8;
};
#endif
