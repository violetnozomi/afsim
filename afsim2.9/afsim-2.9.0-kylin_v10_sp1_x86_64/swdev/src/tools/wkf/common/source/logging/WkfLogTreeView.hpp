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

#ifndef WKF_LOG_TREE_VIEW_HPP
#define WKF_LOG_TREE_VIEW_HPP

#include <QTreeView>

namespace wkf
{
namespace log
{
class TreeView final : public QTreeView
{
   Q_OBJECT
public:
   TreeView(QWidget* aParent) noexcept;

   bool IsExpandingByDefault() const noexcept;
   void SetExpandingByDefault(bool aExpanding) noexcept;

protected:
   bool event(QEvent* aEvent) override;

   void rowsInserted(const QModelIndex& aParent, int aStart, int aEnd) override;

private:
   QString CopySelection() const noexcept;

   bool mExpandingByDefault = false;
};
} // namespace log
} // namespace wkf

#endif
