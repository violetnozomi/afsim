// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoModelSwitchImp.hpp : header file

#if !defined(_UTOMODELSWITCHIMP_HPP_)
#define _UTOMODELSWITCHIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <osg/ref_ptr>

#include "UtoModelSwitch.hpp"

namespace osgSim
{
class MultiSwitch;
}

class UtoModelSwitchImp : public UtoModelSwitch
{
public:
   UtoModelSwitchImp(osgSim::MultiSwitch* snode);
   ~UtoModelSwitchImp() override;

   int         Count() const override;
   std::string SelectedName() const override;

   int  Selected() const override;
   void SetSelected(int value) override;

private:
   UtoModelSwitchImp();
   UtoModelSwitchImp(const UtoModelSwitchImp&);
   UtoModelSwitchImp& operator=(const UtoModelSwitchImp&);

   osg::ref_ptr<osgSim::MultiSwitch> m_MultiSwitch;
};

#endif // !defined(_UTOMODELSWITCHIMP_HPP_)
