// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef DISENTITYPART_HPP
#define DISENTITYPART_HPP

#include "dis_export.h"

#include "DisEntityType.hpp"
#include "DisVariableParameterRecord.hpp"

class GenI;
class GenO;

/**!
 * Defines an DIS supported Entity Part
 */
class DIS_EXPORT DisEntityPart : public DisVariableParameterRecord
{
public:
   DisEntityPart();
   DisEntityPart(const DisEntityPart& aSrc);
   DisEntityPart(const DisVariableParameterRecord& aVariableParameterRecord, GenI& aGenI);
   ~DisEntityPart() = default;

   DisEntityPart* Clone() const override;

   // Accessors

   DisUint8             GetChangeIndicator() const;
   DisUint16            GetPartAttachedTo() const;
   DisUint32            GetParameterType() const;
   DisFloat32           GetFloat32Value() const;
   const DisEntityType& GetEntityTypeValue() const;

   // Mutators

   void SetChangeIndicator(DisUint8 aChangeIndicator);
   void SetPartAttachedTo(DisUint16 aPartAttachedTo);
   void SetParameterType(DisUint32 aParameterType);
   void SetFloat32Value(DisFloat32 aFloat32);
   void SetEntityTypeValue(const DisEntityType& aEntityType);

   // Input/output

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) const override;

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Operators

   DisEntityPart& operator=(const DisEntityPart& aRhs);
   bool           operator==(const DisVariableParameterRecord& aRhs) override;

   // Data Validation
   bool IsValid() const;

   enum
   {
      TypeArticulated = 0,
      TypeAttached    = 1
   };

   enum
   {
      ParameterPosition      = 1,
      ParameterPositionRate  = 2,
      ParameterExtention     = 3,
      ParameterExtentionRate = 4,
      ParameterX             = 5,
      ParameterX_Rate        = 6,
      ParameterY             = 7,
      ParameterY_Rate        = 8,
      ParameterZ             = 9,
      ParameterZ_Rate        = 10,
      ParameterAzimuth       = 11,
      ParameterAzimuthRate   = 12,
      ParameterElevation     = 13,
      ParameterElevationRate = 14,
      ParameterRotation      = 15,
      ParameterRotationRate  = 16
   };

private:
   void GetMemberData(GenI& aGenI) override;

   DisUint8  mChangeIndicator = 0;
   DisUint16 mPartAttachedTo  = 0;
   DisUint32 mParameterType   = 0;

   // Ideally we should define a union (or perhaps use a subclass)
   // that defines the type of the parameter value.  The problem is
   // that C++ does not allow a class to be a member of a union if
   // it has a user-define constructor, destructor or assignment
   // operator, which rules out DisEntityType.  Therefore, we'll
   // waste some space for now...

   union
   {
      DisFloat32 mFloat32[2];
      DisUint32  mUnknown[2];
   } mNumericValue;
   DisEntityType mEntityType;
};

// Accessors
inline DisUint8 DisEntityPart::GetChangeIndicator() const
{
   return mChangeIndicator;
}

inline DisUint16 DisEntityPart::GetPartAttachedTo() const
{
   return mPartAttachedTo;
}

inline DisUint32 DisEntityPart::GetParameterType() const
{
   return mParameterType;
}

inline DisFloat32 DisEntityPart::GetFloat32Value() const
{
   return mNumericValue.mFloat32[0];
}

inline const DisEntityType& DisEntityPart::GetEntityTypeValue() const
{
   return mEntityType;
}

// Mutators

inline void DisEntityPart::SetChangeIndicator(DisUint8 aChangeIndicator)
{
   mChangeIndicator = aChangeIndicator;
}

inline void DisEntityPart::SetPartAttachedTo(DisUint16 aPartAttachedTo)
{
   mPartAttachedTo = aPartAttachedTo;
}

inline void DisEntityPart::SetParameterType(DisUint32 aParameterType)
{
   mParameterType = aParameterType;
}

inline void DisEntityPart::SetFloat32Value(DisFloat32 aFloat32)
{
   mNumericValue.mFloat32[0] = aFloat32;
}

inline void DisEntityPart::SetEntityTypeValue(const DisEntityType& aEntityType)
{
   mEntityType = aEntityType;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisEntityPart& aPart)
{
   aPart.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisEntityPart& aPart)
{
   aPart.Get(aGenI);
   return aGenI;
}

#endif
