// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_ASSESS_ENGAGEMENTS_HPP
#define WSF_BM_ASSESS_ENGAGEMENTS_HPP

#include <WsfBMCommon.hpp>
#include <WsfObject.hpp>
#include <UtInput.hpp>
#include <UtInputBlock.hpp>

#include "WsfBMGenericTypeWrapper.hpp"
#include <iadsLib/assessEngagementsWSPT.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfBMAssessEngagements : public WsfBMUnmanagedTypeWrapper<il::assessEngagementsWSPT>
{
   public:
      WsfBMAssessEngagements();
      WsfBMAssessEngagements(il::assessEngagementsWSPT* data_to_wrap);

      bool ProcessInput(UtInput& aInput);

   protected:
      bool ProcessReportInput(UtInput& aInput, const std::string& end_block_str);
      bool ProcessAllocationInput(UtInput& aInput, const std::string& end_block_str);
};

#endif
