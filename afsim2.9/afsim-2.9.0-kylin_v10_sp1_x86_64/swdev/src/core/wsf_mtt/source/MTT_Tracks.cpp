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

#include "MTT_Tracks.hpp"

#include "SupBlock.hpp"

void MTT_Tracks::ConvertFrom(const SupBlock& aBlock)
{
   mEmbryonicTrackCounter = aBlock.mIntBlock[0];
   mCandidateTrackCounter = aBlock.mIntBlock[2];
   mActiveTrackCounter    = aBlock.mIntBlock[4];
   mAssignmentIdCounter   = aBlock.mIntBlock[6];
}
