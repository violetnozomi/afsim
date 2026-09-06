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
*   File: weaponsManagerEngageInterface.hpp
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

#ifndef IADSLIB_WEAPONS_MANAGER_ENGAGEMENT_INTERFACE_HPP
#define IADSLIB_WEAPONS_MANAGER_ENGAGEMENT_INTERFACE_HPP



namespace il
{

   class WeaponsManagerInterface;

   class IADSLIB_EXPORT WeaponsManagerEngagementInterface
   {
      // Abstract interface
   public:
      ~WeaponsManagerEngagementInterface() = default;
      virtual void Run (const double sim_time_s) = 0;
   };

}

#endif

