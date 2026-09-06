// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSPACETESTSPACETESTSERVICE_HPP
#define WSFSPACETESTSPACETESTSERVICE_HPP

#include <list>
#include <memory>
#include <string>

#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfScenario.hpp"

class SpaceTestService
{
public:
   SpaceTestService()
   {
      int   argc = 0;
      char* argv[1];

      mApplicationPtr =
         ut::make_unique<WsfApplication>("space_test_app", argc, argv, std::list<std::string>{"no_plugins"});
      WSF_REGISTER_EXTENSION(*mApplicationPtr, ext_interface);
      WSF_REGISTER_EXTENSION(*mApplicationPtr, wsf_mil);
      WSF_REGISTER_EXTENSION(*mApplicationPtr, wsf_space);

      mScenarioPtr = ut::make_unique<WsfScenario>(*mApplicationPtr);
   }

   ~SpaceTestService() = default;

   WsfApplication& GetApplication() const { return *mApplicationPtr; }
   WsfScenario&    GetScenario() const { return *mScenarioPtr; }

private:
   std::unique_ptr<WsfApplication> mApplicationPtr{nullptr};
   std::unique_ptr<WsfScenario>    mScenarioPtr{nullptr};
};

#endif // WSFSPACETESTSPACETESTSERVICE_HPP
