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
#include "UtoShaders.hpp"

#include <algorithm>
#include <iostream>

#include <osg/Program>
#include <osg/Shader>

#include "UtLog.hpp"
#include "UtPath.hpp"
#include "UtoShaderUtilities.hpp"

std::map<std::string, UtoShaders::ShaderDetails>             UtoShaders::mShaderFiles;
std::map<UtoShaders::ProgramKey, osg::ref_ptr<osg::Program>> UtoShaders::mPrograms;
std::vector<std::string>                                     UtoShaders::mShaderDirectories;
std::string                                                  UtoShaders::mInjectionCode;

osg::Shader* UtoShaders::LoadFromFile(int                          aShaderType,
                                      const std::string&           aFileName,
                                      const std::set<std::string>& aDefs,
                                      const std::string&           aShaderName)
{
   auto name = aShaderName;
   if (name.empty())
      name = aFileName;

   auto src = LoadSource(aFileName, aDefs);

   ShaderDetails& details = mShaderFiles[name];
   details.mFile          = aFileName;
   details.mDefines       = aDefs;
   if (details.mShader.valid())
   {
      details.mShader->setShaderSource(src);
      details.mShader->setName(aFileName);
   }
   else
   {
      details.mShader = new osg::Shader((osg::Shader::Type)aShaderType, src);
      details.mShader->setName(aFileName);
   }
   return details.mShader.get();
}

std::string UtoShaders::LoadSource(const std::string& aFileName, std::set<std::string> aDefs)
{
#undef SearchPath // Avoid conflict with Windows macro
   auto path = SearchPath(aFileName);
   if (path.empty())
      return "";

   auto src = mInjectionCode;
   if (!mInjectionCode.empty())
   {
      // reset error/warning line numbers for the actual file to be right
      src += "#line 0";
   }
   src += UtoShaderUtilities::PreprocessFile(path, aDefs, mShaderDirectories);

   return src;
}

std::string UtoShaders::SearchPath(const std::string& aFileName)
{
   if (UtPath(aFileName).Stat() == UtPath::cFILE)
      return aFileName;
   for (size_t i = 0; i < mShaderDirectories.size(); ++i)
   {
      UtPath path = UtPath(mShaderDirectories[i]) + aFileName;
      if (path.Stat() == UtPath::cFILE)
         return path.GetSystemPath();
   }
   static std::set<std::string> filesNotFound;
   if (filesNotFound.find(aFileName) == filesNotFound.end())
   {
      auto out = ut::log::warning() << "Shader not found.";
      out.AddNote() << "File: " << aFileName;
      filesNotFound.insert(aFileName);
   }
   return ""; // not found
}

void UtoShaders::ReloadShaders()
{
   for (std::map<std::string, ShaderDetails>::iterator i = mShaderFiles.begin(); i != mShaderFiles.end(); ++i)
   {
      // src/osg/Program.cpp outputs the info log for a failed link at the
      // OSG_WARN level, but a successful link at OSG_INFO, so you normally
      // don't see warnings if it linked.  If there are previous log messages
      // display them now, reload twice for current messages.
      std::string log;
      LoadFromFile(i->second.mShader->getType(), i->second.mFile, i->second.mDefines, i->first);
   }
}

osg::Program* UtoShaders::UseProgram(const std::string&    aVertexShaderFile,
                                     const std::string&    aFragmentShaderFile,
                                     std::set<std::string> aDefs /*= std::set<std::string>()*/)
{
   return UseProgramWithGeometry("", aVertexShaderFile, aFragmentShaderFile, aDefs);
}

osg::Program* UtoShaders::UseProgramWithGeometry(const std::string&           aGeometryShader,
                                                 const std::string&           aVertexShaderFile,
                                                 const std::string&           aFragmentShaderFile,
                                                 const std::set<std::string>& aDefs)
{
   ProgramKey k;
   bool       ok = true;
   if (!aGeometryShader.empty())
   {
      k.mGeometryShaderPtr = LoadFromFile(osg::Shader::GEOMETRY, aGeometryShader, aDefs);

      if (!k.mGeometryShaderPtr)
      {
         ok = false;
      }
   }
   if (ok && !aVertexShaderFile.empty())
   {
      k.mVertexShaderPtr = LoadFromFile(osg::Shader::VERTEX, aVertexShaderFile, aDefs);
      if (!k.mVertexShaderPtr)
      {
         ok = false;
      }
   }

   if (ok && !aFragmentShaderFile.empty())
   {
      k.mFragmentShaderPtr = LoadFromFile(osg::Shader::FRAGMENT, aFragmentShaderFile, aDefs);
      if (!k.mFragmentShaderPtr)
      {
         ok = false;
      }
   }

   ok = ok && (k.mGeometryShaderPtr || (k.mVertexShaderPtr && k.mFragmentShaderPtr));
   if (ok)
   {
      osg::ref_ptr<osg::Program>& prog = mPrograms[k];
      if (!prog.valid())
      {
         prog                = LoadProgram(k.mVertexShaderPtr, k.mFragmentShaderPtr, k.mGeometryShaderPtr);
         std::string namestr = aVertexShaderFile + "|" + aFragmentShaderFile;
         prog->setName(namestr);
      }
      return prog;
   }
   return nullptr;
}

osg::Program* UtoShaders::LoadProgram(osg::Shader* aVertexShader, osg::Shader* aFragmentShader, osg::Shader* aGeometryShaderPtr)
{
   osg::Program* prog = new osg::Program();
   if (aVertexShader)
      prog->addShader(aVertexShader);
   if (aFragmentShader)
      prog->addShader(aFragmentShader);
   if (aGeometryShaderPtr)
   {
      prog->addShader(aGeometryShaderPtr);

      // prog->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 32);
      // prog->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_LINES);
      // prog->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_LINES);
   }
   return prog;
}

void UtoShaders::AddShaderDirectory(const std::string& aDirectoryPath)
{
   if (std::find(mShaderDirectories.begin(), mShaderDirectories.end(), aDirectoryPath) == mShaderDirectories.end())
   {
      mShaderDirectories.push_back(aDirectoryPath);
   }
}

bool UtoShaders::ProgramKey::operator<(const ProgramKey& aRhs) const
{
   if (mVertexShaderPtr < aRhs.mVertexShaderPtr)
      return true;
   if (mVertexShaderPtr > aRhs.mVertexShaderPtr)
      return false;
   if (mFragmentShaderPtr < aRhs.mFragmentShaderPtr)
      return true;
   if (mFragmentShaderPtr > aRhs.mFragmentShaderPtr)
      return false;
   return mGeometryShaderPtr < aRhs.mGeometryShaderPtr;
}

void UtoShaders::InjectCode(const char* aInjectionCode)
{
   mInjectionCode = aInjectionCode;
   ReloadShaders();
}

void UtoShaders::Shutdown()
{
   for (auto&& it : mPrograms)
   {
      it.second = nullptr;
   }
}
