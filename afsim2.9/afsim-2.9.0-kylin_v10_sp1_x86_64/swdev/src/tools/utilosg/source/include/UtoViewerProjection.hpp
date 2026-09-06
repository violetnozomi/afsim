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
// UtoViewerProjection.hpp : header file

#if !defined(_UTOVIEWERPROJECTION_HPP_)
#define _UTOVIEWERPROJECTION_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <osg/Matrix>

#include "UtoMapProjection.hpp"
#include "UtoTypes.hpp"

class UtEntity;

/////////////////////////////////////////////////////////////////////////////
// UtoViewerProjection

class UTILOSG_EXPORT UtoViewerProjection : public UtoMapProjection
{
public:
   UtoViewerProjection() {}
   UtoViewerProjection(UtoMapProjection& aProjection);
   virtual ~UtoViewerProjection();

   void        GetECS_Frame(UtEntity& aEntity, osg::Matrix& aMatrix);
   void        GetNED_Frame(UtEntity& aEntity, osg::Matrix& aMatrix);
   UtoPosition CalcPoint(UtEntity& aEntity);
   UtoPosition CalcPointOrient(UtEntity& aEntity, UtoOrientation& aOrient);
   UtoPosition CalcPointNoOffset(UtEntity& aEntity);
   UtoPosition CalcPointOrientNoOffset(UtEntity&, UtoOrientation&);

protected:
private:
};

#endif // !defined(_UTOVIEWERPROJECTION_HPP_)
