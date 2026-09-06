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
// UtoTessellatedPolygonShape.hpp : header file

#if !defined(_UTOTESSELLATEDPOLYGONSHAPE_HPP_)
#define _UTOTESSELLATEDPOLYGONSHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

class UtoTessellatedPolygonShapeImp;

class UTILOSG_EXPORT UtoTessellatedPolygonShape : public UtoShape
{
public:
   UtoTessellatedPolygonShape();
   UtoTessellatedPolygonShape(const UtoPosition pts[], int num);
   UtoTessellatedPolygonShape(const UtoTessellatedPolygonShape&);
   UtoTessellatedPolygonShape& operator=(const UtoTessellatedPolygonShape&);
   ~UtoTessellatedPolygonShape() override;

   int Insert(int pos, const UtoPosition&);
   int Insert(int pos, const UtoPosition pts[], int num);

   void SetOrientation(const UtoOrientation& aOrientation);

   void        SetOffset(const UtoPosition&);
   UtoPosition GetOffset() const;

   void SetClosed(bool closed);
   bool GetClosed() const;

   // determines if the shape is allowed to be depth tested
   void SetDepthTesting(bool bDepthTest);

   void Rebuild();

   void Clear();
   void EnableLighting(bool aState);

   UtoShape* Clone() const override;

   void Dump(std::ostream&) override;

protected:
#ifdef DOCJET // hide from DocJet or it is tied to the other constructor
#else
   UtoTessellatedPolygonShape(UtoTessellatedPolygonShapeImp*);
#endif

private:
};

#endif // !defined(_UTOTESSELLATEDPOLYGONSHAPE_HPP_)
