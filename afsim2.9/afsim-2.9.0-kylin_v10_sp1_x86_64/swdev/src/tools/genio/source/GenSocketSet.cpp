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
#include "GenSocketSet.hpp"

#include <algorithm>

namespace GenSockets
{

void GenSocketSet::AddSocket(GenSocket* aSocket)
{
   mSockets.push_back(aSocket);
}

void GenSocketSet::RemoveSocket(GenSocket* aSocket)
{
   mSockets.resize(std::remove(mSockets.begin(), mSockets.end(), aSocket) - mSockets.begin());
}

} // namespace GenSockets
