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

#ifndef DISSYSTEMID_HPP
#define DISSYSTEMID_HPP

#include "dis_export.h"

#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisSystemId
{
public:
   DisSystemId();
   ~DisSystemId();
   // Using default copy constructor

   // Accessors

   DisEnum16 GetSystemType() const;
   DisEnum16 GetSystemName() const;
   DisEnum8  GetSystemMode() const;
   DisEnum8  GetChangeOptions() const;

   // Mutators

   void SetSystemType(DisEnum16 aSystemType);
   void SetSystemName(DisEnum16 aSystemName);
   void SetSystemMode(DisEnum8 aSystemMode);
   void SetChangeOptions(DisEnum8 aChangeOptions);

   // Input/Output

   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

private:
   DisEnum16 mSystemType;
   DisEnum16 mSystemName;
   DisEnum8  mSystemMode;
   DisEnum8  mChangeOptions;
};

// Accessors

inline DisEnum16 DisSystemId::GetSystemType() const
{
   return mSystemType;
}
inline DisEnum16 DisSystemId::GetSystemName() const
{
   return mSystemName;
}
inline DisEnum8 DisSystemId::GetSystemMode() const
{
   return mSystemMode;
}
inline DisEnum8 DisSystemId::GetChangeOptions() const
{
   return mChangeOptions;
}

// Mutators

inline void DisSystemId::SetSystemType(DisEnum16 aSystemType)
{
   mSystemType = aSystemType;
}
inline void DisSystemId::SetSystemName(DisEnum16 aSystemName)
{
   mSystemName = aSystemName;
}
inline void DisSystemId::SetSystemMode(DisEnum8 aSystemMode)
{
   mSystemMode = aSystemMode;
}
inline void DisSystemId::SetChangeOptions(DisEnum8 aChangeOptions)
{
   mChangeOptions = aChangeOptions;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisSystemId& aSystemId)
{
   aSystemId.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisSystemId& aSystemId)
{
   aSystemId.Get(aGenI);
   return aGenI;
}

#endif
