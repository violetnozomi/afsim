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
// UtoTextShape.hpp : header file

#if !defined(_UTOTEXTSHAPE_HPP_)
#define _UTOTEXTSHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

// project includes
#include "utilosg_export.h"

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

// forward declarations
class UtoTextShapeImp;

class UTILOSG_DEPRECATED_EXPORT UtoTextShape : public UtoShape
{
public:
   // public enumerations
   enum AlignFlags
   {
      LEFT    = 0x0001,
      CENTER  = 0x0002,
      RIGHT   = 0x0004,
      TOP     = 0x0008,
      VCENTER = 0x0010,
      BOTTOM  = 0x0020,
      DEFAULT = 0x0021
   };

   enum Backdrop
   {
      BACKDROP_BOTTOM_RIGHT,
      BACKDROP_CENTER_RIGHT,
      BACKDROP_TOP_RIGHT,
      BACKDROP_BOTTOM_CENTER,
      BACKDROP_TOP_CENTER,
      BACKDROP_BOTTOM_LEFT,
      BACKDROP_CENTER_LEFT,
      BACKDROP_TOP_LEFT,
      BACKDROP_OUTLINE,
      BACKDROP_NONE,
      BACKDROP_MAX_TYPES
   };

   enum BackdropImp
   {
      BACKDROP_IMP_POLYGON_OFFSET,
      BACKDROP_IMP_NO_DEPTH_BUFFER,
      BACKDROP_IMP_DEPTH_RANGE,
      BACKDROP_IMP_STENCIL_BUFFER,
      BACKDROP_IMP_MAX_TYPES
   };

   // public structures
   struct Border
   {
      // constructor
      Border()
         : oBorderClr(0xFF, 0xFF, 0xFF, 0xFF)
         , oBackgroundClr(0xFF, 0xFF, 0xFF, 0x7F)
         , nBorderWidth(1)
         , nX0(0)
         , nY0(0)
         , nWidth(1)
         , nHeight(1)
         , bDrawDynamic(true)
      {
      }

      Border(UtoColor       oBorderClr,
             UtoColor       oBackgroundClr,
             unsigned int   nBorderWidth,
             short          nX0,
             short          nY0,
             unsigned short nWidth,
             unsigned short nHeight,
             bool           bDrawDynamic)
         : oBorderClr(oBorderClr)
         , oBackgroundClr(oBackgroundClr)
         , nBorderWidth(nBorderWidth)
         , nX0(nX0)
         , nY0(nY0)
         , nWidth(nWidth)
         , nHeight(nHeight)
         , bDrawDynamic(bDrawDynamic)
      {
      }

      // public member variables
      UtoColor       oBorderClr;
      UtoColor       oBackgroundClr;
      unsigned int   nBorderWidth;
      short          nX0, nY0;
      unsigned short nWidth, nHeight;
      bool           bDrawDynamic;
   };

   // constructor / destructor
   UtoTextShape();
   UtoTextShape(const std::string& sString);
   UtoTextShape(const UtoWString& sString);
   UtoTextShape(const UtoTextShape& rTextShape);
   ~UtoTextShape() override;

   // copy operator
   UtoTextShape& operator=(const UtoTextShape& rTextShape);

   // clones the shape
   UtoShape* Clone() const override;

   // sets / gets the string value
   bool       SetString(const std::string& sString);
   bool       SetString(const UtoWString& sString);
   UtoWString String() const;

   void SetShader(osg::Program* aShader);

   // sets / gets the size of the text
   bool SetSize(int nSize);
   int  Size() const;

   // sets / gets the font attributes
   bool        SetFont(const std::string& sFontStyle);
   std::string Face() const;

   // sets / gets the text alignment
   void          SetAlignment(unsigned long flags);
   unsigned long Alignment() const;

   void GetBounds(float& aWidth, float& aHeight);

   // sets / gets the type of backdrop to be used
   void     SetBackdropType(Backdrop nType);
   Backdrop BackdropType() const;

   // sets the backdrop offset
   void SetBackdropOffset(float fOffset = 0.07f);
   void SetBackdropOffset(float fHorizontal, float fVertical);

   // obtains the horizontal and vertical offsets
   float BackdropHorizontalOffset() const;
   float BackdropVerticalOffset() const;

   // specifies the backdrop color
   void            SetBackdropColor(const UtoColor& rColor, unsigned int nClrMask = UtoColor::RGBA);
   const UtoColor& BackdropColor() const;

   // specifies the backdrop rendering implementation
   void        SetBackdropRenderImp(BackdropImp nImpType);
   BackdropImp BackdropRenderImp() const;

   void Dump(std::ostream&) override;
};

#endif // !defined(_UTOTEXTSHAPE_HPP_)
