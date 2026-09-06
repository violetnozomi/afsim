// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITALREFERENCEFRAME_HPP
#define WSFORBITALREFERENCEFRAME_HPP

#include "wsf_space_export.h"

#include <string>

namespace wsf
{
namespace space
{

enum class OrbitalReferenceFrame
{
   cINERTIAL,
   cRIC,
   cNTW,
   cUNKNOWN
};

std::string WSF_SPACE_EXPORT OrbitalReferenceFrameToIdentifier(OrbitalReferenceFrame aFrame);

OrbitalReferenceFrame WSF_SPACE_EXPORT OrbitalReferenceFrameFromIdentifier(const std::string& aIdentifier);

} // namespace space
} // namespace wsf

#endif // WSFORBITALREFERENCEFRAME_HPP
