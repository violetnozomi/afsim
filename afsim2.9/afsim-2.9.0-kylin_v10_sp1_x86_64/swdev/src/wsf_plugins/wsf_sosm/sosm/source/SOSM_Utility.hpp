// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef SOSM_UTILITY_HPP
#define SOSM_UTILITY_HPP

#include <iosfwd>
#include <string>
#include <vector>

#include "UtLog.hpp"

class SOSM_Selector;

//! A collection of utility functions
namespace SOSM_Utility
{
enum SpectralPrintFormat
{
   cSPF_WAVENUMBER,
   cSPF_WAVELENGTH
};

void SetSpectralPrintFormat(SpectralPrintFormat aSpectralPrintFormat);

void PrintSpectralVector(ut::log::MessageStream&   aStream,
                         const SOSM_Selector&      aSelector,
                         const std::vector<float>& aVector,
                         const std::string&        aHeader);

std::string BaseName(const std::string& aPath);

std::string DirName(const std::string& aPath);

void StripPathPrefix(const std::string& aPrefixedPath, std::string& aPrefix, std::string& aPath);

bool MakePath(const std::string& aPath);

bool CreateDirectory(const std::string& aDirName);

bool DirectoryExists(const std::string& aDirName);

bool FileExists(const std::string& aFileName);

bool FileIsCurrent(const std::string& aTargetFile, const std::string& aSourceFile);

double FileModificationTime(const std::string& aFile);
} // namespace SOSM_Utility

#endif
