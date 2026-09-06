// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "ComponentNameHelper.hpp"

namespace wsf
{
namespace multiresolution
{
std::unordered_map<std::string, bool> ComponentTypeRequiresNameMap()
{
   return detail::AppendComponentTypeMap<wsf::comm::Comm, WsfMover, WsfFuel, WsfSensor, WsfProcessor, WsfAcousticSignature, WsfInfraredSignature, WsfOpticalSignature, WsfRadarSignature>(
      std::unordered_map<std::string, bool>());
}
} // namespace multiresolution
} // namespace wsf
