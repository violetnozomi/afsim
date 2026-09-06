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
*   File: unclassifiedDisseminateC2.cpp
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

#include <iadsLib/unclassifiedDisseminateC2.hpp>

namespace il
{

unclassifiedDisseminateC2::unclassifiedDisseminateC2 ()
   : DisseminateC2Interface ()
{

}

DisseminateC2Interface *unclassifiedDisseminateC2::Clone ()
{
   return new unclassifiedDisseminateC2 (*this);
}

unclassifiedDisseminateC2::unclassifiedDisseminateC2 (const unclassifiedDisseminateC2 &from)
   : DisseminateC2Interface (from)
{

}

} // namespace il
