// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoVectorShape.hpp : header file

#if !defined(_UTOVECTORSHAPE_HPP_)
#define _UTOVECTORSHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include <osg/Vec4>

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

class UtoVectorShapeImp;

class UTILOSG_EXPORT UtoVectorShape : public UtoShape
{
public:
   UtoVectorShape();
   UtoVectorShape(const UtoVectorShape& aRHS);
   ~UtoVectorShape() override;
   UtoVectorShape& operator=(const UtoVectorShape&);

   UtoShape* Clone() const override;

   void  SetScalar(float aValue);
   float GetScalar() const;

   int  AddVector(float aAz, float aEl, const osg::Vec4& aColor, bool aHead = true);
   void UpdateVector(int aVectorId, float aAz, float aEl);
   void UpdateVectorColor(int aVectorId, const osg::Vec4& aColor);

   void ShowVector(int aVectorId, bool aState);
   void RemoveVector(int aVectorId);

protected:
private:
};

#endif // !defined(_UTOVECTORSHAPE_HPP_)
