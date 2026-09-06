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
#ifndef Ism__Type__DerivedFrom_h
#define Ism__Type__DerivedFrom_h 1

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

//  
namespace ism {

   //  The namespace in which all generated data types are declared
   namespace type {

      /**  This attribute is used primarily at the resource level. A citation of the authoritative source or reference to
        * multiple sources of the classification markings used in a classified resource. It is manifested only in the 'Derived
        * From' line of a document's classification authority block. ISOO's guidance is: Source of derivative classification.
        * (1) The derivative classifier shall concisely identify the source document or the classification guide on the
        * ‘‘Derived From’’ line, including the agency and, where available, the office of origin, and the date of the source or
        * guide. An example might appear as: Derived From: Memo, ‘‘Funding Problems,’’ October 20, 2008, Office of
        * Administration, Department of Good Works or Derived From: CG No. 1, Department of Good Works, dated October 20, 2008
        * (i) When a document is classified derivatively on the basis of more than one source document or classification guide,
        * the ‘‘Derived From’’ line shall appear as: Derived From: Multiple Sources (ii) The derivative classifier shall
        * include a listing of the source materials on, or attached to, each derivatively classified document. ; ; [Maximum
        * length: 1024]
        */
      typedef xs::String DerivedFrom;

   } // Namespace: type
} // Namespace: ism

#endif // Ism__Type__DerivedFrom_h

