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

#ifndef MISSIONVERIFICATIONRESULTS_HPP
#define MISSIONVERIFICATIONRESULTS_HPP

#include "wsf_spaceg_export.h"

#include <map>
#include <utility>

#include <QModelIndex>

#include "VerificationResults.hpp"

namespace wsfg
{

namespace spaceg
{

class WSF_SPACEG_EXPORT MissionVerificationResults
{
public:
   MissionVerificationResults()
      : mResults{}
   {
   }

   size_t GetSize() const { return mResults.size(); }

   //! Apply an operation for each collected result.
   //!
   //! The provided @p aOperation is any callable construct that
   //! has a signature compatible with void(QModelIndex&, VerificationResults).
   template<typename Callable>
   void Apply(Callable aOperation) const
   {
      for (auto& i : mResults)
      {
         aOperation(i.first, i.second);
      }
   }

   VerificationResults GetResults(int aOrderedIndex) const;

   void SetResults(int aOrderedIndex, const VerificationResults& aResults);

private:
   std::map<int, VerificationResults> mResults;
};

} // namespace spaceg

} // namespace wsfg

#endif // MISSIONVERIFICATIONRESULTS_HPP
