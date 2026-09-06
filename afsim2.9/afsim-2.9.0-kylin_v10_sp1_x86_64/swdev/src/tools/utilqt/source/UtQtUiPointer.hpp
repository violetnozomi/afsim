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

#ifndef UT_QT_UI_POINTER_HPP
#define UT_QT_UI_POINTER_HPP

#include <type_traits>

#include <QPointer>
class QObject;

namespace ut
{
namespace qt
{
// this class exists to clean up ui elements. Ui elements may sometimes either be the
// responsibility of the application or Qt relationships, depending on if the UI ever shows.
// This automates the conditional cleanup.
template<class T>
class UiPointer
{
   static_assert(std::is_base_of<QObject, T>::value,
                 "Template argument to ut::qt::UiPointer must be a subclass of QObject");

public:
   UiPointer() = default;
   UiPointer(T* aPtr)
      : mPtr(aPtr)
   {
   }
   //! Since this acts like a std::unique_ptr, delete the copy constructor
   UiPointer(const UiPointer& /* aSrc */) = delete;
   UiPointer(UiPointer&& aSrc) noexcept
   {
      mPtr      = aSrc.mPtr;
      aSrc.mPtr = nullptr;
   }
   //! Since this acts like a std::unique_ptr, delete the copy assignment operator
   UiPointer<T>& operator=(const UiPointer<T>& /* aSrc */) = delete;
   UiPointer<T>& operator                                  =(UiPointer<T>&& aSrc)
   {
      if (this != &aSrc)
      {
         mPtr      = aSrc.mPtr;
         aSrc.mPtr = nullptr;
      }
      return *this;
   }
   UiPointer& operator=(T* aRHS)
   {
      if (mPtr && mPtr != aRHS)
      {
         delete mPtr.data();
      }
      mPtr = aRHS;
      return *this;
   }
   ~UiPointer() noexcept
   {
      if (mPtr != nullptr)
      {
         delete mPtr.data();
      }
   }
   T* data() const { return mPtr.data(); }
   T* operator->() const { return mPtr.data(); }
      operator T*() const { return mPtr.data(); }

private:
   QPointer<T> mPtr;
};
} // namespace qt
} // namespace ut

#endif
