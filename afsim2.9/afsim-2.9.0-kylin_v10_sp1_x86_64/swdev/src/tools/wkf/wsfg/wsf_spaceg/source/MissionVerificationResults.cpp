// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "MissionVerificationResults.hpp"

#include <algorithm>

namespace wsfg
{

namespace spaceg
{

//! Return the VerificationResults for the event with the given index.
//!
//! Return the mission verification results for the event with the given
//! @p aOrderedIndex. The @p aOrderedIndex is the order of the event in a
//! pre-order traversal of the mission sequence.
//!
//! \param aOrderedIndex - the ordered index of the event in question.
//! \returns             - the verification results for the event in question.
VerificationResults MissionVerificationResults::GetResults(int aOrderedIndex) const
{
   VerificationResults retval{};

   auto iter = mResults.find(aOrderedIndex);
   if (iter != mResults.end())
   {
      retval = iter->second;
   }

   return retval;
}

//! Set the verification results for the event with the given index.
//!
//! This will set the verification results of the event with the given @p aOrderedIndex
//! to be equal to the given @p aResults. The @p aOrderedIndex is the order of
//! the event in a pre-order traversal of the mission sequence.
//!
//! \param aOrderedIndex - the ordered index of the event in question.
//! \param aResults      - the verification results of the event in question.
void MissionVerificationResults::SetResults(int aOrderedIndex, const VerificationResults& aResults)
{
   auto iter = mResults.find(aOrderedIndex);
   if (iter == mResults.end())
   {
      mResults[aOrderedIndex] = aResults;
   }
   else
   {
      iter->second = aResults;
   }
}

} // namespace spaceg

} // namespace wsfg
