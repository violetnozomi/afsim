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

#ifndef WSFCOVERAGETEXTOUTPUTWRITER_HPP
#define WSFCOVERAGETEXTOUTPUTWRITER_HPP

#include "wsf_coverage_export.h"

#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

namespace wsf
{
namespace coverage
{

//! Class defining the capability to write field formatted output into a stream.
//!
//! The output produced by this class is designed around making readable CSV format.
//! Clients can set the format, justification, precision, and width of the fields
//! written by this object.
class WSF_COVERAGE_EXPORT TextOutputWriter
{
public:
   TextOutputWriter() = default;
   TextOutputWriter(const TextOutputWriter& aOther);
   virtual ~TextOutputWriter() = default;

   virtual TextOutputWriter* Clone() const { return new TextOutputWriter{*this}; }
   virtual bool              OpenStream(const std::string& aFileName);
   virtual void              CloseStream();

   //! Return the underlying stream object.
   virtual std::ostream& GetStream() { return mStream; }

   virtual bool CreateContainingFolder(const std::string& aPath) const;

   //! Template pattern method for opening the file stream.
   //!
   //! This will open the file for writing with the given @p aFileName in the
   //! given @p aOutputDir. Subclasses are free to ignore the arguments.
   bool OpenFile(const std::string& aOutputDir, const std::string& aFileName);

   //! Get the field precision.
   int GetPrecision() const { return mPrecision; }

   //! Get the field width.
   int GetWidth() const { return mWidth; }

   //! Return if output should be right justified.
   bool IsRightJustified() const { return mRightJustify; }

   //! Return if output should be in fixed format.
   bool IsFixedFormat() const { return mFixedFormat; }

   //! Set the field precision.
   void SetPrecision(int aPrecision) { mPrecision = aPrecision; }

   //! Set the field width.
   void SetWidth(int aWidth) { mWidth = aWidth; }

   //! Set if output is to be right justified.
   //!
   //! \param aRightJustify - true if data should be right justified;
   //!                        false for left justification.
   void SetRightJustified(bool aRightJustify) { mRightJustify = aRightJustify; }

   //! Set if output should be in fixed format.
   //!
   //! \param aFixedFormat - true if data should be in fixed format;
   //!                       false for scientific notation format.
   void SetFixedFormat(bool aFixedFormat) { mFixedFormat = aFixedFormat; }

   //! Write a value to the stream, overriding the field width.
   //!
   //! This will write the given value to the field, respecting the options
   //! set on this object, but overriding the field width with the given
   //! @p aWidth. A comma is added after the field if @p aEndLine is false,
   //! otherwise a newline is appended.
   //!
   //! \param aValue   - The value to add to the stream.
   //! \param aWidth   - The width of the field to stream.
   //! \param aEndLine - true if this is to be the last field on a line.
   template<typename T>
   void WriteFieldWidth(const T& aValue, int aWidth, bool aEndLine = false)
   {
      std::stringstream field{};
      field << (mFixedFormat ? std::fixed : std::scientific);
      field << aValue;
      if (!aEndLine)
      {
         field << ",";
      }
      GetStream() << std::setw(aWidth) << field.str();
      GetStream() << (aEndLine ? "\n" : " ");
   }

   //! Write a value to the stream.
   //!
   //! This will write the given value to the field, respecting the options
   //! set on this object. A comma is added after the field if @p aEndLine is
   //! false, otherwise a newline is appended.
   //!
   //! \param aValue   - The value to add to the stream.
   //! \param aEndLine - true if this is to be the last field on a line.
   template<typename T>
   void WriteField(const T& aValue, bool aEndLine = false)
   {
      WriteFieldWidth(aValue, mWidth, aEndLine);
   }

   void SetStreamJustify();
   void SetStreamFormat();

   static constexpr int cMIN_PRECISION{6};
   static constexpr int cDEFAULT_PRECISION{6};
   static constexpr int cMAX_PRECISION{20};
   static constexpr int cMIN_WIDTH{9};
   static constexpr int cDEFAULT_WIDTH{15};
   static constexpr int cMAX_WIDTH{23};

private:
   int           mPrecision{cDEFAULT_PRECISION};
   int           mWidth{cDEFAULT_WIDTH};
   bool          mRightJustify{true};
   bool          mFixedFormat{true};
   std::ofstream mStream{};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGETEXTOUTPUTWRITER_HPP
