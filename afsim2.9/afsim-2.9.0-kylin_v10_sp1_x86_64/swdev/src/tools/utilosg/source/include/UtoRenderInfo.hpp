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
// UtoRenderInfo.hpp : header file

#if !defined(_UTORENDERINFO_HPP_)
#define _UTORENDERINFO_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <osg/Referenced>

class UtoViewer;
class UtoEntity;

struct UtoRenderInfo : public osg::Referenced
{
   friend class osg::Referenced;

   UtoRenderInfo(UtoViewer* v)
      : m_Viewer(v)
   {
   }

   UtoViewer* m_Viewer;

private:
   // hide the destructor since osg::Referenced will take care of deletions
   ~UtoRenderInfo() override {}
};

#endif // !defined(_UTORENDERINFO_HPP_)
