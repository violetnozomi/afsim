// This file was generated  on 12/5/2018 at 1:1:49 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__LaunchNumberType_h
#define Uci__Type__LaunchNumberType_h 1

#if !defined(Uci__Base__UnsignedShortAccessor_h)
# include "uci/base/UnsignedShortAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the object’s launch number for the year it was launched. This is part of the unique designation for an
        * Earth orbiting object assigned by the World Data Center-A for Rockets and Satellites (WDC-A-RS).; [Maximum inclusive:
        * 999] [Minimum inclusive: 1]
        */
      typedef uci::base::UnsignedShortAccessor LaunchNumberType;
      //  The type of primitive data accessed by LaunchNumberType
      typedef xs::UnsignedShort LaunchNumberTypeValue;

   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LaunchNumberType_h

