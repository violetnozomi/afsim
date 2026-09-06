// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMMEDIUMTYPEIDENTIFIER_HPP
#define WSFCOMMMEDIUMTYPEIDENTIFIER_HPP

namespace wsf
{
namespace comm
{
namespace medium
{

template<class T>
struct Role
{
   template<class U>
   struct always_false : std::false_type
   {
   };
   static_assert(
      always_false<T>::value,
      "Type has no medium role registered. Use the WSF_COMM_MEDIUM_DECLARE_ROLE_TYPE macro to register the type.");
};

#define WSF_COMM_MEDIUM_DECLARE_ROLE_TYPE(TYPE, ROLE)                                                            \
   template<>                                                                                                    \
   struct Role<TYPE> : std::integral_constant<int, ROLE>                                                         \
   {                                                                                                             \
      static_assert(std::is_base_of<Medium, TYPE>::value,                                                        \
                    "Cannot register medium role for type that does not derive from wsf::comm::medium::Medium"); \
      static_assert(value > 0, "Medium role must be > 0");                                                       \
   };

template<class T>
struct cMEDIUM_ROLE
{
   constexpr operator int() const noexcept { return Role<T>::value; }
};

enum TypeIdentifier : int
{
   cNONE = 0,
   cGUIDED,
   cUNGUIDED
};

} // namespace medium
} // namespace comm
} // namespace wsf

#endif
