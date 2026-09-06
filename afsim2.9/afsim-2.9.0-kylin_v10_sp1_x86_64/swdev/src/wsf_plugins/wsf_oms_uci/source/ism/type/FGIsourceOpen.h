// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* ‒  DISTRIBUTION STATEMENT D.  Distribution authorized to the Department
*    of Defense and U.S. DoD contractors only; Critical Technology; 17 Sep 2015.
*    Other requests shall be referred to Air Force Rapid Capabilities Office,
*    Bolling AFB, Washington DC 20032-6400. 
*
* EXPORT CONTROL:
*    WARNING - ITAR CONTROLLED - US ONLY
*    This distribution contains technical data whose export is restricted by
*    the Arms Export Control Act (Title 22, U.S.C., Sec. 2751 et seq. or the
*    Export Administration Act of 1979 as amended Title 50, U.S.C., App.
*    2401-2420 et seq.), as amended. Violation of these export laws are subject
*    to severe criminal penalties.  Disseminate in accordance with provisions of DoD
*    Directive 5230.25.
*/
#ifndef Ism__Type__FGIsourceOpen_h
#define Ism__Type__FGIsourceOpen_h 1

#if !defined(Ismfgio__Type__CVEnumISMFGIOpen_h)
# include "ismfgio/type/CVEnumISMFGIOpen.h"
#endif

//  
namespace ism {

   //  The namespace in which all generated data types are declared
   namespace type {

      /**  This attribute is used at both the resource and the portion levels. One or more indicators identifying information
        * which qualifies as foreign government information for which the source(s) of the information is not concealed. The
        * attribute can indicate that the source of information of foreign origin is unknown. It is manifested in portion marks
        * and security banners. PERMISSIBLE VALUES 1) The value "UNKNOWN" is permitted under the circumstances described above.
        * 2) The full set of values are defined in the Controlled Value Enumeration: CVEnumISMFGIOpen.xml ;
        */
      typedef ismfgio::type::CVEnumISMFGIOpen FGIsourceOpen;

   } // Namespace: type
} // Namespace: ism

#endif // Ism__Type__FGIsourceOpen_h

