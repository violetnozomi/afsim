// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtInput.hpp"
#include "WsfMessageTable.hpp"

TEST(WsfMessageTable, MessageInputSizeLarge)
{
   WsfMessageTable msgTable;
   UtInput         input;
   input.PushInputString("default 2 gbytes");
   input.PushInputString("comm_type DATALINK");
   input.PushInputString("message_table");
   EXPECT_THROW(msgTable.ProcessInput(input), UtInput::BadValue);
}
