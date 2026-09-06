// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfBMPluginUtilities.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract: This is a core class implementation for a HELIOS-like C2 dissemination
*    model. It may be used or extended to meet individual IADS needs.
*
*******************************************************************************/

#ifndef WSF_BM_PLUGIN_UTILITIES_HPP
#define WSF_BM_PLUGIN_UTILITIES_HPP

#include <string>
#include <script/WsfScriptManager.hpp>
#include <WsfPlugin.hpp>
#include <WsfApplication.hpp>
#include <WsfBMCommon.hpp>

void RegisterBaseTypes(WsfApplication* aApplicationPtr);
bool WSF_IADS_C2_LIB_EXPORT IsClassTypeRegistered(WsfApplication* aApplicationPtr, const std::string& type_name);
// plugin types don't necessarily register themselves through the script manager. Therefore, to check plugin type registration,
// use this call.
// Class name in this case is the registered object type and type name is the object name registered
extern "C" WSF_IADS_C2_LIB_EXPORT bool IsApplicationExtensionRegistered(WsfApplication* aApplicationPtr, const std::string& name);

#endif
