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
#ifndef Ism__Type__FGIsourceProtected_h
#define Ism__Type__FGIsourceProtected_h 1

#if !defined(Ismfgip__Type__CVEnumISMFGIProtected_h)
# include "ismfgip/type/CVEnumISMFGIProtected.h"
#endif

//  
namespace ism {

   //  The namespace in which all generated data types are declared
   namespace type {

      /**  This attribute is used at both the resource and the portion levels. This attribute has unique specific rules
        * concerning its usage. A single indicator that information qualifies as foreign government information for which the
        * source(s) of the information must be concealed. Within protected internal organizational spaces this element may be
        * used to maintain a record of the one or more indicators identifying information which qualifies as foreign government
        * information for which the source(s) of the information must be concealed. Measures must be taken prior to
        * dissemination of the information to conceal the source(s) of the foreign government information. An indication that
        * information qualifies as foreign government information according to CAPCO guidelines for which the source(s) of the
        * information must be concealed when the information is disseminated in shared spaces This data element has a dual
        * purpose. Within shared spaces, the data element serves only to indicate the presence of information which is
        * categorized as foreign government information according to CAPCO guidelines for which the source(s) of the
        * information is concealed, in which case, this data element's value will always be "FGI". The data element may also be
        * employed in this manner within protected internal organizational spaces. However, within protected internal
        * organizational spaces this data element may alternatively be used to maintain a formal record of the foreign country
        * or countries and/or registered international organization(s) that are the non-disclosable owner(s) and/or producer(s)
        * of information which is categorized as foreign government information according to CAPCO guidelines for which the
        * source(s) of the information must be concealed when the resource is disseminated to shared spaces. If the data
        * element is employed in this manner, then additional measures must be taken prior to dissemination of the resource to
        * shared spaces so that any indications of the non-disclosable owner(s) and/or producer(s) of information within the
        * resource are eliminated. In all cases, the corresponding portion marking or banner marking should be compliant with
        * CAPCO guidelines for FGI when the source must be concealed. In other words, even if the data element is being
        * employed within protected internal organizational spaces to maintain a formal record of the non-disclosable owner(s)
        * and/or producer(s) within an XML resource, if the resource is rendered for display within the protected internal
        * organizational spaces in any format by a stylesheet or as a result of any other transformation process, then the
        * non-disclosable owner(s) and/or producer(s) should not be included in the corresponding portion marking or banner
        * marking. PERMISSIBLE VALUES 1) The value "FGI" is permitted under the circumstances described above. 2) The full set
        * of values are defined in the Controlled Value Enumeration: CVEnumISMFGIProtected.xml ;
        */
      typedef ismfgip::type::CVEnumISMFGIProtected FGIsourceProtected;

   } // Namespace: type
} // Namespace: ism

#endif // Ism__Type__FGIsourceProtected_h

