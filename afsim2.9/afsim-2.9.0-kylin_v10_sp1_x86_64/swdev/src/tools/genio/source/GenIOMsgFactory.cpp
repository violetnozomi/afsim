// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "GenIOMsgFactory.hpp"

#include "GenI.hpp"
#include "GenIOMsg.hpp"
#include "GenIOMsgConnect.hpp"
#include "GenIOMsgStatus.hpp"


// Inherits from GenIFactory

GenIOMsgFactory::GenIOMsgFactory() {}

GenIOMsgFactory::~GenIOMsgFactory() {}

GenIOMsg* GenIOMsgFactory::Create(GenI& genIA)
{
   // Read the base class off of the stream
   GenIOMsg genIOMsg(genIA);

   // Look For errors
   if (genIA.GetInputStatus() != GenBuf::NoError)
   {
      // No need to go any futher
      auto out = ut::log::error() << "GenIOMsgFactory: Input status is not NoError.";
      genIOMsg.HandleReadError(out);

      // Buffer clean up will be done by GenIFactory
      return nullptr;
   }

   // Double check we got a good message
   if (genIOMsg.GetFamilyType() != GenMsg::TypeGenIO)
   {
      return nullptr;
   }
   else
   {
      switch (genIOMsg.GetType())
      {
      case GenIOMsg::TypeConnect:
         return CreateGenIOMsgConnect(genIOMsg, genIA);
      case GenIOMsg::TypeStatus:
         return CreateGenIOMsgStatus(genIOMsg, genIA);
      default:
         return nullptr;
      }
   }
}

// Virtual
GenIOMsg* GenIOMsgFactory::CreateGenIOMsgConnect(const GenIOMsg& genIOMsgA, GenI& genIA)
{
   return new GenIOMsgConnect(genIOMsgA, genIA);
}

// Virtual
GenIOMsg* GenIOMsgFactory::CreateGenIOMsgStatus(const GenIOMsg& genIOMsgA, GenI& genIA)
{
   return new GenIOMsgStatus(genIOMsgA, genIA);
}
