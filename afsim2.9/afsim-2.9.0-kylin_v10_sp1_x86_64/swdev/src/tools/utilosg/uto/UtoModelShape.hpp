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
// UtoModelShape.hpp : header file

#if !defined(_UTOMODELSHAPE_HPP_)
#define _UTOMODELSHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include <vector>

#include <osg/Node>

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

class UtoModelPart;
class UtoModelSwitch;
class UtoModelShapeImp;

class UTILOSG_EXPORT UtoModelShape : public UtoShape
{
public:
   UtoModelShape();
   UtoModelShape(const UtoModelShape&);
   UtoModelShape& operator=(const UtoModelShape&);
   ~UtoModelShape() override;

   void SetMetric(const std::string& name, double value);
   bool Metric(const std::string& name, double& value) const;
   bool InGroup(const std::string& group) const;

   bool                            HasPart(const std::string& name);
   UtoModelPart&                   FindPart(const std::string& name);
   std::vector<std::string>        GetParts();
   static std::vector<std::string> GetParts(osg::ref_ptr<osg::Node> aModelNode);

   bool                            HasSwitch(const std::string& name);
   UtoModelSwitch&                 FindSwitch(const std::string& name);
   std::vector<std::string>        GetSwitches();
   static std::vector<std::string> GetSwitches(osg::ref_ptr<osg::Node> aModelNode);
   static std::vector<int>         GetSwitchSizes(osg::ref_ptr<osg::Node> aModelNode);

   void SetDepthTesting(bool aState);
   void SetLighting(bool aState);

   enum CopyOptions
   {
      SHALLOW_COPY              = 0,
      DEEP_COPY_OBJECTS         = 1 << 0,
      DEEP_COPY_NODES           = 1 << 1,
      DEEP_COPY_DRAWABLES       = 1 << 2,
      DEEP_COPY_STATESETS       = 1 << 3,
      DEEP_COPY_STATEATTRIBUTES = 1 << 4,
      DEEP_COPY_TEXTURES        = 1 << 5,
      DEEP_COPY_IMAGES          = 1 << 6,
      DEEP_COPY_ARRAYS          = 1 << 7,
      DEEP_COPY_PRIMITIVES      = 1 << 8,
      DEEP_COPY_SHAPES          = 1 << 9,
      DEEP_COPY_UNIFORMS        = 1 << 10,
      DEEP_COPY_CALLBACKS       = 1 << 11,
      DEEP_COPY_ALL             = 0x7FFFFFFF
   };
   void SetCopyOptions(int copyFlags);

   UtoShape* Clone() const override;
   void      Dump(std::ostream&) override;

   friend class UtoModelShapeImp;

protected:
   UtoModelShape(UtoModelShapeImp* imp);
};

#endif // !defined(_UTOMODELSHAPE_HPP_)
