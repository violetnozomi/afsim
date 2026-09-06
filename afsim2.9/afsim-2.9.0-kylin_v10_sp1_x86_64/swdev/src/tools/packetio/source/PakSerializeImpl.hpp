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

#ifndef PAKSERIALIZEIMPL_HPP
#define PAKSERIALIZEIMPL_HPP
#include "PakSerialize.hpp"
// !!!  THIS FILE SHOULD BE INCLUDED LAST                            !!!
// !!!  If serialization operators are used, this must be included   !!!
// !!!  last in the .cpp file.                                       !!!

// Note: The order dependence for this header is required for new compilers.
//       Overloads of Save(), Load(), and Serialize() must be defined prior to this include file
namespace PakSerialization
{

template<typename AR, typename T>
inline void Serialize(AR& aAr, T& aValue)
{
   aValue.Serialize(aAr);
}

template<class AR, typename T>
inline void Serialize(AR& aAr, T*& aPtr)
{
   PakSerializePtr<T> ptr(aPtr);
   aAr&               ptr;
}

template<typename T>
inline void Load(PakI& aAr, T& aValue)
{
   Serialize(aAr, aValue);
}

template<typename T>
inline void Save(PakO& aAr, T& aValue)
{
   Serialize(aAr, const_cast<typename Non_Const<T>::Type&>(aValue));
}

template<class AR, typename T, typename TRAIT, bool IS_OUTPUT>
struct SerializeDispatch;

template<class AR, typename T>
struct SerializeDispatch<AR, T, DefaultTypeTrait, true>
{
   typedef typename Non_Const<T>::Type ValueType;
   static void                         Go(AR& aAr, T& aValue) { Save(aAr, const_cast<ValueType&>(aValue)); }
};

template<class AR, typename T>
struct SerializeDispatch<AR, T, DefaultTypeTrait, false>
{
   typedef typename Non_Const<T>::Type ValueType;
   static void                         Go(AR& aAr, T& aValue) { Load(aAr, const_cast<ValueType&>(aValue)); }
};

template<class AR, typename T, bool IS_OUTPUT>
struct SerializeDispatch<AR, T, BasicTypeTrait, IS_OUTPUT>
{
   typedef typename Non_Const<T>::Type ValueType;
   static void                         Go(AR& aAr, T& aValue) { aAr.Serialize(const_cast<ValueType&>(aValue)); }
};

template<class AR, typename T>
struct SerializeInterface
{
   typedef T                                                                                       ValueType;
   typedef SerializeTraits<ValueType>                                                              Traits;
   typedef SerializeDispatch<AR, ValueType, typename Traits::serialize_trait_type, AR::cIS_OUTPUT> Dispatch;
};
} // namespace PakSerialization

// Dispatches the correct routine for the >> operator on a 'Serializer'
template<typename T>
inline PakI& operator&(PakI& aAr, T& aValue)
{
   typedef typename PakSerialization::SerializeInterface<PakI, T>::Dispatch Dispatch;
   Dispatch::Go(aAr, aValue);
   return aAr;
}

template<typename T>
inline PakI& operator>>(PakI& aAr, T& aValue)
{
   typedef typename PakSerialization::SerializeInterface<PakI, T>::Dispatch Dispatch;
   Dispatch::Go(aAr, aValue);
   return aAr;
}

// Dispatches the correct routine for the << operator on a 'Serializer'
template<typename T>
inline PakO& operator&(PakO& aAr, T& aValue)
{
   typedef typename PakSerialization::SerializeInterface<PakO, T>::Dispatch Dispatch;
   Dispatch::Go(aAr, aValue);
   return aAr;
}
template<typename T>
inline PakO& operator&(PakO& aAr, const T& aValue)
{
   typedef typename PakSerialization::SerializeInterface<PakO, const T>::Dispatch Dispatch;
   Dispatch::Go(aAr, aValue);
   return aAr;
}

template<typename T>
inline PakO& operator<<(PakO& aAr, T& aValue)
{
   typedef typename PakSerialization::SerializeInterface<PakO, T>::Dispatch Dispatch;
   Dispatch::Go(aAr, aValue);
   return aAr;
}

template<typename T>
inline PakO& operator<<(PakO& aAr, const T& aValue)
{
   typedef typename PakSerialization::SerializeInterface<PakO, const T>::Dispatch Dispatch;
   Dispatch::Go(aAr, aValue);
   return aAr;
}

#endif
