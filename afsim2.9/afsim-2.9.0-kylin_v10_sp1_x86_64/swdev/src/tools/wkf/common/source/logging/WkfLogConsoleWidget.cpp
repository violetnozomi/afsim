// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfLogConsoleWidget.hpp"

#include <QDesktopServices>
#include <QPlainTextEdit>
#include <QSaveFile>
#include <QScrollBar>
#include <QTextStream>

#include "UtMemory.hpp"
#include "WkfEnvironment.hpp"
#include "WkfLogConsolePrefObject.hpp"
#include "WkfLogNotificationsPrefObject.hpp"
#include "WkfMainWindow.hpp"

wkf::log::ConsoleWidget::ConsoleWidget(QWidget* aParent /*= nullptr*/)
   : QTextBrowser(aParent)
{
   setReadOnly(true);
   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &QPlainTextEdit::customContextMenuRequested, this, &wkf::log::ConsoleWidget::ShowContextMenu);

   const auto* prefs = wkfEnv.GetPreferenceObject<wkf::log::ConsolePrefObject>();
   if (prefs)
   {
      connect(prefs, &wkf::log::ConsolePrefObject::FontSizeChanged, this, &wkf::log::ConsoleWidget::SetFontSize);
      connect(prefs, &wkf::log::ConsolePrefObject::HtmlChanged, this, &wkf::log::ConsoleWidget::UpdateColors);

      SetFontSize(prefs->GetFontSize());
   }

   mFile.open();
   Reset();
}

void wkf::log::ConsoleWidget::QueueForConsole(const QString& aText)
{
   mQueuedText += aText;
}

void wkf::log::ConsoleWidget::AppendQueueToConsole()
{
   if (!mQueuedText.isEmpty())
   {
      auto* scrollBar = verticalScrollBar();
      bool  atBottom  = (scrollBar->value() == scrollBar->maximum());

      mPlainText += mQueuedText;
      mLines += mQueuedText.count('\n');

      // To reduce the number of full re-renders, allow the number of lines to exceed the line cap by 50%.
      const auto* prefs = wkfEnv.GetPreferenceObject<wkf::log::ConsolePrefObject>();
      if (mLines > 1.5 * prefs->GetLineCap())
      {
         mLines       = prefs->GetLineCap();
         mPlainText   = GetTailLines(mPlainText, mLines);
         QString html = ConvertToHtml(mPlainText);
         document()->setHtml(html);
      }
      else
      {
         auto inserter = textCursor();
         inserter.movePosition(QTextCursor::End);
         inserter.insertHtml(ConvertToHtml(mQueuedText));
      }

      QTextStream stream{&mFile};
      stream << mQueuedText;

      mQueuedText.clear();

      if (atBottom)
      {
         scrollBar->setValue(scrollBar->maximum());
      }
   }
}

void wkf::log::ConsoleWidget::Reset() noexcept
{
   document()->clear();
   mPlainText.clear();
   mQueuedText.clear();
   mLines = 0;

   mFile.seek(0);
   mFile.resize(0);
}

void wkf::log::ConsoleWidget::showEvent(QShowEvent* aEvent)
{
   const auto* prefs = wkfEnv.GetPreferenceObject<wkf::log::ConsolePrefObject>();
   SetFontSize(prefs->GetFontSize());
   aEvent->accept();
}

void wkf::log::ConsoleWidget::SetFontSize(int aFontSize)
{
   // This is the way font is selected in Wizard's output.
#ifdef _WIN32
   document()->setDefaultFont(QFont{"Consolas", aFontSize});
#else
   document()->setDefaultFont(QFont{"Courier New", aFontSize});
#endif
}

void wkf::log::ConsoleWidget::UpdateColors()
{
   auto*      scrollBar = verticalScrollBar();
   const int  scrollPos = scrollBar->value();
   const bool atBottom  = (scrollPos == scrollBar->maximum());

   mPlainText += mQueuedText;
   mQueuedText.clear();

   const auto* prefs = wkfEnv.GetPreferenceObject<ConsolePrefObject>();
   QString     html  = ConvertToHtml(GetTailLines(mPlainText, prefs->GetLineCap()));
   document()->setHtml(html);

   if (atBottom)
   {
      scrollBar->setValue(scrollBar->maximum());
   }
   else
   {
      scrollBar->setValue(scrollPos);
   }
}

void wkf::log::ConsoleWidget::ShowContextMenu(const QPoint& pos) // this is a slot
{
   QMenu contextMenu(tr("Log Context menu"), this);

   QAction action1("Export Simulation Log", this->parentWidget());
   connect(&action1, &QAction::triggered, this, &wkf::log::ConsoleWidget::ExportText);
   contextMenu.addAction(&action1);

   contextMenu.exec(mapToGlobal(pos));
}

void wkf::log::ConsoleWidget::ExportText()
{
   QString qPath = wkf::getSaveFileName(nullptr, "Select a Save Location", "", "AFSIM Log Save File (*.log)");
   if (!qPath.isEmpty())
   {
      QSaveFile saveFile{qPath};
      if (saveFile.open(QIODevice::WriteOnly))
      {
         constexpr qint64 cBUFFER_LEN = 65536;
         const qint64     pos         = mFile.pos();
         mFile.seek(0);
         char    buffer[cBUFFER_LEN];
         quint64 len = mFile.read(buffer, cBUFFER_LEN);
         while (len > 0)
         {
            saveFile.write(buffer, len);
            len = mFile.read(buffer, cBUFFER_LEN);
         }
         mFile.seek(pos);
         saveFile.commit();
      }
   }
}

QString wkf::log::ConsoleWidget::ConvertToHtml(const QString& aText) const
{
   PatternList patterns = GetCustomPatterns();
   auto        prefs    = wkfEnv.GetPreferenceObject<ConsolePrefObject>();
   if (prefs)
   {
      patterns += prefs->GetPatterns();
   }
   return wkf::log::ApplyPatternsToPlainText(patterns, aText);
}
