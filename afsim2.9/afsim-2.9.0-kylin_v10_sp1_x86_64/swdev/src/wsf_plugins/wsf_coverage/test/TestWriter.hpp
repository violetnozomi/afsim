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

#ifndef WSFCOVERAGETESTTESTWRITER_HPP
#define WSFCOVERAGETESTTESTWRITER_HPP

#include <sstream>

#include "WsfCoverageTextOutputWriter.hpp"

class TestWriter : public wsf::coverage::TextOutputWriter
{
public:
   TestWriter() = default;
   TestWriter(const TestWriter& aOther)
      : wsf::coverage::TextOutputWriter(aOther)
      , mStream{}
   {
   }
   ~TestWriter() override = default;

   TestWriter*   Clone() const override { return new TestWriter{*this}; }
   bool          OpenStream(const std::string& aFileName) override { return true; }
   void          CloseStream() override {}
   std::ostream& GetStream() override { return mStream; }
   bool          CreateContainingFolder(const std::string& aPath) const override { return true; }

   std::string GetStreamContents() const { return mStream.str(); }

private:
   std::stringstream mStream{};
};

#endif // WSFCOVERAGETESTTESTWRITER_HPP
