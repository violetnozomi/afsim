// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "WsfTrackClassifier.hpp"

TEST(WsfClassifierTabulated, GetTrackAccessor)
{
   WsfClassifierTabulated classifier;
   WsfTrack               track;
   auto                   accessor = classifier.GetTrackAccessor(track);
   EXPECT_EQ(0, accessor->GetTrueTypeID());
   delete accessor;
}
