// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTQTGRADIENTEDITDELEGATE_HPP
#define UTQTGRADIENTEDITDELEGATE_HPP

#include "utilqt_export.h"

#include <QItemDelegate>
#include <QString>

//! Edit delegate for the items in the ViGradientEditor color list.
class UTILQT_EXPORT UtQtGradientEditDelegate : public QItemDelegate
{
   Q_OBJECT

public:
   UtQtGradientEditDelegate(double aMinValue, double aMaxValue, QObject* aParent = nullptr);

   QWidget* createEditor(QWidget* aParent, const QStyleOptionViewItem& aStyle, const QModelIndex& aIndex) const override;
   bool     editorEvent(QEvent*                     aEvent,
                        QAbstractItemModel*         aModel,
                        const QStyleOptionViewItem& aStyle,
                        const QModelIndex&          aIndex) override;
   void     setEditorData(QWidget* aEditor, const QModelIndex& aIndex) const override;
   void     setModelData(QWidget* aEditor, QAbstractItemModel* aModel, const QModelIndex& aIndex) const override;
   void updateEditorGeometry(QWidget* aEditor, const QStyleOptionViewItem& aStyle, const QModelIndex& aIndex) const override;

public slots:
   void HandleTextEdited(const QString& aText);

private:
   bool    IsTextValid(const QString& aText) const;
   QString InvalidTextMessage() const;

   double mMinValue;
   double mMaxValue;
};

#endif // UTQTGRADIENTEDITDELEGATE_HPP
