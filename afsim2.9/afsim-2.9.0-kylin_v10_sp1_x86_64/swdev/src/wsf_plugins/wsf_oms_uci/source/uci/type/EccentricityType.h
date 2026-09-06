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
#ifndef Uci__Type__EccentricityType_h
#define Uci__Type__EccentricityType_h 1

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the orbit's shape expressed as eccentricity. Eccentricity is a quantity describing the shape of the
        * satellite's orbit. In mathematics the eccentricity of a conic section is a measure of how much the shape deviates
        * from being circular. A value of zero represents a perfect circle. As the eccentricity varies from zero to 1 the
        * ellipse becomes more "stretched". An eccentricity of 1 is a parabolic rather than elliptical, and values greater than
        * 1 are hyperbolas.; [Minimum inclusive: 0.0] [Maximum inclusive: 1.0]
        */
      typedef uci::base::DoubleAccessor EccentricityType;
      //  The type of primitive data accessed by EccentricityType
      typedef xs::Double EccentricityTypeValue;

   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EccentricityType_h

