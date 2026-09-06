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

#ifndef UTQT_HPP
#define UTQT_HPP

#include "utilqt_export.h"

#include <sstream>

#include <QAbstractButton>
#include <QApplication>
#include <QClipboard>
#include <QColor>
#include <QLineEdit>
#include <QPersistentModelIndex>
#include <QSet>
#include <QWidget>

#include "UtColor.hpp"

class QPersistentModelIndex;
class QStandardItemModel;
class QTreeView;
class QModelIndex;

inline QColor UTILQT_EXPORT UtQtColor(const UtColor& aColor)
{
   return QColor(int(aColor[0] * 255), int(aColor[1] * 255), int(aColor[2] * 255), int(aColor[3] * 255));
}

inline UtColor UTILQT_EXPORT UtQtColor(const QColor& aColor)
{
   return UtColor(aColor.redF(), aColor.greenF(), aColor.blueF(), aColor.alphaF());
}

namespace utqt
{
struct QColorComp
{
   // this provides a comparison operation to permit usage of QColor in sets or maps
   bool operator()(const QColor& aLHS, const QColor& aRHS) const
   {
      if (aLHS.red() < aRHS.red())
      {
         return true;
      }
      else if (aLHS.red() == aRHS.red())
      {
         if (aLHS.green() < aRHS.green())
         {
            return true;
         }
         else if (aLHS.green() == aRHS.green())
         {
            if (aLHS.blue() < aRHS.blue())
            {
               return true;
            }
            else if (aLHS.blue() == aRHS.blue())
            {
               return (aLHS.alpha() < aRHS.alpha());
            }
         }
      }
      return false;
   }
};
} // namespace utqt

inline QString UTILQT_EXPORT UtQtToString(const std::string& aStr)
{
   return QString::fromStdString(aStr);
}
template<typename T>
inline QString UtQtToString(const T& aVal)
{
   std::stringstream ss;
   ss << aVal;
   return UtQtToString(ss.str());
}

inline bool UTILQT_EXPORT UtQtFromString(const QString& aIn, int& aOut)
{
   bool ok;
   aOut = aIn.toInt(&ok);
   return ok;
}
inline bool UtQtFromString(const QString& aIn, double& aOut)
{
   bool ok;
   aOut = aIn.toDouble(&ok);
   return ok;
}
inline bool UtQtFromString(const QString& aIn, std::string& aOut)
{
   aOut = aIn.toStdString();
   return true;
}

template<typename T>
inline bool UtQtFromString(const QString& aIn, T& aOut)
{
   std::stringstream ss;
   ss << (const char*)aIn.toLatin1();
   ss >> aOut;
   if (ss)
   {
      return true;
   }
   return false;
}

void UTILQT_EXPORT UtQtSetTextColor(QWidget* aWidgetPtr, QColor aColor);

inline void UTILQT_EXPORT UtQtSetBackgroundColor(QWidget* aWidgetPtr, QColor& aColor)
{
   QPalette palette(aWidgetPtr->palette());
   palette.setColor(QPalette::Window, aColor);
   aWidgetPtr->setPalette(palette);
}

void UTILQT_EXPORT UtQtSetInvalid(QWidget* aWidgetPtr);

void UTILQT_EXPORT UtQtSetValid(QWidget* aWidgetPtr);

void UTILQT_EXPORT UtQtSetValid(QWidget* aWidgetPtr, bool aIsValid);

inline void UTILQT_EXPORT UtQtCopyToClipboard(const QString& aText)
{
   QClipboard* clipBoard = QApplication::clipboard();
   clipBoard->setText(aText, QClipboard::Clipboard);
}

inline void UTILQT_EXPORT UtQtCopyToClipboard(const std::string& aText)
{
   UtQtCopyToClipboard(QString(aText.c_str()));
}

void UTILQT_EXPORT UtQtRaiseWidget(QWidget* aWidgetPtr);
//
// class UtQtStyle
//{
//   public:
//      UtQtStyle(QWidget& aWidget)
//         : mPal(aWidget.palette()),
//           mSS(aWidget.styleSheet())
//      {
//         QFrame* framePtr = dynamic_cast<QFrame*>(&aWidget);
//         if (framePtr != 0)
//         {
//            framePtr->border
//         }
//      }
//
//      void Apply(QWidget& aWidget)
//      {
//         aWidget.setPalette(mPal);
//         //aWidget.setStyle(mStyle);
//         aWidget.setStyleSheet(mSS);
//      }
//
//   protected:
//
//      QPalette    mPal;
//      QString mSS;
//};
//
void UTILQT_EXPORT UtQtRemoveItemRow(QStandardItemModel& aModel, const QPersistentModelIndex& aIndex);

void UTILQT_EXPORT UtQtHighlightWidget(QWidget* widget);

void UTILQT_EXPORT UtQtShowSimpleMsg(const QString& windowTitle,
                                     const QString& text,
                                     const QString& informativeText = "",
                                     QWidget*       parent          = nullptr);

void UTILQT_EXPORT UtQtGetExpandedItems(QTreeView& aTree, const QModelIndex& aIndex, QSet<QPersistentModelIndex>& aExpanded);

void UTILQT_EXPORT UtQtSetExpandedItems(QTreeView&                         aTree,
                                        const QModelIndex&                 aIndex,
                                        const QSet<QPersistentModelIndex>& aExpanded);

void UTILQT_EXPORT UtQtTranslucentMenu(QMenu& aMenu);

#endif
