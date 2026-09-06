// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTOSHADERUTILITIES_HPP
#define UTOSHADERUTILITIES_HPP

#include "utilosg_export.h"

#include <set>
#include <string>
#include <vector>

// A collection of static shader utilities.
// UTOSG classes may expect your application to have called EnableShaders properly, if they use ShadersEnabled.
// PreprocessFile will #define each aDefs and expand #include "file"

class UTILOSG_EXPORT UtoShaderUtilities
{
public:
   static std::string PreprocessFile(const std::string& aFilename);

   static std::string PreprocessFile(const std::string& aFilename, const std::set<std::string>& aDefs);
   static std::string PreprocessFile(const std::string&           aFilename,
                                     const std::set<std::string>& aDefs,
                                     std::vector<std::string>&    aSearchPath);

   static void EnableShaders(bool aState) { mShadersEnabled = aState; }
   static bool ShadersEnabled() { return mShadersEnabled; }

   static void EnableGeometryShaders(bool aState) { mGeometryShadersEnabled = aState; }
   static bool GeometryShadersEnabled() { return mGeometryShadersEnabled; }

   static double RGBA_ToDouble(unsigned char aR, unsigned char aG, unsigned char aB, unsigned char aA);
   static void DoubleToRGBA(double aValue, unsigned char& aR, unsigned char& aG, unsigned char& aB, unsigned char& aA);

private:
   static std::string SearchPath(const std::string& aFilename, std::vector<std::string>& aIncludeSearchPath);

   static void PreprocessShaderFile(const std::string&           aFilename,
                                    std::string&                 aOutput,
                                    const std::set<std::string>& aDefs,
                                    std::vector<std::string>&    aIncludeSearchPath);

   static char FirstCharacterOfSignificance(const std::string& aString, unsigned int& aStart);

   static bool HandleDirective(const std::string&        aFilename,
                               const std::string&        aStrPtr,
                               const std::string&        aSourceDirectoryString,
                               std::string&              aDefines,
                               std::string&              aOutput,
                               std::vector<std::string>& aIncludeSearchPath);
   // expand to the preprocessor line reset string
   static std::string LineCount(int aLineNum);

   static bool mShadersEnabled;
   static bool mGeometryShadersEnabled;
};

#endif
