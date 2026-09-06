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
// UTOUtility.hpp : header file

#ifndef UTOUTILITY_HPP
#define UTOUTILITY_HPP

#include <functional>

#include <osg/StateSet>

#include "UtoEntity.hpp"
#include "UtoResourceCallback.hpp"
#include "UtoResourceDB.hpp"
#include "UtoShape.hpp"
#include "UtoTypes.hpp"

// Termination function:
int UTILOSG_EXPORT UtoTerm();

/**
\ingroup res_db_lookup
\brief Resource Lookup

Resource Lookup
\param res_name name of the resource to lookup.
\return T & - a reference to the found resource.
*/
template<class T>
T& UtoResDBLookup(const std::string& res_name)
{
   UtoResourceDB& rdb = UtoResourceDB::Instance();
   if (!rdb.IsBound(res_name))
      throw Utok::no_resource(res_name);
   return dynamic_cast<T&>(*rdb.Lookup(res_name)->second);
}

/**
\ingroup res_db_lookup
\brief Resource Lookup

Resource Lookup
\param res_name name of the resource to lookup.
\return UtoShape & - a reference to the found shape.
*/
template<>
inline UtoShape& UtoResDBLookup<UtoShape>(const std::string& res_name)
{
   UtoResourceDB& rdb = UtoResourceDB::Instance();
   if (rdb.IsBound(res_name))
   {
      return *rdb.Lookup(res_name)->second;
   }
   else
   {
      if (rdb.IsBound("Default"))
         return *rdb.Lookup("Default")->second;
      else
         throw Utok::no_resource(res_name);
   }
}

namespace ut
{
namespace osg
{
// this will look for an available texture unit on a stateset
inline unsigned int GetAvailableTextureUnit(::osg::StateSet& aStateSet, unsigned int aStartingPoint = 8)
{
   unsigned int samplerIndex =
      aStartingPoint; // we start at a higher index to avoid other assigned textures deeper in the tree
   ::osg::StateAttribute* sa = nullptr;
   do
   {
      sa = aStateSet.getTextureAttribute(samplerIndex, ::osg::StateAttribute::TEXTURE);
      if (sa != nullptr)
      {
         ++samplerIndex;
      }
   } while (sa != nullptr);
   return samplerIndex;
}
} // namespace osg
} // namespace ut

#endif // _UTOUTILITY_HPP_
