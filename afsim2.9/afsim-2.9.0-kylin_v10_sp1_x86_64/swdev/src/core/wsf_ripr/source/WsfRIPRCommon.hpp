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

#ifndef WSF_RIPR_COMMON_HPP
#define WSF_RIPR_COMMON_HPP

#include <list>
#include <map>
#include <vector>

#include "UtScriptData.hpp"
#include "WsfStringId.hpp"

//! WsfRIPRCommon.hpp defines RIPR typedefs that clients can include
//! without needing to include other RIPR headers.
namespace ripr
{
typedef std::map<UtScriptData, UtScriptData>     RIPRJobDataMap;
typedef std::vector<int>                         RIPRWinnerIdList;
typedef std::map<unsigned int, int>              RIPRWinnerMap;
typedef std::map<unsigned int, double>           RIPRJobProgressMap;
typedef std::pair<unsigned int, double>          RIPRBidderPair;
typedef std::list<RIPRBidderPair>                RIPRBidderList;
typedef std::vector<unsigned int>                JobChannelIdList;
typedef std::map<int, int>                       RIPRChannelJobMap;
typedef std::multimap<std::string, unsigned int> JobTypeChannelsMap;

struct SortBidPairStruct
{
   bool operator()(const RIPRBidderPair& aLhs, const RIPRBidderPair& aRhs) const { return aLhs.first < aRhs.first; }
};

//! The RIPRCommanderInfo struct contains the basic information
//! needed to reconstruct the RIPR Processor's command chain
//! and commander RIPR processor.
struct RIPRCommanderInfo
{
   WsfStringId mCommandChainName;
   size_t      mPlatformIndex;
   WsfStringId mPlatformName;
   WsfStringId mRIPRProcessorName;

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mCommandChainName& mPlatformIndex& mPlatformName& mRIPRProcessorName;
   }
   bool operator<(const RIPRCommanderInfo& aRhs) const
   {
      return mCommandChainName < aRhs.mCommandChainName ||
             (mCommandChainName == aRhs.mCommandChainName && mPlatformName < aRhs.mPlatformName) ||
             (mCommandChainName == aRhs.mCommandChainName && mPlatformName == aRhs.mPlatformName &&
              mRIPRProcessorName < aRhs.mRIPRProcessorName);
   }
};
typedef std::list<RIPRCommanderInfo> RIPRCommanderList;

} // namespace ripr

#endif
