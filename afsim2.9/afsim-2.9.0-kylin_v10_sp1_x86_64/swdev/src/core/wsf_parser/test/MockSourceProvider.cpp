// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "MockSourceProvider.hpp"

#include <iostream>

#include "UtTextDocument.hpp"

namespace wsf
{
namespace parser
{
namespace test
{

SourceProvider::SourceProvider(std::unordered_map<UtPath, std::unique_ptr<UtTextDocument>>& aSources)
   : mSourcesRef(aSources)
{
}

SourceProvider::~SourceProvider() = default;

UtTextDocument* SourceProvider::FindSource(const UtPath& aPath, bool /* aReadAccess */)
{
   // std::cout << "FindSource: aPath: " << aPath.GetNormalizedPath();
   auto found = mSourcesRef.find(aPath);
   if (found != mSourcesRef.end())
   {
      return found->second.get();
   }
   return nullptr;
}

UtTextDocument* SourceProvider::CreateSource(const UtPath& aPath)
{
   // std::cout << "CreateSource: aPath: " << aPath.GetNormalizedPath();
   auto found = mSourcesRef.find(aPath);
   if (found == mSourcesRef.end())
   {
      mSourcesRef.emplace(aPath, ut::make_unique<UtTextDocument>(aPath));
      return mSourcesRef.at(aPath).get();
   }
   return nullptr;
}

UtTextDocument* SourceProvider::CreateVirtualSource(const UtPath& aPath) noexcept
{
   // std::cout << "CreateVirtualSource: aPath: " << aPath.GetNormalizedPath();
   auto found = mSourcesRef.find(aPath);
   if (found == mSourcesRef.end())
   {
      mSourcesRef.emplace(aPath, ut::make_unique<UtTextDocument>());
      return mSourcesRef.at(aPath).get();
   }
   return nullptr;
}

std::size_t SourceProvider::NumberOfSources() const noexcept
{
   return mSourcesRef.size();
}

MockSourceProvider::MockSourceProvider(std::unordered_map<UtPath, std::unique_ptr<UtTextDocument>>& aSources)
   : SourceProvider(aSources)
{
}

} // namespace test
} // namespace parser
} // namespace wsf
