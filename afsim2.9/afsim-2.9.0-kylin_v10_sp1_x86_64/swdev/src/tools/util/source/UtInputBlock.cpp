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

#include "UtInputBlock.hpp"

#include "UtInput.hpp"

//! Construct an input block object.
//!
//! aBlockTerminator is the string that terminates the block. If an empty string
//! is passed then the terminator is assumed to be the concatenation:
//!
//!   "end_" + aInput.GetCommand();
UtInputBlock::UtInputBlock(UtInput& aInput, const std::string& aBlockTerminator)
   : mInput(aInput)
   , mBlockTerminator(aBlockTerminator)
   , mBlockTerminated(false)
{
   if (mBlockTerminator.empty())
   {
      mBlockTerminator = "end_" + aInput.GetCommand();
   }
}

// virtual
UtInputBlock::~UtInputBlock()
{
   if (!mBlockTerminated)
   {
      try
      {
         std::string word(" ");
         while (word != mBlockTerminator)
         {
            mInput.ReadValue(word);
         }
      }
      catch (std::exception&)
      {
         // We should not rethrow the exception because throwing an exception while
         // in an exception will terminate the program!
      }
   }
}

//! Return the command just read by ReadCommand()
const std::string& UtInputBlock::GetCommand()
{
   return mInput.GetCommand();
}

//! Read the next command in the block.
//! @return true if a command has been read and it is NOT the block terminator.
//! @return false if the command WAS the block terminator.
bool UtInputBlock::ReadCommand()
{
   std::string command;
   return ReadCommand(command);
}

//! Read the next command in the block.
//! @param aCommand an output string that will contain the command just read.
//! @return true if a command has been read and it is NOT the block terminator.
//! @return false if the command WAS the block terminator.
bool UtInputBlock::ReadCommand(std::string& aCommand)
{
   bool haveCommand = true;
   mInput.ReadCommand(aCommand);
   if (aCommand == mBlockTerminator)
   {
      haveCommand      = false;
      mBlockTerminated = true;
   }
   return haveCommand;
}

// =================================================================================================
// This is called by the template ProcessInput when an invalid command is detection.
// The body could have been coded directly in the template, but this mechanism avoids the need for
// including UtInput.hpp in the header file (improve compile time) and eliminates some code bloat.
// private
void UtInputBlock::ThrowUnknownCommandException()
{
   throw UtInput::UnknownCommand(mInput);
}
