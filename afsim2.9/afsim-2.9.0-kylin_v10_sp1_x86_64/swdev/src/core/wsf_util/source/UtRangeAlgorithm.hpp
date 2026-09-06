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

#ifndef UTRANGEALGORITHM_HPP
#define UTRANGEALGORITHM_HPP

#include <algorithm>
#include <map>
#include <set>

// Helper functions to simplify calling some std::algorithms which are usually
// used on an entire container

// bool contains(Container, Value);
template<typename Container>
bool contains(Container const& c, typename Container::const_reference v)
{
   return std::find(c.begin(), c.end(), v) != c.end();
}

template<typename Key, typename Cmp, typename Alloc>
bool contains(std::set<Key, Cmp, Alloc> const& s, Key const& k)
{
   return s.find(k) != s.end();
}

template<typename Key, typename Value, typename Cmp, typename Alloc>
bool contains(std::map<Key, Value, Cmp, Alloc> const& m, Key const& k)
{
   return m.find(k) != m.end();
}

template<typename T>
void unique(std::vector<T>& aVector)
{
   aVector.erase(std::unique(aVector.begin(), aVector.end()), aVector.end());
}

template<typename T>
void sort(std::vector<T>& aVector)
{
   std::sort(aVector.begin(), aVector.end());
}

#endif
