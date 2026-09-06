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
*   File: WsfBMEvalAssignment.hpp
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

#ifndef WSF_BM_EVALUATION_ASSIGNMENT_HPP
#define WSF_BM_EVALUATION_ASSIGNMENT_HPP

#include <WsfBMCommon.hpp>
#include <string>

#include <WsfObject.hpp>

#include <iadsLib/assignmentContainers.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfBMEvalAssignment : public WsfObject
{
   public:
      WsfBMEvalAssignment(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger);
      WsfBMEvalAssignment(const il::evaluationAssignment& assignment, std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger);
      WsfBMEvalAssignment* Clone() const override;

      il::evaluationAssignment& GetRawAccess();

      const char* GetScriptClassName() const override;

   protected:
      bool m_is_valid;
      il::evaluationAssignment m_assignment;
      std::string m_script_class_name;
      std::weak_ptr<il::logger::GlobalLogger> mGlobalLogger;
};

#endif
