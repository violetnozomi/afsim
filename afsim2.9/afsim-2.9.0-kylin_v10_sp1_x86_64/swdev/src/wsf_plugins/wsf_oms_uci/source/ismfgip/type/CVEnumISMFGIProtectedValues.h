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
#ifndef Ismfgip__Type__CVEnumISMFGIProtectedValues_h
#define Ismfgip__Type__CVEnumISMFGIProtectedValues_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(StdLibMap)
#  include <map>
//  @cond
#  define StdLibMap 1
//  @endcond
#endif

#if !defined(StdLibSstream)
#  include <sstream>
//  @cond
#  define StdLibSstream 1
//  @endcond
#endif

//  
namespace ismfgip {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** (U) FGI followed by all currently valid ISO Trigraphs except USA in alphabetical order by Trigraph, followed by all
        * currently valid CAPCO Coalition tetragraphs in alphabetical order by tetragraph. PERMISSIBLE VALUES The permissible
        * values for this simple type are defined in the Controlled Value Enumeration: CVEnumISMFGIProtected.xml
        */
      class CVEnumISMFGIProtectedValues : public virtual uci::base::Accessor {
      public:

         //  The following are the enumerated items that comprise this enumeration
         enum EnumerationItem {
            //  Enumerated item that indicates that the enumeration has not been set
            enumNotSet,
            //  ISMFGIP_FGI's ordinal value */
            ISMFGIP_FGI,

            //  ISMFGIP_ABW's ordinal value */
            ISMFGIP_ABW,

            //  ISMFGIP_AFG's ordinal value */
            ISMFGIP_AFG,

            //  ISMFGIP_AGO's ordinal value */
            ISMFGIP_AGO,

            //  ISMFGIP_AIA's ordinal value */
            ISMFGIP_AIA,

            //  ISMFGIP_ALA's ordinal value */
            ISMFGIP_ALA,

            //  ISMFGIP_ALB's ordinal value */
            ISMFGIP_ALB,

            //  ISMFGIP_AND's ordinal value */
            ISMFGIP_AND,

            //  ISMFGIP_ARE's ordinal value */
            ISMFGIP_ARE,

            //  ISMFGIP_ARG's ordinal value */
            ISMFGIP_ARG,

            //  ISMFGIP_ARM's ordinal value */
            ISMFGIP_ARM,

            //  ISMFGIP_ASM's ordinal value */
            ISMFGIP_ASM,

            //  ISMFGIP_ATA's ordinal value */
            ISMFGIP_ATA,

            //  ISMFGIP_ATF's ordinal value */
            ISMFGIP_ATF,

            //  ISMFGIP_ATG's ordinal value */
            ISMFGIP_ATG,

            //  ISMFGIP_AUS's ordinal value */
            ISMFGIP_AUS,

            //  ISMFGIP_AUT's ordinal value */
            ISMFGIP_AUT,

            //  ISMFGIP_AZE's ordinal value */
            ISMFGIP_AZE,

            //  ISMFGIP_BDI's ordinal value */
            ISMFGIP_BDI,

            //  ISMFGIP_BEL's ordinal value */
            ISMFGIP_BEL,

            //  ISMFGIP_BEN's ordinal value */
            ISMFGIP_BEN,

            //  ISMFGIP_BES's ordinal value */
            ISMFGIP_BES,

            //  ISMFGIP_BFA's ordinal value */
            ISMFGIP_BFA,

            //  ISMFGIP_BGD's ordinal value */
            ISMFGIP_BGD,

            //  ISMFGIP_BGR's ordinal value */
            ISMFGIP_BGR,

            //  ISMFGIP_BHR's ordinal value */
            ISMFGIP_BHR,

            //  ISMFGIP_BHS's ordinal value */
            ISMFGIP_BHS,

            //  ISMFGIP_BIH's ordinal value */
            ISMFGIP_BIH,

            //  ISMFGIP_BLM's ordinal value */
            ISMFGIP_BLM,

            //  ISMFGIP_BLR's ordinal value */
            ISMFGIP_BLR,

            //  ISMFGIP_BLZ's ordinal value */
            ISMFGIP_BLZ,

            //  ISMFGIP_BMU's ordinal value */
            ISMFGIP_BMU,

            //  ISMFGIP_BOL's ordinal value */
            ISMFGIP_BOL,

            //  ISMFGIP_BRA's ordinal value */
            ISMFGIP_BRA,

            //  ISMFGIP_BRB's ordinal value */
            ISMFGIP_BRB,

            //  ISMFGIP_BRN's ordinal value */
            ISMFGIP_BRN,

            //  ISMFGIP_BTN's ordinal value */
            ISMFGIP_BTN,

            //  ISMFGIP_BVT's ordinal value */
            ISMFGIP_BVT,

            //  ISMFGIP_BWA's ordinal value */
            ISMFGIP_BWA,

            //  ISMFGIP_CAF's ordinal value */
            ISMFGIP_CAF,

            //  ISMFGIP_CAN's ordinal value */
            ISMFGIP_CAN,

            //  ISMFGIP_CCK's ordinal value */
            ISMFGIP_CCK,

            //  ISMFGIP_CHE's ordinal value */
            ISMFGIP_CHE,

            //  ISMFGIP_CHL's ordinal value */
            ISMFGIP_CHL,

            //  ISMFGIP_CHN's ordinal value */
            ISMFGIP_CHN,

            //  ISMFGIP_CIV's ordinal value */
            ISMFGIP_CIV,

            //  ISMFGIP_CMR's ordinal value */
            ISMFGIP_CMR,

            //  ISMFGIP_COD's ordinal value */
            ISMFGIP_COD,

            //  ISMFGIP_COG's ordinal value */
            ISMFGIP_COG,

            //  ISMFGIP_COK's ordinal value */
            ISMFGIP_COK,

            //  ISMFGIP_COL's ordinal value */
            ISMFGIP_COL,

            //  ISMFGIP_COM's ordinal value */
            ISMFGIP_COM,

            //  ISMFGIP_CPV's ordinal value */
            ISMFGIP_CPV,

            //  ISMFGIP_CRI's ordinal value */
            ISMFGIP_CRI,

            //  ISMFGIP_CUB's ordinal value */
            ISMFGIP_CUB,

            //  ISMFGIP_CUW's ordinal value */
            ISMFGIP_CUW,

            //  ISMFGIP_CXR's ordinal value */
            ISMFGIP_CXR,

            //  ISMFGIP_CYM's ordinal value */
            ISMFGIP_CYM,

            //  ISMFGIP_CYP's ordinal value */
            ISMFGIP_CYP,

            //  ISMFGIP_CZE's ordinal value */
            ISMFGIP_CZE,

            //  ISMFGIP_DEU's ordinal value */
            ISMFGIP_DEU,

            //  ISMFGIP_DJI's ordinal value */
            ISMFGIP_DJI,

            //  ISMFGIP_DMA's ordinal value */
            ISMFGIP_DMA,

            //  ISMFGIP_DNK's ordinal value */
            ISMFGIP_DNK,

            //  ISMFGIP_DOM's ordinal value */
            ISMFGIP_DOM,

            //  ISMFGIP_DZA's ordinal value */
            ISMFGIP_DZA,

            //  ISMFGIP_ECU's ordinal value */
            ISMFGIP_ECU,

            //  ISMFGIP_EGY's ordinal value */
            ISMFGIP_EGY,

            //  ISMFGIP_ERI's ordinal value */
            ISMFGIP_ERI,

            //  ISMFGIP_ESH's ordinal value */
            ISMFGIP_ESH,

            //  ISMFGIP_ESP's ordinal value */
            ISMFGIP_ESP,

            //  ISMFGIP_EST's ordinal value */
            ISMFGIP_EST,

            //  ISMFGIP_ETH's ordinal value */
            ISMFGIP_ETH,

            //  ISMFGIP_FIN's ordinal value */
            ISMFGIP_FIN,

            //  ISMFGIP_FJI's ordinal value */
            ISMFGIP_FJI,

            //  ISMFGIP_FLK's ordinal value */
            ISMFGIP_FLK,

            //  ISMFGIP_FRA's ordinal value */
            ISMFGIP_FRA,

            //  ISMFGIP_FRO's ordinal value */
            ISMFGIP_FRO,

            //  ISMFGIP_FSM's ordinal value */
            ISMFGIP_FSM,

            //  ISMFGIP_GAB's ordinal value */
            ISMFGIP_GAB,

            //  ISMFGIP_GBR's ordinal value */
            ISMFGIP_GBR,

            //  ISMFGIP_GEO's ordinal value */
            ISMFGIP_GEO,

            //  ISMFGIP_GGY's ordinal value */
            ISMFGIP_GGY,

            //  ISMFGIP_GHA's ordinal value */
            ISMFGIP_GHA,

            //  ISMFGIP_GIB's ordinal value */
            ISMFGIP_GIB,

            //  ISMFGIP_GIN's ordinal value */
            ISMFGIP_GIN,

            //  ISMFGIP_GLP's ordinal value */
            ISMFGIP_GLP,

            //  ISMFGIP_GMB's ordinal value */
            ISMFGIP_GMB,

            //  ISMFGIP_GNB's ordinal value */
            ISMFGIP_GNB,

            //  ISMFGIP_GNQ's ordinal value */
            ISMFGIP_GNQ,

            //  ISMFGIP_GRC's ordinal value */
            ISMFGIP_GRC,

            //  ISMFGIP_GRD's ordinal value */
            ISMFGIP_GRD,

            //  ISMFGIP_GRL's ordinal value */
            ISMFGIP_GRL,

            //  ISMFGIP_GTM's ordinal value */
            ISMFGIP_GTM,

            //  ISMFGIP_GUF's ordinal value */
            ISMFGIP_GUF,

            //  ISMFGIP_GUM's ordinal value */
            ISMFGIP_GUM,

            //  ISMFGIP_GUY's ordinal value */
            ISMFGIP_GUY,

            //  ISMFGIP_HKG's ordinal value */
            ISMFGIP_HKG,

            //  ISMFGIP_HMD's ordinal value */
            ISMFGIP_HMD,

            //  ISMFGIP_HND's ordinal value */
            ISMFGIP_HND,

            //  ISMFGIP_HRV's ordinal value */
            ISMFGIP_HRV,

            //  ISMFGIP_HTI's ordinal value */
            ISMFGIP_HTI,

            //  ISMFGIP_HUN's ordinal value */
            ISMFGIP_HUN,

            //  ISMFGIP_IDN's ordinal value */
            ISMFGIP_IDN,

            //  ISMFGIP_IMN's ordinal value */
            ISMFGIP_IMN,

            //  ISMFGIP_IND's ordinal value */
            ISMFGIP_IND,

            //  ISMFGIP_IOT's ordinal value */
            ISMFGIP_IOT,

            //  ISMFGIP_IRL's ordinal value */
            ISMFGIP_IRL,

            //  ISMFGIP_IRN's ordinal value */
            ISMFGIP_IRN,

            //  ISMFGIP_IRQ's ordinal value */
            ISMFGIP_IRQ,

            //  ISMFGIP_ISL's ordinal value */
            ISMFGIP_ISL,

            //  ISMFGIP_ISR's ordinal value */
            ISMFGIP_ISR,

            //  ISMFGIP_ITA's ordinal value */
            ISMFGIP_ITA,

            //  ISMFGIP_JAM's ordinal value */
            ISMFGIP_JAM,

            //  ISMFGIP_JEY's ordinal value */
            ISMFGIP_JEY,

            //  ISMFGIP_JOR's ordinal value */
            ISMFGIP_JOR,

            //  ISMFGIP_JPN's ordinal value */
            ISMFGIP_JPN,

            //  ISMFGIP_KAZ's ordinal value */
            ISMFGIP_KAZ,

            //  ISMFGIP_KEN's ordinal value */
            ISMFGIP_KEN,

            //  ISMFGIP_KGZ's ordinal value */
            ISMFGIP_KGZ,

            //  ISMFGIP_KHM's ordinal value */
            ISMFGIP_KHM,

            //  ISMFGIP_KIR's ordinal value */
            ISMFGIP_KIR,

            //  ISMFGIP_KNA's ordinal value */
            ISMFGIP_KNA,

            //  ISMFGIP_KOR's ordinal value */
            ISMFGIP_KOR,

            //  ISMFGIP_KWT's ordinal value */
            ISMFGIP_KWT,

            //  ISMFGIP_LAO's ordinal value */
            ISMFGIP_LAO,

            //  ISMFGIP_LBN's ordinal value */
            ISMFGIP_LBN,

            //  ISMFGIP_LBR's ordinal value */
            ISMFGIP_LBR,

            //  ISMFGIP_LBY's ordinal value */
            ISMFGIP_LBY,

            //  ISMFGIP_LCA's ordinal value */
            ISMFGIP_LCA,

            //  ISMFGIP_LIE's ordinal value */
            ISMFGIP_LIE,

            //  ISMFGIP_LKA's ordinal value */
            ISMFGIP_LKA,

            //  ISMFGIP_LSO's ordinal value */
            ISMFGIP_LSO,

            //  ISMFGIP_LTU's ordinal value */
            ISMFGIP_LTU,

            //  ISMFGIP_LUX's ordinal value */
            ISMFGIP_LUX,

            //  ISMFGIP_LVA's ordinal value */
            ISMFGIP_LVA,

            //  ISMFGIP_MAC's ordinal value */
            ISMFGIP_MAC,

            //  ISMFGIP_MAF's ordinal value */
            ISMFGIP_MAF,

            //  ISMFGIP_MAR's ordinal value */
            ISMFGIP_MAR,

            //  ISMFGIP_MCO's ordinal value */
            ISMFGIP_MCO,

            //  ISMFGIP_MDA's ordinal value */
            ISMFGIP_MDA,

            //  ISMFGIP_MDG's ordinal value */
            ISMFGIP_MDG,

            //  ISMFGIP_MDV's ordinal value */
            ISMFGIP_MDV,

            //  ISMFGIP_MEX's ordinal value */
            ISMFGIP_MEX,

            //  ISMFGIP_MHL's ordinal value */
            ISMFGIP_MHL,

            //  ISMFGIP_MKD's ordinal value */
            ISMFGIP_MKD,

            //  ISMFGIP_MLI's ordinal value */
            ISMFGIP_MLI,

            //  ISMFGIP_MLT's ordinal value */
            ISMFGIP_MLT,

            //  ISMFGIP_MMR's ordinal value */
            ISMFGIP_MMR,

            //  ISMFGIP_MNE's ordinal value */
            ISMFGIP_MNE,

            //  ISMFGIP_MNG's ordinal value */
            ISMFGIP_MNG,

            //  ISMFGIP_MNP's ordinal value */
            ISMFGIP_MNP,

            //  ISMFGIP_MOZ's ordinal value */
            ISMFGIP_MOZ,

            //  ISMFGIP_MRT's ordinal value */
            ISMFGIP_MRT,

            //  ISMFGIP_MSR's ordinal value */
            ISMFGIP_MSR,

            //  ISMFGIP_MTQ's ordinal value */
            ISMFGIP_MTQ,

            //  ISMFGIP_MUS's ordinal value */
            ISMFGIP_MUS,

            //  ISMFGIP_MWI's ordinal value */
            ISMFGIP_MWI,

            //  ISMFGIP_MYS's ordinal value */
            ISMFGIP_MYS,

            //  ISMFGIP_MYT's ordinal value */
            ISMFGIP_MYT,

            //  ISMFGIP_NAM's ordinal value */
            ISMFGIP_NAM,

            //  ISMFGIP_NCL's ordinal value */
            ISMFGIP_NCL,

            //  ISMFGIP_NER's ordinal value */
            ISMFGIP_NER,

            //  ISMFGIP_NFK's ordinal value */
            ISMFGIP_NFK,

            //  ISMFGIP_NGA's ordinal value */
            ISMFGIP_NGA,

            //  ISMFGIP_NIC's ordinal value */
            ISMFGIP_NIC,

            //  ISMFGIP_NIU's ordinal value */
            ISMFGIP_NIU,

            //  ISMFGIP_NLD's ordinal value */
            ISMFGIP_NLD,

            //  ISMFGIP_NOR's ordinal value */
            ISMFGIP_NOR,

            //  ISMFGIP_NPL's ordinal value */
            ISMFGIP_NPL,

            //  ISMFGIP_NRU's ordinal value */
            ISMFGIP_NRU,

            //  ISMFGIP_NZL's ordinal value */
            ISMFGIP_NZL,

            //  ISMFGIP_OMN's ordinal value */
            ISMFGIP_OMN,

            //  ISMFGIP_PAK's ordinal value */
            ISMFGIP_PAK,

            //  ISMFGIP_PAN's ordinal value */
            ISMFGIP_PAN,

            //  ISMFGIP_PCN's ordinal value */
            ISMFGIP_PCN,

            //  ISMFGIP_PER's ordinal value */
            ISMFGIP_PER,

            //  ISMFGIP_PHL's ordinal value */
            ISMFGIP_PHL,

            //  ISMFGIP_PLW's ordinal value */
            ISMFGIP_PLW,

            //  ISMFGIP_PNG's ordinal value */
            ISMFGIP_PNG,

            //  ISMFGIP_POL's ordinal value */
            ISMFGIP_POL,

            //  ISMFGIP_PRI's ordinal value */
            ISMFGIP_PRI,

            //  ISMFGIP_PRK's ordinal value */
            ISMFGIP_PRK,

            //  ISMFGIP_PRT's ordinal value */
            ISMFGIP_PRT,

            //  ISMFGIP_PRY's ordinal value */
            ISMFGIP_PRY,

            //  ISMFGIP_PSE's ordinal value */
            ISMFGIP_PSE,

            //  ISMFGIP_PYF's ordinal value */
            ISMFGIP_PYF,

            //  ISMFGIP_QAT's ordinal value */
            ISMFGIP_QAT,

            //  ISMFGIP_REU's ordinal value */
            ISMFGIP_REU,

            //  ISMFGIP_ROU's ordinal value */
            ISMFGIP_ROU,

            //  ISMFGIP_RUS's ordinal value */
            ISMFGIP_RUS,

            //  ISMFGIP_RWA's ordinal value */
            ISMFGIP_RWA,

            //  ISMFGIP_SAU's ordinal value */
            ISMFGIP_SAU,

            //  ISMFGIP_SDN's ordinal value */
            ISMFGIP_SDN,

            //  ISMFGIP_SEN's ordinal value */
            ISMFGIP_SEN,

            //  ISMFGIP_SGP's ordinal value */
            ISMFGIP_SGP,

            //  ISMFGIP_SGS's ordinal value */
            ISMFGIP_SGS,

            //  ISMFGIP_SHN's ordinal value */
            ISMFGIP_SHN,

            //  ISMFGIP_SJM's ordinal value */
            ISMFGIP_SJM,

            //  ISMFGIP_SLB's ordinal value */
            ISMFGIP_SLB,

            //  ISMFGIP_SLE's ordinal value */
            ISMFGIP_SLE,

            //  ISMFGIP_SLV's ordinal value */
            ISMFGIP_SLV,

            //  ISMFGIP_SMR's ordinal value */
            ISMFGIP_SMR,

            //  ISMFGIP_SOM's ordinal value */
            ISMFGIP_SOM,

            //  ISMFGIP_SPM's ordinal value */
            ISMFGIP_SPM,

            //  ISMFGIP_SRB's ordinal value */
            ISMFGIP_SRB,

            //  ISMFGIP_SSD's ordinal value */
            ISMFGIP_SSD,

            //  ISMFGIP_STP's ordinal value */
            ISMFGIP_STP,

            //  ISMFGIP_SUR's ordinal value */
            ISMFGIP_SUR,

            //  ISMFGIP_SVK's ordinal value */
            ISMFGIP_SVK,

            //  ISMFGIP_SVN's ordinal value */
            ISMFGIP_SVN,

            //  ISMFGIP_SWE's ordinal value */
            ISMFGIP_SWE,

            //  ISMFGIP_SWZ's ordinal value */
            ISMFGIP_SWZ,

            //  ISMFGIP_SXM's ordinal value */
            ISMFGIP_SXM,

            //  ISMFGIP_SYC's ordinal value */
            ISMFGIP_SYC,

            //  ISMFGIP_SYR's ordinal value */
            ISMFGIP_SYR,

            //  ISMFGIP_TCA's ordinal value */
            ISMFGIP_TCA,

            //  ISMFGIP_TCD's ordinal value */
            ISMFGIP_TCD,

            //  ISMFGIP_TGO's ordinal value */
            ISMFGIP_TGO,

            //  ISMFGIP_THA's ordinal value */
            ISMFGIP_THA,

            //  ISMFGIP_TJK's ordinal value */
            ISMFGIP_TJK,

            //  ISMFGIP_TKL's ordinal value */
            ISMFGIP_TKL,

            //  ISMFGIP_TKM's ordinal value */
            ISMFGIP_TKM,

            //  ISMFGIP_TLS's ordinal value */
            ISMFGIP_TLS,

            //  ISMFGIP_TON's ordinal value */
            ISMFGIP_TON,

            //  ISMFGIP_TTO's ordinal value */
            ISMFGIP_TTO,

            //  ISMFGIP_TUN's ordinal value */
            ISMFGIP_TUN,

            //  ISMFGIP_TUR's ordinal value */
            ISMFGIP_TUR,

            //  ISMFGIP_TUV's ordinal value */
            ISMFGIP_TUV,

            //  ISMFGIP_TWN's ordinal value */
            ISMFGIP_TWN,

            //  ISMFGIP_TZA's ordinal value */
            ISMFGIP_TZA,

            //  ISMFGIP_UGA's ordinal value */
            ISMFGIP_UGA,

            //  ISMFGIP_UKR's ordinal value */
            ISMFGIP_UKR,

            //  ISMFGIP_UMI's ordinal value */
            ISMFGIP_UMI,

            //  ISMFGIP_URY's ordinal value */
            ISMFGIP_URY,

            //  ISMFGIP_UZB's ordinal value */
            ISMFGIP_UZB,

            //  ISMFGIP_VAT's ordinal value */
            ISMFGIP_VAT,

            //  ISMFGIP_VCT's ordinal value */
            ISMFGIP_VCT,

            //  ISMFGIP_VEN's ordinal value */
            ISMFGIP_VEN,

            //  ISMFGIP_VGB's ordinal value */
            ISMFGIP_VGB,

            //  ISMFGIP_VIR's ordinal value */
            ISMFGIP_VIR,

            //  ISMFGIP_VNM's ordinal value */
            ISMFGIP_VNM,

            //  ISMFGIP_VUT's ordinal value */
            ISMFGIP_VUT,

            //  ISMFGIP_WLF's ordinal value */
            ISMFGIP_WLF,

            //  ISMFGIP_WSM's ordinal value */
            ISMFGIP_WSM,

            //  ISMFGIP_YEM's ordinal value */
            ISMFGIP_YEM,

            //  ISMFGIP_ZAF's ordinal value */
            ISMFGIP_ZAF,

            //  ISMFGIP_ZMB's ordinal value */
            ISMFGIP_ZMB,

            //  ISMFGIP_ZWE's ordinal value */
            ISMFGIP_ZWE,

            //  ISMFGIP_ACGU's ordinal value */
            ISMFGIP_ACGU,

            //  ISMFGIP_APFS's ordinal value */
            ISMFGIP_APFS,

            //  ISMFGIP_BWCS's ordinal value */
            ISMFGIP_BWCS,

            //  ISMFGIP_CFCK's ordinal value */
            ISMFGIP_CFCK,

            //  ISMFGIP_CFUD's ordinal value */
            ISMFGIP_CFUD,

            //  ISMFGIP_CMFC's ordinal value */
            ISMFGIP_CMFC,

            //  ISMFGIP_CMFP's ordinal value */
            ISMFGIP_CMFP,

            //  ISMFGIP_CPMT's ordinal value */
            ISMFGIP_CPMT,

            //  ISMFGIP_CWCS's ordinal value */
            ISMFGIP_CWCS,

            //  ISMFGIP_EFOR's ordinal value */
            ISMFGIP_EFOR,

            //  ISMFGIP_EUDA's ordinal value */
            ISMFGIP_EUDA,

            //  ISMFGIP_FVEY's ordinal value */
            ISMFGIP_FVEY,

            //  ISMFGIP_GCTF's ordinal value */
            ISMFGIP_GCTF,

            //  ISMFGIP_GMIF's ordinal value */
            ISMFGIP_GMIF,

            //  ISMFGIP_IESC's ordinal value */
            ISMFGIP_IESC,

            //  ISMFGIP_ISAF's ordinal value */
            ISMFGIP_ISAF,

            //  ISMFGIP_KFOR's ordinal value */
            ISMFGIP_KFOR,

            //  ISMFGIP_MIFH's ordinal value */
            ISMFGIP_MIFH,

            //  ISMFGIP_MLEC's ordinal value */
            ISMFGIP_MLEC,

            //  ISMFGIP_NACT's ordinal value */
            ISMFGIP_NACT,

            //  ISMFGIP_NATO's ordinal value */
            ISMFGIP_NATO,

            //  ISMFGIP_OSTY's ordinal value */
            ISMFGIP_OSTY,

            //  ISMFGIP_SPAA's ordinal value */
            ISMFGIP_SPAA,

            //  ISMFGIP_TEYE's ordinal value */
            ISMFGIP_TEYE,

            //  ISMFGIP_UNCK's ordinal value */
            ISMFGIP_UNCK,

            //  Enumerated item that indicates maximum enumerated item exclusive of this item
            enumMaxExclusive
         };


         /** The destructor */
         virtual ~CVEnumISMFGIProtectedValues()
         { }

         /** Returns this accessor's type constant, i.e. cVEnumISMFGIProtectedValues
           *
           * @return This accessor's type constant, i.e. cVEnumISMFGIProtectedValues
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return ismfgip::type::accessorType::cVEnumISMFGIProtectedValues;
         }


         /** The assignment operator. Sets the contents of this CVEnumISMFGIProtectedValues to the contents of the
           * CVEnumISMFGIProtectedValues on the right hand side (rhs) of the assignment operator.CVEnumISMFGIProtectedValues
           *
           * @param rhs The CVEnumISMFGIProtectedValues on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this CVEnumISMFGIProtectedValues
           * @return A constant reference to this CVEnumISMFGIProtectedValues.
           */
         const CVEnumISMFGIProtectedValues& operator=(const CVEnumISMFGIProtectedValues& rhs)
         {
            setValue(rhs.getValue());
            return *this;
         }


         /** The assignment operator. Sets the value of this CVEnumISMFGIProtectedValues enumeration to the EnumerationItem on the
           * right hand side of the assignment operator.
           *
           * @param rhs The EnumerationItemwhose value is to be assigned to this CVEnumISMFGIProtectedValues enumeration
           * @return A reference to this CVEnumISMFGIProtectedValues enumeration
           */
         const CVEnumISMFGIProtectedValues& operator=(EnumerationItem rhs)
         {
            setValue(rhs);
            return *this;
         }


         /** Sets the value of this CVEnumISMFGIProtectedValues enumeration to the value of the specified enumeration item.
           *
           * @param item The EnumerationItemwhose value is to be used to set the value of this CVEnumISMFGIProtectedValues
           *      enumeration
           */
         virtual void setValue(EnumerationItem item)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of this CVEnumISMFGIProtectedValues enumeration.
           *
           * @param testForValidity=true Specifies whether this CVEnumISMFGIProtectedValues enumeration should be validated (true)
           *      or not (false) as part of retrieving its value.
           * @return The value of this CVEnumISMFGIProtectedValues enumeration.
           */
         virtual EnumerationItem getValue(bool testForValidity=true) const
            throw(uci::base::UCIException) = 0;


         /** Returns the number of items in this CVEnumISMFGIProtectedValues enumeration.
           *
           * @return The number of items in this CVEnumISMFGIProtectedValues enumeration.
           */
         int getNumberOfItems() const
         {
            return 274;
         }


         /** Returns whether this CVEnumISMFGIProtectedValues enumeration is valid or not. A CVEnumISMFGIProtectedValues
           * enumeration is valid if its value is one of the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @return A flag indicating whether this CVEnumISMFGIProtectedValues enumeration is valid (true) or not (false).
           */
         virtual bool isValid() const
         {
            return isValid(getValue(false));
         }


         /** Returns whether the specified enumeration item is valid or not. A enumeration item is valid if its value is one of
           * the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @param item The EnumerationItem to test
           * @return A flag indicating whether the specified EnumerationItem is valid (true) or not (false).
           */
         static bool isValid(EnumerationItem item)
         {
            return (item > enumNotSet) && (item < enumMaxExclusive);
         }


         /** Returns whether the specified name item is valid or not. A name is valid if it is the name of one of the enumeration
           * items other than enumNotSet and enumMaxExclusive.
           *
           * @param name The name to test
           * @return A flag indicating whether the specified name is valid (true) or not (false).
           */
         static bool isValid(const std::string& name)
            throw(uci::base::UCIException)
         {
            initializeTables();
            FromNameTable::const_iterator iter = m_fromNameTable.find(name);
            return !(iter == m_fromNameTable.end());
         }


         /** Tests whether the value of the CVEnumISMFGIProtectedValues enumeration on the right hand side of the == operator is
           * equal to the value as this CVEnumISMFGIProtectedValues enumeration.
           *
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIProtectedValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIProtectedValues enumeration (rhs) is equal to the value of
           *      this CVEnumISMFGIProtectedValues enumeration, false otherwise
           */
         bool operator==(const CVEnumISMFGIProtectedValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() == rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMFGIProtectedValues enumeration on the right hand side of the != operator is
           * not equal to the value as this CVEnumISMFGIProtectedValues enumeration.
           *
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIProtectedValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIProtectedValues enumeration (rhs) is not equal to the value of
           *      this CVEnumISMFGIProtectedValues enumeration, false otherwise
           */
         bool operator!=(const CVEnumISMFGIProtectedValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() != rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMFGIProtectedValues enumeration on the right hand side of the < operator is
           * less than the value as this CVEnumISMFGIProtectedValues enumeration.
           *
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIProtectedValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIProtectedValues enumeration (rhs) is less than the value of
           *      this CVEnumISMFGIProtectedValues enumeration, false otherwise
           */
         bool operator<(const CVEnumISMFGIProtectedValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() < rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMFGIProtectedValues enumeration on the right hand side of the <= operator is
           * less than or equal to the value as this CVEnumISMFGIProtectedValues enumeration.
           *
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIProtectedValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIProtectedValues enumeration (rhs) is less than or equal to the
           *      value of this CVEnumISMFGIProtectedValues enumeration, false otherwise
           */
         bool operator<=(const CVEnumISMFGIProtectedValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() <= rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMFGIProtectedValues enumeration on the right hand side of the > operator is
           * greater than the value as this CVEnumISMFGIProtectedValues enumeration.
           *
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIProtectedValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIProtectedValues enumeration (rhs) is greater than the value of
           *      this CVEnumISMFGIProtectedValues enumeration, false otherwise
           */
         bool operator>(const CVEnumISMFGIProtectedValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() > rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMFGIProtectedValues enumeration on the right hand side of the >= operator is
           * greater than or equal to the value as this CVEnumISMFGIProtectedValues enumeration.
           *
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIProtectedValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIProtectedValues enumeration (rhs) is greater than or equal to
           *      the value of this CVEnumISMFGIProtectedValues enumeration, false otherwise
           */
         bool operator>=(const CVEnumISMFGIProtectedValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() >= rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIProtectedValues enumeration is equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIProtectedValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is equal to the value of this CVEnumISMFGIProtectedValues
           *      enumeration, false otherwise
           */
         bool operator==(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() == rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified
           * CVEnumISMFGIProtectedValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIProtectedValues enumeration.
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is equal to the value of the CVEnumISMFGIProtectedValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator==(EnumerationItem lhs, const CVEnumISMFGIProtectedValues& rhs)
         {
            testForValidItem(lhs);
            return lhs == rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIProtectedValues enumeration is not equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIProtectedValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this CVEnumISMFGIProtectedValues
           *      enumeration, false otherwise
           */
         bool operator!=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() != rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified
           * CVEnumISMFGIProtectedValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIProtectedValues enumeration.
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the CVEnumISMFGIProtectedValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator!=(EnumerationItem lhs, const CVEnumISMFGIProtectedValues& rhs)
         {
            testForValidItem(lhs);
            return lhs != rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIProtectedValues enumeration is less than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIProtectedValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than the value of this CVEnumISMFGIProtectedValues
           *      enumeration, false otherwise
           */
         bool operator<(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() < rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than the value of the specified
           * CVEnumISMFGIProtectedValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIProtectedValues enumeration.
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than the value of the CVEnumISMFGIProtectedValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator<(EnumerationItem lhs, const CVEnumISMFGIProtectedValues& rhs)
         {
            testForValidItem(lhs);
            return lhs < rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIProtectedValues enumeration is less than or equal to the value of the
           * specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIProtectedValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this
           *      CVEnumISMFGIProtectedValues enumeration, false otherwise
           */
         bool operator<=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() <= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
           * CVEnumISMFGIProtectedValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIProtectedValues enumeration.
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the
           *      CVEnumISMFGIProtectedValues enumeration (rhs), false otherwise
           */
         friend bool operator<=(EnumerationItem lhs, const CVEnumISMFGIProtectedValues& rhs)
         {
            testForValidItem(lhs);
            return lhs <= rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIProtectedValues enumeration is greater than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIProtectedValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than the value of this CVEnumISMFGIProtectedValues
           *      enumeration, false otherwise
           */
         bool operator>(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() > rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified
           * CVEnumISMFGIProtectedValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIProtectedValues enumeration.
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than the value of the CVEnumISMFGIProtectedValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator>(EnumerationItem lhs, const CVEnumISMFGIProtectedValues& rhs)
         {
            testForValidItem(lhs);
            return lhs > rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIProtectedValues enumeration is greater than or equal to the value of the
           * specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIProtectedValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this
           *      CVEnumISMFGIProtectedValues enumeration, false otherwise
           */
         bool operator>=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() >= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
           * CVEnumISMFGIProtectedValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIProtectedValues enumeration.
           * @param rhs The CVEnumISMFGIProtectedValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the
           *      CVEnumISMFGIProtectedValues enumeration (rhs), false otherwise
           */
         friend bool operator>=(EnumerationItem lhs, const CVEnumISMFGIProtectedValues& rhs)
         {
            testForValidItem(lhs);
            return lhs >= rhs.getValue();
         }


         /** Returns the name of the specified enumeration item.
           *
           * @param item The enumeration item whose name is to be returned.
           * @return The enumeration item's name if valid
           */
         static const std::string& toName(EnumerationItem item)
            throw(uci::base::UCIException)
         {
            initializeTables();

            ToNameTable::const_iterator iter = m_toNameTable.find(item);
            if (iter == m_toNameTable.end()) {
               std::ostringstream oStream;
               oStream << "Failed to convert enumerated item to its name, item not found in mapping: " << item;
               throw uci::base::UCIException(oStream.str(), 0);
            }

            return iter->second;
         }


         /** Returns the enumeration item having the specified name.
           *
           * @param itemName The name of the enumeration item to be returned.
           * @return The enumeration item having the specified name
           */
         static EnumerationItem fromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            initializeTables();

            FromNameTable::const_iterator iter = m_fromNameTable.find(itemName);
            if (iter == m_fromNameTable.end()) {
               std::ostringstream oStream;
               oStream << "Failed to return enumerated item having specified name, name not found in mapping: " << itemName;
               throw uci::base::UCIException(oStream.str(), 0);
            }

            return iter->second;
         }


         /** Return the name of the enumeration item that the value of this CVEnumISMFGIProtectedValues enumeration is set to.
           *
           * @return The name of the enumeration item.
           */
         virtual const std::string& toName() const
            throw(uci::base::UCIException)
         {
            return toName(getValue());
         }


         /** Sets the value of this CVEnumISMFGIProtectedValues enumeration to the value of the enumeration item having the
           * specified name.
           *
           * @param itemName The name of the enumeration item whose value is to be used to set this CVEnumISMFGIProtectedValues
           *      enumeration to
           */
         virtual void setValueFromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            setValue(fromName(itemName));
         }



      protected:

         /** The default constructor. In addition to constructing a new CVEnumISMFGIProtectedValues, this constructor also makes
           * sure the conversion tables are initialized
           */
         CVEnumISMFGIProtectedValues()
         {
            initializeTables();
         }


         /** Tests that the specified enumeration item is valid or not, throwing an exception if the item is not valid.
           *
           * @param additionalDescription An additional description to include in the error message should an exception be thrown
           * @param item The enumeration item to test
           */
         static void testForValidItem(const char* additionalDescription, EnumerationItem item)
            throw(uci::base::UCIException)
         {
            if (!isValid(item)) {
               std::ostringstream oStream;
               oStream << additionalDescription << ": " << item << " (item must be between " << enumNotSet << " and "<< enumMaxExclusive << " exclusive)";
               throw uci::base::UCIException(oStream.str(), 0);
            }

         }


         /** Tests that the specified enumeration item is valid or not, throwing an exception if the item is not valid.
           *
           * @param item The enumeration item to test
           */
         static void testForValidItem(EnumerationItem item)
            throw(uci::base::UCIException)
         {
            testForValidItem("Invalid enumerated item specified", item);
         }



      private:

         //  The type of table that is used to lookup the name of a enumeration item given that enumeration item.
         typedef std::map<EnumerationItem, const std::string> ToNameTable;

         //  The type of table that is used to lookup an enumeration item given its name.
         typedef std::map<const std::string, EnumerationItem> FromNameTable;

         //  The table that is used to lookup the name of a enumeration item given that enumeration item.
         static ToNameTable m_toNameTable;

         //  The type of table that is used to lookup an enumeration item given its name.
         static FromNameTable m_fromNameTable;

         //  Indicates whether the tables has been initialized (true) or not (false).
         static bool m_tablesInitialized;

         //  Initializes the translation tables
         static void initializeTables();


      protected:

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CVEnumISMFGIProtectedValues to copy from
           */
         CVEnumISMFGIProtectedValues(const CVEnumISMFGIProtectedValues& rhs)
         {
            (void)rhs;
         }

      }; // CVEnumISMFGIProtectedValues


   } // Namespace: type
} // Namespace: ismfgip
template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& oStream, const ismfgip::type::CVEnumISMFGIProtectedValues& enumeration)
{
   oStream << enumeration.toName();
   return oStream;
}

#endif // Ismfgip__Type__CVEnumISMFGIProtectedValues_h

