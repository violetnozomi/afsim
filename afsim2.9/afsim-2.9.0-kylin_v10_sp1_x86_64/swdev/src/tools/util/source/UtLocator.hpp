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

#ifndef UTLOCATOR_HPP
#define UTLOCATOR_HPP

#include <memory>

#include "UtException.hpp"

namespace ut
{

template<class Service>
class Locator
{
public:
   Locator() = delete;

   static Service& get()
   {
      if (service.expired())
      {
         throw UtException("No service available: Locator<");
      }
      return *(service.lock());
   }

   static void provide(const std::shared_ptr<Service>& aService) { service = aService; }

private:
   static std::weak_ptr<Service> service;
};

template<class Service>
std::weak_ptr<Service> Locator<Service>::service;

} // namespace ut

#endif
