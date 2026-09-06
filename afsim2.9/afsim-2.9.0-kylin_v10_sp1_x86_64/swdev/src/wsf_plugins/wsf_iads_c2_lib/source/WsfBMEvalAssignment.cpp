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
*   File: WsfBMEvalAssignment.cpp
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
*   Abstract:
*
*******************************************************************************/

#include "WsfBMEvalAssignment.hpp"
#include <logger.hpp>
#include <iadsLib/assignmentMessage.hpp>

WsfBMEvalAssignment::WsfBMEvalAssignment(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
   : m_is_valid(false)
   , m_assignment(il::evaluationTrack(aGlobalLogger), nullptr)
   , m_script_class_name("WsfBMEvalAssignment")
   , mGlobalLogger(aGlobalLogger)
{
}

WsfBMEvalAssignment::WsfBMEvalAssignment(const il::evaluationAssignment& assignment, std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
   : m_is_valid(true)
   , m_assignment(assignment)
   , m_script_class_name("WsfBMEvalAssignment")
   , mGlobalLogger(aGlobalLogger)
{
}

WsfBMEvalAssignment* WsfBMEvalAssignment::Clone() const
{
   return new WsfBMEvalAssignment(*this);
}

il::evaluationAssignment& WsfBMEvalAssignment::GetRawAccess()
{
   if (!m_is_valid)
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "WsfBMEvalAssignment::GetRawAccess() assignment is unset!";
   }

   return m_assignment;
}

const char* WsfBMEvalAssignment::GetScriptClassName() const
{
   return m_script_class_name.c_str();
}
