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

#ifndef DISLINEARSEGMENTPARAMETER_HPP
#define DISLINEARSEGMENTPARAMETER_HPP

#include "dis_export.h"

#include <iostream>

#include "DisEntityAppearance.hpp"
#include "DisEntityType.hpp"
#include "DisGeneralAppearanceRecord.hpp"
#include "DisObjectType.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/*!
 * The specification of an individual segment of a linear segment synthetic environment object in a Linear
 * Object State PDU shall be represented by a Linear Segment Parameter record.This record shall specify the
 * number of the segment; any modification in the segment’s location or orientation; the dynamic appearance
 * attributes of the segment; the location and orientation of the segment; and the length, height, depth, and
 * width of the segment.
 */
class DIS_EXPORT DisLinearSegmentParameterRecord
{
public:
   DisLinearSegmentParameterRecord();
   DisLinearSegmentParameterRecord(const DisLinearSegmentParameterRecord& aSrc);
   DisLinearSegmentParameterRecord(GenI& aGenI);
   ~DisLinearSegmentParameterRecord() = default;

   // Input/output
   void      Get(GenI& aGenI);
   DisUint16 GetLength();
   void      Put(GenO& aGenO) const;

   // Data Validation
   bool                             IsValid() const;
   DisLinearSegmentParameterRecord* Clone() const;

   // Getters
   DisUint8                         GetSegmentNumber() const;
   DisEnum8                         GetSegmentModification() const;
   const DisGeneralAppearanceRecord GetGeneralSegmentAppearance() const;
   const DisEntityAppearance        GetSpecificSegmentAppearance() const;
   void GetSegmentLocation(DisFloat64& aSegmentLocationX, DisFloat64& aSegmentLocationY, DisFloat64& aSegmentLocationZ) const;
   void       GetSegmentOrientation(DisFloat32& aSegmentOrientationX,
                                    DisFloat32& aSegmentOrientationY,
                                    DisFloat32& aSegmentOrientationZ) const;
   DisFloat32 GetSegmentLength() const;
   DisFloat32 GetSegmentWidth() const;
   DisFloat32 GetSegmentHeight() const;
   DisFloat32 GetSegmentDepth() const;

   // Setters
   void SetSegmentNumber(DisUint8 aSegmentNumber);
   void SetSegmentModification(DisEnum8 aSegmentModifications);
   void SetGeneralSegmentAppearance(const DisGeneralAppearanceRecord& aGeneralSegmentAppearance);
   void SetSpecificSegmentAppearance(const DisEntityAppearance& aSpecificSegmentAppearance);
   void SetSegmentLocation(DisFloat64 aSegmentLocationX, DisFloat64 aSegmentLocationY, DisFloat64 aSegmentLocationZ);
   void SetSegmentOrientation(DisFloat32 aSegmentOrientationX,
                              DisFloat32 aSegmentOrientationY,
                              DisFloat32 aSegmentOrientationZ);
   void SetSegmentLength(DisFloat32 aSegmentLength);
   void SetSegmentWidth(DisFloat32 aSegmentWidth);
   void SetSegmentHeight(DisFloat32 aSegmentHeight);
   void SetSegmentDepth(DisFloat32 aSegmentDepth);

   void        Stream(std::ostream& aStream, DisObjectType aObjectType) const;
   std::string ToString(const DisObjectType& aObjectType) const;

private:
   void GetMemberData(GenI& aGenI);
   //! This field shall identify the individual segment of the linear segment
   DisUint8 mSegmentNumber = 0;
   /*!
    * This field shall identify whether a modification has been made to the point
    * object’s location or orientation
    * mSegmentModification[0] - Is Location Modified: Describes whether the location of the linear segment has been
    * modified since the last update number mSegmentModification[1] - Is Orientation Modified: Describes whether the
    * orientation of the linear segment has been modified since the last update number
    */
   DisEnum8 mSegmentModification = 0;
   //! This field shall specify general dynamic appearance attributes of the segment.
   DisGeneralAppearanceRecord mGeneralSegmentAppearance;
   /*!
    * This field shall specify specific dynamic appearance attributes of the
    * segment
    */
   DisEntityAppearance mSpecificSegmentAppearance;
   //! This field shall specify the location of the linear segment in the simulated world
   DisFloat64 mSegmentLocation[3];

   //! This field shall specify the orientation of the linear segment about the segment location
   DisFloat32 mSegmentOrientation[3];
   //! The length of the linear segment, in meters, extending in the positive x-direction
   DisFloat32 mSegmentLength = 0;
   /*!
    * The total width of the linear segment, in meters, shall be specified by a 32-bit
    * floating point.One half of the width shall extend in the positive y - direction, and one half of the
    * width shall extend in the negative y - direction.
    */
   DisFloat32 mSegmentWidth = 0;
   //! The height of the linear segment, in meters, above ground
   DisFloat32 mSegmentHeight = 0;
   //! The depth of the linear segment, in meters, below ground level
   DisFloat32             mSegmentDepth          = 0;
   DisUint32              mPadding               = 0;
   static const DisUint16 cBASE_LENGTH_IN_OCTETS = 64;
};
#endif
