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

#ifndef VACHOOSER_HPP
#define VACHOOSER_HPP

#include "VaDefs.hpp"

namespace vespa
{
class VaChooser
{
public:
   virtual VaHitEntry Choose(
      const std::map<VaHitEntry, std::string>& aChoiceList,
      int                                      aX,
      int                                      aY,
      const std::function<void(VaHitEntry)>* aHoverFunction = nullptr) const = 0; // allow a user to choose from a list
};
} // namespace vespa
#endif
