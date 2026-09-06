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
// CmeInteractionShape.h : header file

#if !defined(_CMEINTERACTIONSHAPE_H_)
#define _CMEINTERACTIONSHAPE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoAttrPolyLineShape.hpp"
#include "UtoTypes.hpp"

class UtoInteractionShapeImp;
class UtoMapProjection;

class UTILOSG_EXPORT UtoInteractionShape : public UtoAttrPolyLineShape
{
public:
   UtoInteractionShape();
   UtoInteractionShape(const UtoInteractionShape&);
   UtoInteractionShape& operator=(const UtoInteractionShape&);
   ~UtoInteractionShape() override;

   int  AddInboundSegment(UtoEntity* aSrc, int aOffset, const UtoColor& aColor);
   int  AddOutboundSegment(UtoEntity* aSrc, int aOffset, const UtoColor& aColor);
   void Clear();
   void SetTexturePattern(unsigned char aPatter[], const int size);

   UtoShape* Clone() const override;

   void Dump(std::ostream&) override;

   virtual void ArchSegments(bool aState);

   virtual void SetSceneProjection(const UtoMapProjection& aProjection);

   // Sets the portion of the interaction line that is drawn
   // on a scale from 0.0 to 1.0. For example, if this is called
   // with 0.3 as the argument, only the first third of the line
   // from the source to the target will be drawn.
   virtual void SetFillFactor(double aFactor);

protected:
#ifdef DOCJET // hide from DocJet or it is tied to the other constructor
#else
   UtoInteractionShape(UtoInteractionShapeImp*);
#endif

private:
   // I am blocking all of this interface.
   using UtoAttrPolyLineShape::At;
   using UtoAttrPolyLineShape::ColorAt;
   using UtoAttrPolyLineShape::Insert;
   using UtoAttrPolyLineShape::Remove;
   using UtoAttrPolyLineShape::SetAt;
   using UtoAttrPolyLineShape::SetColor;
   using UtoAttrPolyLineShape::Size;
};

#endif // !defined(_CMEINTERACTIONSHAPE_H_)
