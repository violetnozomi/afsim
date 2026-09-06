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

#include "UtScriptPath.hpp"

#include "UtPath.hpp"
#include "UtScriptException.hpp"
#include "UtScriptMethodDefine.hpp"

namespace
{
UtScriptPath* sScriptPathClassPtr = nullptr;
}

UtScriptPath::UtScriptPath(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   UT_SCRIPT_WRAP_CLASS(UtPath);
   sScriptPathClassPtr = this;
   mConstructible      = true;
   mCloneable          = true;
   mEqualityComparable = true;

   // Static methods

   UT_SCRIPT_STATIC_VARIADIC_METHOD(std::unique_ptr<UtPath>, Construct, (), UtScriptData)
   {
      auto path = ut::make_unique<UtPath>();
      for (const auto& arg : aVarArgs)
      {
         if (arg.GetType() == ut::script::Data::Type::cPOINTER)
         {
            auto pathPtr = arg.GetPointer()->GetAppObject<UtPath>();
            if (pathPtr)
            {
               *path += *pathPtr;
            }
         }
         else if (arg.GetType() == ut::script::Data::Type::cSTRING)
         {
            *path += arg.GetString();
         }
         else
         {
            UT_SCRIPT_ABORT("Invalid type: Can only use strings or Paths to construct Path");
         }
      }
      return path;
   };

   UT_SCRIPT_STATIC_METHOD(bool, Exists, (const std::string& aFilePath)) { return UtPath(aFilePath).Exists(); };

   UT_SCRIPT_STATIC_METHOD(UtPath, GetRelativePath, (const std::string& aLocalPath, const std::string& aRefDir))
   {
      return UtPath{aLocalPath}.GetRelativePath(UtPath{aRefDir});
   };

   UT_SCRIPT_WRAP_MEMBER("GetWorkingDirectory", WorkingDirectory);

   UT_SCRIPT_STATIC_METHOD(bool, IsFile, (const std::string& aFilePath)) { return UtPath{aFilePath}.IsFile(); };

   UT_SCRIPT_STATIC_METHOD(bool, IsDirectory, (const std::string& aDirPath)) { return UtPath{aDirPath}.IsDirectory(); };

   UT_SCRIPT_STATIC_METHOD(bool, MakeDirectory, (const std::string& aFilePath)) { return UtPath(aFilePath).Mkdir(); };


   UT_SCRIPT_STATIC_METHOD(bool, MakeDirectory, (const std::string& aFilePath, bool aCreateAll))
   {
      return UtPath(aFilePath).Mkdir(aCreateAll);
   };

   // Methods
   UT_SCRIPT_METHOD(bool, Exists, ()) { return self.Exists(); };
   UT_SCRIPT_METHOD(std::unique_ptr<UtPath>, GetParentDirectory, ())
   {
      auto path = ut::make_unique<UtPath>(self);
      path->Up();
      return path;
   };

   UT_SCRIPT_WRAP_MEMBER("GetFileExtension", GetFileExtension);

   UT_SCRIPT_METHOD(std::string, GetFileName, ()) { return self.GetFileName(); };
   UT_SCRIPT_METHOD(std::string, GetFileName, (bool aWithExtension)) { return self.GetFileName(aWithExtension); };

   UT_SCRIPT_METHOD(UtPath, GetRelativePath, (const std::string& aRefDir))
   {
      return self.GetRelativePath(UtPath{aRefDir});
   };

   UT_SCRIPT_METHOD(UtPath, GetRelativePath, (const UtPath& aRefDir)) { return self.GetRelativePath(aRefDir); };

   UT_SCRIPT_METHOD(bool, IsFile, ()) { return self.IsFile(); };
   UT_SCRIPT_METHOD(bool, IsDirectory, ()) { return self.IsDirectory(); };

   UT_SCRIPT_VARIADIC_METHOD(void, Join, (const UtScriptData*), UtScriptData)
   {
      UtPath path(self);
      for (const auto& arg : aVarArgs)
      {
         if (arg.GetType() == ut::script::Data::Type::cPOINTER)
         {
            auto pathPtr = arg.GetPointer()->GetAppObject<UtPath>();
            if (pathPtr)
            {
               path += *pathPtr;
            }
         }
         else if (arg.GetType() == ut::script::Data::Type::cSTRING)
         {
            path += arg.GetString();
         }
         else
         {
            UT_SCRIPT_ABORT("Invalid type: Can only join a Path with strings or Paths");
         }
      }
      self = path;
   };

   UT_SCRIPT_METHOD(bool, MakeDirectory, ()) { return self.Mkdir(); };
   UT_SCRIPT_METHOD(bool, MakeDirectory, (bool aCreateAll)) { return self.Mkdir(aCreateAll); };

   UT_SCRIPT_WRAP_MEMBER("Up", Up);
}

//! Creates a new script reference of a new UtPath object
UtScriptRef* UtScriptPath::Create(const UtPath& aPath)
{
   return new UtScriptRef(new UtPath(aPath), sScriptPathClassPtr, UtScriptRef::cMANAGE);
}

// =================================================================================================
//! virtual
void* UtScriptPath::Create(const UtScriptContext& aInstance)
{
   return new UtPath();
}

// =================================================================================================
//! virtual
void* UtScriptPath::Clone(void* aObjectPtr)
{
   return new UtPath(*static_cast<UtPath*>(aObjectPtr));
}

// =================================================================================================
//! virtual
void UtScriptPath::Destroy(void* aObjectPtr)
{
   delete static_cast<UtPath*>(aObjectPtr);
}

std::string UtScriptPath::ToString(void* aObjectPtr) const
{
   std::string path;
   if (aObjectPtr != nullptr)
   {
      auto pathPtr = static_cast<UtPath*>(aObjectPtr);
      path         = pathPtr->GetSystemPath();
   }
   return path;
}

bool UtScriptPath::EqualTo(void* aLHS, void* aRHS)
{
   return *static_cast<UtPath*>(aLHS) == *static_cast<UtPath*>(aRHS);
}
