// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTSCRIPTMETHODBUILDER_HPP
#define UTSCRIPTMETHODBUILDER_HPP

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "UtMemory.hpp"
#include "UtScriptAccessible.hpp"
#include "UtScriptClass.hpp"
class UtScriptContext;
#include "UtScriptData.hpp"
#include "UtScriptRef.hpp"
#include "UtTypeTraits.hpp"

// using namespaces to avoid having all the helper classes pollute the global namespace.
namespace ut
{
namespace script
{
//__________________________________________________________________________________________
// ***************
// * Type Traits *
// ***************

//! An index sequence structure.
template<size_t...>
struct index_sequence
{
};

template<size_t N, size_t... IntSeq>
struct make_index_sequence : make_index_sequence<N - 1, N - 1, IntSeq...>
{
};
template<size_t... IntSeq>
struct make_index_sequence<0, IntSeq...> : index_sequence<IntSeq...>
{
};

template<class T>
using remove_cref_t = remove_const_t<remove_reference_t<T>>;

//! C++14, not moved to UtTypeTraits because its from <tuple> instead of <type_traits>.
template<size_t I, class Tuple>
using tuple_element_t = typename std::tuple_element<I, Tuple>::type;

template<class T>
struct remove_all_ptr
{
   using type = T;
};
template<class T>
struct remove_all_ptr<T*> : public remove_all_ptr<T>
{
};
template<class T>
struct remove_all_ptr<T* const> : public remove_all_ptr<T>
{
};
template<class T>
using remove_all_ptr_t = typename remove_all_ptr<T>::type;

template<class T>
struct is_basic_type
{
   using D                     = remove_cref_t<remove_all_ptr_t<T>>;
   static constexpr bool value = std::is_arithmetic<D>::value || std::is_same<std::string, D>::value ||
                                 std::is_enum<D>::value || std::is_same<UtStringId, D>::value;
};

template<class T>
struct is_vector : public false_type
{
};
template<class... T>
struct is_vector<std::vector<T...>> : public true_type
{
};
template<class T>
struct is_map : public false_type
{
};
template<class... T>
struct is_map<std::map<T...>> : public true_type
{
};
template<class T>
struct is_unique_ptr : public false_type
{
};
template<class... T>
struct is_unique_ptr<std::unique_ptr<T...>> : public true_type
{
};

template<class T>
struct is_special_return_type
{
   static constexpr bool value = is_vector<T>::value || is_unique_ptr<T>::value || is_map<T>::value;
   // || is_set<D>::value // MethodBuilder does not currently support use of std::set<T>
};

/// seperated from from is_special_return_type to allow explicit full template specializations
/// of is_special_return_type while still allowing remove_cref_t behavior.
template<class T>
struct is_decay_special_return_type : public is_special_return_type<remove_cref_t<T>>
{
};

/// to_wrapper type is used when binding arguements of a function pointer that
/// will be passed to ConvertDataTo<T>.
template<class T, class SFINAE = void>
struct to_wrapper
{
   static_assert(!is_vector<remove_cref_t<T>>::value || std::is_same<remove_cref_t<T>, std::vector<UtScriptData>>::value,
                 "Must include UtScriptVector.hpp to enable wrapping "
                 "functions that take std::vector as an argument");
   using type = T;
};
template<class T>
using to_wrapper_t = typename to_wrapper<T>::type;

/// to_unwrapped will convert the wrapped type back to it original arguement type.
template<class T, class SFINAE = void>
struct to_unwrapped
{
   using type = T;
};
template<class T>
using to_unwrapped_t = typename to_unwrapped<T>::type;

/// heuristic type trait to determine if SetReturnData Arg will create a shallow DONT_MANAGE UtScriptRef of
/// type Ret. note that this heuristic doesn't 100% gaurentee if that will happen or not.
template<class Ret, class Arg>
struct is_shallow_set_return
{
   static constexpr bool value = (std::is_pointer<Arg>::value || std::is_reference<Arg>::value) &&
                                 (!std::is_base_of<remove_cref_t<Ret>, remove_cref_t<remove_all_ptr_t<Arg>>>::value ||
                                  std::is_const<remove_all_ptr_t<Arg>>::value);
};

template<class T>
struct is_script_class_name_specialized : std::integral_constant<bool, UtScriptClassName<T>() != nullptr>
{
};

//__________________________________________________________________________________________
// *****************
// * GetScriptName *
// *****************
// GetScriptName is responsible for converting a
// c++ type to the corresponding script class name.
// *****************

namespace pretty_function
{
// Compiler-agnostic version of __PRETTY_FUNCTION__ and constants to
// extract the template argument in `ut::script::GetScriptNameImpl`
#if defined(__clang__)
#define UT_SCRIPT_PRETTY_FUNCTION __PRETTY_FUNCTION__
static constexpr size_t PREFIX_LENGTH = sizeof("const char *ut::script::GetScriptNameImpl() [T = ") - 1;
static constexpr size_t SUFFIX_LENGTH = sizeof("]") - 1;
#elif defined(__GNUC__) && !defined(__clang__)
#define UT_SCRIPT_PRETTY_FUNCTION __PRETTY_FUNCTION__
static constexpr size_t PREFIX_LENGTH = sizeof("const char* ut::script::GetScriptNameImpl() [with T = ") - 1;
static constexpr size_t SUFFIX_LENGTH = sizeof("]") - 1;
#elif defined(_MSC_VER)
#define UT_SCRIPT_PRETTY_FUNCTION __FUNCSIG__
static constexpr size_t PREFIX_LENGTH = sizeof("const char *__cdecl ut::script::GetScriptNameImpl<class ") - 1;
static constexpr size_t SUFFIX_LENGTH = sizeof(">(void)") - 1;
#else
#error "unknown macro to get a template name from the __PRETTY_FUNCTION__ expansion."
#endif
} // end namespace pretty_function

template<class T>
const char* GetScriptNameImpl()
{
   return UT_SCRIPT_PRETTY_FUNCTION;
}
#undef UT_SCRIPT_PRETTY_FUNCTION

template<class T, class SFINAE = void>
struct GetScriptName
{
   // TODO use UT_MAP_CLASS_TO_SCRIPT_NAME in the headers of all classes
   // that have an associated script class.
   // That way we don't have to fall back to this __PRETTY_FUNCTION__ logic,
   // as it only works if the class's name and the script's class name are the same.
   // Thankfully AFSIM will spit out an initialization error 'could not find script class'
   // if this fallback doesn't work for figuring out the necessary script class name.
   static std::string op()
   {
      namespace pf = pretty_function;
      std::string type(&GetScriptNameImpl<T>()[pf::PREFIX_LENGTH]);
      type = type.substr(0, type.size() - pf::SUFFIX_LENGTH);
      return type;
   }
   static const UtStringId value;
};
template<class T, class SFINAE>
const UtStringId GetScriptName<T, SFINAE>::value = GetScriptName<T, SFINAE>::op();

template<class T>
struct GetScriptName<T, enable_if_t<is_script_class_name_specialized<T>::value>>
{
   static constexpr const char* op() { return UtScriptClassName<T>(); }
   static const UtStringId      value;
};
template<class T>
const UtStringId GetScriptName<T, enable_if_t<is_script_class_name_specialized<T>::value>>::value = UtScriptClassName<T>();

template<class T> // unravels const
struct GetScriptName<const T> : public GetScriptName<T>
{
};

template<class T> // unravels pointer
struct GetScriptName<T*> : public GetScriptName<T>
{
};

template<class T> // unravels pointer const
struct GetScriptName<T* const> : public GetScriptName<T*>
{
};

template<class T> // unravels value const
struct GetScriptName<const T*> : public GetScriptName<T*>
{
};

template<class T> // unravels pointer const
struct GetScriptName<const T* const> : public GetScriptName<const T*>
{
};

/// intentionally only allowing unique_ptr<T,default_destructor>
/// because UtScriptRef doesn't support custom destructors.
template<class T>
struct GetScriptName<std::unique_ptr<T>> : public GetScriptName<T>
{
};

/// type for full specializations of GetScriptName to inherit from
/// which allows the static UtStringId value to be defined in the header
/// as a template instantiation instead of directly defining value inside
/// the full specialization of GetScriptName because that would require
/// 'value' be defined only once, preventing it from being defined in the header.
template<class T>
struct GetScriptNameLiteral
{
   static const UtStringId value;
};
template<class T>
const UtStringId GetScriptNameLiteral<T>::value = GetScriptName<T>::op();

/// scripting does not differentiate different integral types and simply
/// treats them all as ints.
template<class T>
struct GetScriptName<T, enable_if_t<std::is_integral<T>::value && !std::is_const<T>::value>> : GetScriptNameLiteral<int>
{
   static constexpr const char* op() { return "int"; }
};

template<>
struct GetScriptName<bool> : GetScriptNameLiteral<bool>
{
   static constexpr const char* op() { return "bool"; }
};

template<>
struct GetScriptName<const char*> : GetScriptNameLiteral<const char*>
{
   static constexpr const char* op() { return "string"; }
};

/// intentionally only supporting a std::vector that uses the default allocator
template<class T>
struct GetScriptName<std::vector<T>>
{
   static std::string      op() { return std::string{"Array<"} + GetScriptName<T>::op() + '>'; }
   static const UtStringId value;
};
template<class T>
const UtStringId GetScriptName<std::vector<T>, void>::value = GetScriptName<std::vector<T>, void>::op();

/// enums will be represented in script as ints.
template<class T>
struct GetScriptName<T, enable_if_t<std::is_enum<T>::value && !std::is_const<T>::value>> : public GetScriptName<int>
{
   static_assert(sizeof(int) >= sizeof(T), "enum is too big to be safely used in scripting");
};

/// unravels map
template<class Key, class Val, class... OtherArgs>
struct GetScriptName<std::map<Key, Val, OtherArgs...>>
{
   static std::string op()
   {
      return std::string{"Map<"} + GetScriptName<Key>::op() + ',' + GetScriptName<Val>::op() + '>';
   }
   static const UtStringId value;
};
template<class Key, class Val, class... OtherArgs>
const UtStringId
   GetScriptName<std::map<Key, Val, OtherArgs...>, void>::value = GetScriptName<std::map<Key, Val, OtherArgs...>>::op();

template<class Class>
using ScriptNameOracle = GetScriptName<remove_reference_t<Class>>;

//__________________________________________________________________________________________
// *****************
// * SetReturnData *
// *****************
// SetReturnData is responsible for the type-erasure logic of assigning a
// return value into the UtScriptData return instance.
// *******************

#if defined(_MSC_VER)
#pragma warning(push)
// disable warnings about forcing conversion of int/double to bool.
// which is impossible since the related controlflow is constexpr, and
// with the use of c++17 `if constrexpr` this warning wouldn't be spat out.
#pragma warning(disable : 4800)
#endif

/// variable holder to reduce longwinded function declarations.
struct ScriptReturn
{
   UtScriptData&  mVal;
   UtScriptClass* mClassPtr;
};

/// sets a class
template<class T, class SFINAE = void>
struct SetReturnData
{
   // called during return by value.
   static void op(const ScriptReturn& aReturn, T&& aVal)
   {
      auto* objPtr = new T(std::move(aVal));
      aReturn.mVal.SetPointer(new UtScriptRef(objPtr, aReturn.mClassPtr, UtScriptRef::cMANAGE));
   }

   // called during return by const value (which is bad practice, but still need to handle it).
   static void op(const ScriptReturn& aReturn, const T&& aVal)
   {
      auto* objPtr = new T(std::move(aVal));
      aReturn.mVal.SetPointer(new UtScriptRef(objPtr, aReturn.mClassPtr, UtScriptRef::cMANAGE));
   }

   // called during return by const reference.
   static void op(const ScriptReturn& aReturn, const T& aVal)
   {
      // if scripting supported const correctness we could avoid creating a managed copy, but
      // alas it does not.
      auto* objPtr = new T(aVal);
      aReturn.mVal.SetPointer(new UtScriptRef(objPtr, aReturn.mClassPtr, UtScriptRef::cMANAGE));
   }
   // called during return by reference
   static void op(const ScriptReturn& aReturn, T& aVal)
   {
      aReturn.mVal.SetPointer(new UtScriptRef(&aVal, aReturn.mClassPtr));
   }
};

/// special case for setting scriptings internal data type directly
template<>
struct SetReturnData<UtScriptData>
{
   template<class TT>
   static void op(const ScriptReturn& aReturn, TT&& aVal)
   {
      aReturn.mVal = std::forward<TT>(aVal);
   }
};

/// unwraps a pointer for setting
template<class T>
struct SetReturnData<T*, enable_if_t<!is_basic_type<T>::value>>
{
   static void op(const ScriptReturn& aReturn, T* aPtr)
   {
      if (aPtr == nullptr)
      {
         aReturn.mVal.SetPointer(new UtScriptRef(nullptr, aReturn.mClassPtr));
      }
      else
      {
         SetReturnData<remove_const_t<T>>::op(aReturn, *aPtr);
      }
   }
};

/// builds up scriptings internal vector type and then sets the return data with it.
template<class T, class... A>
struct SetReturnData<std::vector<T, A...>, enable_if_t<!std::is_same<std::vector<UtScriptData>, std::vector<T, A...>>::value>>
{
   static void op(const ScriptReturn& aReturn, std::vector<T, A...>&& aVal)
   {
      UtScriptClass* elementClass = is_basic_type<T>::value ? nullptr : aReturn.mClassPtr->GetContainerDataType();

      auto* vec = new std::vector<UtScriptData>();
      vec->reserve(aVal.size());

      for (auto&& element : aVal)
      {
         vec->emplace_back();
         SetReturnData<remove_cref_t<T>>::op({vec->back(), elementClass}, std::move(element));
      }
      aReturn.mVal.SetPointer(new UtScriptRef(vec, aReturn.mClassPtr, UtScriptRef::cMANAGE));
   }
   static void op(const ScriptReturn& aReturn, const std::vector<T, A...>& aVal)
   {
      UtScriptClass* elementClass = is_basic_type<T>::value ? nullptr : aReturn.mClassPtr->GetContainerDataType();

      auto* vec = new std::vector<UtScriptData>();
      vec->reserve(aVal.size());

      for (auto&& element : aVal)
      {
         vec->emplace_back();
         SetReturnData<remove_cref_t<T>>::op({vec->back(), elementClass}, element);
      }
      aReturn.mVal.SetPointer(new UtScriptRef(vec, aReturn.mClassPtr, UtScriptRef::cMANAGE));
   }
};

/// builds up scriptings internal map type and then sets the return data with it.
using ScriptInternalMapType = std::map<UtScriptData, UtScriptData>;
template<class Key, class Val, class... Extra>
struct SetReturnData<std::map<Key, Val, Extra...>,
                     enable_if_t<!std::is_same<ScriptInternalMapType, std::map<Key, Val, Extra...>>::value>>
{
   using Map = std::map<Key, Val, Extra...>;

   static void op(const ScriptReturn& aReturn, const Map& aVal)
   {
      auto*          map      = new ScriptInternalMapType();
      UtScriptClass* keyClass = is_basic_type<Key>::value ? nullptr : aReturn.mClassPtr->GetContainerKeyType();
      UtScriptClass* valClass = is_basic_type<Val>::value ? nullptr : aReturn.mClassPtr->GetContainerDataType();

      for (auto&& element : aVal)
      {
         UtScriptData key;
         UtScriptData val;

         SetReturnData<remove_cref_t<Key>>::op({key, keyClass}, element.first);
         SetReturnData<remove_cref_t<Val>>::op({val, valClass}, element.second);
         map->insert({key, val});
      }
      aReturn.mVal.SetPointer(new UtScriptRef(map, aReturn.mClassPtr, UtScriptRef::cMANAGE));
   }
};

// unwraps a unique_ptr
template<class T>
struct SetReturnData<std::unique_ptr<T>, enable_if_t<!is_basic_type<T>::value>>
{
   using D = remove_cref_t<T>;
   static constexpr bool needs_construction_wrapping =
      (is_decay_special_return_type<remove_all_ptr_t<T>>::value && !std::is_same<std::vector<UtScriptData>, D>::value &&
       !std::is_same<std::map<UtScriptData, UtScriptData>, D>::value) ||
      std::is_pointer<T>::value;

   static void op(const ScriptReturn& aReturn, std::unique_ptr<T>&& aPtr)
   {
      if /*constexpr*/ (needs_construction_wrapping)
      {
         SetReturnData<remove_const_t<T>>::op(aReturn, std::move(*aPtr.get()));
      }
      else
      {
         aReturn.mVal.SetPointer(new UtScriptRef(aPtr.release(), aReturn.mClassPtr, UtScriptRef::cMANAGE));
      }
   }
   // returning a const unique_ptr, or a reference to a unique_ptr implies it wasn't actually intended to
   // hand over ownership.
   static void op(const ScriptReturn& aReturn, const std::unique_ptr<T>& aPtr)
   {
      if /*constexpr*/ (needs_construction_wrapping)
      {
         SetReturnData<remove_const_t<T>>::op(aReturn, *aPtr.get());
      }
      else
      {
         aReturn.mVal.SetPointer(new UtScriptRef(aPtr.get(), aReturn.mClassPtr));
      }
   }
};

// unwraps a unique_ptr to a arithmatic/string type
template<class T>
struct SetReturnData<std::unique_ptr<T>, enable_if_t<is_basic_type<T>::value>>
{
   static void op(const ScriptReturn& aReturn, const std::unique_ptr<T>& aPtr)
   {
      SetReturnData<remove_cref_t<T>*>::op(aReturn, aPtr.get());
   }
};

/// sets arithmatic/string types
template<class T>
struct SetReturnData<
   T,
   enable_if_t<is_basic_type<T>::value && !std::is_floating_point<T>::value && !std::is_enum<remove_pointer_t<T>>::value &&
               !std::is_pointer<remove_pointer_t<T>>::value && !std::is_integral<T>::value>>
{
   template<class TT>
   static void op(const ScriptReturn& aReturn, const TT* aValPtr)
   {
      if (aValPtr == nullptr)
      {
         aReturn.mVal.Clear();
         return;
      }
      if /*constexpr*/ (std::is_same<bool, TT>::value)
      {
         aReturn.mVal.SetBool(static_cast<bool>(*aValPtr));
      }
      else if /*constexpr*/ (std::is_integral<TT>::value)
      {
         aReturn.mVal.SetInt(static_cast<int>(*aValPtr));
      }
      else
      {
         aReturn.mVal.SetDouble(static_cast<double>(*aValPtr));
      }
   }

   /// return by non-const char* is intentionally deleted due to ambiguity between
   /// pointer to char and cstring. use const char* to return by cstring, or
   /// return `signed char*` instead of `char*` to indicate pointer to char.
   static void op(const ScriptReturn& aReturn, char* aVal) = delete;

   static void op(const ScriptReturn& aReturn, const char* aVal) { aReturn.mVal.SetString(aVal); }
   static void op(const ScriptReturn& aReturn, const std::string& aVal) { aReturn.mVal.SetString(aVal); }
   static void op(const ScriptReturn& aReturn, std::string&& aVal) { aReturn.mVal.SetString(std::move(aVal)); }
   static void op(const ScriptReturn& aReturn, const std::string* aPtr)
   {
      aPtr ? aReturn.mVal.SetString(*aPtr) : aReturn.mVal.Clear();
   }

   static void op(const ScriptReturn& aReturn, UtStringId aVal)
   {
      aReturn.mVal.SetExternallyManagedString(aVal.GetString());
   }
   static void op(const ScriptReturn& aReturn, const UtStringId* aPtr)
   {
      if (aPtr == nullptr)
      {
         aReturn.mVal.Clear();
      }
      else
      {
         aReturn.mVal.SetExternallyManagedString(aPtr->GetString());
      }
   }
};

/// sets bool
template<>
struct SetReturnData<bool>
{
   static void op(const ScriptReturn& aReturn, bool aVal) { aReturn.mVal.SetBool(aVal); }
};

/// sets double value
template<class T>
struct SetReturnData<T, enable_if_t<std::is_floating_point<T>::value>>
{
   static void op(const ScriptReturn& aReturn, double aVal) { aReturn.mVal.SetDouble(aVal); }
};

/// sets integer value
template<class T>
struct SetReturnData<T, enable_if_t<std::is_enum<T>::value || std::is_integral<T>::value>>
{
   static void op(const ScriptReturn& aReturn, T aVal)
   {
      // verify narrowing cast was safe.
      assert(static_cast<remove_cref_t<T>>(static_cast<int>(aVal)) == aVal);
      aReturn.mVal.SetInt(static_cast<int>(aVal));
   }
};

/// sets enum pointer
template<class T>
struct SetReturnData<T*, enable_if_t<std::is_enum<T>::value>>
{
   static void op(const ScriptReturn& aReturn, T* aPtr)
   {
      aPtr ? aReturn.mVal.SetInt(static_cast<int>(*aPtr)) : aReturn.mVal.Clear();
   }
};

/// unravels an int** to int*
template<class T>
struct SetReturnData<T*, enable_if_t<is_basic_type<T>::value && std::is_pointer<T>::value>>
{
   static void op(const ScriptReturn& aReturn, T* aPtr) { SetReturnData<remove_const_t<T>>::op(aReturn, *aPtr); }
};


//__________________________________________________________________________________________
// *****************
// * ConvertDataTo *
// *****************
// ConvertDataTo is responsible for converting the value stored in
// a UtScriptData to a desired type.
// *****************

/// convert to reference or value of class
template<class Ref, class SFINAE = void>
struct ConvertDataTo
{
   static Ref& op(const UtScriptData& aData)
   {
      static_assert(!std::is_pointer<remove_reference_t<Ref>>::value,
                    "converting to a reference of a pointer is not supported");
      static_assert(!is_decay_special_return_type<Ref>::value, "cannot convert to container type");
      return *static_cast<remove_reference_t<Ref>*>(aData.GetPointer()->GetAppObject());
   }
};

/// convert to pointer of class
template<class T>
struct ConvertDataTo<T*,
                     enable_if_t<!is_basic_type<T>::value && !is_decay_special_return_type<T>::value &&
                                 !std::is_same<UtScriptData, remove_const_t<T>>::value>>
{
   static T* op(const UtScriptData& aData)
   {
      static_assert(!std::is_pointer<T>::value, "converting to a pointer of a pointer is not supported");
      return static_cast<T*>(aData.GetPointer()->GetAppObject());
   }
};

template<class T>
struct ConvertDataTo<T* const> : public ConvertDataTo<T*>
{
};

/// transparent passthrough of scripts internal UtScriptData.
template<class T>
struct ConvertDataTo<T, enable_if_t<std::is_same<remove_cref_t<T>, UtScriptData>::value>>
{
   static const UtScriptData& op(const UtScriptData& aData) { return aData; }
};

template<class T>
struct BasicReferenceMaker
{
   BasicReferenceMaker(UtScriptData& aData, T aVal)
      : mData(aData)
      , mVal(aVal)
      , mOrigVal(aVal)
   {
   }

   virtual ~BasicReferenceMaker()
   {
      if (mVal != mOrigVal)
      {
         SetReturnData<T>::op({mData, nullptr}, mVal);
      }
   }

   // implict conversion to reference type
   operator T&() { return mVal; }

private:
   UtScriptData& mData;

protected:
   T mVal;
   T mOrigVal;
};

/// convert to arithmetic or enum type
template<class T>
struct ConvertDataTo<
   T,
   enable_if_t<is_basic_type<T>::value && !std::is_pointer<T>::value &&
               !std::is_same<std::string, remove_cref_t<T>>::value && !std::is_same<UtStringId, remove_cref_t<T>>::value>>
{
   static remove_reference_t<T> op(const UtScriptData& aData)
   {
      // this limititation exists due to inability to expose writes on a reference back
      // into the const UtScriptData.
      static_assert(!std::is_lvalue_reference<T>::value || std::is_const<remove_reference_t<T>>::value,
                    "you may not pass by non-const reference to a primitive, change to const& or by value.");
      using D = remove_cref_t<T>;
      if /*constexpr*/ (std::is_same<bool, D>::value)
      {
         return static_cast<D>(aData.GetBool());
      }
      else if /*constexpr*/ (std::is_floating_point<T>::value)
      {
         return static_cast<D>(aData.GetDouble());
      } /*else if constexpr(std::is_same_v<UtStringId,D>){
          return UtStringId{aData.GetString()};
       }*/
      else
      { // integral or enum
         return static_cast<D>(aData.GetInt());
      }
   }

   template<class TT = T, class C = enable_if_t<!std::is_const<remove_reference_t<TT>>::value && std::is_reference<TT>::value>>
   static BasicReferenceMaker<remove_cref_t<T>> op(UtScriptData& aData)
   {
      return {aData, ConvertDataTo<remove_cref_t<T>>::op(static_cast<const UtScriptData&>(aData))};
   }
};

/// convert to string type
template<class T>
struct ConvertDataTo<T, enable_if_t<std::is_same<std::string, remove_cref_t<T>>::value>>
{
   static const std::string& op(const UtScriptData& aData)
   {
      // conversion to non-const std::string is not supported, curtousy of the GetString return type.
      static_assert(!std::is_lvalue_reference<T>::value || std::is_const<remove_reference_t<T>>::value,
                    "you may not pass by non-const reference to a std::string, change to const&");
      return aData.GetString();
   }

   template<class TT = T, class = enable_if_t<!std::is_const<remove_reference_t<TT>>::value>>
   static std::string& op(UtScriptData& aData)
   {
      aData.CastToString();
      return *static_cast<std::string*>(aData.GetVoidPointer());
   }
};

/// convert to UtStringId type
template<class T>
struct ConvertDataTo<T, enable_if_t<std::is_same<UtStringId, remove_cref_t<T>>::value>>
{
   static UtStringId op(const UtScriptData& aData)
   {
      // conversion to a reference is not supported.
      static_assert(!std::is_lvalue_reference<T>::value || std::is_const<remove_reference_t<T>>::value,
                    "you may not pass by non-const reference to a UtStringId, change to const& or by value");
      return UtStringId{aData.GetString()};
   }

   template<class TT = T, class C = enable_if_t<!std::is_const<remove_reference_t<TT>>::value && std::is_reference<TT>::value>>
   static BasicReferenceMaker<UtStringId> op(UtScriptData& aData)
   {
      return {aData, ConvertDataTo<remove_cref_t<T>>::op(static_cast<const UtScriptData&>(aData))};
   }
};

/// convert to pointer of arithmatic or enum type
template<class T>
struct ConvertDataTo<T*,
                     enable_if_t<is_basic_type<T>::value && !std::is_same<std::string, remove_const_t<T>>::value &&
                                 !std::is_same<char, remove_const_t<T>>::value>>
{
   struct PointerMaker
   {
      bool is_nullptr;
      // technically we don't have to remove the const here,
      // but some non-conformant compilers error out when a const value
      // is left uninitialized.
      remove_const_t<T> addressableT;
      // implicit conversion to underlying pointer type
      operator T*() { return is_nullptr ? nullptr : &addressableT; }
   };

   static PointerMaker op(const UtScriptData& aData)
   {
      // if UtScriptClass::InterfaceMethod::operator() passed aVarArgs by
      // non-const, then we could support pass by non-const ptr.
      static_assert(std::is_const<T>::value,
                    "pass by pointer to non-const primitive is not supported "
                    "due to inability to expose writes on the pointed value back to script");

      if (aData.GetType() == UtScriptData::Type::cUNDEFINED)
      {
         return {true};
      }
      return {false, ConvertDataTo<T>::op(aData)};
   }

   struct MutablePointerMaker : protected BasicReferenceMaker<T>
   {
      MutablePointerMaker(UtScriptData& aData)
         : BasicReferenceMaker<T>(aData, T{})
         , is_nullptr(aData.GetType() == UtScriptData::Type::cUNDEFINED)
      {
         if (!is_nullptr)
         {
            BasicReferenceMaker<T>::mVal     = ConvertDataTo<T>::op(static_cast<const UtScriptData&>(aData));
            BasicReferenceMaker<T>::mOrigVal = BasicReferenceMaker<T>::mVal;
         }
      }

      // implicit conversion to underlying pointer type
      operator T*() { return is_nullptr ? nullptr : &BasicReferenceMaker<T>::mVal; }

   private:
      bool is_nullptr;
   };

   template<class TT = T, class C = enable_if_t<!std::is_const<TT>::value>>
   static MutablePointerMaker op(UtScriptData& aData)
   {
      return {aData};
   }
};

/// convert to cstring
template<class T>
struct ConvertDataTo<T*, enable_if_t<std::is_same<char, remove_const_t<T>>::value>>
{
   // you can pass by `signed char*` for pass by pointer to char,
   // or `const char*` for pass by cstring.
   static_assert(std::is_const<T>::value,
                 "pass by non-const char* is intentionally not supported due "
                 "to ambiguity between cstring and pointer to char");
   static const char* op(const UtScriptData& aData) { return aData.GetString().c_str(); }
};

/// convert to std::string*
template<class T>
struct ConvertDataTo<T*, enable_if_t<std::is_same<std::string, remove_const_t<T>>::value>>
{
   static const std::string* op(const UtScriptData& aData)
   {
      if (aData.GetType() == UtScriptData::Type::cUNDEFINED)
      {
         return nullptr;
      }
      return &aData.GetString();
   }

   // non-const UtScriptData which allows returning a mutable std::string
   static std::string* op(UtScriptData& aData)
   {
      if (aData.GetType() != UtScriptData::Type::cSTRING)
      {
         return nullptr;
      }
      return static_cast<std::string*>(aData.GetVoidPointer());
   }
};

/// convert to std::vector<UtScriptData>
template<template<class, class...> class Vec, class T, class... A>
struct ConvertDataTo<Vec<T, A...>, enable_if_t<std::is_same<std::vector<T, A...>, remove_cref_t<Vec<T, A...>>>::value>>
{
   static Vec<T>& op(const UtScriptData& aData)
   {
      // instead of creating a temporary std::vector, tell the user to use
      // a UtScriptVector<T> to transparently wrap the std::vector<UtScriptData>
      static_assert(std::is_same<UtScriptData, remove_const_t<T>>::value,
                    "can not efficiently convert vector to anything other than "
                    "std::vector<UtScriptData>. "
                    "pass by UtScriptVector<T> instead, as it will efficiently wrap the "
                    "std::vector<UtScriptData> with the api of std::vector<T>");
      return *static_cast<remove_cref_t<Vec<T>>*>(aData.GetPointer()->GetAppObject());
   }
};

template<template<class...> class Map, class Key, class Val, class... Extra>
struct ConvertDataTo<Map<Key, Val, Extra...>,
                     enable_if_t<std::is_same<std::map<Key, Val, Extra...>, remove_cref_t<Map<Key, Val, Extra...>>>::value>>
{
   static Map<Key, Val, Extra...>& op(const UtScriptData& aData)
   {
      // instead of creating a temporary std::map, force it on the user to
      // handle unwrapping UtScriptData key/vals themselves.
      static_assert(std::is_same<UtScriptData, Key>::value && std::is_same<UtScriptData, Val>::value,
                    "can not efficiently convert map to anything other than "
                    "UtScriptMap::Map<UtScriptData,UtScriptData>, you will need to take care of manually "
                    "unwrapping the UtScriptData elements yourself");
      return *static_cast<remove_cref_t<Map<Key, Val, Extra...>>*>(aData.GetPointer()->GetAppObject());
   }
};

template<class T>
struct ConvertDataTo<T&, enable_if_t<is_decay_special_return_type<T>::value>> : public ConvertDataTo<remove_const_t<T>>
{
};

template<class T>
struct ConvertDataTo<const T, enable_if_t<is_decay_special_return_type<T>::value>> :
   public ConvertDataTo<remove_const_t<T>>
{
};

template<class T>
struct ConvertDataTo<T*, enable_if_t<is_decay_special_return_type<T>::value || std::is_same<UtScriptData, remove_const_t<T>>::value>>
{
   static T* op(const UtScriptData& aData) { return &ConvertDataTo<remove_const_t<T>>::op(aData); }
};

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
//__________________________________________________________________________________________

/// helper class responsible for checking that none of the paramaters
/// of a method will cause a null dereference.
/// @tparam ArgsToCheck - list of bools that indicate if the param index
///         is a type that needs to be checked to ensure a null dereference
///         doesn't occur.
template<bool... ArgsToCheck>
struct NullDereferenceChecker
{
   static bool Check(UtScriptMethodArgs aVarArgs)
   {
      constexpr bool needs_checking[] = {ArgsToCheck...};
      for (size_t i = 0; i < sizeof...(ArgsToCheck); i++)
      {
         if (needs_checking[i])
         {
            const auto& arg = aVarArgs[i];
            if (arg.GetType() == ut::script::Data::Type::cPOINTER && !arg.GetPointer()->IsValid())
            {
               return false;
            }
         }
      }
      return true;
   }
};

// some non-conformant compilers need a specialization of empty param pack
// in order to compile NullDereferenceChecker.
template<>
struct NullDereferenceChecker<>
{
   static constexpr bool Check(UtScriptMethodArgs) { return true; }
};

//__________________________________________________________________________________________

template<class Ret, class... Args>
struct MethodInvoker
{
   template<bool VariadicCall, class Lambda, class... AdditionalArgs, size_t... ParamIndices>
   static inline enable_if_t<!VariadicCall> op(const Lambda&       aFunc,
                                               UtScriptContext&    aContext,
                                               const ScriptReturn& aReturn,
                                               UtScriptMethodArgs  aVarArgs,
                                               index_sequence<ParamIndices...>,
                                               AdditionalArgs&... aAdditionalArgs)
   {
      SetReturnData<remove_cref_t<Ret>>::op(aReturn,
                                            aFunc(aAdditionalArgs...,
                                                  aContext,
                                                  ConvertDataTo<tuple_element_t<ParamIndices, std::tuple<Args...>>>::op(
                                                     aVarArgs[ParamIndices])...));
   }

   template<bool VariadicCall, class Lambda, class... AdditionalArgs, size_t... ParamIndices>
   static inline enable_if_t<VariadicCall> op(const Lambda&       aFunc,
                                              UtScriptContext&    aContext,
                                              const ScriptReturn& aReturn,
                                              UtScriptMethodArgs  aVarArgs,
                                              index_sequence<ParamIndices...>,
                                              AdditionalArgs&... aAdditionalArgs)
   {
      SetReturnData<remove_cref_t<Ret>>::op(aReturn,
                                            aFunc(aAdditionalArgs...,
                                                  aContext,
                                                  aVarArgs,
                                                  ConvertDataTo<tuple_element_t<ParamIndices, std::tuple<Args...>>>::op(
                                                     aVarArgs[ParamIndices])...));
   }
};

/// void return type overload that doesn't need to populate a return UtScriptData.
template<class... Args>
struct MethodInvoker<void, Args...>
{
   template<bool VariadicCall, class Lambda, class... AdditionalArgs, size_t... ParamIndices>
   static inline enable_if_t<!VariadicCall> op(const Lambda&       aFunc,
                                               UtScriptContext&    aContext,
                                               const ScriptReturn& aReturn,
                                               UtScriptMethodArgs  aVarArgs,
                                               index_sequence<ParamIndices...>,
                                               AdditionalArgs&... aAdditionalArgs)
   {
      aFunc(aAdditionalArgs...,
            aContext,
            ConvertDataTo<tuple_element_t<ParamIndices, std::tuple<Args...>>>::op(aVarArgs[ParamIndices])...);
   }
   template<bool VariadicCall, class Lambda, class... AdditionalArgs, size_t... ParamIndices>
   static inline enable_if_t<VariadicCall> op(const Lambda&       aFunc,
                                              UtScriptContext&    aContext,
                                              const ScriptReturn& aReturn,
                                              UtScriptMethodArgs  aVarArgs,
                                              index_sequence<ParamIndices...>,
                                              AdditionalArgs&... aAdditionalArgs)
   {
      aFunc(aAdditionalArgs...,
            aContext,
            aVarArgs,
            ConvertDataTo<tuple_element_t<ParamIndices, std::tuple<Args...>>>::op(aVarArgs[ParamIndices])...);
   }
};

//__________________________________________________________________________________________

UTIL_SCRIPT_EXPORT bool IsIllegallReturnedMemMangement(UtScriptData& aReturnVal, UtScriptMethodArgs aVarArgs);

/// Helper class for registering script methods using the builder idiom.
template<class WrappedClass>
struct UtScriptMethodBuilder
{
   using Class = WrappedClass;

   template<class Signature, class VariadicPackArg>
   struct SigBuilder;

   /// alias of SigBuilder used because Visual Studio has false-positive warnings
   /// if we try to default VariadicPackArg=void directly on the SigBuilder class.
   template<class Signature, class VariadicPackArg = void>
   using SignatureBuilder = SigBuilder<Signature, VariadicPackArg>;

   template<class Ret, class VariadicPackArg, class... Args>
   struct SigBuilder<Ret(Args...), VariadicPackArg>
   {
      static constexpr bool IsVariadic = !std::is_same<void, VariadicPackArg>::value;

      using IndexSequence = make_index_sequence<sizeof...(Args)>;

      using NullArgsChecker = NullDereferenceChecker<(!std::is_pointer<Args>::value && !is_basic_type<Args>::value)...>;

      //! Adds a member method to `mScriptClass` with name `mFuncName` that calls `aFunc`
      template<class Lambda>
      UtScriptClass::InterfaceMethod& AddMethod(Lambda&& aFunc)
      {
         using LambdaValue = remove_reference_t<Lambda>;
         struct MemberMethod final : public UtScriptClass::InterfaceMethod
         {
            MemberMethod(Lambda&& aFunc, UtStringId aName)
               : UtScriptClass::InterfaceMethod(aName, ScriptNameOracle<Ret>::value, "", IsVariadic ? -1 : 0)
               , mFunc(std::forward<Lambda>(aFunc))
            {
               // set the mArgTypes ourselves instead of having the InterfaceMethod constructor
               // populate it via parsing of a comma-seperated string.
               mArgTypes = {ScriptNameOracle<Args>::value...};
               if /*constexpr*/ (IsVariadic)
               {
                  // the last argument in a variadic methods signature indiciates the type that
                  // all trailing arguments should be.
                  mArgTypes.push_back(ScriptNameOracle<VariadicPackArg>::value);
               }
            }

            void operator()(UtScriptExecutor*  aExecutorPtr,
                            UtScriptContext&   aContext,
                            const UtScriptRef& aReference,
                            UtScriptMethodArgs aVarArgs,
                            UtScriptData&      aReturnVal) override
            {
               assert((IsVariadic ? (aVarArgs.size() >= sizeof...(Args)) : (aVarArgs.size() == sizeof...(Args))));
               if (!aReference.IsValid() || !NullArgsChecker::Check(aVarArgs))
               {
                  ReportCallErrors(aExecutorPtr, &aReference, &aVarArgs, aReturnVal);
                  return;
               }
               auto& self = *static_cast<WrappedClass*>(aReference.GetAppObject());
               MethodInvoker<Ret, Args...>::template op<IsVariadic>(mFunc,
                                                                    aContext,
                                                                    {aReturnVal, GetReturnClass()},
                                                                    aVarArgs,
                                                                    IndexSequence{},
                                                                    self);

               // Reason this assert exists:
               // A nuance of the script macros is that `return self' breaks the garbage collection by creating two
               // seperate memory counters for the same instance. The alternative is to return a copy of self, otherwise
               // an entirely seperate macro and AddMethod logic would need to be implemented to support returning self
               // without breaking the garbage collection. Another nuance of the script macros is that returning one of
               // the parameters that was passed in as a non-const reference or non-const pointer can cause the the same
               // instance allocation to become tracked by two seperate memory counters. check to make sure that isn't
               // happening. If such behavior is desired, then you'll need to take and return a UtScriptData and then
               // modify the parameters name in InterfaceMethod::mArgTypes to be what it -actually- should be instead of
               // 'Object'.
               assert((disjunction<std::is_same<void, Ret>,
                                   is_basic_type<Ret>,
                                   is_special_return_type<remove_cref_t<Ret>>,
                                   conjunction<negation<std::is_base_of<remove_cref_t<Ret>, WrappedClass>>,
                                               is_shallow_set_return<Ret, Args>...>>::value ||
                       !IsIllegallReturnedMemMangement(aReturnVal, aVarArgs) ||
                       aReturnVal.GetPointer()->GetAppObject() == aReference.GetAppObject()));
            }

         private:
            LambdaValue mFunc;
         };
         auto* methodPtr = new MemberMethod(std::forward<Lambda>(aFunc), mFuncName);
         mScriptClass.AddMethod(std::unique_ptr<UtScriptClass::InterfaceMethod>(methodPtr));
         return *methodPtr;
      }

      //! Adds a static method to `mScriptClass` with name `mFuncName` that calls `aFunc`
      template<class Lambda>
      UtScriptClass::InterfaceMethod& AddStaticMethod(Lambda&& aFunc)
      {
         using LambdaValue = remove_reference_t<Lambda>;
         struct StaticMethod final : public UtScriptClass::InterfaceMethod
         {
            StaticMethod(Lambda&& aFunc, UtStringId aName)
               : UtScriptClass::InterfaceMethod(aName, ScriptNameOracle<Ret>::value, "", IsVariadic ? -1 : 0)
               , mFunc(std::forward<Lambda>(aFunc))
            {
               // set the mArgTypes ourselves instead of having the InterfaceMethod constructor
               // populate it via parsing of a comma-seperated string.
               mArgTypes = {ScriptNameOracle<Args>::value...};
               if /*constexpr*/ (IsVariadic)
               {
                  // the last argument in a variadic methods signature indiciates the type that
                  // all trailing arguments should be.
                  mArgTypes.push_back(ScriptNameOracle<VariadicPackArg>::value);
               }
            }

            void operator()(UtScriptExecutor*  aExecutorPtr,
                            UtScriptContext&   aContext,
                            const UtScriptRef& aReference,
                            UtScriptMethodArgs aVarArgs,
                            UtScriptData&      aReturnVal) override
            {
               assert((IsVariadic ? (aVarArgs.size() >= sizeof...(Args)) : (aVarArgs.size() == sizeof...(Args))));
               if (!NullArgsChecker::Check(aVarArgs))
               {
                  ReportCallErrors(aExecutorPtr, &aReference, &aVarArgs, aReturnVal);
                  return;
               }

               MethodInvoker<Ret, Args...>::template op<IsVariadic>(mFunc,
                                                                    aContext,
                                                                    {aReturnVal, GetReturnClass()},
                                                                    aVarArgs,
                                                                    IndexSequence{});

               // Reason this assert exists:
               // A nuance of this logic is that returning one of the parameters that was passed in as a non-const
               // reference or non-const pointer can cause the the same instance allocation to become tracked by two
               // seperate memory counters. check to make sure that isn't happening. If such behavior is desired, then
               // you'll need to take and return a UtScriptData and then modify the parameters name in
               // InterfaceMethod::mArgTypes to be what it -actually- should be instead of 'Object'.
               assert((disjunction<std::is_same<void, Ret>,
                                   is_basic_type<Ret>,
                                   is_special_return_type<remove_cref_t<Ret>>,
                                   conjunction<is_shallow_set_return<Ret, Args>...>>::value ||
                       !IsIllegallReturnedMemMangement(aReturnVal, aVarArgs)));
            }

         private:
            LambdaValue mFunc;
         };
         auto* methodPtr = new StaticMethod(std::forward<Lambda>(aFunc), mFuncName);
         mScriptClass.AddStaticMethod(std::unique_ptr<UtScriptClass::InterfaceMethod>(methodPtr));
         return *methodPtr;
      }

      //! @name infix operators for attaching methods
      //!
      //! an infix operator is needed to allow a trailing function definition (a lambda) without
      //! any extraneous candy (like closing parenthesis) after the definition.
      //!
      //! These will return the created IntefaceMethod to allow you to make further manipulations
      //! to the method if desired/necessary.
      //! @{

      //! Adds a Member method using the Lambda definition.
      template<class Lambda>
      UtScriptClass::InterfaceMethod& operator&&(Lambda&& aFunc)
      {
         return AddMethod(std::forward<Lambda>(aFunc));
      }

      //! Adds a Static method using the Lambda definition.
      template<class Lambda>
      UtScriptClass::InterfaceMethod& operator||(Lambda&& aFunc)
      {
         return AddStaticMethod(std::forward<Lambda>(aFunc));
      }

      //! @}

      UtScriptClass& mScriptClass; ///< class to add created methods to.
      UtStringId     mFuncName;    ///< name that will be given to created methods.
   };

   //! overload for accepting global/static functions
   template<class Ret, class... Args>
   static UtScriptClass::InterfaceMethod& WrapFuncPtr(UtScriptClass& aScriptClass,
                                                      UtStringId     aFuncName,
                                                      Ret (*aFuncPtr)(Args...))
   {
      return SignatureBuilder<Ret(to_wrapper_t<Args>...)>{aScriptClass, aFuncName}.AddStaticMethod(
         [aFuncPtr](UtScriptContext&, to_wrapper_t<Args>... aArgs) -> Ret
         { return (*aFuncPtr)(static_cast<to_unwrapped_t<to_wrapper_t<Args>>>(aArgs)...); });
   }

   //! overload for accepting non-const member functions
   template<class Ret, class Base, class... Args>
   static UtScriptClass::InterfaceMethod& WrapFuncPtr(UtScriptClass& aScriptClass,
                                                      UtStringId     aFuncName,
                                                      Ret (Base::*aMemFuncPtr)(Args...))
   {
      return SignatureBuilder<Ret(to_wrapper_t<Args>...)>{aScriptClass, aFuncName}.AddMethod(
         [aMemFuncPtr](WrappedClass& self, UtScriptContext&, to_wrapper_t<Args>... aArgs) -> Ret
         { return (self.*aMemFuncPtr)(static_cast<to_unwrapped_t<to_wrapper_t<Args>>>(aArgs)...); });
   }

   //! overload for accepting const member functions.
   template<class Ret, class Base, class... Args>
   static UtScriptClass::InterfaceMethod& WrapFuncPtr(UtScriptClass& aScriptClass,
                                                      UtStringId     aFuncName,
                                                      Ret (Base::*aMemFuncPtr)(Args...) const)
   {
      return SignatureBuilder<Ret(to_wrapper_t<Args>...)>{aScriptClass, aFuncName}.AddMethod(
         [aMemFuncPtr](WrappedClass& self, UtScriptContext&, to_wrapper_t<Args>... aArgs) -> Ret
         { return (self.*aMemFuncPtr)(static_cast<to_unwrapped_t<to_wrapper_t<Args>>>(aArgs)...); });
   }

   //! overload for accepting a pointer to member variable.
   // note: this enable_if sfinae is not necessary at all, as the compiler
   // will resolve the correct overload without it. However VS2015's intellisense
   // wasn't able resolve it, so the SFINAE helps it do so.
   template<class MemVarT, class Base, class = enable_if_t<!std::is_function<MemVarT>::value>>
   static UtScriptClass::InterfaceMethod& WrapFuncPtr(UtScriptClass& aScriptClass,
                                                      UtStringId     aFuncName,
                                                      MemVarT Base::*aPtrToMemVar)
   {
      return SignatureBuilder<MemVarT()>{aScriptClass, aFuncName}.AddMethod(
         [aPtrToMemVar](WrappedClass& self, UtScriptContext&) -> const MemVarT& { return self.*aPtrToMemVar; });
   }
};

//__________________________________________________________________________________________

struct MarkConstexpr
{
   UtScriptClass::InterfaceMethod& operator|=(UtScriptClass::InterfaceMethod& aMethod)
   {
      // implementation logic for marking a method to allow it to be evaluated at
      // script compile time will appear in a future enhancement nomination.
      return aMethod;
   }
};

//__________________________________________________________________________________________

struct MarkDeprecated
{
   MarkDeprecated()
      : mMessage("")
   {
   }
   MarkDeprecated(std::string aMessage)
      : mMessage(std::move(aMessage))
   {
   }
   UtScriptClass::InterfaceMethod& operator|=(UtScriptClass::InterfaceMethod& aMethod)
   {
      // implementation logic for marking a method as deprecated and emitting script
      // compliation warnings when used will appear in a future enhancement nomination.
      return aMethod;
   }
   std::string mMessage;
};

} // end namespace script
} // end namespace ut

//__________________________________________________________________________________________

/// allows overriding the script name of a c++ type for use
/// when the c++ name and the script name don't match up.
/// This differs fom the macro in UtScriptAccessible by not requiring
/// the CppClass have a member GetScriptClassName function.
#define UT_MAP_TYPE_TO_SCRIPT_NAME(CppClass, QuotedScriptName) \
   template<>                                                  \
   constexpr const char* UtScriptClassName<CppClass>()         \
   {                                                           \
      return QuotedScriptName;                                 \
   }

UT_MAP_TYPE_TO_SCRIPT_NAME(long double, "double")
UT_MAP_TYPE_TO_SCRIPT_NAME(double, "double")
UT_MAP_TYPE_TO_SCRIPT_NAME(float, "double")
UT_MAP_TYPE_TO_SCRIPT_NAME(void, "void")
UT_MAP_TYPE_TO_SCRIPT_NAME(std::string, "string")
UT_MAP_TYPE_TO_SCRIPT_NAME(UtStringId, "string")
UT_MAP_TYPE_TO_SCRIPT_NAME(UtScriptData, "Object")

//__________________________________________________________________________________________

// Mappings of class to script names for classes from the util library. These are placed here
// because the util library can't include UtScriptAccesible where the public mapping macro is defined,
// thus util lib classes are unable to use the macro in its header files. Alternatively the macro
// definition could be moved to a header in the util library instead of in the util_script libray.
class UtAtmosphere;
class UtCalendar;
class UtColor;
class UtCovariance;
class UtCovarianceEllipsoid;
class UtDCM;
class UtFileStream;
class UtPath;
class UtQuaternion;
namespace ut
{
template<class T, class Tag>
class Vec3;
namespace detail
{
class Vec3Tag;
// This exists because parsing in macros doesn't like template arguments. This is an implementation detail and should
// not be used outside this file.
using Vec3ScriptHelper = Vec3<double, Vec3Tag>;
} // namespace detail
} // namespace ut
class UtVec3dX;

UT_MAP_TYPE_TO_SCRIPT_NAME(UtAtmosphere, "Atmosphere")
UT_MAP_TYPE_TO_SCRIPT_NAME(UtCalendar, "Calendar")
UT_MAP_TYPE_TO_SCRIPT_NAME(UtColor, "Color");
UT_MAP_TYPE_TO_SCRIPT_NAME(UtCovariance, "WsfCovariance")
UT_MAP_TYPE_TO_SCRIPT_NAME(UtCovarianceEllipsoid, "Ellipsoid");
UT_MAP_TYPE_TO_SCRIPT_NAME(UtDCM, "DCM");
UT_MAP_TYPE_TO_SCRIPT_NAME(UtFileStream, "FileIO");
UT_MAP_TYPE_TO_SCRIPT_NAME(UtPath, "Path");
UT_MAP_TYPE_TO_SCRIPT_NAME(UtQuaternion, "Quaternion")
UT_MAP_TYPE_TO_SCRIPT_NAME(ut::detail::Vec3ScriptHelper, "Vec3")
UT_MAP_TYPE_TO_SCRIPT_NAME(UtVec3dX, "Vec3X")

//__________________________________________________________________________________________

// strict ansi tends to be defined if -std=c++11 is passed to gcc instead of -std=gnu++11.
// cmake uses c++11 instead of gnu++11 when CMAKE_CXX_EXTENSIONS is set to OFF
#ifdef __STRICT_ANSI__
#define UTDETAIL_USE_STANDARD_COMPLIANT_UNWRAP_PAREN
#endif

#ifdef UTDETAIL_USE_STANDARD_COMPLIANT_UNWRAP_PAREN
// implementation details of UT_VAOPT_COMMA
#define UTDETAIL_VAOPT_ARG16(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, N, ...) N
#define UTDETAIL_HAS_COMMA(...) UTDETAIL_VAOPT_ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0)
#define UTDETAIL_TRIGGER_PARENTHESIS(...) ,
#define UTDETAIL_VAOPT_HELPER1(a, b) UTDETAIL_VAOPT_HELPER2(a, b)
#define UTDETAIL_VAOPT_HELPER2(a, b) UTDETAIL_VAOPT_HELPER3_##a##b
#define UTDETAIL_VAOPT_HELPER3_01 /*01 means varpack was empty, so do not expand to a comma*/
#define UTDETAIL_VAOPT_HELPER3_00 ,
#define UTDETAIL_VAOPT_HELPER3_11 ,

/// calling UT_VAOPT_COMMA(__VA_ARGS__) is equivalent to calling c++20's __VA_OPT__(,)
///
/// expands to ` ` if this is called with no arguments (e.g. UT_VAOPT_COMMA())
/// expands to `,` if this is called with arguments.
/// Note: This macro is not compatible with _MSVC_VER<=1400 (Visual Studio 2005)
///       and could be modifed to use BOOST_PP_IS_EMPTY if such ancient
///       compatiblity is needed.
#define UT_VAOPT_COMMA(...)                                                                                           \
   UTDETAIL_VAOPT_HELPER1(/* test if there is just one argument, eventually an empty one*/                            \
                          UTDETAIL_HAS_COMMA(__VA_ARGS__), /* test if placing it between UTDETAIL_TRIGGER_PARENTHESIS \
                                                              and the parenthesis adds a comma */                     \
                          UTDETAIL_HAS_COMMA(UTDETAIL_TRIGGER_PARENTHESIS __VA_ARGS__()))

// despite the value of using a macro for UT_UNWRAP_PARENTHESIS that complies
// with the c++ preprocessor standard,  VS2015 intellisense craps out on parsing
// it when no arguments are passed to the macro. That being said, msvc still compiles
// it correctly.
/// strips parenthesis and adds a preceding comma if called with a non-empty varg pack
#define UT_UNWRAP_PARENTHESIS(...) UT_VAOPT_COMMA(__VA_ARGS__) __VA_ARGS__

#else
// detected compiler supports the common extension that causes the preceding comma
// next to ##__VA_ARGS__ to be consumed if it was empty.
// I've confirmed gcc, clang, icc, and msvc all support this non-standard extension,
// which allows the UT_VAOPT_COMMA macro logic to be bypassed.
#define UT_UNWRAP_PARENTHESIS(...) , ##__VA_ARGS__

#endif

// TODO in c++20 replace with the builtin __VA_OPT__
//#define UT_UNWRAP_PARENTHESIS(...) __VA_OPT__(,) __VA_ARGS__

//__________________________________________________________________________________________
#endif
