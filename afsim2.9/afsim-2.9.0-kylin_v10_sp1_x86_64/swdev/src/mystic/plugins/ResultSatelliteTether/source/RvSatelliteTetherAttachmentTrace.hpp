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

#ifndef RVSATELLITETETHERATTACHMENTTRACE_HPP
#define RVSATELLITETETHERATTACHMENTTRACE_HPP

#include "satellite_tether/WkfAttachmentSatelliteTetherTrace.hpp"

namespace rv
{
class ResultData;
}

namespace RvSatelliteTether
{
class AttachmentTrace : public wkf::AttachmentSatelliteTetherTrace
{
public:
   AttachmentTrace(vespa::VaEntity& aParentEntity,
                   vespa::VaViewer& aViewer,
                   vespa::VaEntity& aRelativeToEntity,
                   const QColor&    aColor);

   ~AttachmentTrace() override = default;

   void UpdateData() override;
   void Update(double aTime) override;

private:
   using TimeRange = std::pair<float, float>;

   const TimeRange& GetTimeRange() const { return mTimeRange; }
   void             SetTimeRange(const TimeRange& aRange) { mTimeRange = aRange; }
   void UpdateDataRange(float aCullBefore, float aCullAfter, const TimeRange& aAddBefore, const TimeRange& aAddAfter);


   TimeRange                mTimeRange;
   std::map<double, size_t> mTimeToIndex;
};
} // namespace RvSatelliteTether

#endif
