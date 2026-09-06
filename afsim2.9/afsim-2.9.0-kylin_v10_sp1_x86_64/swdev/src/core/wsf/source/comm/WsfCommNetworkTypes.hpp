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

#ifndef WSFCOMMNETWORKTYPES_HPP
#define WSFCOMMNETWORKTYPES_HPP

#include "WsfCommNetwork.hpp"
#include "WsfObjectTypeList.hpp"
class WsfScenario;

namespace wsf
{
namespace comm
{

class NetworkTypes : public WsfObjectTypeList<Network>
{
public:
   static WSF_EXPORT NetworkTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const NetworkTypes& Get(const WsfScenario& aScenario);

   explicit NetworkTypes(WsfScenario& aScenario);
};

} // namespace comm
} // namespace wsf

#endif
