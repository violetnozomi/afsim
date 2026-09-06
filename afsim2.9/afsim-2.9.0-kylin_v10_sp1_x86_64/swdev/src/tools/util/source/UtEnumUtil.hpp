// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

namespace ut
{

//! @brief utility functor for hashing in std::unordered_map based on an enum/enum class
//! @note needed to suppress GCC compiler error
template<typename T>
struct EnumHash
{
   static_assert(std::is_enum<T>::value, "T must be an enumeration type.");
   //! @brief templated function for hashing
   //! @tparam T is the type of enum/enum class
   //! @param aEnum is the value of the enum to hash
   //! @return the value of the hash (as std::size_t)
   std::size_t operator()(const T& aEnum) const noexcept { return static_cast<std::size_t>(aEnum); }
};

} // namespace ut
