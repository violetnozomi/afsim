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

#ifndef UTOMEMORY_HPP
#define UTOMEMORY_HPP

#include <type_traits>
#include <utility>

#include <osg/Referenced>
#include <osg/ref_ptr>


namespace ut
{
namespace osg
{
template<class T, typename... Args>
::osg::ref_ptr<T> make_ref(Args&&... args)
{
   static_assert(std::is_base_of<::osg::Referenced, T>::value,
                 "ut::osg::make_ref template type must derive from osg::Referenced");
   return ::osg::ref_ptr<T>(new T(std::forward<Args>(args)...));
}
} // namespace osg
} // namespace ut
#endif
