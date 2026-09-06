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
// UtoTexturedPolygonImp.h : header file

#if !defined(_UTOTEXTUREDPOLYGONIMP_HPP_)
#define _UTOTEXTUREDPOLYGONIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include <osg/Array>

namespace osg
{
class Geometry;
class DrawArrays;
} // namespace osg

namespace UtoTexturedPolygonImp
{
enum WrapType
{
   WRAP_REPEAT,
   WRAP_CLAMP
};

osg::Geometry* CreateModel(const std::string& TextureFile,
                           bool&              o_TranslucentImage,
                           bool               AntiAliased = true,
                           bool               MipMapping  = true,
                           WrapType           wt          = WRAP_REPEAT);
osg::Geometry* CreateModel(const std::string& TextureFile,
                           bool&              o_TranslucentImage,
                           double             Width,
                           double             Height,
                           bool               AntiAliased,
                           bool               MipMapping,
                           WrapType           wt = WRAP_REPEAT);
// Allocate only one set of these for all textured polygons.
extern osg::ref_ptr<osg::Vec2Array>  s_TexCoord;
extern osg::ref_ptr<osg::Vec3Array>  s_Normal;
extern osg::ref_ptr<osg::Vec4Array>  s_Color;
extern osg::ref_ptr<osg::DrawArrays> s_Primitive;

// creates the geometry for a textured subimage
osg::Geometry* CreateSubImage(const std::string& rTexture,
                              unsigned short     nX0,
                              unsigned short     nY0,
                              unsigned short     nX1,
                              unsigned short     nY1,
                              double             dWidth,
                              double             dHeight,
                              bool               bInvertY,
                              bool&              rTransImage);

} // namespace UtoTexturedPolygonImp

#endif // !defined(_UTOTEXTUREDPOLYGONIMP_HPP_)
