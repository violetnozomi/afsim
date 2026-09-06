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
#ifndef WKF_LOG_DOCK_WIDGET_HPP
#define WKF_LOG_DOCK_WIDGET_HPP

#include "wkf_common_export.h"

#include <QTemporaryFile>
#include <QTextBrowser>

#include "WkfLogPatternUtils.hpp"

namespace wkf
{
namespace log
{
//! The DockWidget contains a single tabbed widget.
//! The first tab contains raw console output and cannot be closed.
//! The rest of the tabs can be created or destroyed at will and allow the user to
//!    filter, sort, and search the message logs.
class WKF_COMMON_EXPORT ConsoleWidget : public QTextBrowser
{
   Q_OBJECT

public:
   explicit ConsoleWidget(QWidget* aParent = nullptr);

   //! Queues aText to be added to the console.
   //! When adding text in bulk, use this and call @see AppendQueueToConsole() when finished.
   void QueueForConsole(const QString& aText);

   //! Adds all queued text to the console.
   //! If console was scrolled to the bottom, stays at the bottom.
   //! Otherwise, stays where it was.
   void AppendQueueToConsole();

   //! Clears the console.
   void Reset() noexcept;

protected:
   //! Updates the console if it is the currently displayed tab.
   void showEvent(QShowEvent* aEvent) override;

   //! Returns any additional patterns to be applied to the colors.
   virtual PatternList GetCustomPatterns() const { return {}; }

private:
   //! Sets the font for the raw console output.
   //! Sets the font to "Consolas" on Windows, and "Courier New" on Linux.
   void SetFontSize(int aFontSize);
   void UpdateColors();

   void ShowContextMenu(const QPoint& pos);
   //! Prompts the user for a save file, and writes mPlainText to it.
   //! Non-const because it needs to manipulate mFile.
   void ExportText();

   //! Converts aText from plain text to HTML.
   //! Applies the colors in preferences and GetCustomPatterns().
   QString ConvertToHtml(const QString& aText) const;

   QString mPlainText;  //!< Contains the latest mLines lines of plain text for quick access.
   QString mQueuedText; //!< Contains the text received between gui updates.
   int     mLines = 0;  //!< Contains the number of lines rendered.
                   //!< To reduce the number of full re-renders, this is allowed to go up to 1.5 times the line cap.
   QTemporaryFile mFile; //!< Stores the complete plain text received.
                         //!< Storing several megabytes of data in a QString can lead to memory errors.
};
} // namespace log
} // namespace wkf

#endif
