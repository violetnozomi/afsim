// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "ProfilingRegion.hpp"

#include "ProfilingHooks.hpp"

namespace profiling
{

namespace detail
{

extern const ProfilingHooks& GetGlobalProfilingHooks();

} // namespace detail

ProfilingRegion::ProfilingRegion(const char aRegionLabel[])
{
   const ProfilingHooks& hooks = detail::GetGlobalProfilingHooks();
   if (hooks.mBeginRegion)
   {
      hooks.mBeginRegion(aRegionLabel);
   }
}

ProfilingRegion::~ProfilingRegion()
{
   const ProfilingHooks& hooks = detail::GetGlobalProfilingHooks();
   if (hooks.mEndRegion)
   {
      hooks.mEndRegion();
   }
}

/// Create a profiling section.
uint32_t CreateProfilingSection(const char label[])
{
   const ProfilingHooks& hooks = detail::GetGlobalProfilingHooks();
   if (hooks.mCreateSection)
   {
      return hooks.mCreateSection(label);
   }
   else
   {
      return 0;
   }
}

/// Destroy a profiling section that was created by CreateProfilingSection.
void DestroyProfilingSection(uint32_t sectionId)
{
   const ProfilingHooks& hooks = detail::GetGlobalProfilingHooks();
   if (hooks.mDestroySection)
   {
      hooks.mDestroySection(sectionId);
   }
}

void BeginProfilingSection(uint32_t sectionId)
{
   const ProfilingHooks& hooks = detail::GetGlobalProfilingHooks();
   if (hooks.mBeginSection)
   {
      hooks.mBeginSection(sectionId);
   }
}

void EndProfilingSection(uint32_t sectionId)
{
   const ProfilingHooks& hooks = detail::GetGlobalProfilingHooks();
   if (hooks.mEndSection)
   {
      hooks.mEndSection(sectionId);
   }
}

} // namespace profiling
