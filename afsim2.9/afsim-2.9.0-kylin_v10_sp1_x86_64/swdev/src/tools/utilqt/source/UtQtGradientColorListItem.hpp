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

#ifndef UTQTGRADIENTCOLORLISTITEM_HPP
#define UTQTGRADIENTCOLORLISTITEM_HPP

#include "utilqt_export.h"

#include <QListWidgetItem>

//! Small subclass that exists only for the purpose of sorting items numerically
//! in the ViGradientEditor color list.
class UTILQT_EXPORT UtQtGradientColorListItem : public QListWidgetItem
{
public:
   UtQtGradientColorListItem(const QIcon& aIcon, const QString& aText, QListWidget* aParent = nullptr, int aType = Type);

   bool operator<(const QListWidgetItem& aOther) const override;
};

#endif // UTQTGRADIENTCOLORLISTITEM_HPP
