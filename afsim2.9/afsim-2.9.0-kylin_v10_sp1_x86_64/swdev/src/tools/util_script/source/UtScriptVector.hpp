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

#ifndef UTSCRIPTVECTORWRAPPER_HPP
#define UTSCRIPTVECTORWRAPPER_HPP

#include "util_script_export.h"

#include <stdexcept> // for std::logic_error

#include "UtMemory.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptMethodBuilder.hpp"
#include "UtScriptTypes.hpp"

namespace ut
{
namespace script
{

template<class... Args>
struct is_directly_emplacable
{
   using first_arg = tuple_element_t<0, std::tuple<Args...>>;
   static constexpr bool value =
      (sizeof...(Args) == 1) &&
      std::is_constructible<UtScriptData, first_arg>::value
      // disallow situations where UtScriptData is constructible due to pointer implicit conversion to bool
      && !std::is_pointer<remove_reference_t<first_arg>>::value
      // disallow unforseen implicit conversion operators
      && is_basic_type<first_arg>::value;
};

class UTIL_SCRIPT_EXPORT VectorWrapperBase
{
public:
   using ScriptVector = std::vector<UtScriptData>;
   using size_type    = ScriptVector::size_type;

   VectorWrapperBase() = delete;

   explicit VectorWrapperBase(UtScriptContext& aContext);

   VectorWrapperBase(ScriptVector& aScriptVec, UtScriptClass* aElementClassPtr)
      : mVec(&aScriptVec)
      , mElementClass(aElementClassPtr)
      , mOwnsVec(false)
   {
   }

   VectorWrapperBase(const VectorWrapperBase& aCopy);

   VectorWrapperBase(VectorWrapperBase&& aMove) noexcept;

   ~VectorWrapperBase();

   using allocator_type = ScriptVector::allocator_type;
   allocator_type get_allocator();

   /// removes the last element
   void pop_back();

   /// @name capacity
   /// @{
   /// reserves storage
   void reserve(size_type aCapacity);
   void resize(size_type aSize);
   /// reduces the memory usage by freeing unused memory
   void shrink_to_fit();
   /// returns the number of elements that can be held in currently allocated storage
   size_type capacity() const noexcept { return mVec->capacity(); }
   /// returns the number of elements
   size_type size() const noexcept { return mVec->size(); }
   /// checks whether the conatiner is empty
   /*[[nodiscard]]*/ bool empty() const noexcept { return mVec->empty(); }
   /// @}

   /// clears the contents
   void clear() noexcept;

   /// direct access to the underlying array
   UtScriptData* data() const noexcept;

   /// swaps the contents
   void swap(VectorWrapperBase& aOther) noexcept;

   class UTIL_SCRIPT_EXPORT IteratorBase
   {
   public:
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::random_access_iterator_tag;

      explicit IteratorBase(UtScriptData* aPtr) noexcept
         : mPtr(aPtr)
      {
      }

      // explicit operator bool() const noexcept { return mPtr != nullptr; }
      bool operator==(const IteratorBase& aRhs) const noexcept { return mPtr == aRhs.mPtr; }
      bool operator!=(const IteratorBase& aRhs) const noexcept { return mPtr != aRhs.mPtr; }

      difference_type operator-(const IteratorBase& aRhs) const noexcept { return std::distance(aRhs.mPtr, mPtr); }

      bool operator<(const IteratorBase& aRhs) const noexcept { return (aRhs - *this) > 0; }
      bool operator>(const IteratorBase& aRhs) const noexcept { return aRhs < *this; }
      bool operator>=(const IteratorBase& aRhs) const noexcept { return !(*this < aRhs); }
      bool operator<=(const IteratorBase& aRhs) const noexcept { return !(*this > aRhs); }

      UtScriptData* GetPtr() const noexcept { return mPtr; }

   protected:
      UtScriptData* mPtr;
   };

   template<class T>
   class Iterator : public VectorWrapperBase::IteratorBase
   {
   protected:
      using IteratorBase = VectorWrapperBase::IteratorBase;

   public:
      using iterator_category = std::random_access_iterator_tag;

      using Refer           = conditional_t<std::is_pointer<T>::value, T, T&>;
      using ConstRefer      = conditional_t<std::is_pointer<T>::value, const T, const T&>;
      using value_type      = T;
      using reference       = decltype(ConvertDataTo<Refer>::op(*mPtr));
      using const_reference = decltype(ConvertDataTo<ConstRefer>::op(*mPtr));

      using IteratorBase::IteratorBase; // pull in constructor

      Iterator& operator=(const Iterator&) noexcept = default;

      Iterator& operator+=(std::ptrdiff_t aMovement) noexcept
      {
         mPtr += aMovement;
         return *this;
      }
      Iterator& operator-=(std::ptrdiff_t aMovement) noexcept
      {
         mPtr -= aMovement;
         return *this;
      }
      Iterator& operator++() noexcept
      {
         ++mPtr;
         return *this;
      }
      Iterator& operator--() noexcept
      {
         --mPtr;
         return *this;
      }
      Iterator operator++(int) noexcept
      {
         auto temp(*this);
         ++mPtr;
         return temp;
      }
      Iterator operator--(int) noexcept
      {
         auto temp(*this);
         --mPtr;
         return temp;
      }

      using IteratorBase::operator-;
      Iterator            operator+(std::ptrdiff_t aMovement) noexcept { return Iterator{mPtr + aMovement}; }
      Iterator            operator-(std::ptrdiff_t aMovement) noexcept { return Iterator{mPtr - aMovement}; }

      reference       operator[](size_type aIndex) { return *(*this + aIndex); }
      const_reference operator[](size_type aIndex) const { return *(*this + aIndex); }

      reference       operator*() { return ConvertDataTo<Refer>::op(*mPtr); }
      const_reference operator*() const { return ConvertDataTo<ConstRefer>::op(*mPtr); }

      reference       operator->() { return operator*(); }
      const_reference operator->() const { return operator*(); }

      /// one way implicit conversion: iterator -> const_iterator
      operator Iterator<const T>() const noexcept { return Iterator<const T>{mPtr}; }
   };

   template<class T>
   class ReverseIterator : public VectorWrapperBase::Iterator<T>
   {
      using Iterator = VectorWrapperBase::Iterator<T>;
      using Iterator::mPtr;

   public:
      using iterator_category = std::random_access_iterator_tag;
      using reference         = typename Iterator::reference;
      using const_reference   = typename Iterator::const_reference;

      using Iterator::Iterator; // pull in constructor

      ReverseIterator& operator=(const ReverseIterator&) noexcept = default;
      ReverseIterator& operator                                   =(const Iterator& aRhs) noexcept
      {
         mPtr = aRhs.mPtr;
         return *this;
      }

      ReverseIterator& operator+=(std::ptrdiff_t aMovement) noexcept
      {
         mPtr -= aMovement;
         return *this;
      }
      ReverseIterator& operator-=(std::ptrdiff_t aMovement) noexcept
      {
         mPtr += aMovement;
         return *this;
      }
      ReverseIterator& operator++() noexcept
      {
         --mPtr;
         return *this;
      }
      ReverseIterator& operator--() noexcept
      {
         ++mPtr;
         return *this;
      }
      ReverseIterator operator++(int) noexcept
      {
         auto temp(*this);
         --mPtr;
         return temp;
      }
      ReverseIterator operator--(int) noexcept
      {
         auto temp(*this);
         ++mPtr;
         return temp;
      }
      ReverseIterator operator+(std::ptrdiff_t aMovement) noexcept { return Iterator{mPtr - aMovement}; }
      ReverseIterator operator-(std::ptrdiff_t aMovement) noexcept { return Iterator{mPtr + aMovement}; }

      reference       operator[](size_type aIndex) { return *(*this + aIndex); }
      const_reference operator[](size_type aIndex) const { return *(*this + aIndex); }

      std::ptrdiff_t operator-(const ReverseIterator& aRhs) const noexcept { return std::distance(mPtr, aRhs.mPtr); }

      bool operator<(const ReverseIterator& aRhs) const noexcept { return (aRhs - *this) > 0; }
      bool operator>(const ReverseIterator& aRhs) const noexcept { return aRhs < *this; }
      bool operator>=(const ReverseIterator& aRhs) const noexcept { return !(*this < aRhs); }
      bool operator<=(const ReverseIterator& aRhs) const noexcept { return !(*this > aRhs); }

      /// one way implicit conversion: iterator -> const_iterator
      operator ReverseIterator<const T>() const noexcept { return ReverseIterator<const T>{mPtr}; }
   };

   struct null_dereference_error : public std::logic_error
   {
      using std::logic_error::logic_error; // pull in constructors
   };

protected:
   /// emplace back an empty UtScriptData
   UtScriptData& PushBack();

   /// emplace an empty UtScriptData at the given location
   UtScriptData& EmplaceAt(UtScriptData* aPos);

   VectorWrapperBase& operator=(const VectorWrapperBase& aOther) noexcept;
   VectorWrapperBase& operator=(VectorWrapperBase&& aOther) noexcept;

   ScriptVector*    mVec;
   UtScriptContext* mContext{nullptr};
   UtScriptClass*   mElementClass{nullptr}; ///< cached as a member variable
   bool             mOwnsVec;
};


template<class T>
class VectorWrapper : public VectorWrapperBase
{
   static_assert(!std::is_reference<T>::value, "not allowed to create a UtScriptVector of references");

public:
   using Refer           = conditional_t<std::is_pointer<T>::value, T, T&>;
   using ConstRefer      = conditional_t<std::is_pointer<T>::value, const T, const T&>;
   using value_type      = T;
   using reference       = decltype(ConvertDataTo<Refer>::op(std::declval<UtScriptData&>()));
   using const_reference = decltype(ConvertDataTo<ConstRefer>::op(std::declval<UtScriptData&>()));

   using VectorWrapperBase::VectorWrapperBase; // pull in constructor
   VectorWrapper(const VectorWrapper& aCopy)
      : VectorWrapperBase(aCopy)
   {
   }

   VectorWrapper(VectorWrapper&& aMove) noexcept
      : VectorWrapperBase(std::move(aMove))
   {
   }

   /// @name Element Access
   /// @{

   /// access the specified element with bounds and null dereference checking.
   reference at(size_type aIndex)
   {
      auto& element = mVec->at(aIndex);
      if /*constexpr*/ (!is_basic_type<T>::value && !std::is_pointer<T>::value)
      {
         if (element.GetType() != UtScriptData::Type::cPOINTER || !element.GetPointer()->IsValid())
         {
            throw null_dereference_error("dereference error: The UtScriptData is null");
         }
      }
      return operator[](aIndex);
   }

   /// access the specified element with bounds and null dereference checking.
   const_reference at(size_type aIndex) const
   {
      auto& element = mVec->at(aIndex);
      if /*constexpr*/ (!is_basic_type<T>::value && !std::is_pointer<T>::value)
      {
         if (element.GetType() == UtScriptData::Type::cPOINTER && !element.GetPointer()->IsValid())
         {
            throw null_dereference_error("dereference error: The UtScriptData is null");
         }
      }
      return operator[](aIndex);
   }

   /// access the specified element
   reference operator[](size_type aIndex) { return ConvertDataTo<Refer>::op((*mVec)[aIndex]); }

   /// access the specified element
   const_reference operator[](size_type aIndex) const { return ConvertDataTo<ConstRefer>::op((*mVec)[aIndex]); }

   /// accesss the first element
   reference front() { return operator[](0); }
   /// accesss the first element
   const_reference front() const { return operator[](0); }

   /// accesss the last element
   reference back() { return operator[](size() - 1); }
   /// accesss the last element
   const_reference back() const { return operator[](size() - 1); }

   /// @}

   /// @name Iterators
   /// @{
   using iterator       = Iterator<T>;
   using const_iterator = Iterator<const T>;

   using reverse_iterator       = ReverseIterator<T>;
   using const_reverse_iterator = ReverseIterator<const T>;

   /// returns an iterator to the beginning
   iterator begin() noexcept { return iterator(&data()[0]); }
   /// returns an iterator to the end
   iterator end() noexcept { return iterator(&data()[size()]); }

   /// returns an iterator to the beginning
   const_iterator begin() const noexcept { return cbegin(); }
   /// returns an iterator to the end
   const_iterator end() const noexcept { return cend(); }

   /// returns an iterator to the beginning
   const_iterator cbegin() const noexcept { return const_iterator(&data()[0]); }
   /// returns an iterator to the end
   const_iterator cend() const noexcept { return const_iterator(&data()[size()]); }

   /// returns a reverse iterator to the beginning
   reverse_iterator rbegin() noexcept { return reverse_iterator(&data()[size() - 1]); }
   /// returns a reverse iterator to the end
   reverse_iterator rend() noexcept { return reverse_iterator(&data()[-1]); }

   /// returns a reverse iterator to the beginning
   const_reverse_iterator rbegin() const noexcept { return crbegin(); }
   /// returns a reverse iterator to the end
   const_reverse_iterator rend() const noexcept { return crend(); }

   /// returns a reverse iterator to the beginning
   const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(&data()[size() - 1]); }
   /// returns a reverse iterator to the end
   const_reverse_iterator crend() const noexcept { return const_reverse_iterator(&data()[-1]); }
   /// @}


   /// @name modifiers
   /// @{

   /// swaps the contents
   void swap(VectorWrapper& aOther) noexcept { VectorWrapperBase::swap(aOther); }

   /// adds an element to the end
   void push_back(const T& aVal) { PushBackP(aVal); }

   /// adds an element to the end
   void push_back(T&& aVal) { PushBackP(std::move(aVal)); }

   /// constructs an element in-place at the end.
   template<class... Args>
   auto emplace_back(Args&&... aArgs) -> enable_if_t<!is_directly_emplacable<Args...>::value>
   {
      static_assert(std::is_constructible<remove_const_t<value_type>, Args&&...>::value,
                    "value_type is not constructible from the provided arguements");

      if /*constexpr*/ (is_basic_type<T>::value || is_decay_special_return_type<T>::value ||
                        std::is_pointer<value_type>::value)
      {
         ScriptReturn ret{PushBack(), GetElementScriptClass()};
         SetReturnData<remove_const_t<value_type>>::op(ret, value_type{std::forward<Args>(aArgs)...});
      }
      else
      {
         ScriptReturn ret{PushBack(), GetElementScriptClass()};
         SetReturnData<std::unique_ptr<remove_const_t<value_type>>>::op(ret,
                                                                        ut::make_unique<remove_const_t<value_type>>(
                                                                           std::forward<Args>(aArgs)...));
      }
   }
   /// constructs an element in-place at the end.
   template<class... Args>
   auto emplace_back(Args&&... aArgs) -> enable_if_t<is_directly_emplacable<Args...>::value>
   {
      // specialized emplace_back for basic type data, where UtScriptData supports
      // being constructed directly from the value type.
      static_assert(std::is_constructible<remove_const_t<value_type>, Args&&...>::value,
                    "value_type is not constructible from the provided arguements");
      mVec->emplace_back(std::forward<Args>(aArgs)...);
   }

   /// inserts elements
   void insert(const_iterator aPos, const T& aVal)
   {
      ScriptReturn ret{EmplaceAt(aPos.GetPtr()), GetElementScriptClass()};
      SetReturnData<remove_const_t<T>>::op(ret, aVal);
   }

   /// inserts elements
   void insert(const_iterator aPos, T&& aVal)
   {
      ScriptReturn ret{EmplaceAt(aPos.GetPtr()), GetElementScriptClass()};
      SetReturnData<remove_const_t<T>>::op(ret, std::move(aVal));
   }

   /// inserts elements
   void insert(const_iterator aPos, size_type aCount, const T& aVal)
   {
      UtScriptData scriptData;
      ScriptReturn ret{scriptData, GetElementScriptClass()};
      SetReturnData<remove_const_t<T>>::op(ret, aVal);
      mVec->insert(mVec->begin() + (std::distance(aPos.GetPtr(), data())), aCount, scriptData);
   }

   /// inserts elements
   template<class InputIt>
   void insert(const_iterator aPos, InputIt aFirst, InputIt aLast)
   {
      ptrdiff_t dist = std::distance(cbegin(), aPos);
      reserve(size() + std::distance(aFirst, aLast));
      aPos = cbegin() + dist; // restore aPos after it was invalidated by reserve
      while (aFirst != aLast)
      {
         insert(aPos, *aFirst);
         aPos++;
         aFirst++;
      }
   }

   /// inserts elements
   void insert(const_iterator aPos, std::initializer_list<T> aList)
   {
      ptrdiff_t dist = std::distance(cbegin(), aPos);
      reserve(size() + aList.size());
      aPos = cbegin() + dist; // restore aPos after it was invalidated by reserve
      for (const auto& val : aList)
      {
         insert(aPos, val);
         aPos++;
      }
   }

   /// constructs element in-place
   template<class... Args>
   void emplace(const_iterator aPos, Args&&... aArgs)
   {
      ScriptReturn ret{EmplaceAt(aPos), GetElementScriptClass()};
      if (is_basic_type<T>::value || is_decay_special_return_type<T>::value || std::is_pointer<value_type>::value)
      {
         SetReturnData<remove_const_t<value_type>>::op(ret, value_type{std::forward<Args>(aArgs)...});
      }
      else
      {
         SetReturnData<std::unique_ptr<remove_const_t<value_type>>>::op(ret,
                                                                        ut::make_unique<remove_const_t<value_type>>(
                                                                           std::forward<Args>(aArgs)...));
      }
   }

   /// erases an element
   iterator erase(const_iterator aPos) { return {&(*(mVec->erase(mVec->begin() + (aPos() - begin()))))}; }

   /// erases elements
   iterator erase(const_iterator aFirst, const_iterator aLast)
   {
      return iterator{&(*(mVec->erase(mVec->begin() + (aFirst() - begin()), mVec->begin() + (aLast() - begin()))))};
   }

   /// changes the number of elements stored
   using VectorWrapperBase::resize;
   /// changes the number of elements stored
   void resize(size_type aCount, const value_type& aVal)
   {
      if (aCount > size())
      {
         UtScriptData scriptData;
         ScriptReturn ret{scriptData, GetElementScriptClass()};
         SetReturnData<remove_const_t<value_type>>::op(ret, aVal);
         mVec->resize(aCount, scriptData);
      }
      else
      {
         resize(aCount);
      }
   }
   /// @}

   /// @name assignment operators
   /// @{
   VectorWrapper& operator=(const std::vector<T>& aOther)
   {
      clear();
      reserve(aOther.size());
      for (const auto& element : aOther)
      {
         push_back(element);
      }
      return *this;
   }

   VectorWrapper& operator=(std::vector<T>&& aOther)
   {
      clear();
      reserve(aOther.size());
      for (auto&& element : aOther)
      {
         push_back(std::move(element));
      }
      return *this;
   }

   VectorWrapper& operator=(const VectorWrapper& aOther)
   {
      VectorWrapperBase::operator=(aOther);
      return *this;
   }

   VectorWrapper& operator=(VectorWrapper&& aOther) noexcept
   {
      VectorWrapperBase::operator=(std::move(aOther));
      return *this;
   }

   VectorWrapper& operator=(std::initializer_list<T> aList)
   {
      clear();
      reserve(aList.size());
      for (const auto& val : aList)
      {
         push_back(val);
      }
      return *this;
   }

   void assign(size_type aCount, const T& aValue)
   {
      clear();
      insert(begin(), aValue);
   }

   template<class InputIt>
   void assign(InputIt first, InputIt last)
   {
      clear();
      insert(begin(), first, last);
   }

   void assign(std::initializer_list<T> aList)
   {
      clear();
      insert(begin(), std::move(aList));
   }
   /// @}

   /// allow static_cast conversion to a normal std::vector<T>
   explicit operator std::vector<T>() const { return std::vector<T>(begin(), end()); }
   /// allow  conversion to a normal std::vector<T>
   std::vector<T> toStdVector() const { return std::vector<T>(begin(), end()); }

private:
   template<class TT>
   auto PushBackP(TT&& aVal) -> enable_if_t<!is_directly_emplacable<TT>::value>
   {
      ScriptReturn ret{PushBack(), GetElementScriptClass()};
      SetReturnData<remove_const_t<T>>::op(ret, std::forward<TT>(aVal));
   }

   template<class TT>
   auto PushBackP(TT&& aVal) -> enable_if_t<is_directly_emplacable<TT>::value>
   {
      // specialized push_back for basic type data, where UtScriptData supports
      // being constructed directly from the value type.
      mVec->emplace_back(std::forward<TT>(aVal));
   }

   /// get the UtScriptClass that corresponds to the element type
   UtScriptClass* GetElementScriptClass()
   {
      if /*constexpr*/ (is_basic_type<T>::value)
      {
         // setting the UtScriptData with basic types does not need to use the
         // UtScriptClass associated with that data type.
         return nullptr;
      }
      if (mElementClass == nullptr)
      {
         assert(mContext != nullptr);
         mElementClass = mContext->GetTypes()->GetClass(ScriptNameOracle<value_type>::value);
      }
      return mElementClass;
   }

   friend struct SetReturnData<VectorWrapper<T>>;
};

template<class T>
struct is_special_return_type<VectorWrapper<T>> : true_type
{
};

template<class T>
struct to_vector_wrapper;
template<class T>
struct to_vector_wrapper<std::vector<T>>
{
   using type = VectorWrapper<T>;
};

template<class T>
struct to_wrapper<T, enable_if_t<is_vector<remove_cref_t<T>>::value && !std::is_same<std::vector<UtScriptData>, remove_cref_t<T>>::value>> :
   public to_vector_wrapper<remove_cref_t<T>>
{
   static_assert(!std::is_lvalue_reference<T>::value || std::is_const<remove_reference_t<T>>::value,
                 "support for wrapping a non-const std::vector<T>& is not implemented");
};

template<class T>
struct to_unwrapped<VectorWrapper<T>>
{
   using type = std::vector<T>;
};

template<class T>
struct GetScriptName<VectorWrapper<T>>
{
   static std::string      op() { return std::string{"Array<"} + GetScriptName<T>::op() + '>'; }
   static const UtStringId value;
};
template<class T>
const UtStringId GetScriptName<VectorWrapper<T>>::value = GetScriptName<VectorWrapper<T>>::op();

/// convert to a wrapper around std::vector<UtScriptData> that
/// emulates the API of std::vector<T>
template<class T>
struct ConvertDataTo<VectorWrapper<T>>
{
   static VectorWrapper<T> op(const UtScriptData& aData)
   {
      UtScriptRef* ref = aData.GetPointer();
      return {*ref->GetAppObject<std::vector<UtScriptData>>(), ref->GetScriptClass()->GetContainerDataType()};
   }
};

/// sets a VectorWrapper
template<class T>
struct SetReturnData<VectorWrapper<T>>
{
   /// called during return by value.
   static void op(const ScriptReturn& aReturn, VectorWrapper<T>&& aVec)
   {
      if (aVec.mOwnsVec)
      {
         aReturn.mVal.SetPointer(new UtScriptRef(aVec.mVec, aReturn.mClassPtr, UtScriptRef::cMANAGE));
         aVec.mOwnsVec = false;
      }
      else
      {
         auto* objPtr = new VectorWrapperBase::ScriptVector(std::move(*aVec.mVec));
         aReturn.mVal.SetPointer(new UtScriptRef(objPtr, aReturn.mClassPtr, UtScriptRef::cMANAGE));
      }
   }

   /// called during return by const reference/const value.
   static void op(const ScriptReturn& aReturn, const VectorWrapper<T>& aVec)
   {
      auto* objPtr = new VectorWrapperBase::ScriptVector((*aVec.mVec));
      aReturn.mVal.SetPointer(new UtScriptRef(objPtr, aReturn.mClassPtr, UtScriptRef::cMANAGE));
   }
};


} // namespace script
} // namespace ut

template<class T>
using UtScriptVector = ut::script::VectorWrapper<T>;

namespace std
{
/// specialization for UtScriptVector iterator
template<class T>
struct iterator_traits<::ut::script::VectorWrapperBase::Iterator<T>>
{
   using Iter              = ::ut::script::VectorWrapperBase::Iterator<T>;
   using difference_type   = ptrdiff_t;
   using iterator_category = random_access_iterator_tag;
   using reference         = typename Iter::reference;
   using value_type        = T;
   using pointer           = reference;
};

/// specialization for UtScriptVector reverse iterator
template<class T>
struct iterator_traits<::ut::script::VectorWrapperBase::ReverseIterator<T>>
{
   using Iter              = ::ut::script::VectorWrapperBase::Iterator<T>;
   using difference_type   = ptrdiff_t;
   using iterator_category = random_access_iterator_tag;
   using reference         = typename Iter::reference;
   using value_type        = T;
   using pointer           = reference;
};

/// specialization for swapping UtScriptVector types
template<class T>
void swap(UtScriptVector<T>& aLhs, UtScriptVector<T>& aRhs)
{
   aLhs.swap(aRhs);
}
} // namespace std

#endif
