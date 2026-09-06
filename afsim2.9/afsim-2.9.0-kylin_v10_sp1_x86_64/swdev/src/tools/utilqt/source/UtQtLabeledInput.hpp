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

#ifndef UT_QT_LABELED_INPUT_HPP
#define UT_QT_LABELED_INPUT_HPP

#include <type_traits>

#include <QHBoxLayout>
#include <QLabel>

#include "UtQtMemory.hpp"

namespace ut
{
namespace qt
{
template<typename T>
class LabeledInput : public QWidget
{
   static_assert(std::is_base_of<QWidget, T>::value, "LabeledInput<T>: T must derive from QWidget.");

public:
   explicit LabeledInput(QWidget* aParent = nullptr, const QString& aLabel = {})
      : QWidget(aParent)
   {
      mLayout = ut::qt::make_qt_ptr<QHBoxLayout>(this);

      mLayout->setMargin(0);

      mInputWidget = ut::qt::make_qt_ptr<T>(this);
      mLabel       = ut::qt::make_qt_ptr<QLabel>(this);
      mLabel->setText(aLabel);

      mLayout->addWidget(mLabel);
      mLayout->addWidget(mInputWidget);

      mLayout->setStretch(1, 1);

      setLayout(mLayout);
   }

   void SetLabel(const QString& aLabel) { mLabel->setText(aLabel); }

   QString GetLabel() const { return mLabel->text(); }

   T* GetWidget() noexcept { return mInputWidget; }

   const T* GetWidget() const noexcept { return mInputWidget; }

private:
   QHBoxLayout* mLayout      = nullptr;
   T*           mInputWidget = nullptr;
   QLabel*      mLabel       = nullptr;
};
} // namespace qt
} // namespace ut

#endif
