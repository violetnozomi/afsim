// ****************************************************************************
// CUI//REL TO USA ONLY
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

#ifndef OUTPUTITEM_HPP
#define OUTPUTITEM_HPP

#include <iosfwd>
#include <string>

class UtInput;


namespace engage
{

class Simulation;

//! This represents a single item in an output record.
//!
//! While something more complex could be implement, it was kept simple so it could be maintained
//! in STL containers and could be easily copy constructed. The aren't all that many of these so
//! space really isn't an issue.
class OutputItem
{
public:
   enum ItemType
   {
      cREAL_VARIABLE,
      cINTEGER_VARIABLE,
      cSTRING_VARIABLE,
      cLITERAL,
      cNEW_LINE
   };

   OutputItem();

   bool ProcessInput(UtInput& aInput);

   void SetRealVariable(size_t aVariableIndex, double aScaleFactor)
   {
      mItemType      = cREAL_VARIABLE;
      mVariableIndex = aVariableIndex;
      mScaleFactor   = aScaleFactor;
   }

   void DoOutput(std::ostream& aStream, Simulation& aSimulation) const;

   ItemType    mItemType;
   size_t      mVariableIndex;
   std::string mFormat;
   std::string mLiteral;
   double      mScaleFactor;
};

} // namespace engage

#endif
