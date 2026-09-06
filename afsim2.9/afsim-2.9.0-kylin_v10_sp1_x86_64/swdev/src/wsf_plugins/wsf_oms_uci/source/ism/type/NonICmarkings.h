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
#ifndef Ism__Type__NonICmarkings_h
#define Ism__Type__NonICmarkings_h 1

#if !defined(Ismnonic__Type__CVEnumISMNonIC_h)
# include "ismnonic/type/CVEnumISMNonIC.h"
#endif

//  
namespace ism {

   //  The namespace in which all generated data types are declared
   namespace type {

      /**  This attribute is used at both the resource and the portion levels. One or more indicators of the expansion or
        * limitation on the distribution of an information resource or portion within the domain of information originating
        * from non-intelligence components. It is manifested in portion marks and security banners. PERMISSIBLE VALUES The
        * permissible values for this attribute are defined in the Controlled Value Enumeration: CVEnumISMNonIC.xml ;
        */
      typedef ismnonic::type::CVEnumISMNonIC NonICmarkings;

   } // Namespace: type
} // Namespace: ism

#endif // Ism__Type__NonICmarkings_h

