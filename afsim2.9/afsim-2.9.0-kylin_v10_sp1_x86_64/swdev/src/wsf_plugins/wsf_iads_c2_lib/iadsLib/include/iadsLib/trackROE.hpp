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
*   File: trackROE.hpp
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
*   Abstract: Class for abstracting track rules of engagement. All rules default
*  to false (i.e. if not present, will be treated as false).
*
*******************************************************************************/

#ifndef IADSLIB_TRACK_ROE_HPP
#define IADSLIB_TRACK_ROE_HPP

#include <map>
#include <memory>
#include <iadsLib/enum.hpp>


namespace il
{
   class trackRecord;

   class trackROE
   {
   public:
      ~trackROE() = default;
      virtual void SetEngageIffRule (const eIFFType iff, bool value);
      virtual bool IsEngageable (const std::shared_ptr<trackRecord> &track) const;
   protected:
      std::map<eIFFType, bool> m_rules;
   };


}

#endif