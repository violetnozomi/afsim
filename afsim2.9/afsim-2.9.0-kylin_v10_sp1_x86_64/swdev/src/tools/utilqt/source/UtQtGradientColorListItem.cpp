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

#include "UtQtGradientColorListItem.hpp"

#include <QString>

UtQtGradientColorListItem::UtQtGradientColorListItem(const QIcon& aIcon, const QString& aText, QListWidget* aParent, int aType)
   : QListWidgetItem(aIcon, aText, aParent, aType)
{
}

//! Compare based on the numeric value of the list item's text.
bool UtQtGradientColorListItem::operator<(const QListWidgetItem& aOther) const
{
   return (text().toDouble() < aOther.text().toDouble());
}
