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
// UtoPointListShapeImp.hpp : header file

#if !defined(_UTOPOINTLISTSHAPEIMP_HPP_)
#define _UTOPOINTLISTSHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtoPolyShapeImp.hpp"
#include "UtoTypes.hpp"

class UtoPointListShape;

class UtoPointListShapeImp : public UtoPolyShapeImp
{
public:
   UtoPointListShapeImp();
   UtoPointListShapeImp(const UtoPosition pts[], int num);
   UtoPointListShapeImp(const UtoPointListShapeImp&);
   ~UtoPointListShapeImp() override;

private:
   UtoPointListShapeImp& operator=(const UtoPointListShapeImp&);
};

#endif // !defined(_UTOPOINTLISTSHAPEIMP_HPP_)
