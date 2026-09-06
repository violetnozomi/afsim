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
// UtoModelPart.hpp : header file

#if !defined(_UTOMODELPART_HPP_)
#define _UTOMODELPART_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoTypes.hpp"

namespace osg
{
class Node;
}

class UTILOSG_EXPORT UtoModelPart
{
public:
   virtual ~UtoModelPart();

   virtual UtoPosition Position() const                    = 0;
   virtual void        SetPosition(const UtoPosition& pos) = 0;

   virtual UtoOrientation Orientation() const                          = 0;
   virtual void           SetOrientation(const UtoOrientation& orient) = 0;

   virtual UtoPosOrient PosOrient() const                           = 0;
   virtual void         SetPosOrient(const UtoPosOrient& posorient) = 0;

   virtual osg::Node* Node() = 0;

protected:
   UtoModelPart();

private:
   UtoModelPart(const UtoModelPart&);
   UtoModelPart& operator=(const UtoModelPart&);
};

#endif // !defined(_UTOMODELPART_HPP_)
