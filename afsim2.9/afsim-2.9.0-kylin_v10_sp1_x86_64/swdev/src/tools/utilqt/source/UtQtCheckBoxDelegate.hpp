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

#ifndef UTQTCHECKBOXDELEGATE_HPP
#define UTQTCHECKBOXDELEGATE_HPP

#include "utilqt_export.h"

#include <QStyledItemDelegate>

class UTILQT_EXPORT UtQtCheckBoxDelegate : public QStyledItemDelegate
{
public:
   UtQtCheckBoxDelegate(QObject* aParent = nullptr);
   ~UtQtCheckBoxDelegate() override;

protected:
   // Qt reimplemented
   void     paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
   QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
   void     setEditorData(QWidget* editor, const QModelIndex& index) const override;
   void     setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
   void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
   bool editorEvent(QEvent*                     event,
                    QAbstractItemModel*         model,
                    const QStyleOptionViewItem& option,
                    const QModelIndex&          index) override;

private:
   QRect CheckBoxRect(const QStyleOptionViewItem& aOptions);
};

#endif // UTQTCHECKBOXDELEGATE_HPP
