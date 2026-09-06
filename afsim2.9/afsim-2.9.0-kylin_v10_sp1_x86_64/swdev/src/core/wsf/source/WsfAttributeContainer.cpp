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

#include "WsfAttributeContainer.hpp"

#include <cassert>
#include <map>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputString.hpp"
#include "UtLog.hpp"
#include "UtMatrix.hpp"
#include "UtStringUtil.hpp"

bool WsfAttributeContainer::mPrototypesRegistered = false;

// virtual
//! Process input from a generic source.
//! Input can be of a standard type (bool, int, double);
//! or, it can be a generic "attribute", of a name that has been
//! registered via the WsfAttributeContainer::RegisterPrototype method.
//! See the WSF User Input Guide for more information about valid
//! inputs.
//!   @param aInput The UtInput object from which commands are extracted.
bool WsfAttributeContainer::ProcessInput(UtInput& aInput)
{
   bool        processed = true;
   std::string tag       = aInput.GetCommand();
   try
   {
      if (tag == "bool")
      {
         std::string name;
         std::string value;
         ParseAssignment(aInput, name, value);

         if ((value == "false") || (value.empty()))
         {
            AssignBool(name, false);
         }
         else if (value == "true")
         {
            AssignBool(name, true);
         }
         else
         {
            throw UtInput::BadValue(aInput);
         }
      }
      else if (tag == "double")
      {
         std::string name;
         std::string value;
         ParseAssignment(aInput, name, value);

         double initialValue = 0.0;
         if (!value.empty())
         {
            aInput.PushBack(value);
            aInput.ReadValue(initialValue);
         }
         AssignDouble(name, initialValue);
      }
      else if (tag == "unitary")
      {
         std::string name;
         std::string value;
         double      unconvertedValue;
         ParseAssignment(aInput, name, value);

         if (value.empty())
         {
            // In this case, we must have an initial value,
            // but ParseAssignement returned the null string because there was no '=' sign.
            aInput.ReadValue(unconvertedValue);
         }
         else // ParseAssignment returned an initial value.
         {
            aInput.PushBack(value);
            aInput.ReadValue(unconvertedValue);
         }
         std::string units;
         aInput.ReadValue(units);
         double convertedValue = UtInput::ConvertValueFrom(unconvertedValue, units, aInput.GetValueType(units));
         AssignDouble(name, convertedValue);
      }
      else if (tag == "int")
      {
         std::string name;
         std::string value;
         ParseAssignment(aInput, name, value);

         int initialValue = 0;
         if (!value.empty())
         {
            aInput.PushBack(value);
            aInput.ReadValue(initialValue);
         }
         AssignInt(name, initialValue);
      }
      else if (tag == "string")
      {
         std::string name;
         std::string value;
         ParseAssignment(aInput, name, value);

         if (!value.empty())
         {
            if (value[0] != '"')
            {
               // The string should be in quotes
               throw UtInput::BadValue(aInput, " You must enclose string values in quotes.");
            }
            std::string::size_type quoteLocation = value.find("\"", 1); // find second quotes
            if (value.find("\"", 1) != std::string::npos)
            {
               value = value.substr(1, (quoteLocation - 1));
            }
            else
            {
               // In this case we need to parse the rest of the line.
               std::string command;
               aInput.ReadLine(command, false);       // value constrained to current line.
               quoteLocation = command.find("\"", 1); // find second quotes
               if (command.find("\"", 1) == std::string::npos)
               {
                  throw UtInput::BadValue(aInput, " String values must be enclosed in quotes on a single line.");
               }
               value = value.substr(1, (value.size() - 1));
               value += command.substr(0, quoteLocation);
               command = command.substr((quoteLocation + 1));
               UtStringUtil::TrimWhiteSpace(command);
               if (!command.empty())
               {
                  UtInput cmdStr;
                  cmdStr.PushInput(ut::make_unique<UtInputString>(command));
                  try
                  {
                     while (true)
                     {
                        cmdStr.ReadCommand(command);
                        aInput.PushBack(command);
                     }
                  }
                  catch (UtInput::EndOfData&)
                  {
                  }
               }
            }
         }
         UtStringUtil::TrimWhiteSpace(value);
         AssignString(name, value);
      }
      else if (tag == "attribute") // deprecated
      {
         auto out = ut::log::warning() << "WsfAttributeContainer: Input command 'attribute' is deprecated.";
         out.AddNote() << "Please remove, keeping only the type name.";
         out.Send();
         aInput.ReadCommand(tag); // Advance the input stream past "attribute"
         processed = ProcessAttribute(aInput);
      }
      else if (tag == "struct") // a structure definition.
      {
         const WsfAttributeContainer* structPtr = ProcessStruct(aInput);
         if (structPtr != nullptr)
         {
            mAttributeMap.insert(std::begin(structPtr->mAttributeMap), std::end(structPtr->mAttributeMap));
         }
         else
         {
            processed = false;
         }
      }
      else
      {
         processed = ProcessAttribute(aInput);
      }
   }
   catch (UtInput::ExceptionBase&)
   {
      throw;
   }
   catch (UtException& attributeException)
   {
      // Forward any exceptions from the attribute container as input exceptions
      throw UtInput::BadValue(aInput, attributeException.what());
   }
   return processed;
}

//! Called from the simulation loader, this method processes input file "struct" type declarations.
bool WsfAttributeContainer::LoadType(UtInput& aInput)
{
   bool        processed = false;
   std::string tag       = aInput.GetCommand();

   if (tag == "struct")
   {
      ProcessStruct(aInput);
      processed = true;
   }

   return processed;
}

// static
void WsfAttributeContainer::ClearTypes()
{
   ClearStatic();
   mPrototypesRegistered = false;
}

// static
//! Process data associated with a 'struct' block.
const WsfAttributeContainer* WsfAttributeContainer::ProcessStruct(UtInput& aInput)
{
   UtInputBlock           structBlock(aInput, "end_struct");
   auto                   structPtr = ut::make_unique<WsfAttributeContainer>();
   WsfAttributeContainer* returnPtr = nullptr;

   // The first input is the type name.
   // Read it outside the loop and set it.
   std::string type;
   bool        processed = structBlock.ReadCommand(type);

   if (processed)
   {
      // Type can't already exist
      bool alreadyExists = (UtAttributeContainer::GetPrototype(type) != nullptr);
      if (alreadyExists)
      {
         throw UtInput::OutOfContext(aInput, "WsfAttributeContainer: Can't duplicate structure type name.");
      }

      structPtr->SetType(type);
      while (structBlock.ReadCommand())
      {
         // We can have nested structs.
         if (structBlock.GetCommand() == "struct")
         {
            ProcessStruct(structBlock.GetInput());
         }
         else
         {
            processed = structPtr->ProcessInput(structBlock.GetInput());
            if (!processed)
            {
               throw UtInput::UnknownCommand(structBlock.GetInput());
            }
         }
      }

      returnPtr = structPtr.get();
      RegisterPrototype(type, std::move(structPtr));
      assert(UtAttributeContainer::GetPrototype(type) != nullptr);
   }

   return returnPtr;
}

bool WsfAttributeContainer::ProcessAttribute(UtInput& aInput)
{
   // Read type and name from input.
   std::string type;
   bool        processed = true;
   aInput.GetCommand(type);
   UtAttributeBase* prototypePtr = UtAttributeBase::GetPrototype(type);

   if (prototypePtr != nullptr)
   {
      std::string name;
      aInput.ReadCommand(name);
      auto attributePtr = ut::clone(prototypePtr);
      attributePtr->SetName(name);
      AddAttribute(std::move(attributePtr));
   }
   else
   {
      std::string errorString = "WsfAttributeContainer: " + type + " not registered as a type, or " + type +
                                " not preceded by '=' as initial value.";
      throw UtInput::BadValue(aInput, errorString);
   }
   return processed;
}

//! Process the assignment from the input.
//! Discards the optional '=' character and returns the variable name.
//! @param aInput The UtInput object from which commands are extracted
//! @param aName  The name of the variable being assigned as a string
//! @param aValue [output] The value to be assigned to the variable.
void WsfAttributeContainer::ParseAssignment(UtInput& aInput, std::string& aName, std::string& aValue)
{
   aInput.ReadCommand(aName);
   aInput.ReadCommand(aValue);

   if (aValue == "=")
   {
      // skip
      aInput.ReadCommand(aValue);
   }
   else if ((aValue.find("=") != std::string::npos) || (aName.find("=") != std::string::npos))
   {
      std::string errorString =
         "WsfAttributeContainer: Assignments with '=' must be separated on both sides with whitespace.";
      throw UtInput::BadValue(aInput, errorString);
   }
   else
   {
      aInput.PushBack(aValue);
      aValue = "";
   }
}
