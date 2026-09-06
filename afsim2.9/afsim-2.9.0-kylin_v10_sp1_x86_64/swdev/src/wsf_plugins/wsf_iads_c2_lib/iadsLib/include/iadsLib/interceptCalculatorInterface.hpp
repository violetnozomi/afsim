// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*********************************
*
*  Classification: UNCLASSIFIED
*
**********************************/

#ifndef INTERCEPT_CALCULATOR_HPP
#define INTERCEPT_CALCULATOR_HPP

#include <iadsLib/trackRecord.hpp>
#include <iadsLib/weaponRecordInterface.hpp>

namespace il
{
   class InterceptCalculatorIface
   {
      public:
         virtual ~InterceptCalculatorIface() = default;
         virtual bool CanIntercept (const trackRecord& track,
                                    const weaponRecord* weapon,
                                    double& interceptTime_s,
                                    double pred_pt_ecef_m[3],
                                    double& est_pk,
                                    double& weap_max_range,
                                    std::weak_ptr<logger::GlobalLogger> aGlobalLogger) = 0;
   };
}

#endif
