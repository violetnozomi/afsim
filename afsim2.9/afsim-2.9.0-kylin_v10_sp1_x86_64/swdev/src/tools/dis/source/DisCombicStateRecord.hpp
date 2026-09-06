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

#ifndef DISCOMBICSTATERECORD_HPP
#define DISCOMBICSTATERECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisEntityType.hpp"
#include "DisEnvironmentalStateRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines a Combic state record as defined by the SISO-REF-010-2019.pdf (pg. 592)
 */
class DIS_EXPORT DisCombicStateRecord : public DisEnvironmentalStateRecord
{
public:
   DisCombicStateRecord();
   DisCombicStateRecord(const DisCombicStateRecord& aSrc);
   DisCombicStateRecord(GenI& aGenI);
   ~DisCombicStateRecord() override = default;

   // Input/output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) const override;

   // Getters
   DisUint32      GetTimeSinceCreation() const;
   DisEntityType& GetMunitionSource();
   DisInt32       GetNumberOfSources() const;
   DisUint16      GetGeometryIndex() const;
   DisFloat32     GetSourceType() const;
   DisFloat32     GetBarrageRate() const;
   DisFloat32     GetBarrageDuration() const;
   DisFloat32     GetBarrageCrosswindLength() const;
   DisFloat32     GetBarrageDownwindLength() const;
   void           GetDetonationVelocity(DisFloat32& aDetonationVelocityX,
                                        DisFloat32& aDetonationVelocityY,
                                        DisFloat32& aDetonationVelocityZ);

   // Setters
   void SetTimeSinceCreation(DisUint32 aTimeSinceCreation);
   void SetMunitionSource(const DisEntityType& aMunitionSource);
   void SetNumberOfSources(const DisInt32 aNumberOfSources);
   void SetGeometryIndex(DisUint16 aGeometryIndex);
   void SetSourceType(DisFloat32 DisInt32);
   void SetBarrageRate(DisFloat32 aBarrageRate);
   void SetBarrageDuration(DisFloat32 aBarrageDuration);
   void SetBarrageCrosswindLength(DisFloat32 aBarrageCrosswindLength);
   void SetBarrageDownwindLength(DisFloat32 aBarrageDownwindLength);
   void SetDetonationVelocity(DisFloat32 aDetonationVelocityX,
                              DisFloat32 aDetonationVelocityY,
                              DisFloat32 aDetonationVelocityZ);

   // Data Validation
   bool                  IsValid() const override;
   DisCombicStateRecord* Clone() const override;

   // Logging
   void        Stream(std::ostream& aStream) const;
   std::string ToString() const override;

private:
   void                   GetMemberData(GenI& aGenI) override;
   DisUint32              mTimeSinceCreation = 0;
   DisEntityType          mMunitionSource;
   DisInt32               mNumberOfSources        = 0;
   DisUint16              mGeometryIndex          = 0;
   DisUint16              mPadding                = 0;
   DisFloat32             mSourceType             = 0;
   DisFloat32             mBarrageRate            = 0;
   DisFloat32             mBarrageDuration        = 0;
   DisFloat32             mBarrageCrosswindLength = 0;
   DisFloat32             mBarrageDownwindLength  = 0;
   DisFloat32             mDetonationVelocity[3];
   DisUint32              mPadding2          = 0;
   static const DisUint16 cMIN_LENGTH_OCTETS = 56;
};
#endif
