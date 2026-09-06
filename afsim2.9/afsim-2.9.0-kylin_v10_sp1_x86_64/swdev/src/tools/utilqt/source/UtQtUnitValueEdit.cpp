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
#include "UtQtUnitValueEdit.hpp"

#include <QMenu>

UtQtUnitValueEdit::UtQtUnitValueEdit(QWidget*          aParent /*= 0*/,
                                     UtUnits::UnitType aUnitType /*= static_cast<UtUnits::UnitType>(0)*/)
   : QLineEdit(aParent)
{
   SetUnitType(aUnitType);
   connect(this, &QLineEdit::textChanged, this, &UtQtUnitValueEdit::HandleTextChanged);
}

void UtQtUnitValueEdit::HandleTextChanged(const QString& aValue)
{
   mIsValid = false;
   if (mUnitTypePtr)
   {
      QStringList splitValues = aValue.trimmed().split(QRegExp("\\s+"));
      // A valid input should be split into a number and a unit, with enforced whitespace in between. Additional text is not allowed.
      if (splitValues.size() == 2)
      {
         bool   conversionOk;
         double value = splitValues[0].toDouble(&conversionOk);
         if (conversionOk)
         {
            int unitId = mUnitTypePtr->ReadUnit(splitValues[1].toStdString());
            if (unitId != -1)
            {
               if (ApplyBounds(value, unitId))
               {
                  bool block = blockSignals(true);
                  setText(QString("%1 %2").arg(value).arg(QString::fromStdString(mUnitTypePtr->FindUnitName(unitId))));
                  blockSignals(block);
               }
               mUnitId  = unitId;
               mValue   = value;
               mIsValid = true;
            }
         }
      }
   }
   UpdateValidityColor();
   emit ValueChanged();
}

void UtQtUnitValueEdit::SetUnitType(UtUnitType* aUnitType)
{
   ClearBounds();
   mUnitTypePtr = aUnitType;
   HandleTextChanged(text());
}

void UtQtUnitValueEdit::ClearBounds()
{
   mMinBound = std::numeric_limits<double>::lowest();
   mMaxBound = std::numeric_limits<double>::max();
}

void UtQtUnitValueEdit::contextMenuEvent(QContextMenuEvent* e)
{
   QMenu* popup = createStandardContextMenu();

   if (IsValid() && mUnitTypePtr)
   {
      const auto& unitToString   = mUnitTypePtr->GetUnitToString();
      QMenu*      convertSubmenu = nullptr;
      double      stdVal         = mUnitTypePtr->ConvertToStandard(mValue, mUnitId);
      for (const auto& unit : unitToString)
      {
         if (unit.first != mUnitId)
         {
            if (!convertSubmenu)
            {
               popup->addSeparator();
               convertSubmenu = popup->addMenu("Convert To");
            }
            double             newVal = mUnitTypePtr->ConvertFromStandard(stdVal, unit.first);
            std::ostringstream oss;
            oss << newVal << ' ' << unit.second;
            QString  newValStr = oss.str().c_str();
            QAction* actionPtr = convertSubmenu->addAction(newValStr);
            QObject::connect(actionPtr, &QAction::triggered, this, [=]() { setText(newValStr); });
         }
      }
   }
   popup->popup(QCursor::pos());
}

bool UtQtUnitValueEdit::ApplyBounds(double& aValue, const int aUnit)
{
   double value(aValue); // Copy input value to compare with output value
   aValue = UtMath::Limit(value,
                          mUnitTypePtr->ConvertFromStandard(mMinBound, aUnit),
                          mUnitTypePtr->ConvertFromStandard(mMaxBound, aUnit));
   return value != aValue;
}
