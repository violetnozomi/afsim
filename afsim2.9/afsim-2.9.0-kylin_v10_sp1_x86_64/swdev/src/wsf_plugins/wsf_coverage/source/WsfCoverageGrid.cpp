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

#include "WsfCoverageGrid.hpp"

#include <string>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "WsfCoverageTextOutputWriter.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"

namespace wsf
{
namespace coverage
{

Grid::Grid(const WsfScenario& aScenario)
   : mScenario(aScenario)
   , mCentralBodyPtr{ut::make_unique<ut::EarthWGS84>()}
{
}

bool Grid::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command = aInput.GetCommand();

   if (command == "central_body")
   {
      UtInputBlock block{aInput};
      std::string  body;
      block.ReadCommand(body);
      mCentralBodyPtr = ut::CentralBody::Create(body);
      if (mCentralBodyPtr == nullptr)
      {
         throw UtInput::BadValue{aInput, "Unrecognized central body type"};
      }
      if (!mCentralBodyPtr->IsEarth())
      {
         throw UtInput::BadValue{aInput, "Only Earth based central bodies are currently supported"};
      }
      block.ProcessInput(mCentralBodyPtr.get());
      myCommand = true;
   }
   else if (command == "grid_data_file")
   {
      aInput.ReadValueQuoted(mGridDataFileName);
   }
   else if (command == "suppress_grid_platforms")
   {
      mSuppressGridPlatforms = aInput.ReadBool();
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

std::pair<std::size_t, std::size_t> Grid::GetComponentSizes(std::size_t aComponentIndex) const
{
   // The default implementation indicates that it is not structured by returning the size in the
   // first part of this pair.
   return std::make_pair(GetSize(), 1);
}

std::pair<std::size_t, std::size_t> Grid::GetPointIndices(PointId aPointId) const
{
   return std::make_pair(0, 0);
}

//! Initialize this Grid.
//!
//! Initialize this grid. This is called during simulation initialization. This is a
//! template method in which all common initialization tasks can be performed. Specific
//! details for subclasses are handled in InitializeP.
//!
//! \returns         - true if this Grid was successfully initialized; false otherwise.
bool Grid::Initialize()
{
   if (!IsUsed())
   {
      // warn and stop processing
      auto msg = ut::log::warning() << "Coverage grid is not used. Initialization of coverage grid will not continue.";
      msg.AddNote() << "Grid: " << GetName();
      return true;
   }
   mInitialized = InitializeP();

   if (mInitialized && !GetSize())
   {
      auto msg = ut::log::error() << "Initialized grid has no grid points.";
      msg.AddNote() << "Grid: " << GetName();
      mInitialized = false;
   }

   return mInitialized;
}

//! Set the provided category on this object's grid assets.
//!
//! This method will set two categories for each platform that makes up this grid. The first
//! is the universal coverage grid category of which all grid asset platforms are members.
//! The second is a specific category for a given coverage computation.
//!
//! \param aSimulation - The simulation in which this grid's platforms exist.
//! \param aCategory   - The coverage-specific category to assign to this grid's platforms.
void Grid::CategorizeAssets(const WsfSimulation& aSimulation, const std::string& aCategory) const
{
   std::string universal{cUNIVERSAL_GRID_ASSET_CATEGORY};
   auto        actionPtr = CreateGridAction(
      [&aSimulation, &aCategory, &universal](const Grid& aGrid, const Point& aPoint)
      {
         auto         ga      = aGrid.GetAsset(aPoint.GetID());
         WsfPlatform* platPtr = aSimulation.GetPlatformByName(ga.GetPlatformName());
         if (platPtr)
         {
            platPtr->AddCategory(aCategory);
            platPtr->AddCategory(universal);
         }
         else
         {
            throw std::runtime_error{"Grid asset platform does not exist when it should."};
         }
      });
   Invoke(*actionPtr);
}

//! Set the central body of this grid.
void Grid::SetCentralBody(const ut::CentralBody& aCentralBody)
{
   mCentralBodyPtr = aCentralBody.Clone();
}

//! Write the details of the grid points to file.
//!
//! Grids optionally write details of their points to file. This operation
//! occurs when the simulation is ending. This data is generated only if the
//! user has provided a filename for this file during this object's configuration.
//! This form of this method is intended to be used during unit testing.
//!
//! \param aWriter - The output writer into which the data will be generated.
//! \returns       - false if there was some problem opening and writing to the file; true otherwise.
bool Grid::WriteGridDataFile(TextOutputWriter& aWriter) const
{
   bool retval{true};

   // No file name set means no file to write, which is a success.
   if (mGridDataFileName.empty())
   {
      return retval;
   }

   // Make sure the path to the file exists, returning if unable to create it.
   UtPath path{mGridDataFileName};
   path.Up();
   path.MakeFullPath();
   if (!aWriter.CreateContainingFolder(path.GetSystemPath()))
   {
      return false;
   }

   if (aWriter.OpenStream(mGridDataFileName))
   {
      // Set stream options
      aWriter.SetWidth(16);
      aWriter.SetPrecision(12);
      aWriter.SetStreamFormat();
      aWriter.SetStreamJustify();

      // Write intro and header info
      aWriter.WriteField("Point ID");
      WriteHeaderExtension(aWriter);
      aWriter.WriteField("Latitude [deg]");
      aWriter.WriteField("Longitude [deg]");
      aWriter.WriteField("Alt. (MSL) [m]");
      aWriter.WriteField("Alt. (AGL) [m]", true);

      // Write the data lines
      wsf::Terrain terrain{wsf::TerrainInterface::Get(mScenario)};
      auto         writeLines = CreateGridAction(
         [this, &aWriter, &terrain](const Grid& aGrid, const Point& aPoint)
         {
            aWriter.WriteField(aPoint.GetID());
            WriteLineExtension(aWriter, aPoint.GetID());
            aWriter.WriteField(aPoint.GetLatitude());
            aWriter.WriteField(aPoint.GetLongitude());
            aWriter.WriteField(aPoint.GetAltitude());
            float height;
            terrain.GetElevInterp(aPoint.GetLatitude(), aPoint.GetLongitude(), height);
            aWriter.WriteField(aPoint.GetAltitude() - height, true);
         });
      Invoke(*writeLines);
      aWriter.CloseStream();
   }
   else
   {
      auto msg = ut::log::error() << "Unable to create file.";
      msg.AddNote() << "File: " << mGridDataFileName;
      retval = false;
   }

   return retval;
}

//! Write the details of the grid points to file.
//!
//! Grids optionally write details of their points to file. This operation
//! occurs when the simulation is ending. This data is generated only if the
//! user has provided a filename for this file during this object's configuration.
//!
//! \returns - false if there was some problem opening and writing to the file; true otherwise.
bool Grid::WriteGridDataFile() const
{
   TextOutputWriter writer{};
   return WriteGridDataFile(writer);
}

PointId Grid::GetNextAvailableId()
{
   // NOTE: Pre-increment because we initialize to the invalid value (0).
   return ++mNextPointId;
}

} // namespace coverage
} // namespace wsf
