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
// UtoModelSwitch.hpp : header file

#if !defined(_UTOMODELSWITCH_HPP_)
#define _UTOMODELSWITCH_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoTypes.hpp"

class UTILOSG_EXPORT UtoModelSwitch
{
public:
   virtual ~UtoModelSwitch();

   virtual int         Count() const        = 0;
   virtual std::string SelectedName() const = 0;

   virtual int Selected() const = 0;
   // -1 for select none
   virtual void SetSelected(int value) = 0;

protected:
   UtoModelSwitch();

private:
   UtoModelSwitch(const UtoModelSwitch&);
   UtoModelSwitch& operator=(const UtoModelSwitch&);
};

#endif // !defined(_UTOMODELSWITCH_HPP_)
