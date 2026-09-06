// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFUCI_MESSAGEFACTORY_HPP
#define WSFUCI_MESSAGEFACTORY_HPP

#include <map>
#include <memory>

#include "uci/type/MessageType.h"
#include "UtMemory.hpp"
#include "WsfMessage.hpp"

namespace wsf
{

class UCI_Interface;
class UCI_MessageBaseType;

//! A simple factory to create new UCI messages based on the type found in the header.
class UCI_MessageFactory
{
   public:
      explicit UCI_MessageFactory(UCI_Interface& aInterface);
      ~UCI_MessageFactory();

      std::unique_ptr<UCI_MessageBaseType> Create(const uci::type::MessageType* aMessage) const;

   private:

      UCI_Interface& mInterface;
};

} // wsf
#endif
