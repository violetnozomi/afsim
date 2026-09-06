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
// UtoGeodeShapeImp.hpp : header file

#if !defined(_UTOGEODE_SHAPEIMP_HPP_)
#define _UTOGEODE_SHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtoShapeImp.hpp"
#include "UtoTypes.hpp"

namespace osg
{
class Geode;
class LineStipple;
class LineWidth;
class Point;
} // namespace osg

class UtoGeodeShapeImp : public UtoShapeImp
{
public:
   UtoGeodeShapeImp();
   UtoGeodeShapeImp(const UtoGeodeShapeImp&);
   ~UtoGeodeShapeImp() override;

   void SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved = 0) override = 0;

   void SetOwner(UtoEntity* owner) override;

   virtual short  Pattern() const;
   virtual int    PatternFactor() const;
   virtual void   SetPattern(short pattern, int factor);
   virtual double Width() const;
   virtual void   SetWidth(double width);
   virtual double PointSize() const;
   virtual void   SetPointSize(double pointSize);
   virtual bool   PointSmooth() const;
   virtual void   SetPointSmooth(bool pointsmooth);

protected:
   osg::ref_ptr<osg::Geode> m_Geode;

private:
   const osg::LineStipple* GetStippleAttributeSet() const;
   const osg::LineWidth*   GetLineWidthAttributeSet() const;
   const osg::Point*       GetPointAttributeSet() const;
   osg::LineStipple*       GetStippleAttributeSet();
   osg::LineWidth*         GetLineWidthAttributeSet();
   osg::Point*             GetPointAttributeSet();
   UtoGeodeShapeImp&       operator=(const UtoGeodeShapeImp&);
};

#endif // !defined(_UTOGEODE_SHAPEIMP_HPP_)
