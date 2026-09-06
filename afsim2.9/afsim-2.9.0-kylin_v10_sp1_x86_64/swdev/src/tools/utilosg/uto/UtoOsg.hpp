// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTOOSG_HPP
#define UTOOSG_HPP
#include <osg/Vec3>
#include <osg/Vec3d>

#include "UtVec3.hpp"

// in light of UtQt.hpp, these are common conversion functions between Ut implementations and their respective Osg implementations
template<typename T>
osg::Vec3 UtToOsgVec3(const UtVec3<T>& aUtVec3)
{
   return {static_cast<osg::Vec3::value_type>(aUtVec3[0]),
           static_cast<osg::Vec3::value_type>(aUtVec3[1]),
           static_cast<osg::Vec3::value_type>(aUtVec3[2])};
}

template<typename T>
osg::Vec3f UtToOsgVec3f(const UtVec3<T>& aUtVec3)
{
   return {static_cast<float>(aUtVec3[0]), static_cast<float>(aUtVec3[1]), static_cast<float>(aUtVec3[2])};
}

template<typename T>
osg::Vec3d UtToOsgVec3d(const UtVec3<T>& aUtVec3)
{
   return {static_cast<double>(aUtVec3[0]), static_cast<double>(aUtVec3[1]), static_cast<double>(aUtVec3[2])};
}

inline osg::Vec3f UtToOsg(const UtVec3<float>& aUtVec3)
{
   return UtToOsgVec3f(aUtVec3);
}

inline osg::Vec3d UtToOsg(const UtVec3<double>& aUtVec3)
{
   return UtToOsgVec3d(aUtVec3);
}

#endif