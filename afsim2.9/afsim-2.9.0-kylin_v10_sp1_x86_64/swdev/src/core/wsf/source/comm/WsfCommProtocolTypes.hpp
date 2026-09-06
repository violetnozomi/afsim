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

#ifndef WSFCOMMPROTOCOLTYPES_HPP
#define WSFCOMMPROTOCOLTYPES_HPP

#include "WsfCommProtocolInterface.hpp"
#include "WsfObjectTypeList.hpp"

class WsfScenario;

namespace wsf
{
namespace comm
{

class ProtocolTypes : public WsfObjectTypeList<ProtocolInterface>
{
public:
   static WSF_EXPORT ProtocolTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const ProtocolTypes& Get(const WsfScenario& aScenario);

   explicit ProtocolTypes(WsfScenario& aScenario);

   //! The base load/unload component methods always assume usage
   //! of WsfPlatform, so we have to redefine our own.
   bool LoadProtocolComponent(UtInput& aInput, Comm& aComm, bool aIsAdding, int aRole, bool aIsEditable) const;
};

} // namespace comm
} // namespace wsf

#endif
