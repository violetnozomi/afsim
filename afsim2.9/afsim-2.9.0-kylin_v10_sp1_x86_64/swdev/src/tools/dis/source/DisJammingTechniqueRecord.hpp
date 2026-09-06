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

#ifndef DisJammingTechniqueRecord_hpp
#define DisJammingTechniqueRecord_hpp

#include "dis_export.h"

// An overloaded operator<<(ostream& aOut, const DisJammingTechniqueRecord)
// function to write the contents of this class to an ostream can be found in
// DisIO.hpp.  This function is not defined here due to portability issues
// dealing with the old and new I/O stream libraries (i.e.: one typically can't
// use <iostream.h> and <iostream> in the same application).  See DisIO.hpp for
// more details.

#include <iomanip>

#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisJammingTechniqueRecord
{
public:
   //! Default Constructor
   DisJammingTechniqueRecord();
   //! Initialized Constructor
   DisJammingTechniqueRecord(DisEnum8 aKind, DisEnum8 aCategory, DisEnum8 aSubcategory, DisEnum8 aSpecific);
   //! Initialized Constructor (from legacy)
   DisJammingTechniqueRecord(DisUint32 aJammingModeSequence);
   //! Copy Constructor
   DisJammingTechniqueRecord(const DisJammingTechniqueRecord& aSrc);
   //! Destructor
   ~DisJammingTechniqueRecord() = default;

   // Accessors
   DisEnum8 GetKind() const;
   DisEnum8 GetCategory() const;
   DisEnum8 GetSubcategory() const;
   DisEnum8 GetSpecific() const;

   // Mutators
   void SetKind(DisEnum8 aKind);
   void SetCategory(DisEnum8 aCategory);
   void SetSubcategory(DisEnum8 aSubcategory);
   void SetSpecific(DisEnum8 aSpecific);

   // Input/output
   void        Get(GenI& aGenI);
   void        Put(GenO& aGenO) const;
   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;
   // Data Validation
   bool IsValid() const;

   // Operators
   DisJammingTechniqueRecord& operator=(const DisJammingTechniqueRecord& aRhs);
   DisJammingTechniqueRecord& operator=(const DisUint32& aRhs);
   bool                       operator==(const DisJammingTechniqueRecord& aRhs) const;
   bool                       operator<(const DisJammingTechniqueRecord& aRhs) const;

   // This operator is provided so convert this vale to the legacy value
   DisUint32 operator()() const;

private:
   DisEnum8 mKind;
   DisEnum8 mCategory;
   DisEnum8 mSubcategory;
   DisEnum8 mSpecific;
};

// Accessors

inline DisEnum8 DisJammingTechniqueRecord::GetKind() const
{
   return mKind;
}
inline DisEnum8 DisJammingTechniqueRecord::GetCategory() const
{
   return mCategory;
}
inline DisEnum8 DisJammingTechniqueRecord::GetSubcategory() const
{
   return mSubcategory;
}
inline DisEnum8 DisJammingTechniqueRecord::GetSpecific() const
{
   return mSpecific;
}

// Mutators

inline void DisJammingTechniqueRecord::SetKind(DisEnum8 aKind)
{
   mKind = aKind;
}
inline void DisJammingTechniqueRecord::SetCategory(DisEnum8 aCategory)
{
   mCategory = aCategory;
}
inline void DisJammingTechniqueRecord::SetSubcategory(DisEnum8 aSubcategory)
{
   mSubcategory = aSubcategory;
}
inline void DisJammingTechniqueRecord::SetSpecific(DisEnum8 aSpecific)
{
   mSpecific = aSpecific;
}

// Inline Operators

inline bool DisJammingTechniqueRecord::operator==(const DisJammingTechniqueRecord& aRhs) const
{
   return ((mKind == aRhs.mKind) && (mCategory == aRhs.mCategory) && (mSubcategory == aRhs.mSubcategory) &&
           (mSpecific == aRhs.mSpecific));
}

inline bool DisJammingTechniqueRecord::operator<(const DisJammingTechniqueRecord& aRhs) const
{
   DisUint32 l = (*this)();
   return (l < aRhs());
}

inline DisUint32 DisJammingTechniqueRecord::operator()() const
{
   return DisUint32((static_cast<DisUint32>(mKind) << 24) | (static_cast<DisUint32>(mCategory) << 16) |
                    (static_cast<DisUint32>(mSubcategory) << 8) | static_cast<DisUint32>(mSpecific));
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisJammingTechniqueRecord& aRecord)
{
   aRecord.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisJammingTechniqueRecord& aRecord)
{
   aRecord.Get(aGenI);
   return aGenI;
}

#endif
