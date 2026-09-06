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

#ifndef UTINPUTBLOCK_HPP
#define UTINPUTBLOCK_HPP

#include "ut_export.h"

#include <string>

class UtInput;

//! A class to assist in processing input 'blocks' in the presence of exceptions.
//!
//! This class exists to provide a common framework for dealing with block input
//! (e.g. 'platform_type' .... 'end_platform_type').
//!
//! The application should create a local instance of a variable of this type before
//! reading the first command inside the block.  It should then enter a loop to
//! read and process the commands as follows
//! \code
//!    UtInputBlock block(aInput, "end_block");
//!    std::string command;
//!    while (block.ReadCommand(command))
//!    {
//!      // process the command
//!    }
//! \endcode
//! If an instance of this variable is destroyed and the block is not terminated,
//! then all of the input up to the block terminator will be read and discarded.
//! Thus, if an exception is thrown inside the processing loop, command processing
//! can restart after the block terminator (assuming, of course, the exception
//! is properly handled).

class UT_EXPORT UtInputBlock
{
public:
   UtInputBlock(UtInput& aInput, const std::string& aBlockTerminator = "");
   UtInputBlock(const UtInputBlock&) = delete;
   UtInputBlock(UtInputBlock&&)      = default;
   UtInputBlock& operator=(const UtInputBlock&) = delete;
   UtInputBlock& operator=(UtInputBlock&&) = default;
   virtual ~UtInputBlock();

   const std::string& GetCommand();

   //! Return the pointer to the associated UtInput object.
   UtInput& GetInput() const { return mInput; }

   //! Return the block terminator string.
   const std::string& GetBlockTerminator() const { return mBlockTerminator; }

   bool ReadCommand();

   bool ReadCommand(std::string& aCommand);

   //! Explicitly terminate the block.
   //! This can be used if the code also accepts some other block termination condition
   //! other than the defined block terminator. This would typically be followed by a
   //! a 'break;' to exit the block command processing loop.
   void TerminateBlock() { mBlockTerminated = true; }

   //! Read all of the commands in the block and pass them to the ProcessInput() method of an object.
   //! This is a convenience method that implements a commonly used construct.
   template<class T>
   void ProcessInput(T* aObjectPtr)
   {
      while (ReadCommand())
      {
         if (!aObjectPtr->ProcessInput(mInput))
         {
            ThrowUnknownCommandException();
         }
      }
   }

private:
   void ThrowUnknownCommandException();

   UtInput&    mInput;
   std::string mBlockTerminator;
   bool        mBlockTerminated;
};

#endif
