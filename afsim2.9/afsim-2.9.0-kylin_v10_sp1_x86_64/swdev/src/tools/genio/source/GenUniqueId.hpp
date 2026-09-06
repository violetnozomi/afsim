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
#ifndef GENUNIQUEID_HPP
#define GENUNIQUEID_HPP

#include "genio_export.h"

#include <string>
//! A 96 bit Unique ID.  Uses the following data to attempt uniqueness:
//! - Computer IP Address
//! - Thread ID
//! - Time of day
//! - Counter
class GENIO_EXPORT GenUniqueId
{
public:
   static const int cSIZE = 12;

   GenUniqueId();

   GenUniqueId(unsigned int aA, unsigned int aB, unsigned int aC);

   GenUniqueId(const GenUniqueId& aSrc);

   GenUniqueId& operator=(const GenUniqueId& aSrc);

   const char* GetData() const;

   unsigned int GetData(int aWord) const;

   bool operator<(const GenUniqueId& aRHS) const;

   bool operator==(const GenUniqueId& aRHS) const;

   template<typename T>
   void Serialize(T& aBuff)
   {
      if (T::cIS_OUTPUT)
      {
         Validate();
      }
      aBuff& mID[0].mValue& mID[1].mValue& mID[2].mValue;
      mIsCreated = true;
   }

   std::string ToString();

private:
   void Validate() const
   {
      if (!mIsCreated)
      {
         Create();
      }
   }
   void         Create() const;
   mutable bool mIsCreated;
   union Word
   {
      char         mBytes[4];
      unsigned int mValue;
   };
   mutable Word mID[3];
};

#endif
