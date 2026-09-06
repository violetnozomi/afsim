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

#ifndef MOCK_SOURCE_PROVIDER_HPP
#define MOCK_SOURCE_PROVIDER_HPP

#include <memory>
#include <unordered_map>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "UtPath.hpp"
class UtTextDocument;
#include "WsfParseSourceProvider.hpp"

namespace wsf
{
namespace parser
{
namespace test
{

//! Light-weight implementation of the WsfParseSourceProvider interface
class SourceProvider : public WsfParseSourceProvider
{
public:
   SourceProvider(std::unordered_map<UtPath, std::unique_ptr<UtTextDocument>>& aSources);
   ~SourceProvider() override = 0;

   //! Finds a source in the cache
   //! @param aPath is the file path to the source
   //! @param aReadAccess is whether the WsfParser can read the source
   //! @return the found source
   //! @retval nullptr if it did not find the source
   UtTextDocument* FindSource(const UtPath& aPath, bool /* aReadAccess */) override;

   //! Creates a source
   //! @param aPath is the file path to the source
   //! @return the created source
   //! @retval nullptr if the source already exists
   UtTextDocument* CreateSource(const UtPath& aPath) override;

   //! Creates a "virtual" source
   //! @par details
   //!   Normally, the WsfParser will find the source on the filesystem via the given path.
   //!   Some cases, however, one may need to create a source programmatically.
   //!   A "virtual" source is such source.
   //!   This function uses the path to store it in the cache, but does not read any data from the filesystem.
   //! @param aPath is the file path to the source
   //! @return the created source
   //! @retval nullptr if the source already exists
   UtTextDocument* CreateVirtualSource(const UtPath& aPath) noexcept;

   //! Get the number of sources in the cache
   //! @return the number of sources in the cache
   std::size_t NumberOfSources() const noexcept;

private:
   std::unordered_map<UtPath, std::unique_ptr<UtTextDocument>>& mSourcesRef;
}; // class SourceProvider

//! Mock of SourceProvider
class MockSourceProvider final : public wsf::parser::test::SourceProvider
{
public:
   MockSourceProvider(std::unordered_map<UtPath, std::unique_ptr<UtTextDocument>>& aSources);
   ~MockSourceProvider() override = default;

   MOCK_METHOD2(FindSource, UtTextDocument*(const UtPath& aPath, bool aReadAccess));
   MOCK_METHOD1(CreateSource, UtTextDocument*(const UtPath& aPath));
   MOCK_CONST_METHOD0(NumberOfSources, std::size_t());
}; // class MockSourceProvider

} // namespace test
} // namespace parser
} // namespace wsf

#endif // MOCK_SOURCE_PROVIDER_HPP
