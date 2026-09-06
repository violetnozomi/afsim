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
// UtoWindow.hpp : header file

#if !defined(_UTOWINDOW_HPP_)
#define _UTOWINDOW_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoTypes.hpp"

class UtoWindowImp;
class UtoOverlay;
struct UtoWindowOverlayIter_i;

namespace osg
{
class GraphicsContext;
}

class UTILOSG_EXPORT UtoWindow
{
public:
   virtual ~UtoWindow();
   virtual UtoWindowId Window() const                                          = 0;
   virtual bool        AttachToWindowId(UtoWindowId wid, bool releaseContext)  = 0;
   virtual bool        AttachToGraphicsContext(osg::GraphicsContext* aContext) = 0;

protected:
   UtoWindow();

private:
   UtoWindow(const UtoWindow&);
   UtoWindow& operator=(const UtoWindow&);
};

#endif // !defined(_UTOWINDOW_HPP_)
