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
// UtoIconBoardShape.h : header file

#if !defined(_UTOICONBOARDSHAPE_H_)
#define _UTOICONBOARDSHAPE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

class UtoIconBoardShapeImp;

class UTILOSG_EXPORT UtoIconBoardShape : public UtoShape
{
public:
   UtoIconBoardShape();
   UtoIconBoardShape(const UtoIconBoardShape&);
   UtoIconBoardShape& operator=(const UtoIconBoardShape&);
   ~UtoIconBoardShape() override;

   void Clear();
   void AddTexture(const std::string& aCardId, unsigned int aWidth, unsigned int aHeight, unsigned char* aBits);
   void AddCard(const std::string& aId);

   UtoShape* Clone() const override;

protected:
#ifdef DOCJET // hide from DocJet or it is tied to the other constructor
#else
   UtoIconBoardShape(UtoIconBoardShapeImp*);
#endif

private:
};

#endif // !defined(_UTOICONBOARDSHAPE_H_)
