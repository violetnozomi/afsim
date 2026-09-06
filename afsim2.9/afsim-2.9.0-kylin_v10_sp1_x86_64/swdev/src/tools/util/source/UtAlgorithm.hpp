// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
#ifndef UTALGORITHM_HPP
#define UTALGORITHM_HPP

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

struct UtSequenceDiffSection
{
   enum SectionType
   {
      cREMOVED,
      cSAME,
      cADDED
   };

   SectionType mSectionType{cSAME};
   // for cREMOVED and cSAME
   std::pair<size_t, size_t> mBeforeRange;
   // for cSAME and cADDED
   std::pair<size_t, size_t> mAfterRange;
};

//! Diffs two sequences to arrive at a list of sections mapping the old sequence to the new sequence.
// Usage:
// typedef std::vector<std::string>    SequenceType;
// typedef std::equal_to<std::string>  CompareFunctor;
// SequenceType s1;
// SequenceType s2;
//   // Fill sequences ...
// UtSequenceDiff<SequenceType, CompareFunctor> differ(s1, s2, CompareFunctor());
// differ.Diff();
// for (size_t i = 0; i < differ.mDiffs.size(); ++i) {
//    // Do something with the section differ.mDiffs[i]...
// }
template<typename CONTAINER, typename COMPARE_FN>
struct UtSequenceDiff
{
public:
   using MatchList = std::vector<std::pair<size_t, size_t>>;

   // Supports larger sequence diffing, but expects to operate on containers with values
   // that are significant.  A container of characters would probably do poorly.
   // Expected use-case is lines contained in std::vector<std::string>
   static std::vector<UtSequenceDiffSection> DiffLarge(const CONTAINER& aLhs, const CONTAINER& aRhs, COMPARE_FN aCompare)
   {
      // check for empty
      if (aLhs.empty() || aRhs.empty())
      {
         return ReportNoMatches(aLhs, aRhs);
      }

      // try to find a common suffix / prefix
      size_t prefixLength = 0;
      while (prefixLength < aLhs.size() && prefixLength < aRhs.size() && aLhs[prefixLength] == aRhs[prefixLength])
      {
         ++prefixLength;
      }

      // quit not if exact match
      if (prefixLength == aLhs.size() && prefixLength == aRhs.size())
      {
         UtSequenceDiffSection sc;
         sc.mSectionType = UtSequenceDiffSection::cSAME;
         sc.mBeforeRange = sc.mAfterRange = std::make_pair(0, prefixLength);
         return std::vector<UtSequenceDiffSection>(1, sc);
      }

      const size_t cMIN_CONTAINER_SIZE = std::min(aLhs.size(), aRhs.size());
      size_t       suffixLength        = 0;
      while (suffixLength + prefixLength < cMIN_CONTAINER_SIZE &&
             aLhs[aLhs.size() - 1 - suffixLength] == aRhs[aRhs.size() - 1 - suffixLength])
      {
         ++suffixLength;
      }

      UtSequenceDiffSection sc;
      if (suffixLength > 0)
      {
         sc.mSectionType = UtSequenceDiffSection::cSAME;
         sc.mBeforeRange = std::make_pair(aLhs.size() - suffixLength, aLhs.size());
         sc.mAfterRange  = std::make_pair(aRhs.size() - suffixLength, aRhs.size());
      }

      UtSequenceDiffSection pc;
      if (prefixLength > 0)
      {
         pc.mSectionType = UtSequenceDiffSection::cSAME;
         pc.mBeforeRange = pc.mAfterRange = std::make_pair(0, prefixLength);
      }

      if (prefixLength || suffixLength)
      {
         // a common suffix / prefix is found, recurse
         CONTAINER                          lhsSub(aLhs.begin() + prefixLength, aLhs.end() - suffixLength);
         CONTAINER                          rhsSub(aRhs.begin() + prefixLength, aRhs.end() - suffixLength);
         std::vector<UtSequenceDiffSection> changes = DiffLarge(lhsSub, rhsSub, aCompare);
         if (prefixLength)
         {
            TranslateChanges(changes, prefixLength, prefixLength);
            changes.insert(changes.begin(), pc);
         }
         if (suffixLength)
         {
            changes.push_back(sc);
         }
         return changes;
      }

      // No common suffix / prefix, if the size is small enough do the optimal approach
      if (aLhs.size() <= 100 && aRhs.size() <= 100)
      {
         UtSequenceDiff d(aLhs, aRhs, aCompare);
         d.Diff();
         return d.GetDiffs();
      }

      // size is too big, reduce the problem size

      // pick the biggest container, and divide it in half
      const CONTAINER* biggest  = &aLhs;
      const CONTAINER* smallest = &aRhs;
      if (biggest->size() < smallest->size())
      {
         std::swap(biggest, smallest);
      }

      size_t       bestSplitIndex(smallest->size() / 2); // default split index if one is not found
      const size_t halfSize(biggest->size() / 2);
      using ValueType = typename CONTAINER::value_type;
      // Now figure out what split of the other side matches the most lines
      // relaxing line ordering
      {
         // gather a count of all lines in front and back halves
         std::unordered_map<ValueType, size_t> countInFront, countInBack;
         for (size_t i = 0; i < biggest->size(); ++i)
         {
            const ValueType& v = (*biggest)[i];
            if (i < halfSize)
            {
               ++countInFront[v];
            }
            else
            {
               ++countInBack[v];
            }
         }

         // for each split position, compute the maximum number of matches
         // starting from top matching against the first half
         // and them from the bottom matching against the second half

         std::vector<size_t> before(smallest->size());
         std::vector<size_t> after(smallest->size());

         size_t beforeCurrentMatches = 0;
         for (size_t i = 0; i < smallest->size(); ++i)
         {
            const auto& v         = (*smallest)[i];
            auto        iterFront = countInFront.find(v);
            if (iterFront != countInFront.end())
            {
               ++beforeCurrentMatches;
               if (--iterFront->second <= 0)
               {
                  countInFront.erase(iterFront);
               }
            }
            before[i] = beforeCurrentMatches;
         }

         size_t afterCurrentMatches = 0;
         for (size_t i = 0; i < smallest->size(); ++i)
         {
            size_t      idx      = smallest->size() - 1 - i;
            const auto& v        = (*smallest)[idx];
            auto        iterBack = countInBack.find(v);
            if (iterBack != countInBack.end())
            {
               ++afterCurrentMatches;
               if (--iterBack->second <= 0)
               {
                  countInBack.erase(iterBack);
               }
            }
            after[idx] = afterCurrentMatches;
         }

         // now find the best split index
         size_t bestSplitMatch(0);
         for (size_t i = 0; i < smallest->size(); ++i)
         {
            size_t totalMatches = after[i] + before[i];
            if (totalMatches >= bestSplitMatch)
            {
               bestSplitMatch = totalMatches;
               bestSplitIndex = i;
            }
         }
      }

      CONTAINER bigHalf1(biggest->begin(), biggest->begin() + halfSize);
      CONTAINER smallHalf1(smallest->begin(), smallest->begin() + bestSplitIndex);
      CONTAINER bigHalf2(biggest->begin() + halfSize, biggest->end());
      CONTAINER smallHalf2(smallest->begin() + bestSplitIndex, smallest->end());

      if (biggest == &aLhs)
      {
         std::vector<UtSequenceDiffSection> halfMatches1 = DiffLarge(bigHalf1, smallHalf1, aCompare);
         std::vector<UtSequenceDiffSection> halfMatches2 = DiffLarge(bigHalf2, smallHalf2, aCompare);
         TranslateChanges(halfMatches2, bigHalf1.size(), smallHalf1.size());
         halfMatches1.insert(halfMatches1.end(), halfMatches2.begin(), halfMatches2.end());
         return halfMatches1;
      }
      else
      {
         std::vector<UtSequenceDiffSection> halfMatches1 = DiffLarge(smallHalf1, bigHalf1, aCompare);
         std::vector<UtSequenceDiffSection> halfMatches2 = DiffLarge(smallHalf2, bigHalf2, aCompare);
         TranslateChanges(halfMatches2, smallHalf1.size(), bigHalf1.size());
         halfMatches1.insert(halfMatches1.end(), halfMatches2.begin(), halfMatches2.end());
         return halfMatches1;
      }
   }

   UtSequenceDiff(const CONTAINER& aLhs, const CONTAINER& aRhs, COMPARE_FN aCompare)
      : mLHS(aLhs)
      , mRHS(aRhs)
      , mCompare(aCompare)
      , mSectionsGenerated(false)
   {
      mSize1        = mLHS.size();
      mSize2        = mRHS.size();
      size_t size12 = mSize1 * mSize2;
      mMem          = std::vector<int>(size12, -1);
   }

   void Diff()
   {
      subdp();
      backtrack();
   }

   // Returns the list of matched pairs.  The last match is a pseudo-match of the end-of-list
   const MatchList& GetMatches() const { return mMatches; }

   // Return the list of diff sections
   const std::vector<UtSequenceDiffSection>& GetDiffs()
   {
      if (!mSectionsGenerated)
      {
         size_t                lhsIndex = 0;
         size_t                rhsIndex = 0;
         UtSequenceDiffSection currentMatch;
         currentMatch.mSectionType = UtSequenceDiffSection::cSAME;

         for (size_t matchIndex = 0; matchIndex < mMatches.size(); ++matchIndex)
         {
            auto i = mMatches[matchIndex].first;
            auto j = mMatches[matchIndex].second;

            if (lhsIndex < i || rhsIndex < j)
            {
               // need to write current match
               if (currentMatch.mAfterRange.first != currentMatch.mAfterRange.second)
               {
                  mDiffs.push_back(currentMatch);
                  currentMatch.mAfterRange = currentMatch.mBeforeRange = std::pair<int, int>();
               }
            }

            if (lhsIndex < i)
            {
               UtSequenceDiffSection mod;
               mod.mSectionType        = UtSequenceDiffSection::cREMOVED;
               mod.mBeforeRange.first  = lhsIndex;
               mod.mBeforeRange.second = i;
               mDiffs.push_back(mod);
            }

            if (rhsIndex < j)
            {
               UtSequenceDiffSection mod;
               mod.mSectionType       = UtSequenceDiffSection::cADDED;
               mod.mAfterRange.first  = rhsIndex;
               mod.mAfterRange.second = j;
               mDiffs.push_back(mod);
            }

            if (currentMatch.mAfterRange.first != currentMatch.mAfterRange.second)
            {
               ++currentMatch.mAfterRange.second;
               ++currentMatch.mBeforeRange.second;
            }
            else
            {
               // start new match
               currentMatch.mBeforeRange.first  = i;
               currentMatch.mBeforeRange.second = i + 1;
               currentMatch.mAfterRange.first   = j;
               currentMatch.mAfterRange.second  = j + 1;
            }
            lhsIndex = i + 1;
            rhsIndex = j + 1;
         }
         mSectionsGenerated = true;
      }
      return mDiffs;
   }

protected:
   const CONTAINER& mLHS;
   const CONTAINER& mRHS;
   COMPARE_FN       mCompare;
   std::vector<int> mMem;
   size_t           mSize1;
   size_t           mSize2;
   bool             mSectionsGenerated;

   // Intermediate list of matched values
   MatchList mMatches;
   //! Sections in order that are either removed, added, or remain the same
   std::vector<UtSequenceDiffSection> mDiffs;

private:
   static void TranslateChanges(std::vector<UtSequenceDiffSection>& aChanges, size_t aBeforeOffset, size_t aAfterOffset)
   {
      for (auto& change : aChanges)
      {
         if (change.mSectionType == UtSequenceDiffSection::cREMOVED || change.mSectionType == UtSequenceDiffSection::cSAME)
         {
            change.mBeforeRange.first += aBeforeOffset;
            change.mBeforeRange.second += aBeforeOffset;
         }

         if (change.mSectionType == UtSequenceDiffSection::cADDED || change.mSectionType == UtSequenceDiffSection::cSAME)
         {
            change.mAfterRange.first += aAfterOffset;
            change.mAfterRange.second += aAfterOffset;
         }
      }
   }

   static std::vector<UtSequenceDiffSection> ReportNoMatches(const CONTAINER& aLhs, const CONTAINER& aRhs)
   {
      std::vector<UtSequenceDiffSection> changes;
      UtSequenceDiffSection              c;
      if (!aLhs.empty())
      {
         c.mSectionType        = UtSequenceDiffSection::cREMOVED;
         c.mBeforeRange.first  = 0;
         c.mBeforeRange.second = aLhs.size();
         changes.push_back(c);
      }

      if (!aRhs.empty())
      {
         c.mSectionType       = UtSequenceDiffSection::cADDED;
         c.mAfterRange.first  = 0;
         c.mAfterRange.second = aRhs.size();
         changes.push_back(c);
      }
      return changes;
   }

   int& L(size_t i, size_t j) { return mMem[i * mSize2 + j]; }
   int  subL(size_t i, size_t j)
   {
      if (i >= mSize1)
      {
         return 0;
      }
      if (j >= mSize2)
      {
         return 0;
      }
      return L(i, j);
   }
   // A dynamic programming solution:
   // we always compute L(i,j) using L(>i, >j)
   void subdp()
   {
      for (size_t i = mSize1 - 1; i < mSize1; --i)
      {
         for (size_t j = mSize2 - 1; j < mSize2; --j)
         {
            if (mCompare(mLHS[i], mRHS[j]))
            {
               L(i, j) = 1 + subL(i + 1, j + 1);
            }
            else
            {
               L(i, j) = std::max(subL(i + 1, j), subL(i, j + 1));
            }
         }
      }
   }
   void backtrack()
   {
      size_t i = 0, j = 0;
      while (i < mSize1 && j < mSize2)
      {
         if (mCompare(mLHS[i], mRHS[j]))
         {
            mMatches.push_back(std::make_pair(i, j));
            ++i;
            ++j;
         }
         else
         {
            if (subL(i, j + 1) > subL(i + 1, j))
            {
               ++j;
            }
            else
            {
               ++i;
            }
         }
      }
      mMatches.push_back(std::make_pair(mSize1, mSize2));
   }
};

using UtSequenceDiffLines = UtSequenceDiff<std::vector<std::string>, std::equal_to<std::string>>;

namespace ut
{
//! Equivalent to the C++14 overload of std::equal
template<typename It1, typename It2>
bool equal(It1 aBegin1, It1 aEnd1, It2 aBegin2, It2 aEnd2) noexcept
{
   if (std::distance(aBegin1, aEnd1) == std::distance(aBegin2, aEnd2))
   {
      return std::equal(aBegin1, aEnd1, aBegin2);
   }
   return false;
}

//! Returns true if the first range starts with the second range.
//! Requires both iterator types to be forward-iterable. (++it)
template<typename It1, typename It2>
bool starts_with(It1 aBegin1, const It1 aEnd1, It2 aBegin2, const It2 aEnd2) noexcept
{
   while (true)
   {
      if (aBegin2 == aEnd2)
      {
         return true;
      }
      else if (aBegin1 == aEnd1)
      {
         return false;
      }
      else if (*aBegin1 != *aBegin2)
      {
         return false;
      }

      ++aBegin1;
      ++aBegin2;
   }
}

//! Returns true if the first range ends with the second range.
//! Requires both iterator types to be reverse-iterable. (--it)
template<typename It1, typename It2>
bool ends_with(const It1 aBegin1, It1 aEnd1, const It2 aBegin2, It2 aEnd2) noexcept
{
   while (true)
   {
      if (aEnd2 == aBegin2)
      {
         return true;
      }
      else if (aEnd1 == aBegin1)
      {
         return false;
      }

      --aEnd1;
      --aEnd2;

      if (*aEnd1 != *aEnd2)
      {
         return false;
      }
   }
}

//! std::end("c_style_string") will return a pointer 1 past the null terminator.
//! This overload set returns std::end(...) for all types except for array literals.
//! For array literals, returns pointer to the last element.
//! This allows c-string and string classes to be used together in string_starts_with and string_ends_with.
//! @{
template<typename T>
constexpr auto string_end(T& aString) noexcept -> decltype(std::end(aString))
{
   return std::end(aString);
}

template<typename T, int N>
constexpr T* string_end(T (&aString)[N]) noexcept
{
   return aString + N - 1;
}
//! @}

//! Returns true if aStr1 begins with aStr2.
//! This is separate from ut::ranges::starts_with because of issues relating to string literals.
template<typename Str1, typename Str2>
bool string_starts_with(Str1&& aStr1, Str2&& aStr2) noexcept
{
   return ::ut::starts_with(std::begin(aStr1), ut::string_end(aStr1), std::begin(aStr2), ut::string_end(aStr2));
}

//! Returns true if aStr1 ends with aStr2.
//! This is separate from ut::ranges::ends_with because of issues relating to string literals.
template<typename Str1, typename Str2>
bool string_ends_with(Str1&& aStr1, Str2&& aStr2) noexcept
{
   return ::ut::ends_with(std::begin(aStr1), string_end(aStr1), std::begin(aStr2), string_end(aStr2));
}

namespace ranges
{
//! Returns true if the first range is equal to the second range.
template<typename Range1, typename Range2>
bool equal(Range1&& aRange1, Range2&& aRange2) noexcept
{
   return ::ut::equal(std::begin(aRange1), std::end(aRange1), std::begin(aRange2), std::end(aRange2));
}

//! Returns true if the first range starts with the second range.
//! For string types, prefer ut::string_starts_with.
template<typename Range1, typename Range2>
bool starts_with(Range1&& aRange1, Range2&& aRange2) noexcept
{
   return ::ut::starts_with(std::begin(aRange1), std::end(aRange1), std::begin(aRange2), std::end(aRange2));
}

//! Returns true if the first range ends with the second range.
//! For string types, prefet ut::string_ends_with.
template<typename Range1, typename Range2>
bool ends_with(Range1&& aRange1, Range2&& aRange2) noexcept
{
   return ::ut::ends_with(std::begin(aRange1), std::end(aRange1), std::begin(aRange2), std::end(aRange2));
}
} // namespace ranges
} // namespace ut

#endif
