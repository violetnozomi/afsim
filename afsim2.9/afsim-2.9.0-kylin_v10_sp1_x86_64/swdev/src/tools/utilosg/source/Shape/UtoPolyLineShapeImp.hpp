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
// UtoPolyLineShapeImp.hpp : header file

#if !defined(_UTOPOLYLINESHAPEIMP_HPP_)
#define _UTOPOLYLINESHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtoPolyShapeImp.hpp"
#include "UtoTypes.hpp"

class UtoPolyLineShape;

class UtoPolyLineShapeImp : public UtoPolyShapeImp
{
public:
   UtoPolyLineShapeImp();
   UtoPolyLineShapeImp(const UtoPosition pts[], int num);
   UtoPolyLineShapeImp(const UtoPolyLineShapeImp&);
   ~UtoPolyLineShapeImp() override;

   void Update(osg::NodeVisitor* nv, osg::Drawable* drawable) override;

   unsigned int Aggregation() const;
   void         SetAggregation(unsigned int aggregation);

   void SetOrientationTransform(const UtoOrientation& aOrientation);

   void EnableAntiAliasLines(bool enable);
   bool AntiAliasLines() const { return m_AntiAliasLines; }

   void SetDepthBin(int depthBin, bool useNestedBins = false);
   void UnsetDepthBin();

   // determines if the shape is allowed to be depth tested
   void SetDepthTesting(bool bDepthTest);

protected:
   osg::StateSet* GetDefaultStateset();
   void           AddSidesPreOffset(bool close) override {}
   void           AddSidesPostOffset(bool close) override;
   bool           m_AntiAliasLines;
   int            mDepthBin;
   bool           mUseNestedBins;
   bool           mUsingSides;
   bool           m_DepthTest;

private:
   UtoPolyLineShapeImp& operator=(const UtoPolyLineShapeImp&);

   osg::MatrixTransform* mOrientationTransform;
};

#endif // !defined(_UTOPOLYLINESHAPEIMP_HPP_)
