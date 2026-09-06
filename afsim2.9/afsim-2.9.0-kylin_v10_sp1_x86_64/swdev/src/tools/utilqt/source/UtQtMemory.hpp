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

#ifndef UTQTMEMORY_HPP
#define UTQTMEMORY_HPP

#include <memory>

#include <QObject>
#include <QTreeWidgetItem>

#include "UtMemory.hpp"

namespace ut
{
namespace qt
{
// This method is used to obfuscate static analysis tools such as Fortify, to reduce the number
// of false positive reportings of memory leaks.
template<class T, class... Args>
T* make_qt_ptr(Args&&... args)
{
   static_assert(std::is_base_of<QObject, T>::value || std::is_same<QTreeWidgetItem, T>::value,
                 "make_qt_ptr: template T must derive from QObject or be a QTreeWidgetItem");
   return ut::make_unique<T>(std::forward<Args>(args)...).release();
}
} // namespace qt
} // namespace ut

#endif // UTQTMEMORY_HPP
