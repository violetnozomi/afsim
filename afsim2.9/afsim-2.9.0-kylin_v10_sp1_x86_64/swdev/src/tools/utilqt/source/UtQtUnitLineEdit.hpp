// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef UTQTUNITLINEEDIT_HPP
#define UTQTUNITLINEEDIT_HPP

#include <QItemDelegate>
#include <QLineEdit>

#include "UtAngleR.hpp"
#include "UtCallback.hpp"
#include "UtLatPos.hpp"
#include "UtLength.hpp"
#include "UtLonPos.hpp"
#include "UtQt.hpp"
#include "UtQtText.hpp"
#include "UtSpeed.hpp"
#include "UtTime.hpp"
#include "UtUnitTypes.hpp"
#include "UtUnitValue.hpp"

template<typename UNIT_CLASS>
class UtQtUnitLineEdit : public QLineEdit
{
public:
   using UnitType = UNIT_CLASS;

   UtQtUnitLineEdit(QWidget* aParent = nullptr)
      : QLineEdit(aParent)
      , mIsValid(false)
      , mMinBound(UnitType())
      , mMaxBound(UnitType())
      , mBounded(false)
   {
      connect(this, &QLineEdit::textChanged, this, &UtQtUnitLineEdit::HandleTextChanged);
   }
   bool IsValid() const { return mIsValid; }
   void SetIsValid(bool aIsValid) { mIsValid = aIsValid; }

   UnitType GetValue() const { return mValue; }
   void     SetValue(const UnitType& aValue, bool aIsValid = true)
   {
      mValue   = aValue;
      mIsValid = aIsValid;
      UtQtText::SetValue(*this, mValue, aIsValid);
   }

   void SetBounds(const UnitType& aMinVal, const UnitType& aMaxVal)
   {
      mMinBound = aMinVal;
      mMaxBound = aMaxVal;
      mBounded  = true;
   }

   UtCallbackListN<void()> ValueChanged;

protected:
   void HandleTextChanged(const QString& aText)
   {
      mIsValid = UtQtText::GetValue(*this, mValue);

      UtQtText::SetValid(mIsValid, *this);

      if (mIsValid && mBounded)
      {
         if (mValue < mMinBound)
         {
            SetValue(mMinBound);
         }
         else if (mValue > mMaxBound)
         {
            SetValue(mMaxBound);
         }
         else
         {
            ValueChanged();
         }
      }
      else
      {
         ValueChanged();
      }
   }

   bool     mIsValid;
   UnitType mValue;
   UnitType mMinBound;
   UnitType mMaxBound;
   bool     mBounded;
};

// aliases allow use in Qt Creator
using UtQtLatPosLineEdit = UtQtUnitLineEdit<UtLatPos>;
using UtQtLonPosLineEdit = UtQtUnitLineEdit<UtLonPos>;

template<typename UNIT_CLASS>
class UtQtUnitLineEditDelegate : public QItemDelegate
{
public:
   UtQtUnitLineEditDelegate()           = default;
   ~UtQtUnitLineEditDelegate() override = default;
   QWidget* createEditor(QWidget* aParent, const QStyleOptionViewItem& aOption, const QModelIndex& aIndex) const override
   {
      return new UtQtUnitLineEdit<UNIT_CLASS>(aParent);
   }
   void setModelData(QWidget* aEditor, QAbstractItemModel* aModel, const QModelIndex& aIndex) const override
   {
      UtQtUnitLineEdit<UNIT_CLASS>* editor = (UtQtUnitLineEdit<UNIT_CLASS>*)(aEditor);
      if (editor->IsValid()) // reject the data if it is invalid
      {
         QItemDelegate::setModelData(aEditor, aModel, aIndex);
      }
   }
};

#endif
