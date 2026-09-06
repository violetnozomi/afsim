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

#ifndef WSFFIELDOFVIEWTYPES_HPP
#define WSFFIELDOFVIEWTYPES_HPP

#include "wsf_export.h"

#include <memory>

class WsfFieldOfView;
#include "WsfStringId.hpp"

class WSF_EXPORT WsfFieldOfViewTypes
{
public:
   static std::unique_ptr<WsfFieldOfView> Create(WsfStringId aType);
};

#endif
