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
// UtoVectorShapeImp.hpp : header file

#if !defined(_UTOVECTORSHAPEIMP_HPP_)
#define _UTOVECTORSHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtoTypes.hpp"

// uto includes
#include "UtoShapeImp.hpp"

// NOTE: we have only tested this with a curved map projection

class UtoVectorShape;

namespace osg
{
class Material;
}

class UtoVectorShapeImp : public UtoShapeImp
{
public:
   UtoVectorShapeImp();
   UtoVectorShapeImp(const UtoVectorShapeImp& aRHS);
   ~UtoVectorShapeImp() override;

   void  SetScalar(float aValue);
   float GetScalar() const;

   int  AddVector(float aAz, float aEl, const osg::Vec4& aColor, bool aHead);
   void RemoveVector(int aVectorId);
   void UpdateVector(int aVectorId, float aAz, float aEl);
   void UpdateVectorColor(int aVectorId, const osg::Vec4& aColor);
   void ShowVector(int aVectorId, bool aState);

private:
   std::map<int, osg::ref_ptr<osg::PositionAttitudeTransform>> mTransformVector;
   UtoColor                                                    mColor;
   int                                                         mNextVectorId{0};
   float                                                       mScalar{1.0F};
};

#endif // !defined(_UTOVECTORSHAPEIMP_HPP_)
