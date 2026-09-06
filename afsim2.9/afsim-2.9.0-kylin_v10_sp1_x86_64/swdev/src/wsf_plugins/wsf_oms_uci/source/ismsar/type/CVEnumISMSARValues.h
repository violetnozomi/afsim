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
#ifndef Ismsar__Type__CVEnumISMSARValues_h
#define Ismsar__Type__CVEnumISMSARValues_h 1

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

//  
namespace ismsar {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** (U) All currently valid SAR controls from the published register PERMISSIBLE VALUES The permissible values for this
        * simple type are defined in the Controlled Value Enumeration: CVEnumISMSAR.xml ; [Pattern: "[A-Z][A-Z][A-Z]?"]
        * [Pattern: "[A-Z]{2,3}-[A-Z][A-Z0-9]{1,2}"] [Pattern: "[A-Z]{2,3}-[A-Z][A-Z0-9]{1,2}-[A-Z0-9]{2,3}"]
        */
      typedef xs::Token CVEnumISMSARValues;

   } // Namespace: type
} // Namespace: ismsar

#endif // Ismsar__Type__CVEnumISMSARValues_h

