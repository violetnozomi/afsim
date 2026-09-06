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
// UtoRawShape.hpp : header file

#if !defined(_UTORAWSHAPE_HPP_)
#define _UTORAWSHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

class UtoShapeImp;

namespace osg
{
class Transform;
}

class UTILOSG_EXPORT UtoRawShape : public UtoShape
{
public:
   UtoRawShape();
   UtoRawShape(const UtoRawShape& aSrc);
   // clones the specified shape
   UtoShape*       Clone() const override;
   osg::Transform* GetRoot();
};

#endif // !defined(_UTORAWSHAPE_HPP_)
