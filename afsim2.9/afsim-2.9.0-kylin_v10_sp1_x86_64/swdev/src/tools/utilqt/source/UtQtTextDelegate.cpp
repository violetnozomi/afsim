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
#include "UtQtTextDelegate.hpp"

#include <QPainter>

void UtQtTextDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   QStyleOptionViewItem opt = option;
   initStyleOption(&opt, index);

   // draw background
   QStyle* style = mParentWidgetPtr->style();
   QString text;
   std::swap(text, opt.text);
   style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, mParentWidgetPtr);

   painter->save();

   // draw text
   mDocument.setDefaultFont(option.font);
   mDocument.setHtml(text);
   const int cTOP_MARGIN_CORRECTION = -3;
   painter->translate(opt.rect.left(), opt.rect.top() + cTOP_MARGIN_CORRECTION);
   QRect clip(0, -cTOP_MARGIN_CORRECTION, opt.rect.width(), opt.rect.height() - cTOP_MARGIN_CORRECTION);
   mDocument.drawContents(painter, clip);
   painter->restore();
}
