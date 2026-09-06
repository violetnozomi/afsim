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
#ifndef UTOSHADERS_HPP
#define UTOSHADERS_HPP
#include "utilosg_export.h"

#include <map>
#include <set>
#include <string>
#include <vector>

#include <osg/ref_ptr>

namespace osg
{
class Shader;
class Program;
} // namespace osg

class UTILOSG_EXPORT UtoShaders
{
public:
   static void         AddShaderDirectory(const std::string& aDirectoryPath);
   static osg::Shader* LoadFromFile(int                          aShaderType,
                                    const std::string&           aFileName,
                                    const std::set<std::string>& aDefs,
                                    const std::string&           aShaderName = "");

   static std::string LoadSource(const std::string& aFileName, std::set<std::string> aDefs = std::set<std::string>());
   static void        ReloadShaders();

   //       static void             RegisterProgram(const std::string& aName,
   //                                               osg::Program*      aProgramPtr);

   static osg::Program* LoadProgram(osg::Shader* aVertexShader,
                                    osg::Shader* aFragmentShader,
                                    osg::Shader* aGeometryShader = nullptr);

   //! Create or use a program to be shared by multiple objects
   static osg::Program* UseProgram(const std::string&    aVertexShaderFile,
                                   const std::string&    aFragmentShaderFile,
                                   std::set<std::string> aDefs = std::set<std::string>());

   static osg::Program* UseProgramWithGeometry(const std::string&           aGeometryShader,
                                               const std::string&           aVertexShaderFile,
                                               const std::string&           aFragmentShaderFile,
                                               const std::set<std::string>& aDefs = std::set<std::string>());

   static void InjectCode(const char* aInjectionCode);

   static void Shutdown();

private:
   static std::string SearchPath(const std::string& aFileName);

   struct ShaderDetails
   {
      std::string               mFile;
      osg::ref_ptr<osg::Shader> mShader;
      std::set<std::string>     mDefines;
   };

   struct ProgramKey
   {
      ProgramKey()
         : mVertexShaderPtr(nullptr)
         , mFragmentShaderPtr(nullptr)
         , mGeometryShaderPtr(nullptr)
      {
      }
      bool         operator<(const ProgramKey& aRhs) const;
      osg::Shader* mVertexShaderPtr;
      osg::Shader* mFragmentShaderPtr;
      osg::Shader* mGeometryShaderPtr;
   };

   static std::map<std::string, ShaderDetails>             mShaderFiles;
   static std::map<ProgramKey, osg::ref_ptr<osg::Program>> mPrograms;
   static std::vector<std::string>                         mShaderDirectories;

   static std::string mInjectionCode;
};
#endif
