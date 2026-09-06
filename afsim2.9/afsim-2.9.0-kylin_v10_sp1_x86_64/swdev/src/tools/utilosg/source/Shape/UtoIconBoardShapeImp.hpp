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
// UtoIconBoardShapeImp.hpp : header file

#ifndef UTOICONBOARDSHAPEIMP_HPP
#define UTOICONBOARDSHAPEIMP_HPP

#include "UtoTypes.hpp"

// uto includes
#include "UtoShapeImp.hpp"

// NOTE: we have only tested this with a curved map projection

namespace osg
{
class Texture2D;
}

class UtoIconBoardShape;

class UtoIconBoardShapeImp : public UtoShapeImp
{
public:
   UtoIconBoardShapeImp();
   UtoIconBoardShapeImp(const UtoIconBoardShapeImp& aRHS);
   ~UtoIconBoardShapeImp() override;

   void Clear();
   void AddTexture(const std::string& aCardId, unsigned int aWidth, unsigned int aHeight, unsigned char* aBits);
   void AddCard(const std::string& aCardId);

private:
   void BuildNode();

   std::map<std::string, osg::ref_ptr<osg::Texture2D>> mTextureMap;
   int                                                 mCards;
};

#endif // !defined(_UTOICONBOARDSHAPEIMP_HPP_)
