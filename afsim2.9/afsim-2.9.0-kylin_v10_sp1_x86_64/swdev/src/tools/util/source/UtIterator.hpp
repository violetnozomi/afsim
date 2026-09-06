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

#ifndef UTITERATOR_HPP
#define UTITERATOR_HPP

#include <iterator>

namespace ut
{
template<class T>
std::reverse_iterator<T> make_reverse_iterator(T aIterator)
{
   return std::reverse_iterator<T>(aIterator);
}
} // namespace ut

#endif
