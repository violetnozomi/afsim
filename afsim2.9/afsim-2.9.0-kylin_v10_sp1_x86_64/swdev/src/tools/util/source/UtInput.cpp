// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtInput.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

#include <sys/stat.h>
#include <sys/types.h>

#include "UtAngleR.hpp"
#include "UtCalendar.hpp"
#include "UtColor.hpp"
#include "UtEarth.hpp"
#include "UtException.hpp"
#include "UtInputFile.hpp"
#include "UtInputPreprocessor.hpp"
#include "UtInputString.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSpeed.hpp"
#include "UtStringId.hpp"
#include "UtTime.hpp"

// Include Windows.h last, it #defines a lot of junk
#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{
//! Make sure the preceding input processed by the ReadValue() methods was 'valid'.
//!
//! This routine is called to make sure the proceeding input operation was
//! successfully completed.  This means there is no error on the stream object
//! and that there is nothing but white space following the value just read.
//!
//! @param aIn An input string stream object for the current word being processed.
//! @returns 'true' if valid or 'false' if invalid.
bool InputIsValid(std::istringstream& aIn)
{
   bool isValid = false;
   if (aIn)
   {
      // make sure there is nothing but trailing white space
      isValid = true;
      int nextChar;
      while ((nextChar = aIn.get()) != EOF)
      {
         if (isspace(nextChar) == 0)
         {
            isValid = false;
            break;
         }
      }
   }
   return isValid;
}
} // namespace

// ================================================================================================
UtInput::UtInput()
   : mCommand()
   , mPushedBackStrings()
   , mInputPtr(nullptr)
   , mPreprocessor(std::make_shared<UtInputPreprocessor>())
   , mInputStack()
   , mBufferCache(std::make_shared<BufferCache>())
   , mCurrentOrdering(0.0)
   , mDebug(false)
{
}

// ================================================================================================
//! Get the name of the file that from the top of the input file stack.
std::string UtInput::GetCurrentFileName() const
{
   // NOTE: This must not be virtual!
   return mInputPtr->GetFileName();
}

// ================================================================================================
// virtual
//! Get the name of the file that is the source for THIS input object.
//! @returns The name of the file or an empty string if the source of the input is not a file.
//! @note This is a virtual method that meant to be used internally to return the name of a file for
//! a specific entry in the input stack. Use GetCurrentFileName to get the name of the current file.
std::string UtInput::GetFileName() const
{
   return std::string();
}

// ================================================================================================
//! Append a path to the path list.
//! @param aPathName The directory to be appended to the list of directories to be search for 'include' files
//! (or for any file passed to the 'SearchPathList' method.
//! @returns 'true' if successful or 'false' if the specified path does not exist or is not a directory.
//! @see LocateFile
bool UtInput::AddToPathList(const std::string& aPathName)
{
   bool        ok       = false;
   std::string pathName = SubstitutePathVariables(aPathName);
   if (!pathName.empty())
   {
      std::string::size_type pathSize = pathName.size();
      if (pathSize > 1)
      {
         // Do not include the trailing '/' (or '\' on Windows). It will be included during the concatenation process.
         char lastChar = pathName[pathSize - 1];
         if ((lastChar == '/') || (lastChar == '\\'))
         {
            pathName = pathName.substr(0, pathSize - 1);
         }
      }

      // Make sure the specified path exists and is indeed a directory.

      struct stat statBuf;
      if (stat(pathName.c_str(), &statBuf) == 0)
      {
         if ((statBuf.st_mode & S_IFDIR) != 0)
         {
            // Don't add the entry if it already exists in the list.
            if (find(mPathList.begin(), mPathList.end(), pathName) == mPathList.end())
            {
               mPathList.push_back(pathName);
            }
            ok = true;
         }
         else
         {
            auto out = ut::log::warning() << "Specified 'include_path' is not a directory.";
            out.AddNote() << "Path: " << pathName;
         }
      }
      else
      {
         auto out = ut::log::warning() << "Specified 'include_path' does not exist.";
         out.AddNote() << "Path: " << pathName;
      }
   }

   return ok;
}

// ================================================================================================
//! Clear the include path list.
void UtInput::ResetPathList()
{
   mPathList.clear();
}

// ================================================================================================
void UtInput::DefinePathVariable(const std::string& aName, const std::string& aValue)
{
   if (!aName.empty())
   {
      mPathVariables[aName] = aValue;
   }
}

// ================================================================================================
void UtInput::UndefinePathVariable(const std::string& aName)
{
   if (!aName.empty())
   {
      mPathVariables.erase(aName);
   }
}

// ================================================================================================
//! Delete all of the path variable definitions.
void UtInput::ResetPathVariableList()
{
   mPathVariables.clear();
}

// ================================================================================================
/** Convert a relative input file name to account for the current input file path.

    This method is used to convert a 'relative' file name (one that doesn't start
    with a '/' or a '\') into a file name that is relative to the path of the current
    input file.

    If the supplied file name is relative, this routine will extract the path name
    from the current input file. If the path name is not empty then it will prepend
    the path onto the supplied file name and determine if the file exists. If it
    does then it will return the supplied file name with the prepended path. If it does
    not exist then the original file name will be returned.

    @param aFileName [input] File to be converted.
    @returns The converted file name.

    @note This routine should only be used for files that are going to be read as it
    attempts to verify that the file name exists in the converted directory.
*/
std::string UtInput::ConvertRelativeInputFileName(const std::string& aFileName)
{
   std::string fileName(aFileName);
   if ((fileName[0] != '/') && (fileName[0] != '\\'))
   {
      // Get the file name from the input file and extract its path component.
      std::string inputFileName(GetCurrentFileName());
      if (!inputFileName.empty())
      {
         // Prepend the path from the active file to the specified file.
         std::string::size_type endOfPathIndex = inputFileName.find_last_of("/\\");
         if (endOfPathIndex != std::string::npos)
         {
            std::string   inputFilePath(inputFileName.substr(0, endOfPathIndex + 1));
            std::string   testFileName(inputFilePath + fileName);
            std::ifstream ifsTest(testFileName.c_str(), std::ios::in);
            if (ifsTest)
            {
               fileName = testFileName;
            }
         }
      }
   }
   return fileName;
}

// ================================================================================================
//! Given a file name and the path list, determine the path to be used to access the file.
//!
//! The rules for locating the file are:
//! - If the specified file name represents an absolute path (i.e.: starts with a '/' or '\') or it contains a
//!   DOS drive letter specification ('x:') then the file is returned unmodified.
//! - If an input file is currently being processed, the file name will be appended to the path from that file
//!   and checked for existence. If it exists then the resulting name will be returned.
//! - Finally, the file path list in searched in order of definition. The specified file name is successively
//!   appended to the entries in the path list. If the resulting file exists then its file name will be returned.
//!
//! @param aFileName The file name whose path is to be located.
//! @returns The file name with the appropriate path prefix.
//!
//! @note This routine does not guarantee that the resulting file is readable.
std::string UtInput::LocateFile(const std::string& aFileName)
{
   // Perform path variable substitution and locate the file.

   std::string fileName(SubstitutePathVariables(aFileName));
   return LocateFile(fileName, GetCurrentFileName());
}

// ================================================================================================
// static
//! Given a string, see if it matches the enumerated list of unit types that are convertible to base units.
//! To be successful, the string must be equivalent to the header-file enumeration, but in all caps, with
//! no leading 'c' (i.e. "LENGTH" is a match to UtInput::ValueType::cLENGTH).
//!
//! @param aTypeName The unit type string.
//! @param aValueType The converted ValueType.
//! @returns True if a match was found, and aValueType is valid.
bool UtInput::StringToValueType(const std::string& aTypeName, ValueType& aValueType)
{
   bool        success     = true;
   UtUnitType* unitTypePtr = UtUnits::FindUnitType(aTypeName);
   if (unitTypePtr != nullptr)
   {
      aValueType = static_cast<ValueType>(unitTypePtr->GetUnitTypeId());
   }
   else if (aTypeName == "LATITUDE")
   {
      aValueType = UtInput::cLATITUDE;
   }
   else if (aTypeName == "LONGITUDE")
   {
      aValueType = UtInput::cLONGITUDE;
   }
   else if (aTypeName == "NON_DIMENSIONAL")
   {
      aValueType = UtInput::cNON_DIMENSIONAL;
   }
   else
   {
      success = false;
   }
   return success;
}

// ================================================================================================
/** Convert a value from 'input' units to standard units.

    @param aValue      [input] The value to be converted.
    @param aUnits      [input] The string containing the input units.
    @param aValueType  [input] Enumeration represent the unit type (length, time, etc).
    @return The value in standard units.

    @throws BadValue if the input units are invalid for the specified unit type.

Example:
/code
double meters = ConvertValue(1.0, "feet", UtInput::cLENGTH);
/endcode
will convert 1 foot to the equivalent number of meters.  Note that passing 1.0 for aValue
will give the conversion factor for those conversions that can be expressed as a multiplier.
*/
double UtInput::ConvertValue(double aValue, const std::string& aUnits, ValueType aValueType)
{
   double      value    = aValue;
   std::string unitName = aUnits;
   UtStringUtil::ToLower(unitName);
   bool unitsOk = ConvertValueP(value, unitName, aValueType);
   if (!unitsOk)
   {
      throw BadValue(*this);
   }
   return value;
}

// ================================================================================================
/** Convert a value to arbitrary units from standard units.
    @param aValue        [input] The value, in standard units, to be converted.
    @param aDesiredUnits [input] The string containing the desired output units.
    @param aValueType    [input] Enumeration represent the unit type (length, time, etc).
    @return The value in the specified units.

    @throws an exception if the input units are invalid for the specified unit type.

   See description for non-static UtInput::ConvertValue method.
*/
// static
double UtInput::ConvertValueTo(double aValue, const std::string& aDesiredUnits, ValueType aValueType)
{
   bool   unitsOk = false;
   double value   = 0.0;
   if (aValueType < cFIRST_NON_UNITARY)
   {
      UtUnitType* unitTypePtr = UtUnits::GetUnitType(static_cast<UtUnits::UnitType>(aValueType));
      std::string unitName    = aDesiredUnits;
      UtStringUtil::ToLower(unitName);
      int unit = unitTypePtr->ReadUnit(unitName);
      if (unit != -1)
      {
         unitsOk = true;
         value   = unitTypePtr->ConvertFromStandard(aValue, unit);
      }
   }
   if (!unitsOk)
   {
      throw UtException("Could not convert to units: " + aDesiredUnits);
   }
   return value;
}

// ================================================================================================
/** Convert a value from arbitrary units to standard units.
    @param aValue      [input] The value, in arbitrary units, to be converted.
    @param aFromUnits  [input] The string containing the units of the input value.
    @param aValueType  [input] Enumeration represent the unit type (length, time, etc).
    @return The value in standard units.

    @throws an exception if the input units are invalid for the specified unit type.

   See description for non-static UtInput::ConvertValue method.
*/
// static
double UtInput::ConvertValueFrom(double aValue, const std::string& aFromUnits, ValueType aValueType)
{
   double value   = aValue;
   bool   unitsOk = ConvertValueP(value, aFromUnits, aValueType);
   if (!unitsOk)
   {
      throw UtException("Could not convert from units: " + aFromUnits);
   }
   return value;
}


// ================================================================================================
//! Determine if a unit string is value for the specified value type.
//! @param aUnits  [input] The string containing the units of the input value.
//! @param aValueType  [input] Enumeration represent the unit type (length, time, etc).
//! @return true if the units are valid or false if not.
// static
bool UtInput::ValidateUnits(const std::string& aUnits, ValueType aValueType)
{
   if (aValueType < cFIRST_NON_UNITARY)
   {
      UtUnitType* unitTypePtr = UtUnits::GetUnitType(static_cast<UtUnits::UnitType>(aValueType));
      return unitTypePtr->ReadUnit(aUnits) != -1;
   }
   else
   {
      return false;
   }
}

// ================================================================================================
/** Convert the string to its respective bool, if it can be converted.
    @param aValue The string to be converted.
    @return The bool that the string represents.
    @throws BadValue exception if string does not match a bool keyword.
*/
bool UtInput::ConvertStringToBool(const std::string& aValue)
{
   if ((aValue == "true") || (aValue == "yes") || (aValue == "on") || (aValue == "enable") || (aValue == "enabled"))
   {
      return true;
   }
   if ((aValue == "false") || (aValue == "no") || (aValue == "off") || (aValue == "disable") ||
       (aValue == "disabled") || (aValue == "unknown"))
   {
      return false;
   }
   throw BadValue(*this);
}

// ================================================================================================
/** Get the current command (the last command read by ReadCommand()).

    @return The last command read by ReadCommand.  If a command hasn't yet been
    read then it will read and return a command.
*/
const std::string& UtInput::GetCommand()
{
   if (mCommand.empty())
   {
      ReadCommand(mCommand);
   }
   return mCommand;
}

// ================================================================================================
/** Get the current command (the last command read by ReadCommand()).

    @param aCommand The last command read by ReadCommand.  If a command hasn't yet
    been read then it will read and return a command.
*/
void UtInput::GetCommand(std::string& aCommand)
{
   if (mCommand.empty())
   {
      ReadCommand(mCommand);
   }
   aCommand = mCommand;
}

// ================================================================================================
/** Get the source name of the stream (filename, string, etc.).
    @return A string representing the source name of the stream.
*/
std::string UtInput::GetSource()
{
   return mInputPtr->RealGetSource();
}

// ================================================================================================
/** Get the current location within the input stream for reporting errors.

    @return A string representing the current position in the file.
    @note This is only considered valid during exception processing.
*/
std::string UtInput::GetLocation()
{
   return mInputPtr->RealGetLocation();
}

// ================================================================================================
/** Push back a string so it can be processed again.

  The strings are reprocessed in the order they are pushed (i.e.: this is a FIFO queue).

   @param aString The string to be reprocessed.  In theory this should be the last thing read
   by ReadCommand or ReadValue(std::string).
*/
void UtInput::PushBack(const std::string& aString)
{
   mPushedBackStrings.push(aString);
}

// ================================================================================================
/** Try to read the next command from the input stream.

@param aCommand The next command from the input stream.
@return 'false' if the no more commands are present in the input stream
*/
bool UtInput::TryReadCommand(std::string& aCommand)
{
   bool foundInclude;
   do
   {
      // Assume the command is not an include on each iteration
      foundInclude = false;
      if (mPushedBackStrings.empty())
      {
         while (!mInputPtr->RealReadString(mCommand))
         {
            // No more data in current input stream.  If we're in an 'include' then process it.
            if (mInputStack.empty())
            {
               return false;
            }
            PopInput();
         }
      }
      else
      {
         mCommand = mPushedBackStrings.front();
         mPushedBackStrings.pop();
      }

      if ((mCommand == "include") || (mCommand == "include_once"))
      {
         // This is an "include" or "include_once" command.
         // Read the file name.
         std::string fileName;
         ReadValueQuoted(fileName);
         fileName = LocateFile(fileName);
         FileID fileId(fileName);
         if (!fileId.IsValid())
         {
            throw UtInput::BadValue(*this, "Unable to open file '" + fileName + "'");
         }

         auto iter = mBufferCache->mFileBuffers.find(fileId);
         // If the command was 'include' or the file hasn't already been included, then process it.
         if ((mCommand == "include") || (iter == mBufferCache->mFileBuffers.end()))
         {
            if (iter == mBufferCache->mFileBuffers.end())
            {
               PushInput(ut::make_unique<UtInputFile>(fileName));
               DoFileOpenCallback(fileName);
            }
            else
            {
               auto bufferClone = ut::clone(iter->second.get());
               bufferClone->SeekTo(0);
               PushInput(std::move(bufferClone));
            }
         }

         // Found an include, so continue processing commands
         foundInclude = true;
      }
      else
      {
         aCommand = mCommand;
         if (mDebug)
         {
            auto out = ut::log::debug() << "Read stream token.";
            out.AddNote() << "Token: " << aCommand;
         }
      }
   } while (foundInclude);
   return true;
}

// ================================================================================================
/** Read the next command from the input stream.

    @param aCommand The next command from the input stream.

    @throws EndOfData if no more commands remain.
*/
bool UtInput::ReadCommand(std::string& aCommand)
{
   if (!TryReadCommand(aCommand))
   {
      throw UtInput::EndOfData(*this);
   }
   return true;
}

// ================================================================================================
/** Read the next line from the input stream.

    @param aValue The next line from the input stream.
    @param aIgnoreCurrentLine If 'true' (the default value) then any partial line that hasn't yet
    been processed will be ignored and the returned value will be the next complete line.
    If 'false' then only the remaining partially unprocessed line will be returned.
    @throws EndOfData if there is no data left to read.
*/
void UtInput::ReadLine(std::string& aValue, bool aIgnoreCurrentLine)
{
   if (!mInputPtr->RealReadLine(aValue, aIgnoreCurrentLine))
   {
      throw UtInput::EndOfData(*this);
   }

   // If we are ignoring any partial line then we must discard any pushed back strings. If not then they
   // are considered to be part of the partial line and must be prepended to what has been read.

   while (!mPushedBackStrings.empty())
   {
      if (!aIgnoreCurrentLine)
      {
         aValue = mPushedBackStrings.front() + aValue;
      }
      mPushedBackStrings.pop();
   }
}

// ================================================================================================
/** Read the next word from the input stream as bool value.

@param aValue The value read from the input stream.
@throws EndOfData if there is no data left to read.
@throws BadValue if the value was improperly formatted.
*/
void UtInput::ReadValue(bool& aValue)
{
   std::string strValue;
   if (mPushedBackStrings.empty())
   {
      if ((!mInputPtr->RealReadString(strValue)) || strValue.empty())
      {
         throw EndOfData(*this);
      }
   }
   else
   {
      strValue = mPushedBackStrings.front();
      mPushedBackStrings.pop();
   }
   UtStringUtil::ToLower(strValue);
   aValue = ConvertStringToBool(strValue);
}

// ================================================================================================
/** Read the next word from the input stream as string value.

    @param aValue The value read from the input stream.
    @throws EndOfData if there is no data left to read.
    @throws BadValue if the value was improperly formatted.
*/
void UtInput::ReadValue(std::string& aValue)
{
   if (mPushedBackStrings.empty())
   {
      while (!mInputPtr->RealReadString(aValue))
      {
         // No more data in current input stream.  If we're in an 'include' then process it.
         PopInput();
      }
   }
   else
   {
      aValue = mPushedBackStrings.front();
      mPushedBackStrings.pop();
   }
}

// ================================================================================================
//! Reads a value that is enclosed in quotation marks
//!
//! @param aValue The value read from the input stream.
//! @throws EndOfData if there is no data left to read.
//! @throws BadValue if the value was improperly formatted.
// public
void UtInput::ReadValueQuoted(std::string& aValue)
{
   if (mPushedBackStrings.empty())
   {
      while (!mInputPtr->RealReadStringQuoted(aValue))
      {
         // No more data in current input stream.  If we're in an 'include' then process it.
         PopInput();
      }
   }
   else
   {
      aValue = mPushedBackStrings.front();
      mPushedBackStrings.pop();
   }
}

// ================================================================================================
//! Read the next word from the input stream as string value.
// public
void UtInput::ReadValue(UtStringId& aStringId)
{
   std::string value;
   ReadValue(value);
   aStringId = value;
}

// ================================================================================================
//! Read in a date and time from the input stream.
//!
//! This reads in a UtCalendar date/time from the given @p aInput. The
//! expected format is <month> <day> <year> <hh:mm:ss>.
//!
//! @param aCalendar The value read from this input stream.
//! @throws BadValue if the value was improperly formatted.
//! @throws EndOfData if the input stream does not contain every needed field.
void UtInput::ReadValue(UtCalendar& aCalendar)
{
   std::string monthString;
   ReadValue(monthString);
   int month = UtCalendar::GetMonth(monthString);
   if (month == 0)
   {
      throw UtInput::BadValue(*this);
   }
   int day;
   ReadValue(day);
   int year;
   ReadValue(year);
   std::string timeString;
   ReadValue(timeString);
   int    hour;
   int    minute;
   double second;
   bool   valid = UtCalendar::GetTime(timeString, hour, minute, second);
   if (!valid)
   {
      throw UtInput::BadValue(*this);
   }
   if (!aCalendar.SetDate(year, month, day))
   {
      throw UtInput::BadValue(*this);
   }
   if (!aCalendar.SetTime(hour, minute, second))
   {
      throw UtInput::BadValue(*this);
   }
}

// ================================================================================================
/** Read the next word from the input stream as an 'UtColor' value.

@param aColor The value read from the input stream.
@throws EndOfData if there is no data left to read.
@throws BadValue if the value was improperly formatted.
*/
void UtInput::ReadValue(UtColor& aColor)
{
   std::string strValue;
   ReadValue(strValue);
   UtStringUtil::ToLower(strValue);

   if (!UtColor::IsHexColor(strValue) && !UtColor::ColorExists(strValue))
   {
      // First string read was not a hex value or a named color. Was either "float" or the red component.
      if (strValue == "float")
      {
         // String was "float" indicating floating point values [0, 1]. Read the red component.
         std::string red;
         ReadValue(red);
         strValue += ' ' + red;
      }

      // Read the green and blue components.
      std::string green;
      ReadValue(green);
      std::string blue;
      ReadValue(blue);
      // Try to read an alpha component and cast to a numeric value
      std::string alpha;
      try
      {
         ReadValue(alpha);
         (void)std::stof(alpha); // Ignore return value, care only whether it throws
      }
      catch (std::invalid_argument&)
      {
         // Read value was not numeric, push it back into input stream.
         PushBack(alpha);
         alpha.clear();
      }
      catch (ExceptionBase&)
      {
         // Another exception was thrown, e.g. EndOfData if no data was
         // available to read. Catch it silently.
      }

      strValue += ' ' + green + ' ' + blue;
      if (!alpha.empty())
      {
         strValue += ' ' + alpha;
      }
   }

   std::istringstream iss(strValue);
   iss >> aColor;
   if (!InputIsValid(iss))
   {
      throw BadValue(*this);
   }
}

// ================================================================================================
/** Determine if a string is the expected value

    @param aValue         String value to be tested.
    @param aExpectedValue The expected string value.
    @throws BadValue if aValue != aExpectedValue.
*/
void UtInput::StringEqual(const std::string& aValue, const std::string& aExpectedValue)
{
   if (aValue != aExpectedValue)
   {
      throw BadValue(*this);
   }
}

// ================================================================================================
// Exception classes
// ================================================================================================
namespace
{
std::string GenerateWhatText(UtInput& aInput, const std::string& aMessage)
{
   std::stringstream ss;
   ss << "***** ERROR: " << aMessage << std::endl;
   ss << "             " << aInput.GetLocation() << std::endl;
   return ss.str();
}
} // namespace

UtInput::ExceptionBase::ExceptionBase(UtInput& aInput, const std::string& aWhat)
   : UtException(GenerateWhatText(aInput, aWhat))
   , mMessage(aWhat)
   , mLocation(aInput.GetLocation())
{
}

UtInput::BadValue::BadValue(UtInput& aInput)
   : ExceptionBase(aInput, "Bad value for: " + aInput.GetCommand())
{
}

UtInput::BadValue::BadValue(UtInput& aInput, const std::string& aMessage)
   : ExceptionBase(aInput, aMessage)
{
}

UtInput::EndOfData::EndOfData(UtInput& aInput)
   : ExceptionBase(aInput, "Unexpected End Of Data")
{
}

UtInput::UnknownCommand::UnknownCommand(UtInput& aInput)
   : ExceptionBase(aInput, "Unknown command: " + aInput.GetCommand())
{
}

UtInput::OutOfContext::OutOfContext(UtInput& aInput)
   : ExceptionBase(aInput, "'" + aInput.GetCommand() + "' cannot be used in this context")
{
}

UtInput::OutOfContext::OutOfContext(UtInput& aInput, const std::string& aMessage)
   : ExceptionBase(aInput, aMessage)
{
}

UtInput::UnknownUnits::UnknownUnits(UtInput& aInput)
   : ExceptionBase(aInput, "Unknown units: " + aInput.GetCommand())
{
}

// ================================================================================================
// static private
//! Evaluate a complex unit, given a value, operator ('*' or '/'), and value types.
//! If it is desired to evaluate a unit with more than two parts,
//! it should be called with a predicate value type of "cDEFERRED",
//! then called again for the complex predicate.
bool UtInput::EvaluateComplexUnit(double&            aValue,
                                  const std::string& aUnits,
                                  const char&        aOperator,
                                  ValueType          aSubjectType,
                                  ValueType          aPredicateType)
{
   std::string subject;
   std::string predicate;
   bool        success = BreakComplexUnit(aUnits, aOperator, subject, predicate);
   if (success)
   {
      success = ConvertValueP(aValue, subject, aSubjectType);
      if (success)
      {
         double predicateValue = 1.0;
         success               = ConvertValueP(predicateValue, predicate, aPredicateType);
         if (success)
         {
            if (aOperator == '/')
            {
               aValue /= predicateValue;
            }
            else if (aOperator == '*')
            {
               aValue *= predicateValue;
            }
            else
            {
               success = false;
            }
         }
      }
   }
   return success;
}

// ================================================================================================
// static
//! Break a complex unit into a (simple) subject and a predicate.
//! This method can be called recursively, with a complex predicate,
//! until the predicate is also simplified.
bool UtInput::BreakComplexUnit(const std::string& aUnits, std::string& aSubject, std::string& aPredicate)
{
   // This method is the same as the four-parameter version,
   // Except that we don't care about the operator.
   char oper; // to be used as a return value.
   return BreakComplexUnit(aUnits, aSubject, aPredicate, oper);
}

// ================================================================================================
// static
//! Break a complex unit into a (simple) subject and a predicate,
//! also returning the operator between the simple subject and predicate.
//! This method can be called recursively, with a complex predicate,
//! until the predicate is also simplified.
//! @param aUnits The complex unit to be evaluated.
//! @param aSubject The returned simple subject unit.
//! @param aPredicate The returned simple or complex predicate unit.
//!  If the predicate value is complex, the return value will be 'true.'
//! @param aOperator The operator between the subject and predicate,
//!  '*', '/', or 0 if no operator is present.
//! @return Whether the returned predicate is itself a complex unit.
//!  If so, this method may be called again to further simplify the predicate.
bool UtInput::BreakComplexUnit(const std::string& aUnits, std::string& aSubject, std::string& aPredicate, char& aOperator)
{
   std::string::size_type multOperatorPos = aUnits.find('*');
   std::string::size_type divOperatorPos  = aUnits.find('/');
   bool                   isBreakable     = false;

   if (multOperatorPos != std::string::npos)
   {
      aOperator = '*';
      if ((divOperatorPos != std::string::npos) && (divOperatorPos < multOperatorPos))
      {
         isBreakable = BreakComplexUnit(aUnits, '/', aSubject, aPredicate);
      }
      else
      {
         isBreakable = BreakComplexUnit(aUnits, '*', aSubject, aPredicate);
      }
   }
   else if (divOperatorPos != std::string::npos)
   {
      aOperator   = '/';
      isBreakable = BreakComplexUnit(aUnits, '/', aSubject, aPredicate);
   }
   else
   {
      aOperator = 0;
   }

   return isBreakable;
}

// ================================================================================================
// static private
//! Break a complex unit into a (simple) subject and a predicate.
//! This method can be called recursively, with a complex predicate,
//! until the predicate is also simplified.
bool UtInput::BreakComplexUnit(const std::string& aUnits, const char& aOperator, std::string& aSubject, std::string& aPredicate)
{
   std::string::size_type operatorPos = aUnits.find(aOperator);
   bool isBreakable = ((operatorPos != std::string::npos) && (operatorPos != 0) && ((operatorPos + 1) != aUnits.size()));
   if (isBreakable)
   {
      aSubject   = aUnits.substr(0, operatorPos);
      aPredicate = aUnits.substr(operatorPos + 1);
   }

   return isBreakable;
}

// ================================================================================================
// static private
bool UtInput::ConvertValueP(double& aValue, const std::string& aUnits, ValueType aValueType)
{
   bool unitsOk = true;
   if (aValueType == cNON_DIMENSIONAL)
   {
      unitsOk = true;
   }
   else if (aUnits.empty())
   {
      unitsOk = false;
   }
   else if (aValueType < cFIRST_NON_UNITARY)
   {
      UtUnitType* unitTypePtr = UtUnits::GetUnitType(static_cast<UtUnits::UnitType>(aValueType));
      if (unitTypePtr)
      {
         int unit = unitTypePtr->ReadUnit(aUnits);
         if (unit != -1)
         {
            aValue = unitTypePtr->ConvertToStandard(aValue, unit);
         }
         else
         {
            unitsOk = false;
         }
      }
   }
   else
   {
      unitsOk = false;
   }
   return unitsOk;
}

// ================================================================================================
//! See (static method) UtInput::GetValueType.
//! This method throws an exception of type UtInput::UnknownUnits if
//! the units cannot be resolved into an existing simple type.
UtInput::ValueType UtInput::GetValueType(const std::string& aUnits)
{
   ValueType returnValue = GetValueTypeOf(aUnits);
   if (returnValue == UtInput::cINVALID)
   {
      // Could not resolve.
      throw UtInput::UnknownUnits(*this);
   }
   return returnValue;
}

// ================================================================================================
// static
//! Given a valid specific unit type (e.g., "meters"), return the general type to
//! which the unit belongs (e.g., UtInput::cLENGTH).  Only "simple" types will be
//! returned (cFLUENCE or cIRRADIANCE are not simple).
//! @note Some unit designations are ambiguous (m -> meters or minutes).
//!       In these situations, this function will return cINVALID
UtInput::ValueType UtInput::GetValueTypeOf(const std::string& aUnits)
{
   UtInput::ValueType unitType = cINVALID;
   std::string        name     = aUnits;
   UtStringUtil::ToLower(name);
   for (const auto& unitPtr : UtUnits::mUnitTypes)
   {
      if (unitPtr && unitPtr->IsUnitValid(name))
      {
         if (unitType != cINVALID)
         {
            unitType = cINVALID;
            break;
         }
         unitType = static_cast<ValueType>(unitPtr->GetUnitTypeId());
      }
   }
   return unitType;
}

// ================================================================================================
//! Locate the specified file.
//! @param aFileName The file to be located.
//! @param aCurrentFileName The name of the current input file.
//! @returns The string that can be used to open the file (path name and file name).
std::string UtInput::LocateFile(const std::string& aFileName, const std::string& aCurrentFileName)
{
   // Return immediately if the file contains an absolute path prefix.

   if ((aFileName[0] == '/') || (aFileName[0] == '\\'))
   {
      return aFileName;
   }

#ifdef _WIN32

   // Return immediately if a drive letter specification is present.

   if ((aFileName.size() > 1) && ((isalpha(aFileName[0])) != 0) && (aFileName[1] == ':'))
   {
      return aFileName;
   }
#endif

   // Check if the file exists relative to the directory containing the current input file.

   struct stat statBuf;

   if (!aCurrentFileName.empty())
   {
      std::string::size_type endOfPathIndex = aCurrentFileName.find_last_of("/\\");
      if (endOfPathIndex != std::string::npos)
      {
         // The current file has a path specification.  Prefix the specified file with the path from the current file
         // and determine if the resulting file exists.

         std::string currentFilePath(aCurrentFileName.substr(0, endOfPathIndex + 1));
         std::string fileName(currentFilePath + aFileName);
         if (stat(fileName.c_str(), &statBuf) == 0)
         {
            return fileName;
         }
      }
      else if (stat(aFileName.c_str(), &statBuf) == 0)
      {
         // No path specification on the current file. Specified file exists so use it.
         return aFileName;
      }
   }

   // Attempt to locate the file using the path list.

   for (const auto& path : mPathList)
   {
      std::string fileName = path + '/' + aFileName;
      if (stat(fileName.c_str(), &statBuf) == 0)
      {
         return fileName;
      }
   }

   // File not found anywhere. Just return the original name and let the caller deal with it.

   return aFileName;
}

// ================================================================================================
//! Substitute path variables in a string.
//!
//! This method will search the input string for path variable substitutions, that is, a string of the form
//! '$(name)' or '${name}'. When such a string is found, the '$(name)' will be replaced by the value the first
//! of the following:
//!
//! - The value of internally defined path variable with the specified name.
//! - The value of the environment variable with the specified name.
//!
//! Recursive substitution will NOT be performed (i.e.: after a substitution is performed, scanning continues at
//! the character that follows the replacement text). Two adjacent dollar signs ('$$') are replaced with one and
//! scanning continues with the character following the retained '$'.
//!
//! @param aString The string for which substitution is being requested.
//! @return The input string with substitutions performed.
std::string UtInput::SubstitutePathVariables(const std::string& aString)
{
   std::string            result(aString);
   std::string::size_type startPos = 0;

   while (startPos < result.size())
   {
      // Attempt to locate the next '$' that follows the end of the last substitution.

      std::string::size_type varBeg = result.find('$', startPos);
      if (varBeg == std::string::npos)
      {
         break; // '$' not found
      }
      if ((varBeg + 1) >= result.size())
      {
         break; // '$' at end of string.
      }

      // A '$' has been found. See if it is the start of a substitution request.

      char nextChar = result[varBeg + 1];
      if ((nextChar == '(') || (nextChar == '{'))
      {
         // '$(' or '${' - possible start of a macro
         if ((varBeg + 3) >= result.size())
         {
            break; // Not enough for '$(x)'
         }

         char                   endChar = (nextChar == '(') ? ')' : '}';
         std::string::size_type varEnd  = result.find(endChar, varBeg + 2);
         if (varEnd == std::string::npos)
         {
            break; // end of macro not found - leave the string as-is.
         }

         if (varEnd > (varBeg + 2))
         {
            // Extract the macro name and get the value. of the environment variable with the same name.

            std::string::size_type                             nameSize = varEnd - varBeg - 2;
            std::string                                        name     = result.substr(varBeg + 2, nameSize);
            std::string                                        value;
            std::map<std::string, std::string>::const_iterator varIter = mPathVariables.find(name);
            if (varIter != mPathVariables.end())
            {
               // Variable has been defined internally, so use its value.
               value = (*varIter).second;
            }
            else
            {
               // Variable has not been defined internally. Attempt to use the environment.
               char* valuePtr = ::getenv(name.c_str());
               if (valuePtr != nullptr)
               {
                  value = valuePtr;
               }
            }
            result.replace(varBeg, nameSize + 3, value);
            startPos = varBeg + value.size(); // Continue the scan after the replaced text.
         }
         else
         {
            startPos = varEnd + 1; // Null macro - '$()'. Just leave it.
         }
      }
      else if (nextChar == '$')
      {
         // '$$' - keep one '$' and continue with the character after the retained '$'
         result.erase(varBeg, 1);
         startPos = varBeg + 1;
      }
      else
      {
         // '$' followed by something other than '(', '{' or '$'.
         // Retain the '$' and continue with the character after the '$'.
         startPos = varBeg + 1;
      }
   }

   return result;
}

// ================================================================================================
//! Returns the UtInput object currently being processed.
//! This will change in cases of 'include' or 'include_once'
UtInputBuffer* UtInput::GetCurrentInput()
{
   return mInputPtr.Pointer();
}

// ================================================================================================
//! Remove any pushed back strings from the UtInput stream
void UtInput::ClearPushedBackStrings()
{
   while (!mPushedBackStrings.empty())
   {
      mPushedBackStrings.pop();
   }
}

//! Adds an input buffer to the input stream for processing next.
void UtInput::PushInput(std::unique_ptr<UtInputBuffer> aInputPtr)
{
   // When using a preprocessor, create a new preprocess buffer
   if (mPreprocessor)
   {
      if (dynamic_cast<UtInputFile*>(aInputPtr.get()) != nullptr)
      {
         aInputPtr = ut::make_unique<UtInputPreprocessorBuffer>(mPreprocessor.get(), std::move(aInputPtr));
      }
   }

   std::string fileName = aInputPtr->GetFileName();
   if (mInputPtr != nullptr)
   {
      if (!fileName.empty())
      {
         auto it =
            std::find_if(std::begin(mInputStack),
                         std::end(mInputStack),
                         [&fileName](const UtInputBuffer* aBufferPtr) { return fileName == aBufferPtr->GetFileName(); });

         if (it != mInputStack.end() || fileName == mInputPtr->GetFileName())
         {
            throw UtInput::ExceptionBase(*this, "Circular dependency caused by including file '" + fileName + "'");
         }
      }

      mInputStack.push_back(mInputPtr);
   }

   if (!fileName.empty())
   {
      FileID fileId(fileName);
      if (mBufferCache->mFileBuffers.find(fileId) == mBufferCache->mFileBuffers.end())
      {
         (mBufferCache->mFileBuffers)[fileId] = ut::clone(aInputPtr);
      }
   }
   mInputPtr = std::move(aInputPtr);
}

//! Add an input string buffer to the input stream for processing next
void UtInput::PushInputString(const std::string& aString)
{
   PushInput(ut::make_unique<UtInputString>(aString));
}

//! Link two UtInput objects so that they share information about shared files and preprocessor variables
void UtInput::LinkInputs(UtInput& aOtherInput)
{
   LinkBuffers(aOtherInput);
   mPreprocessor     = aOtherInput.mPreprocessor;
   mFileOpenCallback = aOtherInput.mFileOpenCallback;
   CopyAuxData(aOtherInput);

   mPathList      = aOtherInput.mPathList;
   mPathVariables = aOtherInput.mPathVariables;
}

//! Sets this UtInput's buffer cache to one in aOtherInput.
//! This allows two UtInput objects to share files
void UtInput::LinkBuffers(UtInput& aOtherInput)
{
   mBufferCache = aOtherInput.mBufferCache;
}

void UtInput::PopInput()
{
   if (mInputStack.empty())
   {
      throw EndOfData(*this);
   }
   mInputPtr = std::move(mInputStack.back());
   mInputStack.pop_back();
}

UtInput::StoredLocation UtInput::StoreLocation()
{
   StoredLocation loc;
   loc.mInputPtr = nullptr;
   if (mInputPtr != nullptr)
   {
      loc.mInputPtr = mInputPtr->Clone();
   }
   loc.mOffset               = static_cast<size_t>(-1);
   loc.mCurrentCommand       = mCommand;
   UtInputDocument* docInput = dynamic_cast<UtInputDocument*>(mInputPtr.Pointer());
   if (docInput != nullptr)
   {
      loc.mOffset = docInput->GetDocumentPosition().GetIndex();
   }
   return loc;
}

void UtInput::RestoreLocation(const StoredLocation& aLocation)
{
   mInputPtr = aLocation.mInputPtr;
   mCommand  = aLocation.mCurrentCommand;
   if (aLocation.mOffset != static_cast<size_t>(-1))
   {
      mInputPtr->SeekTo(aLocation.mOffset);
   }
}

// ================================================================================================
void UtInput::SetAuxData(int aIndex, void* aAuxData)
{
   if (static_cast<int>(mAuxData.size()) <= aIndex)
   {
      mAuxData.resize(aIndex + 1);
   }
   mAuxData[aIndex] = aAuxData;
}

// ================================================================================================
void* UtInput::GetAuxData(int aIndex)
{
   if (static_cast<int>(mAuxData.size()) <= aIndex)
   {
      return nullptr;
   }
   return mAuxData[aIndex];
}

// ================================================================================================
// Copy contents of aux data contained in aOtherInput into this input object
void UtInput::CopyAuxData(const UtInput& aOtherInput)
{
   mAuxData = aOtherInput.mAuxData;
}

// ================================================================================================
//! Returns the preprocessor used by this input object.  The preprocessor may be shared with other
//! UtInput instances
UtInputPreprocessor* UtInput::GetPreprocessor() const
{
   return mPreprocessor.get();
}

std::string UtInput::StoredLocation::GetLocation() const
{
   if (mOffset != static_cast<size_t>(-1))
   {
      UtInputDocument* docInput = static_cast<UtInputDocument*>(mInputPtr.Pointer());
      return docInput->GetLocationAtOffset(mOffset);
   }
   return "<input>";
}

// ================================================================================================
UtInput::FileID::FileID(const std::string& aFileName)
   : mFileName(aFileName)
   , mDeviceId(0)
   , mINodeId(0)
{
#ifdef _WIN32
   //! use the pathname to identify the file
   char  path[2000];
   char* fileNamePtr;
   if (0 != GetFullPathName(mFileName.c_str(), 2000, path, &fileNamePtr))
   {
      mWindowsFullPathName = path;
      std::transform(mWindowsFullPathName.begin(), mWindowsFullPathName.end(), mWindowsFullPathName.begin(), tolower);
   }
#else
   //! use the device id & inode to identify the file
   struct stat statData;
   if (0 == stat(aFileName.c_str(), &statData))
   {
      mDeviceId = static_cast<unsigned>(statData.st_dev);
      mINodeId  = static_cast<unsigned>(statData.st_ino);
   }
#endif
}

//! Compares two FileID's
//! @param aEq The FileID to compare against
//! @return 'true' if both FileID's refer to the same file
bool UtInput::FileID::operator==(const FileID& aEq) const
{
#ifdef _WIN32
   return aEq.mWindowsFullPathName == mWindowsFullPathName;
#else
   return aEq.mDeviceId == mDeviceId && aEq.mINodeId == mINodeId;
#endif
}

bool UtInput::FileID::operator<(const FileID& aRhs) const
{
#ifdef _WIN32
   return mWindowsFullPathName < aRhs.mWindowsFullPathName;
#else
   if (mDeviceId < aRhs.mDeviceId)
   {
      return true;
   }
   if (mDeviceId > aRhs.mDeviceId)
   {
      return false;
   }
   return mINodeId < aRhs.mINodeId;
#endif
}

bool UtInput::FileID::IsValid() const
{
#ifdef _WIN32
   return !mWindowsFullPathName.empty();
#else
   return mINodeId != 0;
#endif
}
