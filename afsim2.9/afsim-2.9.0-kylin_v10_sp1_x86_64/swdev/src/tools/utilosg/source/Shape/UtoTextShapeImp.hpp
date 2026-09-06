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
// UtoTextShapeImp.hpp : header file

#if !defined(_UTOTEXTSHAPEIMP_HPP_)
#define _UTOTEXTSHAPEIMP_HPP_

// uto includes
#include <osgText/Text>

#include "UtoGeodeShapeImp.hpp"
#include "UtoRenderInfo.hpp"
#include "UtoUpdateCallback.hpp"
// stl includes
#include <vector>

class UtoTextShapeImp : public UtoGeodeShapeImp
{
public:
   // constructor / destructor
   UtoTextShapeImp(
      const UtoWString& sString =
         L" !\"#$%&'()*+,-./0123\n456789:;<=>?@ABCDEFG\nHIJKLMNOPQRSTUVWXYZ\n[\\]^_`abcdefghijklmn\nopqrstuvwxyz{|}~");
   UtoTextShapeImp(const UtoTextShapeImp& rTextImp);
   ~UtoTextShapeImp() override;

   // sets / gets the string
   bool       SetString(const UtoWString& sString);
   UtoWString String() const { return m_String; }

   void GetBounds(float& aWidth, float& aHeight);

   // sets / gets the string
   bool SetSize(int nSize);
   int  Size() const { return m_Size; }

   void SetShader(osg::Program* aShader);

   // sets / gets the font attributes
   bool        SetFont(const std::string& sFontStyle);
   std::string Face() const { return m_FontFace; }

   // sets / gets the alignment
   void          SetAlignment(unsigned long flags);
   unsigned long Alignment() const { return m_Alignment; }

   // sets the color for the text
   void SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved = 0) override;

   // called by the graph to update the text correctly
   void Update(UtoRenderInfo* info, osg::NodeVisitor* nv);

   // sets / gets the type of backdrop to be used
   void          SetBackdropType(unsigned long nType);
   unsigned long BackdropType() const;

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
   void          SetBackdropRenderImp(unsigned long nImpType);
   unsigned long BackdropRenderImp() const;

private:
   // prohibit the copy operator
   UtoTextShapeImp& operator=(const UtoTextShapeImp&);

   // adds / removes the callback
   void SetupCallback();
   void RemoveCallback();

   // determines how to set the transparency
   void SetTransparency();

   // private member variables
   osg::ref_ptr<osgText::Text>                      mTextNode;
   osg::ref_ptr<UtoUpdateCallback<UtoTextShapeImp>> m_Callback;
   std::string                                      m_FontFace;
   UtoWString                                       m_String;
   unsigned long                                    m_Alignment;
   unsigned long                                    m_nBackdropType;
   unsigned long                                    m_nBackdropImpType;
   UtoColor                                         m_oBackdropColor;
   int                                              m_Size;
};

#endif // !defined(_UTOTEXTSHAPEIMP_HPP_)
