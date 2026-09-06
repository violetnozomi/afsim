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
// UtoModelPartImp.hpp : header file

#if !defined(_UTOMODELPARTIMP_HPP_)
#define _UTOMODELPARTIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <osg/Matrix>
#include <osg/ref_ptr>

#include "UtoModelPart.hpp"

namespace osgSim
{
class DOFTransform;
}

class UtoModelPartImp : public UtoModelPart
{
public:
   UtoModelPartImp(osgSim::DOFTransform* tnode);
   ~UtoModelPartImp() override;

   UtoPosition Position() const override;
   void        SetPosition(const UtoPosition& pos) override;

   UtoOrientation Orientation() const override;
   void           SetOrientation(const UtoOrientation& orient) override;

   UtoPosOrient PosOrient() const override;
   void         SetPosOrient(const UtoPosOrient& posorient) override;

   osg::Node* Node() override;

private:
   UtoModelPartImp();
   UtoModelPartImp(const UtoModelPartImp&);
   UtoModelPartImp& operator=(const UtoModelPartImp&);

   osg::ref_ptr<osgSim::DOFTransform> m_Transform;
};

#endif // !defined(_UTOMODELPARTIMP_HPP_)
