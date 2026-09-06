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
#ifndef Uci__Type__UnitIdentifierType_h
#define Uci__Type__UnitIdentifierType_h 1

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Unique identifier for each unit. ([A-Z][A-Z] Pos. 1-2, SYSTEM ASSIGNED RECORD ORIGINATOR. Two character code
        * associated with the organization originating the unique UNIT ID. Assigned by system at element creation time.
        * [ABCDEGJMNSX] Pos. 3, OB_TYPE A Air Force B Joint Forces C Civilian D Defensive Missile Forces E Net G Army J Space
        * Order of Battle (SOB) M Ministry, Other Than Ministry of Defense (MOD) N Navy S Strategic Missile Forces X Air
        * Defense Order of Battle (ADOB) [A-Z][A-Z] Pos. 4-5, ALLEGIANCE. This item is selected from the 2 character list of
        * valid State Department Allegiance codes. [00001-99999] Pos. 6-10, ACCESSION_NUMBER.); [Pattern: "[A-Z0-9]{10}"]
        */
      typedef xs::String UnitIdentifierType;

   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__UnitIdentifierType_h

