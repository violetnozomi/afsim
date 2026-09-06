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

#ifndef WSFDEFILEMANAGER_HPP
#define WSFDEFILEMANAGER_HPP

#include <map>
#include <memory>

#include "WsfDE_File.hpp"
#include "WsfDE_FileHandle.hpp"
class WsfSimulation;
#include "WsfSimulationExtension.hpp"

//! This simulation extension provides a central repository for loaded DE files.
//!
//! This allows for the clients of the JPL Development Ephemeris files to store
//! only a reference to the file, and maintain separate work spaces to use
//! during interpolation.
class WsfDE_FileManager : public WsfSimulationExtension
{
public:
   WsfDE_FileManager()           = default;
   ~WsfDE_FileManager() override = default;

   static WsfDE_FileManager&    Get(const WsfSimulation* aSimulationPtr);
   constexpr static const char* cEXTENSION_NAME{"wsf_de_file_manager"};

   std::unique_ptr<WsfDE_FileHandle> GetOrLoadFile(const std::string& aFileName);

private:
   std::map<std::string, std::unique_ptr<WsfDE_File>> mFiles{};
};

#endif // WSFDEFILEMANAGER_HPP
