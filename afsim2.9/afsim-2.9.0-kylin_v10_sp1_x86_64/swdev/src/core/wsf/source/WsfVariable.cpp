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

#include "WsfVariable.hpp"

#include <sstream>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtScriptContext.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfStringId.hpp"
#include "WsfSystemLog.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"

// =================================================================================================
WsfVariableBase::WsfVariableBase()
   : mRefVarNameId()
   , mHasDefaultValue(false)
{
}

// =================================================================================================
//! Determine if the '/default' option is present.
bool WsfVariableBase::DefaultOptionPresent(UtInput& aInput)
{
   std::string command;
   aInput.TryReadCommand(command);
   if (command == "/default")
   {
      mHasDefaultValue = true;
   }
   else
   {
      aInput.PushBack(command);
      mHasDefaultValue = false;
   }
   return mHasDefaultValue;
}

// =================================================================================================
//! Return the name of the object for messages.
std::string WsfVariableBase::ObjectName(WsfComponent* aObjectPtr)
{
   if (aObjectPtr != nullptr)
   {
      std::string name(aObjectPtr->GetComponentName());
      // Next try to see if it is a component on a platform.
      WsfPlatformComponent* componentPtr = dynamic_cast<WsfPlatformComponent*>(aObjectPtr);
      if (componentPtr != nullptr)
      {
         // This is a platform component, so get its parent. This should be the owning platform.
         WsfPlatform* parentPtr = componentPtr->GetComponentParent();
         if (parentPtr != nullptr)
         {
            name = parentPtr->GetComponentName() + "." + name;
         }
      }
      return name;
   }
   // Must be the simulation.
   return "<simulation>";
}

// =================================================================================================
//! Read a potential reference.
//!
//! This reads the next item from the input stream, and if it is the start of reference, extract the
//! name of the referenced script variable and any other optional data. If the next item is not
//! a reference, the item is pushed back on the input stream for processing by the caller.
//!
//! @returns 'true' if the input was a variable reference. 'false' is returned if it is not a
//! variable reference, in which case the caller should perform a 'ReadValue(OfType)' to read the value.
bool WsfVariableBase::ReadReference(UtInput& aInput)
{
   std::string refVarName;
   aInput.ReadValue(refVarName);
   if (refVarName == "/variable")
   {
      // Indirect reference. Capture the name of the reference variable.
      aInput.ReadValue(refVarName);
      mRefVarNameId = refVarName;
   }
   else
   {
      // Not a indirect reference. Push the value back on the data stream so it can be processed
      // as a normal data value;
      aInput.PushBack(refVarName);
      mRefVarNameId = "";
   }
   return !mRefVarNameId.IsNull();
}

// =================================================================================================
//! If the variable is a reference, determine the script context and index of the reference.
//!
//! @param aVariableName  The name of the variable as known by the user (for system log/error output)
//! @param aObjectPtr     The name of the object containing the variable (for system log/error output)
//! @param aContextPtr    The context from which to start searching to satisfy a reference.
//! @param aRefValuePtr   [output] The script data object that points to the resolved script variable.
//!                       This will be 0 if the variable is not a reference or if the reference
//!                       could not be satisfied.
//! @param aGlobalContext The script manager
//!
//! @returns 'true' if the variable is a reference and it was successfully resolved, or if the
//! variable was not a reference. 'false' is returned if the variable is a reference and it
//! cannot be resolved.
bool WsfVariableBase::ResolveReference(const std::string&   aVariableName,
                                       WsfComponent*        aObjectPtr,
                                       WsfScriptContext*    aContextPtr,
                                       const UtScriptData*& aRefValuePtr,
                                       WsfScriptContext&    aGlobalContext)
{
   bool resolved = true;
   aRefValuePtr  = nullptr;
   if (!mRefVarNameId.IsNull()) // Variable is a reference
   {
      // Starting with the supplied context, work back up the context stack to find the first context
      // that contains the specified variable.
      //
      // If a context was not supplied then start with the global context.

      UtScriptContext* refContextPtr = nullptr;
      if (aContextPtr != nullptr)
      {
         refContextPtr = &(aContextPtr->GetContext());
      }
      else
      {
         refContextPtr = &aGlobalContext.GetContext();
      }
      UtScriptData* refVarPtr = refContextPtr->FindGlobalVar(mRefVarNameId);
      if (refVarPtr != nullptr) // variable successfully resolved
      {
         aRefValuePtr = refVarPtr;
         if (WsfScriptContext::GetSCENARIO(aGlobalContext.GetContext())->GetSystemLog().LoggingActive())
         {
            std::ostringstream oss;
            oss << "variable_reference " << aVariableName << " in " << ObjectName(aObjectPtr);

            // Write the name of the variable and the script context `from which it was resolved.
            oss << " referenced " << mRefVarNameId << " from ";
            if (WsfScriptContext::GetPROCESSOR(*refContextPtr) != nullptr)
            {
               oss << "<processor>";
            }
            else if (WsfScriptContext::GetPLATFORM(*refContextPtr) != nullptr)
            {
               oss << "<platform>";
            }
            else
            {
               oss << "<simulation>";
            }
            oss << "; value: ";

            // Write the value of the variable.
            if (aRefValuePtr != nullptr)
            {
               switch (aRefValuePtr->GetType())
               {
               case ut::script::Data::Type::cBOOL:
               {
                  std::string boolStr = (aRefValuePtr->GetBool()) ? "true" : "false";
                  oss << boolStr;
               }
               break;
               case ut::script::Data::Type::cINT:
                  oss << aRefValuePtr->GetInt();
                  break;
               case ut::script::Data::Type::cDOUBLE:
                  oss << aRefValuePtr->GetDouble();
                  break;
               case ut::script::Data::Type::cSTRING:
                  oss << aRefValuePtr->GetString();
                  break;
               default:
                  oss << "<unknown>";
                  break;
               }
            }
            else
            {
               oss << "<unknown>";
            }
            WsfScriptContext::GetSCENARIO(aGlobalContext.GetContext())->GetSystemLog().WriteLogEntry(oss.str());
         }
      }
      else if (!HasDefaultValue())
      {
         // Reference could not be resolved and a default value does not exist.
         resolved = false;
         auto out = ut::log::error() << "Unable to resolve reference to script variable.";
         out.AddNote() << "Script: " << ObjectName(aObjectPtr);
         out.AddNote() << "Variable: " << aVariableName;
         out.AddNote() << "Reference: " << mRefVarNameId;
      }
   }
   return resolved;
}

// =================================================================================================
// Processing for variables of type 'double'.
// =================================================================================================

// =================================================================================================
bool WsfVariableBase::Initialize(const std::string& aName,
                                 WsfComponent*      aObjectPtr,
                                 WsfScriptContext*  aContextPtr,
                                 double&            aValue,
                                 WsfScriptContext&  aGlobalContext)
{
   const UtScriptData* varDataPtr = nullptr;
   bool                ok         = ResolveReference(aName, aObjectPtr, aContextPtr, varDataPtr, aGlobalContext);
   if (varDataPtr != nullptr)
   {
      aValue = varDataPtr->GetDouble();
   }
   return ok;
}

// =================================================================================================
void WsfVariableBase::ReadValue(UtInput& aInput, double& aValue)
{
   mHasDefaultValue = false;
   if (ReadReference(aInput))
   {
      if (DefaultOptionPresent(aInput))
      {
         aInput.ReadValue(aValue);
      }
   }
   else
   {
      aInput.ReadValue(aValue);
   }
}

// =================================================================================================
void WsfVariableBase::ReadValueOfType(UtInput& aInput, UtInput::ValueType aType, double& aValue)
{
   mHasDefaultValue = false;
   if (ReadReference(aInput))
   {
      if (DefaultOptionPresent(aInput))
      {
         aInput.ReadValueOfType(aValue, aType);
      }
   }
   else
   {
      aInput.ReadValueOfType(aValue, aType);
   }
}

// =================================================================================================
// Processing for variables of type 'float'.
// =================================================================================================

// =================================================================================================
bool WsfVariableBase::Initialize(const std::string& aName,
                                 WsfComponent*      aObjectPtr,
                                 WsfScriptContext*  aContextPtr,
                                 float&             aValue,
                                 WsfScriptContext&  aGlobalContext)
{
   const UtScriptData* varDataPtr = nullptr;
   bool                ok         = ResolveReference(aName, aObjectPtr, aContextPtr, varDataPtr, aGlobalContext);
   if (varDataPtr != nullptr)
   {
      aValue = static_cast<float>(varDataPtr->GetDouble());
   }
   return ok;
}

// =================================================================================================
void WsfVariableBase::ReadValue(UtInput& aInput, float& aValue)
{
   mHasDefaultValue = false;
   if (ReadReference(aInput))
   {
      if (DefaultOptionPresent(aInput))
      {
         aInput.ReadValue(aValue);
      }
   }
   else
   {
      aInput.ReadValue(aValue);
   }
}

// =================================================================================================
void WsfVariableBase::ReadValueOfType(UtInput& aInput, UtInput::ValueType aType, float& aValue)
{
   mHasDefaultValue = false;
   if (ReadReference(aInput))
   {
      if (DefaultOptionPresent(aInput))
      {
         aInput.ReadValueOfType(aValue, aType);
      }
   }
   else
   {
      aInput.ReadValueOfType(aValue, aType);
   }
}

// =================================================================================================
// Processing for variables of type 'int'.
// =================================================================================================

// =================================================================================================
bool WsfVariableBase::Initialize(const std::string& aName,
                                 WsfComponent*      aObjectPtr,
                                 WsfScriptContext*  aContextPtr,
                                 int&               aValue,
                                 WsfScriptContext&  aGlobalContext)
{
   const UtScriptData* varDataPtr = nullptr;
   bool                ok         = ResolveReference(aName, aObjectPtr, aContextPtr, varDataPtr, aGlobalContext);
   if (varDataPtr != nullptr)
   {
      aValue = varDataPtr->GetInt();
   }
   return ok;
}

// =================================================================================================
void WsfVariableBase::ReadValue(UtInput& aInput, int& aValue)
{
   mHasDefaultValue = false;
   if (ReadReference(aInput))
   {
      if (DefaultOptionPresent(aInput))
      {
         aInput.ReadValue(aValue);
      }
   }
   else
   {
      aInput.ReadValue(aValue);
   }
}

// =================================================================================================
void WsfVariableBase::ReadValueOfType(UtInput& aInput, UtInput::ValueType aType, int& aValue)
{
   mHasDefaultValue = false;
   if (ReadReference(aInput))
   {
      if (DefaultOptionPresent(aInput))
      {
         aInput.ReadValueOfType(aValue, aType);
      }
   }
   else
   {
      aInput.ReadValueOfType(aValue, aType);
   }
}

// =================================================================================================
// Processing for variables of type 'bool'.
// =================================================================================================

// =================================================================================================
bool WsfVariableBase::Initialize(const std::string& aName,
                                 WsfComponent*      aObjectPtr,
                                 WsfScriptContext*  aContextPtr,
                                 bool&              aValue,
                                 WsfScriptContext&  aGlobalContext)
{
   const UtScriptData* varDataPtr = nullptr;
   bool                ok         = ResolveReference(aName, aObjectPtr, aContextPtr, varDataPtr, aGlobalContext);
   if (varDataPtr != nullptr)
   {
      aValue = varDataPtr->GetBool();
   }
   return ok;
}

// =================================================================================================
void WsfVariableBase::ReadValue(UtInput& aInput, bool& aValue)
{
   mHasDefaultValue = false;
   if (ReadReference(aInput))
   {
      if (DefaultOptionPresent(aInput))
      {
         aInput.ReadValue(aValue);
      }
   }
   else
   {
      aInput.ReadValue(aValue);
   }
}

// =================================================================================================
// Processing for variables of type 'std::string'.
// =================================================================================================

// =================================================================================================
bool WsfVariableBase::Initialize(const std::string& aName,
                                 WsfComponent*      aObjectPtr,
                                 WsfScriptContext*  aContextPtr,
                                 std::string&       aValue,
                                 WsfScriptContext&  aGlobalContext)
{
   const UtScriptData* varDataPtr = nullptr;
   bool                ok         = ResolveReference(aName, aObjectPtr, aContextPtr, varDataPtr, aGlobalContext);
   if (varDataPtr != nullptr)
   {
      aValue = varDataPtr->GetString();
   }
   return ok;
}

// =================================================================================================
void WsfVariableBase::ReadValue(UtInput& aInput, std::string& aValue)
{
   mHasDefaultValue = false;
   if (ReadReference(aInput))
   {
      if (DefaultOptionPresent(aInput))
      {
         aInput.ReadValue(aValue);
      }
   }
   else
   {
      aInput.ReadValue(aValue);
   }
}

// =================================================================================================
// Processing for variables of type 'WsfStringId'.
// =================================================================================================

// =================================================================================================
bool WsfVariableBase::Initialize(const std::string& aName,
                                 WsfComponent*      aObjectPtr,
                                 WsfScriptContext*  aContextPtr,
                                 WsfStringId&       aValue,
                                 WsfScriptContext&  aGlobalContext)
{
   const UtScriptData* varDataPtr = nullptr;
   bool                ok         = ResolveReference(aName, aObjectPtr, aContextPtr, varDataPtr, aGlobalContext);
   if (varDataPtr != nullptr)
   {
      aValue = varDataPtr->GetString();
   }
   return ok;
}

// =================================================================================================
void WsfVariableBase::ReadValue(UtInput& aInput, WsfStringId& aValue)
{
   mHasDefaultValue = false;
   if (ReadReference(aInput))
   {
      if (DefaultOptionPresent(aInput))
      {
         std::string value;
         aInput.ReadValue(value);
         aValue = value;
      }
   }
   else
   {
      std::string value;
      aInput.ReadValue(value);
      aValue = value;
   }
}

// ================================================================================================
std::ostream& operator<<(std::ostream& aOut, const WsfVariable<std::string>& aVariable)
{
   aOut << aVariable.operator std::string();
   return aOut;
}

// ================================================================================================
std::ostream& operator<<(std::ostream& aOut, const WsfVariable<WsfStringId>& aVariable)
{
   aOut << aVariable.GetString();
   return aOut;
}
