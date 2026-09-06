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

#ifndef PROFILINGREGION_HPP
#define PROFILINGREGION_HPP

/// Declaration of a "scope guard" to mark regions of code for profiling.

#include "profiling_export.h"

#include <cstdint>

namespace profiling
{

/// Scope guard that marks a region of code for profiling.
///
/// This class' constructor begins the profiling region.
/// Its destructor ends the profiling region.
///
/// Profiling regions must be nested.  This implies that
/// @a ProfilingRegion instances should never be handled by pointer
/// or passed as arguments to functions.
///
/// Always name the variable representing the @a ProfilingRegion instance.
/// Don't just invoke the constructor without giving the variable a name.
/// If you forget to name the variable, then the region will close right away,
/// since the unnamed temporary object's destructor
/// will be invoked at the end of the statement.
///
/// Exactly one @a ProfilingSystem instance must exist
/// before constructing or destroying a @a ProfilingRegion instance.
/// That is, the lifetime of any @a ProfilingRegion
/// must fit within the lifetime of exactly one @a ProfilingSystem instance.
///
/// @warning Do NOT use this at @a main scope!
///   Always make sure the region is closed before @a main returns.
///
/// @warning @a ProfilingRegion should only be used by one thread at a time.
///    It's safe to have thread parallelism happening inside of a @a ProfilingRegion,
///    but it's not safe to have multiple threads creating @a ProfilingRegion
///    instances concurrently.
///
/// Here is an example of how to use this class in a function,
/// assuming that the function is only ever called within the lifetime
/// of a @a ProfilingSystem object.
///
/// @code
/// void InnerFunction() {
///    ProfilingRegion r("name of my choice");
///    ComputeStuff();
/// }
/// void MiddleFunction() {
///    ProfilingRegion r2("another name of my choice");
///    ComputeOtherStuff();
///    for(int i = 0; i < 3; ++i) {
///       InnerFunction();
///    }
///    ComputeEvenMoreStuff();
/// }
/// void OuterFunction() {
///    ComputeSomething();
///    MiddleFunction();
///    MiddleFunction();
///    ComputeSomeMoreStuff();
/// }
/// @endcode
///
/// This results in the following nested regions:
///
/// <ol>
/// <li> First instance of "another name of my choice" region, containing:
///    <ol>
///    <li> First instance of "name of my choice" region </li>
///    <li> Second instance of "name of my choice" region </li>
///    <li> Third instance of "name of my choice" region </li>
///    </ol>
/// </li>
/// <li> Second instance of "another name of my choice" region, containing:
///    <ol>
///    <li> Fourth instance of "name of my choice" region </li>
///    <li> Fifth instance of "name of my choice" region </li>
///    <li> Sixth instance of "name of my choice" region </li>
///    </ol>
/// </li>
/// </ol>
///
class PROFILING_EXPORT ProfilingRegion
{
public:
   /// @brief Begin a profiling region.
   ///
   /// @param aRegionLabel String label for the region
   ProfilingRegion(const char aRegionLabel[]);
   //! End the current profiling region.
   ~ProfilingRegion();

   ProfilingRegion()                       = delete;
   ProfilingRegion(const ProfilingRegion&) = delete;
   ProfilingRegion& operator=(const ProfilingRegion&) = delete;
   ProfilingRegion(ProfilingRegion&&)                 = delete;
   ProfilingRegion& operator=(ProfilingRegion&&) = delete;
};

/// Create a profiling section.
///
/// @param label The section's label.
///
/// @return The section's key.
///
/// Section labels need not be unique.  Calling this function twice
/// with the same label will result in two distinct sections, with different keys.
/// This is useful for timing things that happen inside thread-parallel code.
///
/// Sections differ from regions (as created by @a ProfilingRegion)
/// in the following ways.
///
/// 1. Sections are allowed to overlap partially.
///    (This is not true for regions.  Given any two regions,
///    they are either disjoint, or one contains the other entirely.
///    This means that one can always map regions onto a single timeline.)
/// 2. Section creation happens separately from beginning a section.
/// 3. Section destruction happens separately from ending a section.
///
/// All profiling libraries shall let separate threads begin or end
/// (but not necessarily create or destroy) separate sections concurrently.
/// Profiling libraries shall make this thread safe at least.
/// As a minimum quality-of-implementation requirement,
/// they should not synchronize (e.g., via mutexes or atomic operations)
/// when beginning or ending a section.
/// This, plus (1) above, make sections the right idiom for timing events
/// happening concurrently on separate threads.  (We do <i>not</i> permit
/// multiple threads to begin or end the <i>same</i> section concurrently.)
PROFILING_EXPORT uint32_t CreateProfilingSection(const char label[]);

/// Destroy a profiling section.
///
/// @param sectionId A section key returned by @a CreateProfilingSection.
///
/// You must call @a DestroyProfilingSection exactly once
/// for every unique section key returned by @a CreateProfilingSection.
PROFILING_EXPORT void DestroyProfilingSection(uint32_t sectionId);

/// Begin a previously created profiling section.
///
/// @param sectionId A section key returned by @a CreateProfilingSection.
///    The key must not refer to a section that has been destroyed.
///
/// Every section that has been begun must be ended.
/// After being ended, a section may be begun again.
///
/// Different threads may <i>not</i> begin or end the same section concurrently.
/// However, different threads <i>are</i> permitted to begin or end
/// <i>different</i> sections concurrently.
PROFILING_EXPORT void BeginProfilingSection(uint32_t sectionId);

/// End a previously created and begun profiling section.
///
/// @param sectionId A section key returned by @a CreateProfilingSection.
///    The key must not refer to a section that has been destroyed.
///    The section must have been begun (via @a BeginProfilingSection).
///
/// The key must refer to a section that has been begun.
/// Every section that has been begun must be ended.
/// After being ended, a section may be begun again.
PROFILING_EXPORT void EndProfilingSection(uint32_t sectionId);

} // namespace profiling

#endif // PROFILINGREGION_HPP
