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


#ifndef GENIO_MSG_FACTORY_HPP
#define GENIO_MSG_FACTORY_HPP

#include "genio_export.h"

#include "GenIFactory.hpp"
#include "GenIOMsg.hpp"

class GenI;

class GENIO_EXPORT GenIOMsgFactory : public GenIFactory<GenIOMsg>
{
public:
   GenIOMsgFactory();
   ~GenIOMsgFactory() override;

   GenIOMsg* Create(GenI& genIA) override;

private:
   virtual GenIOMsg* CreateGenIOMsgConnect(const GenIOMsg& genIOMsgA, GenI& genIA);
   virtual GenIOMsg* CreateGenIOMsgStatus(const GenIOMsg& genIOMsgA, GenI& genIA);
};

#endif
