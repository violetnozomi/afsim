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

#ifndef WSFCOMMROUTERPROTOCOLTYPES_HPP
#define WSFCOMMROUTERPROTOCOLTYPES_HPP

#include "WsfCommRouter.hpp"
#include "WsfCommRouterProtocolInterface.hpp"
#include "WsfObjectTypeList.hpp"

class WsfScenario;

namespace wsf
{
namespace comm
{
namespace router
{

class ProtocolTypes : public WsfObjectTypeList<ProtocolInterface>
{
public:
   static WSF_EXPORT ProtocolTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const ProtocolTypes& Get(const WsfScenario& aScenario);

   explicit ProtocolTypes(WsfScenario& aScenario);

   //! The base load/unload component methods always assume usage
   //! of WsfPlatform, so we have to redefine our own.
   bool LoadProtocolComponent(UtInput& aInput, Router& aRouter, bool aIsAdding, int aRole, bool aIsEditable) const;
};

} // namespace router
} // namespace comm
} // namespace wsf

#endif
