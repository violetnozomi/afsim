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

#include "WkfLogPatternUtils.hpp"

#include <stdexcept>

//! Returns the index after the first plain-text character in an HTML string, starting from aHtmlPosition.
//! If the first character is followed by an HTML markup, returns the index of the beginning of the markup.
//! '&escaped;' and '<br/>' both count as a single "plain-text" character.
static int AdvancePosition(int aHtmlPosition, const QString& aHtml)
{
   if (aHtmlPosition >= aHtml.size())
   {
      throw std::runtime_error("Position exceeds string size.");
   }

   const QChar character = aHtml.at(aHtmlPosition);
   if (character == '&')
   {
      // Treats entire escape sequence as a single character.
      while (aHtml.at(aHtmlPosition) != ';')
      {
         aHtmlPosition++;
      }
   }
   else if (character == '<')
   {
      // The only HTML markup that should be after aHtmlPosition in the string is '<br/>'.
      while (aHtml.at(aHtmlPosition) != '>')
      {
         aHtmlPosition++;
      }
   }

   return aHtmlPosition + 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

wkf::log::ColorPattern::ColorPattern(QString aRawPattern, QColor aColor) noexcept
   : mRawPattern(std::move(aRawPattern))
   , mColor(std::move(aColor))
{
   Update();
}

wkf::log::PatternMatch wkf::log::ColorPattern::FindInPlainText(int aStartPosition, const QString& aPlainText) const
{
   return wkf::log::FindRegexPatternMatch(mRegex, *this, aStartPosition, aPlainText);
}

QString wkf::log::ColorPattern::PrefixTag(const QString& aRawText) const
{
   const QString format = "<font color=\"%1\">";
   return format.arg(mColor.name());
}

QString wkf::log::ColorPattern::PostfixTag() const
{
   return "</font>";
}

void wkf::log::ColorPattern::Update()
{
   // mRawPattern = mRawPattern.trimmed();
   QString pattern = mRawPattern;
   if (!mUseRegex)
   {
      pattern = QRegularExpression::escape(pattern);
   }
   if (mMatchWholeWord)
   {
      pattern.push_front("\\b");
      pattern.push_back("\\b");
   }

   auto options = mRegex.patternOptions();
   options.setFlag(QRegularExpression::CaseInsensitiveOption, !mMatchCase);
   options.setFlag(QRegularExpression::MultilineOption);
   mRegex.setPatternOptions(options);
   mRegex.setPattern(pattern);
}

bool wkf::log::operator==(const ColorPattern& aLeft, const ColorPattern& aRight) noexcept
{
   return (aLeft.mRawPattern == aRight.mRawPattern) && (aLeft.mColor == aRight.mColor) &&
          (aLeft.mMatchCase == aRight.mMatchCase) && (aLeft.mMatchWholeWord == aRight.mMatchWholeWord) &&
          (aLeft.mUseRegex == aRight.mUseRegex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

wkf::log::PatternMatch wkf::log::FindRegexPatternMatch(const QRegularExpression& aRegex,
                                                       const PatternBase&        aPattern,
                                                       const int                 aStartPosition,
                                                       const QString&            aPlainText)
{
   if (aRegex.isValid())
   {
      QRegularExpressionMatch match;
      int                     position = aPlainText.indexOf(aRegex, aStartPosition, &match);

      while (position >= 0)
      {
         const bool skip = (match.capturedLength() == 0);
         if (!skip)
         {
            PatternMatch retval;
            retval.mBegin   = position;
            retval.mEnd     = match.capturedEnd();
            retval.mPrefix  = aPattern.PrefixTag(aPlainText.mid(position, match.capturedLength()));
            retval.mPostfix = aPattern.PostfixTag();
            return retval;
         }
         position = aPlainText.indexOf(aRegex, position + 1, &match);
      }
   }
   return {};
}

QString wkf::log::GetTailLines(const QString& aPlainText, int aLines)
{
   int index = 0;
   for (int i = 0; i < aLines; i++)
   {
      index = aPlainText.lastIndexOf('\n', index - 1);
      if (index <= 0)
      {
         return aPlainText;
      }
   }
   return aPlainText.mid(index);
}

QString wkf::log::ToHtmlWhitespaceEscaped(const QString& aPlainText)
{
   QString html = aPlainText.toHtmlEscaped();
   html.replace("\n", "<br/>");
   html.replace(" ", "&nbsp;");
   // Qt tries to be helpful by not inserting trailing <br>.
   // Adding an extraneous space prevents this.
   html.push_back(" ");
   return html;
}

QString wkf::log::ApplyPatternsToPlainText(const wkf::log::PatternList& aPatterns, const QString& aPlainText)
{
   struct Algorithm
   {
      Algorithm(const PatternList& aPatterns, const QString& aPlainText)
         : mPatterns(aPatterns)
         , mNumPatterns(aPatterns.size())
         , mPlainText(aPlainText)
         , mHtml(ToHtmlWhitespaceEscaped(aPlainText))
      {
      }

      QString ComputeHtml()
      {
         // Initialize mMatches.
         for (const auto* pattern : mPatterns)
         {
            mMatches.emplace_back(pattern->FindInPlainText(0, mPlainText));
         }

         // Apply matches.
         while (ApplyNextMatch())
         {
            UpdateMatches();
         }
         return mHtml;
      }

      bool ApplyNextMatch()
      {
         const int matchIndex = GetNextMatchIndex();
         if (matchIndex >= 0)
         {
            const auto& match = mMatches[matchIndex];
            InsertHtml(match.mBegin, match.mPrefix);
            InsertHtml(match.mEnd, match.mPostfix);
            return true;
         }
         return false;
      }

      void UpdateMatches()
      {
         for (int i = 0; i < mNumPatterns; i++)
         {
            auto& match = mMatches[i];
            if (match.mBegin >= 0 && match.mBegin < mTextIndex)
            {
               match = mPatterns[i]->FindInPlainText(mTextIndex, mPlainText);
            }
         }
      }

      int GetNextMatchIndex() const
      {
         int firstStartPosition = std::numeric_limits<int>::max();
         int nextMatchIndex     = -1;
         for (int i = 0; i < mNumPatterns; i++)
         {
            const int beginPosition = mMatches[i].mBegin;
            if (beginPosition >= 0 && beginPosition < firstStartPosition)
            {
               firstStartPosition = beginPosition;
               nextMatchIndex     = i;
            }
         }
         return nextMatchIndex;
      }

      void InsertHtml(int aPosition, const QString& aText)
      {
         while (mTextIndex < aPosition)
         {
            mHtmlIndex = ::AdvancePosition(mHtmlIndex, mHtml);
            mTextIndex++;
         }
         mHtml.insert(mHtmlIndex, aText);
         mHtmlIndex += aText.size();
      }

      const PatternList& mPatterns;
      const int          mNumPatterns;
      const QString&     mPlainText;

      QString mHtml;
      int     mHtmlIndex = 0;
      int     mTextIndex = 0;

      std::vector<PatternMatch> mMatches;
   };

   return Algorithm(aPatterns, aPlainText).ComputeHtml();
}
