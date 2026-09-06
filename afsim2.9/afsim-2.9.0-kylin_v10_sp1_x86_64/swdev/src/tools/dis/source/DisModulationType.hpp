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

#ifndef DISMODULATIONTYPE_HPP
#define DISMODULATIONTYPE_HPP

#include "dis_export.h"

#include <iomanip>

#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisModulationType
{
public:
   DisModulationType();
   ~DisModulationType();
   // Using default copy constructor

   // Accessors

   DisUint16 GetSpreadSpectrum() const;
   DisEnum16 GetMajor() const;
   DisEnum16 GetDetail() const;
   DisEnum16 GetSystem() const;

   // Mutators

   void SetSpreadSpectrum(DisUint16 aSpreadSpectrum);
   void SetMajor(DisEnum16 aMajor);
   void SetDetail(DisEnum16 aDetail);
   void SetSystem(DisEnum16 aSystem);

   // Input/output

   void        Get(GenI& aGenI);
   void        Put(GenO& aGenO) const;
   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   bool IsValid() const;
   // Operators

   // Using default operator=
   bool operator==(const DisModulationType& aRhs) const;
   bool operator!=(const DisModulationType& aRhs) const;

private:
   DisUint16 mSpreadSpectrum;
   DisEnum16 mMajor;
   DisEnum16 mDetail;
   DisEnum16 mSystem;
};

// Accessors

inline DisUint16 DisModulationType::GetSpreadSpectrum() const
{
   return mSpreadSpectrum;
}
inline DisEnum16 DisModulationType::GetMajor() const
{
   return mMajor;
}
inline DisEnum16 DisModulationType::GetDetail() const
{
   return mDetail;
}
inline DisEnum16 DisModulationType::GetSystem() const
{
   return mSystem;
}

// Mutators

inline void DisModulationType::SetSpreadSpectrum(DisUint16 aSpreadSpectrum)
{
   mSpreadSpectrum = aSpreadSpectrum;
}
inline void DisModulationType::SetMajor(DisEnum16 aMajor)
{
   mMajor = aMajor;
}
inline void DisModulationType::SetDetail(DisEnum16 aDetail)
{
   mDetail = aDetail;
}
inline void DisModulationType::SetSystem(DisEnum16 aSystem)
{
   mSystem = aSystem;
}

// Inline Operators

inline bool DisModulationType::operator==(const DisModulationType& aRhs) const
{
   // The goofy order is attempt to optimize the code.
   // There should be the most variation in "System".
   return ((mSystem == aRhs.mSystem) && (mDetail == aRhs.mDetail) && (mMajor == aRhs.mMajor) &&
           (mSpreadSpectrum == aRhs.mSpreadSpectrum));
}

inline bool DisModulationType::operator!=(const DisModulationType& aRhs) const
{
   // The goofy order is attempt to optimize the code.
   // There should be the most variation in "System".
   return ((mSystem != aRhs.mSystem) || (mDetail != aRhs.mDetail) || (mMajor != aRhs.mMajor) ||
           (mSpreadSpectrum != aRhs.mSpreadSpectrum));
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisModulationType& aModulationType)
{
   aModulationType.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisModulationType& aModulationType)
{
   aModulationType.Get(aGenI);
   return aGenI;
}

#endif
