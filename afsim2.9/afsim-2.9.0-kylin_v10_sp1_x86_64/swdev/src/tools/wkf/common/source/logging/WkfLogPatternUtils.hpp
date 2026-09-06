// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKF_LOG_PATTERN_UTILS_HPP
#define WKF_LOG_PATTERN_UTILS_HPP

#include "wkf_common_export.h"

#include <QColor>
#include <QList>
#include <QRegularExpression>
#include <QString>

namespace wkf
{
namespace log
{
//! Details about a pattern match in plain text.
struct PatternMatch final
{
   int     mBegin = -1;
   int     mEnd   = -1;
   QString mPrefix;
   QString mPostfix;
};

//! Represents a pattern that the console can use.
class PatternBase
{
public:
   virtual ~PatternBase() = default;

   //! Finds the pattern in aPlainText.
   //! Returns details on how to mark it up.
   virtual PatternMatch FindInPlainText(int aStartPosition, const QString& aPlainText) const = 0;

   //! Returns the HTML prefix tag for the given raw text.
   //! If this returns anything other than an HTML tag,
   //!    the text replacement algorithm will fail.
   virtual QString PrefixTag(const QString& aRawText) const = 0;

   //! Returns the HTML postfix tag for the given raw text.
   //! If this returns anything other than an HTML tag,
   //!    the text replacement algorithm will fail.
   virtual QString PostfixTag() const = 0;
};
using PatternList = QList<const PatternBase*>;

//! The specific pattern used by the wkf::log::ConsolePrefObject.
class ColorPattern final : public PatternBase
{
public:
   ColorPattern() = default;
   ColorPattern(QString aRawPattern, QColor aColor) noexcept;

   PatternMatch FindInPlainText(int aStartPosition, const QString& aPlainText) const override;
   QString      PrefixTag(const QString& aRawText) const override;
   QString      PostfixTag() const override;

   //! Sets mRegex properties.
   //! Ensure this is called before attempting to apply patterns to the text.
   void Update();

   QString mRawPattern;
   QColor  mColor;
   bool    mMatchCase      = true;
   bool    mMatchWholeWord = false;
   bool    mUseRegex       = false;

private:
   QRegularExpression mRegex;
};

bool operator==(const ColorPattern& aLeft, const ColorPattern& aRight) noexcept;

//! Finds aRegex in aPlainText.
//! For each find, uses aPattern to create a PatternMatch.
WKF_COMMON_EXPORT PatternMatch FindRegexPatternMatch(const QRegularExpression& aRegex,
                                                     const PatternBase&        aPattern,
                                                     const int                 aStartPosition,
                                                     const QString&            aPlainText);

//! Returns the last aLines lines of aPlainText.
//! If aPlainText does not have that many lines, just returns aPlainText.
QString GetTailLines(const QString& aPlainText, int aLines);

//! Takes a string and replaces all spaces with &nbsp; and all newlines with <br/>.
//! @note Qt tries to be helpful by not inserting trailing <br/>.
//!       This function adds an extra space on the end of the string to counteract this.
QString ToHtmlWhitespaceEscaped(const QString& aPlainText);

//! Takes a list of patterns and a piece of plain text.
//! Converts the plain text into HTML and applies the aPatterns.
//! @note Qt tries to be helpful by not inserting trailing <br>.
//!       This function adds an extra space on the end of the string to counteract this.
QString ApplyPatternsToPlainText(const PatternList& aPatterns, const QString& aPlainText);
} // namespace log
} // namespace wkf

#endif
