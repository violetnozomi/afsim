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
#ifndef Uci__Type__NIIRS_Type_h
#define Uci__Type__NIIRS_Type_h 1

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates a desired/observed rating level of an image in the National Imagery Interpretability Rating Scale. Scale
        * levels are expressed as digit, dot, digit, like "3.2" with a range 0.0 to 9.9. NIIRS defines "scales" (with levels
        * and associated rating criteria) for images of different types and/or collected in different bands of the
        * electromagnetic spectrum. These scales include Visible NIIRS (visible panchromatic, military focused), Civil NIIRS
        * (visible panchromatic, civil focused), Radar NIIRS (synthetic aperture radar), IR NIIRS (infrared/thermal), MS IIRS
        * (multispectral) and MT IIRS (moving target radar).; [Pattern: "[0-9]\.[0-9]"]
        */
      typedef uci::base::DoubleAccessor NIIRS_Type;
      //  The type of primitive data accessed by NIIRS_Type
      typedef xs::Double NIIRS_TypeValue;

   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__NIIRS_Type_h

