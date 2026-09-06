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
#ifndef UTQTTRANSPARENTBACKGROUNDDELEGATE_HPP
#define UTQTTRANSPARENTBACKGROUNDDELEGATE_HPP

#include <QPainter>
#include <QStyledItemDelegate>

class UtQtTransparentBackgroundDelegate : public QStyledItemDelegate
{
public:
   explicit UtQtTransparentBackgroundDelegate(QObject* parent = nullptr)
      : QStyledItemDelegate(parent)
   {
   }

   enum
   {
      eHIDE_TEXT       = 1,
      eHIDE_DECORATION = 2
   };

   void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
   {
      // Fill the background of the cell, and lighten if selected
      QVariant background = index.data(Qt::BackgroundRole);
      if (background.canConvert<QBrush>())
      {
         QBrush brush = background.value<QBrush>();
         if (option.state & QStyle::State_Selected)
         {
            QColor color = brush.color();
            brush.setColor(color.lighter());
         }
         painter->fillRect(option.rect, brush);
      }
      QRect textrect = option.rect;

      if (!(index.data(Qt::UserRole).toInt() & eHIDE_DECORATION))
      {
         QVariant vicon = index.data(Qt::DecorationRole);
         if ((vicon != QVariant()) && (vicon != QIcon()))
         {
            textrect.setLeft(textrect.left() + 18);
            QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
            painter->drawImage(option.rect.topLeft(), icon.pixmap(16, 16).toImage());
         }
      }

      if (!(index.data(Qt::UserRole).toInt() & eHIDE_TEXT))
      {
         QString text = index.data(Qt::DisplayRole).toString();
         painter->drawText(textrect, text);
      }
      // QStyledItemDelegate::paint(painter, option, index);

      // To draw a border on selected cells
      if (option.state & QStyle::State_Selected)
      {
         painter->save();
         QPen pen(option.palette.text().color(), 1, Qt::DotLine, Qt::SquareCap, Qt::MiterJoin);
         int  w = pen.width() / 2;
         painter->setPen(pen);
         // painter->drawRect(option.rect.adjusted(w,w,-w,-w));
         QRect rect = option.rect.adjusted(w, w, -w, -w);
         painter->drawLine(rect.topLeft(), rect.topRight());
         painter->drawLine(rect.bottomLeft(), rect.bottomRight());
         painter->restore();
      }
   }
};

#endif
