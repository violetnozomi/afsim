// This file was generated  on 12/5/2018 at 1:1:50 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__UnitNameType_h
#define Uci__Type__UnitNameType_h 1

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Translated unit name or identification given the unit by appropriate authority or orders as used in official orders
        * or communications within the national military or civilian establishment of the country of allegiance. A unit name
        * must be established for every unit in the database. For each Unit logical record, unit naming conventions established
        * in production programs should be employed. If official sources are not available, the unit name believed most correct
        * is used. A unit's primary designation usually includes service speciality and command echelon.; [Pattern: "[a-zA-Z0-9
        * '\(\).,@;+\-]*"]
        */
      typedef uci::type::VisibleString256Type UnitNameType;

   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__UnitNameType_h

