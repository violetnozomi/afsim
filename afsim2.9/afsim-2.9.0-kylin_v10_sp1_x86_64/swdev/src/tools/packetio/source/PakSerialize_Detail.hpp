// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef PAKSERIALIZE_DETAIL_HPP
#define PAKSERIALIZE_DETAIL_HPP
/* This file contains helper-classes used for PakSerialize.hpp
 */
namespace PakSerialization
{
// Provides a non-const typedef of any type
template<typename T>
struct Non_Const
{
   typedef T Type;
};
template<typename T>
struct Non_Const<const T>
{
   typedef T Type;
};
} // namespace PakSerialization

#endif
