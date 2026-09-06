// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef SCENEGENINPUTFILEREADER_HPP
#define SCENEGENINPUTFILEREADER_HPP

/// @file SceneGenInputFileReader.hpp
/// @brief Declaration of the data structure that reads in the input file
///    and parses and validates the data contained within.

#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace sceneGen
{
//! forward declaration of data structure to store group creation data
struct SceneGenInputData;

/// @brief SceneGenInputFileReader handles opening the input file for platform
///    generation. It is designed to parse the file, line by line, and immediately
///    stop the program and report improperly formed input. Bad data will generate an
///    error message containing the line number of the invalid line. The main purpose of
///    this class is the help the user quickly correct any errors in the input file and
///    not allow the generation of non-working scripts.
class SceneGenInputFileReader
{
public:
   SceneGenInputFileReader(const std::string& aFilepath);

   ~SceneGenInputFileReader();

   /// @brief Checks if file was successfully opened for read
   /// @return true if file was opened, false otherwise.
   bool IsOpen() const;

   /// @brief Closes the file handle
   void Close();

   /// @brief Helper function that fetches the number of groups
   ///    to generate specified in the input file. Calls Failure()
   ///    if the first line in the file does not specify a group count.
   void GetGroupCount();

   /// @brief Helper function to keep track of the number of groups left
   ///    to parse in the input file.
   /// @returns count of remaining groups to parse.
   unsigned int GroupsLeft() const;

   /// @brief Helper function that walks through the input file, retrieving each line.
   ///    Fetches the description (first argument) of each line to determine how and in
   ///    what variable of the InputData struct to store the data in. Will Call
   ///    Failure() if the description is unhandled.
   /// @param aData - reference of the struct to store the data in.
   void FetchData(SceneGenInputData& aData, std::set<std::string>& aPlatformType);

   /// @brief Helper function to determine if the end of file has been reached.
   /// @returns flag that is set to true when the end of file is reached.
   bool IsEOF() const;

   /// @brief Get the line number the parser is currently on in the input file.
   /// @return integer value of currently parsed line.
   unsigned int GetCurrentLine() const;

private:
   /// @brief Helper function that assigns string data to the passed in variable. Will Call
   ///    Failure() if no data retrieved.
   /// @param aVariable - the string storage location.
   void AssignString(std::string& aVariable);

   /// @brief Helper function that assigns float data to the passed in variable.
   /// @param aVariable - the float storage location.
   void AssignFloat(float& aVariable);

   /// @brief Helper function that assigns string data to a vector of strings. Will Call
   ///    Failure() if no data retrieved.
   /// @param aList - the vector storage location.
   void AssignList(std::vector<std::string>& aList);

   /// @brief Helper function that assigns the count of groups to generate. Will Call
   ///    Failure() if no data retrieved or count is less than 1.
   /// @param aCount - int storage location.
   void AssignCount(int& aCount);

   /// @brief Helper function that assigns the Latitude and Longitude of the group. Will Call
   ///    Failure() if no data retrieved or ABS(latitude) > 90 or ABS(longitude) > 180.
   /// @param aLat - latitude storage location.
   /// @param aLon - longitude storage location.
   void AssignLatLon(float& aLat, float& aLon);

   /// @brief Helper function to fetch entire line in file. Increments a counter indicating
   ///    which line the parser is currently on and checks for EOF flag.
   /// @return boolean of successfully getting new line.
   bool GetLine();

   //! Handle to input file.
   std::ifstream mFile;
   //! Helper stringstream to parse tokens in a file line.
   std::stringstream mFileData;
   //! Helper member to store file line
   std::string mCurrentLine;
   //! Counter keeping track of what line in the file the parser is in.
   unsigned int mCurrentLineNumber{};
   //! Number of groups to generate
   unsigned int mGroups{};
   //! Flag to store detection of end of file
   bool mEOF{false};
};

} // namespace sceneGen

#endif // SCENEGENINPUTFILEREADER_HPP
