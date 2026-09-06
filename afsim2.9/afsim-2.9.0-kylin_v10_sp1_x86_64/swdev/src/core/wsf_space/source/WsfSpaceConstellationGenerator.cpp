// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSpaceConstellationGenerator.hpp"

#include "WsfConstellationOptions.hpp"

namespace // {anonymous}
{

//! Generate the file header.
//!
//! This is used to create the header for constellation input files.
//!
//! \param aOptions - the constellation options.
//! \param aStream  - the stream into which the header is written.
void GenerateHeader(const WsfConstellationOptions& aOptions, std::ostream& aStream)
{
   aOptions.OutputOptions(aStream);
   aStream << "\n";
}

//! Generate the orbit size specification into the given stream.
//!
//! \param aOptions - the constellation options.
//! \param aStream  - the stream into which the constellation input will be generated.
void GenerateOrbitSize(const WsfConstellationOptions& aOptions, std::ostream& aStream)
{
   if (aOptions.SpecifiedAsRevolutionsPerDay())
   {
      aStream << "      revs_per_day " << aOptions.GetRevolutionsPerDay() << "\n";
   }
   else
   {
      aStream << "      semi_major_axis " << aOptions.GetSemiMajorAxis() << "\n";
   }
}

//! Generate the output for a single satellite.
//!
//! \param aOptions - the constellation options.
//! \param aPlane   - the plane of the satellite to generate.
//! \param aSat     - the number in the plane of the satellite to generate.
//! \param aStream  - the stream into which the constellation input will be generated.
void GenerateOne(const WsfConstellationOptions& aOptions, unsigned int aPlane, unsigned int aSat, std::ostream& aStream)
{
   aStream << "platform " << aOptions.GetConstellationName() << "_" << aPlane << "_" << aSat << " "
           << aOptions.GetPlatformType() << "\n";
   aStream << "   mover\n";
   aStream << "      inclination " << aOptions.GetInclination() << "\n";
   GenerateOrbitSize(aOptions, aStream);
   aStream << "      raan " << aOptions.GetMemberRAAN(aPlane) << "\n";
   aStream << "      true_anomaly " << aOptions.GetMemberAnomaly(aPlane, aSat) << "\n";
   aStream << "   end_mover\n";
   aStream << "end_platform\n\n";
}

} // namespace

namespace wsf
{
namespace space
{

//! Generate the output into the provided stream.
//!
//! \param aOptions - the constellation options.
//! \param aStream  - the stream into which the constellation input will be generated.
void ConstellationGenerator::Generate(const WsfConstellationOptions& aOptions, std::ostream& aStream)
{
   GenerateHeader(aOptions, aStream);

   for (unsigned int plane = 0; plane < aOptions.GetNumPlanes(); ++plane)
   {
      for (unsigned int sat = 0; sat < aOptions.GetSatsPerPlane(); ++sat)
      {
         GenerateOne(aOptions, plane, sat, aStream);
      }
   }
}

} // namespace space
} // namespace wsf
