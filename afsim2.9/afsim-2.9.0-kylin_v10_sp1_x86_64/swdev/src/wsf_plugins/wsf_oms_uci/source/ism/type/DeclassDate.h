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
#ifndef Ism__Type__DeclassDate_h
#define Ism__Type__DeclassDate_h 1

#if !defined(Uci__Base__DateAccessor_h)
# include "uci/base/DateAccessor.h"
#endif

//  
namespace ism {

   //  The namespace in which all generated data types are declared
   namespace type {

      /**  This attribute is used primarily at the resource level. A specific year, month, and day upon which the information
        * shall be automatically declassified if not properly exempted from automatic declassification. It is manifested in the
        * 'Declassify On' line of a resource's classification authority block. ;
        */
      typedef uci::base::DateAccessor DeclassDate;
      //  The type of primitive data accessed by DeclassDate
      typedef xs::Date DeclassDateValue;

   } // Namespace: type
} // Namespace: ism

#endif // Ism__Type__DeclassDate_h

