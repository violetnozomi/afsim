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

#include "WkfLogTreeView.hpp"

#include <stack>

#include <QClipboard>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QScrollBar>

static int Depth(QModelIndex aIndex) noexcept
{
   int retval = 0;
   while (aIndex.isValid())
   {
      retval++;
      aIndex = aIndex.parent();
   }
   return retval - 1;
}

static bool Compare(QModelIndex aLeft, QModelIndex aRight) noexcept
{
   auto lp = aLeft.parent();
   auto rp = aRight.parent();
   if (lp == rp)
   {
      return aLeft.row() < aRight.row();
   }
   else if (lp == aRight)
   {
      return false;
   }
   else if (aLeft == rp)
   {
      return true;
   }

   int ld = Depth(aLeft);
   int rd = Depth(aRight);
   if (ld < rd)
   {
      return Compare(aLeft, rp);
   }
   else if (ld > rd)
   {
      return Compare(lp, aRight);
   }
   else
   {
      return Compare(lp, rp);
   }
}

wkf::log::TreeView::TreeView(QWidget* aParent) noexcept
   : QTreeView(aParent)
{
   setUniformRowHeights(true);
}

bool wkf::log::TreeView::IsExpandingByDefault() const noexcept
{
   return mExpandingByDefault;
}

void wkf::log::TreeView::SetExpandingByDefault(bool aExpanding) noexcept
{
   mExpandingByDefault = aExpanding;
}

bool wkf::log::TreeView::event(QEvent* aEvent)
{
   if (aEvent->type() == QEvent::KeyPress)
   {
      auto keyEvent = static_cast<QKeyEvent*>(aEvent);
      if ((keyEvent->key() == Qt::Key::Key_C) && (keyEvent->modifiers() & Qt::KeyboardModifier::ControlModifier))
      {
         aEvent->accept();
         if (!selectedIndexes().empty())
         {
            QGuiApplication::clipboard()->setText(CopySelection());
         }
         return true;
      }
   }
   return QTreeView::event(aEvent);
}

void wkf::log::TreeView::rowsInserted(const QModelIndex& aParent, int aStart, int aEnd)
{
   auto* scrollBar = verticalScrollBar();
   bool  atBottom  = (scrollBar->value() == scrollBar->maximum());

   QTreeView::rowsInserted(aParent, aStart, aEnd);

   if (mExpandingByDefault)
   {
      // Note: aEnd is inclusive for this function
      for (int i = aStart; i <= aEnd; i++)
      {
         expand(model()->index(i, 0, aParent));
      }
   }

   if (atBottom)
   {
      scrollToBottom();
   }
}

QString wkf::log::TreeView::CopySelection() const noexcept
{
   auto selected = selectedIndexes();

   std::sort(selected.begin(), selected.end(), Compare);

   QString retval;

   int                     indent = -1;
   std::stack<QModelIndex> parents;

   for (const auto& index : selected)
   {
      // Add new line, except for first time
      if (indent >= 0)
      {
         retval += "\n";
      }

      // Determine how indent should be adjusted.
      indent++;
      while (true)
      {
         if (parents.empty())
         {
            indent = 0;
            break;
         }
         else if (index.parent() == parents.top())
         {
            break;
         }
         else
         {
            parents.pop();
            indent--;
         }
      }

      // Apply indent
      for (int i = 0; i < indent; i++)
      {
         retval += "    ";
      }

      // Add contents
      retval += index.data().toString();

      // Prepare for next item
      parents.emplace(index);
   }

   return retval;
}
