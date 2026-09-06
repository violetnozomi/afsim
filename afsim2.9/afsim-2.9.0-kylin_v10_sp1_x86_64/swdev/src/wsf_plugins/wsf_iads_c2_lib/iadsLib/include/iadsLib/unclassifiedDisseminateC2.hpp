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
*   File: unclassifiedDisseminateC2.hpp
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

#ifndef IADSLIB_UNCLASSIFIED_DISSEMINATE_C2
#define IADSLIB_UNCLASSIFIED_DISSEMINATE_C2

namespace il
{
   class AssetManagerInterface;
}

#include <iadsLib/disseminateC2Interface.hpp>

namespace il
{

   class IADSLIB_EXPORT unclassifiedDisseminateC2 : public DisseminateC2Interface
   {
      // Abstract interface
   public:
      unclassifiedDisseminateC2 ();
      virtual ~unclassifiedDisseminateC2() = default;
      virtual DisseminateC2Interface *Clone ();

   protected:
      unclassifiedDisseminateC2 (const unclassifiedDisseminateC2 &from);
   };

}

#endif

