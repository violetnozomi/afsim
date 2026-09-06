// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef L16_FIELDENUMDEFINES_HPP
#define L16_FIELDENUMDEFINES_HPP

#define ADD_ENUM(X) sEnumMap.emplace(X, #X);

#define ADD_ENUM2(A, B) ADD_ENUM(A) ADD_ENUM(B)
#define ADD_ENUM4(A, B, C, D) ADD_ENUM2(A, B) ADD_ENUM2(C, D)
#define ADD_ENUM8(A, B, C, D, E, F, G, H) ADD_ENUM4(A, B, C, D) ADD_ENUM4(E, F, G, H)
#define ADD_ENUM16(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P) \
   ADD_ENUM8(A, B, C, D, E, F, G, H) ADD_ENUM8(I, J, K, L, M, N, O, P)
#define DEFINE_GET_ENUM_STRING(_CLASS_)                               \
   const char* _CLASS_::GetEnumString(int aValue)                     \
   {                                                                  \
      std::map<int, const char*>::iterator i = sEnumMap.find(aValue); \
      if (i != sEnumMap.end())                                        \
      {                                                               \
         return i->second;                                            \
      }                                                               \
      return 0;                                                       \
   }

#define IMPLEMENT_ENUM_MAP(_CLASS_)              \
   std::map<int, const char*> _CLASS_::sEnumMap; \
   DEFINE_GET_ENUM_STRING(_CLASS_)               \
   void _CLASS_::CreateEnumMap()

#endif
