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
#ifndef Ismownpro__Type__CVEnumISMOwnerProducerValues_h
#define Ismownpro__Type__CVEnumISMOwnerProducerValues_h 1

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
namespace ismownpro {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** (U) FGI followed by all currently valid ISO Trigraphs in alphabetical order by Trigraph, followed by all currently
        * valid CAPCO Coalition tetragraphs in alphabetical order by tetragraph. PERMISSIBLE VALUES The permissible values for
        * this simple type are defined in the Controlled Value Enumeration: CVEnumISMOwnerProducer.xml
        */
      class CVEnumISMOwnerProducerValues : public virtual uci::base::Accessor {
      public:

         //  The following are the enumerated items that comprise this enumeration
         enum EnumerationItem {
            //  Enumerated item that indicates that the enumeration has not been set
            enumNotSet,
            //  ISMOWNPRO_FGI's ordinal value */
            ISMOWNPRO_FGI,

            //  ISMOWNPRO_ABW's ordinal value */
            ISMOWNPRO_ABW,

            //  ISMOWNPRO_AFG's ordinal value */
            ISMOWNPRO_AFG,

            //  ISMOWNPRO_AGO's ordinal value */
            ISMOWNPRO_AGO,

            //  ISMOWNPRO_AIA's ordinal value */
            ISMOWNPRO_AIA,

            //  ISMOWNPRO_ALA's ordinal value */
            ISMOWNPRO_ALA,

            //  ISMOWNPRO_ALB's ordinal value */
            ISMOWNPRO_ALB,

            //  ISMOWNPRO_AND's ordinal value */
            ISMOWNPRO_AND,

            //  ISMOWNPRO_ARE's ordinal value */
            ISMOWNPRO_ARE,

            //  ISMOWNPRO_ARG's ordinal value */
            ISMOWNPRO_ARG,

            //  ISMOWNPRO_ARM's ordinal value */
            ISMOWNPRO_ARM,

            //  ISMOWNPRO_ASM's ordinal value */
            ISMOWNPRO_ASM,

            //  ISMOWNPRO_ATA's ordinal value */
            ISMOWNPRO_ATA,

            //  ISMOWNPRO_ATF's ordinal value */
            ISMOWNPRO_ATF,

            //  ISMOWNPRO_ATG's ordinal value */
            ISMOWNPRO_ATG,

            //  ISMOWNPRO_AUS's ordinal value */
            ISMOWNPRO_AUS,

            //  ISMOWNPRO_AUT's ordinal value */
            ISMOWNPRO_AUT,

            //  ISMOWNPRO_AZE's ordinal value */
            ISMOWNPRO_AZE,

            //  ISMOWNPRO_BDI's ordinal value */
            ISMOWNPRO_BDI,

            //  ISMOWNPRO_BEL's ordinal value */
            ISMOWNPRO_BEL,

            //  ISMOWNPRO_BEN's ordinal value */
            ISMOWNPRO_BEN,

            //  ISMOWNPRO_BES's ordinal value */
            ISMOWNPRO_BES,

            //  ISMOWNPRO_BFA's ordinal value */
            ISMOWNPRO_BFA,

            //  ISMOWNPRO_BGD's ordinal value */
            ISMOWNPRO_BGD,

            //  ISMOWNPRO_BGR's ordinal value */
            ISMOWNPRO_BGR,

            //  ISMOWNPRO_BHR's ordinal value */
            ISMOWNPRO_BHR,

            //  ISMOWNPRO_BHS's ordinal value */
            ISMOWNPRO_BHS,

            //  ISMOWNPRO_BIH's ordinal value */
            ISMOWNPRO_BIH,

            //  ISMOWNPRO_BLM's ordinal value */
            ISMOWNPRO_BLM,

            //  ISMOWNPRO_BLR's ordinal value */
            ISMOWNPRO_BLR,

            //  ISMOWNPRO_BLZ's ordinal value */
            ISMOWNPRO_BLZ,

            //  ISMOWNPRO_BMU's ordinal value */
            ISMOWNPRO_BMU,

            //  ISMOWNPRO_BOL's ordinal value */
            ISMOWNPRO_BOL,

            //  ISMOWNPRO_BRA's ordinal value */
            ISMOWNPRO_BRA,

            //  ISMOWNPRO_BRB's ordinal value */
            ISMOWNPRO_BRB,

            //  ISMOWNPRO_BRN's ordinal value */
            ISMOWNPRO_BRN,

            //  ISMOWNPRO_BTN's ordinal value */
            ISMOWNPRO_BTN,

            //  ISMOWNPRO_BVT's ordinal value */
            ISMOWNPRO_BVT,

            //  ISMOWNPRO_BWA's ordinal value */
            ISMOWNPRO_BWA,

            //  ISMOWNPRO_CAF's ordinal value */
            ISMOWNPRO_CAF,

            //  ISMOWNPRO_CAN's ordinal value */
            ISMOWNPRO_CAN,

            //  ISMOWNPRO_CCK's ordinal value */
            ISMOWNPRO_CCK,

            //  ISMOWNPRO_CHE's ordinal value */
            ISMOWNPRO_CHE,

            //  ISMOWNPRO_CHL's ordinal value */
            ISMOWNPRO_CHL,

            //  ISMOWNPRO_CHN's ordinal value */
            ISMOWNPRO_CHN,

            //  ISMOWNPRO_CIV's ordinal value */
            ISMOWNPRO_CIV,

            //  ISMOWNPRO_CMR's ordinal value */
            ISMOWNPRO_CMR,

            //  ISMOWNPRO_COD's ordinal value */
            ISMOWNPRO_COD,

            //  ISMOWNPRO_COG's ordinal value */
            ISMOWNPRO_COG,

            //  ISMOWNPRO_COK's ordinal value */
            ISMOWNPRO_COK,

            //  ISMOWNPRO_COL's ordinal value */
            ISMOWNPRO_COL,

            //  ISMOWNPRO_COM's ordinal value */
            ISMOWNPRO_COM,

            //  ISMOWNPRO_CPV's ordinal value */
            ISMOWNPRO_CPV,

            //  ISMOWNPRO_CRI's ordinal value */
            ISMOWNPRO_CRI,

            //  ISMOWNPRO_CUB's ordinal value */
            ISMOWNPRO_CUB,

            //  ISMOWNPRO_CUW's ordinal value */
            ISMOWNPRO_CUW,

            //  ISMOWNPRO_CXR's ordinal value */
            ISMOWNPRO_CXR,

            //  ISMOWNPRO_CYM's ordinal value */
            ISMOWNPRO_CYM,

            //  ISMOWNPRO_CYP's ordinal value */
            ISMOWNPRO_CYP,

            //  ISMOWNPRO_CZE's ordinal value */
            ISMOWNPRO_CZE,

            //  ISMOWNPRO_DEU's ordinal value */
            ISMOWNPRO_DEU,

            //  ISMOWNPRO_DJI's ordinal value */
            ISMOWNPRO_DJI,

            //  ISMOWNPRO_DMA's ordinal value */
            ISMOWNPRO_DMA,

            //  ISMOWNPRO_DNK's ordinal value */
            ISMOWNPRO_DNK,

            //  ISMOWNPRO_DOM's ordinal value */
            ISMOWNPRO_DOM,

            //  ISMOWNPRO_DZA's ordinal value */
            ISMOWNPRO_DZA,

            //  ISMOWNPRO_ECU's ordinal value */
            ISMOWNPRO_ECU,

            //  ISMOWNPRO_EGY's ordinal value */
            ISMOWNPRO_EGY,

            //  ISMOWNPRO_ERI's ordinal value */
            ISMOWNPRO_ERI,

            //  ISMOWNPRO_ESH's ordinal value */
            ISMOWNPRO_ESH,

            //  ISMOWNPRO_ESP's ordinal value */
            ISMOWNPRO_ESP,

            //  ISMOWNPRO_EST's ordinal value */
            ISMOWNPRO_EST,

            //  ISMOWNPRO_ETH's ordinal value */
            ISMOWNPRO_ETH,

            //  ISMOWNPRO_FIN's ordinal value */
            ISMOWNPRO_FIN,

            //  ISMOWNPRO_FJI's ordinal value */
            ISMOWNPRO_FJI,

            //  ISMOWNPRO_FLK's ordinal value */
            ISMOWNPRO_FLK,

            //  ISMOWNPRO_FRA's ordinal value */
            ISMOWNPRO_FRA,

            //  ISMOWNPRO_FRO's ordinal value */
            ISMOWNPRO_FRO,

            //  ISMOWNPRO_FSM's ordinal value */
            ISMOWNPRO_FSM,

            //  ISMOWNPRO_GAB's ordinal value */
            ISMOWNPRO_GAB,

            //  ISMOWNPRO_GBR's ordinal value */
            ISMOWNPRO_GBR,

            //  ISMOWNPRO_GEO's ordinal value */
            ISMOWNPRO_GEO,

            //  ISMOWNPRO_GGY's ordinal value */
            ISMOWNPRO_GGY,

            //  ISMOWNPRO_GHA's ordinal value */
            ISMOWNPRO_GHA,

            //  ISMOWNPRO_GIB's ordinal value */
            ISMOWNPRO_GIB,

            //  ISMOWNPRO_GIN's ordinal value */
            ISMOWNPRO_GIN,

            //  ISMOWNPRO_GLP's ordinal value */
            ISMOWNPRO_GLP,

            //  ISMOWNPRO_GMB's ordinal value */
            ISMOWNPRO_GMB,

            //  ISMOWNPRO_GNB's ordinal value */
            ISMOWNPRO_GNB,

            //  ISMOWNPRO_GNQ's ordinal value */
            ISMOWNPRO_GNQ,

            //  ISMOWNPRO_GRC's ordinal value */
            ISMOWNPRO_GRC,

            //  ISMOWNPRO_GRD's ordinal value */
            ISMOWNPRO_GRD,

            //  ISMOWNPRO_GRL's ordinal value */
            ISMOWNPRO_GRL,

            //  ISMOWNPRO_GTM's ordinal value */
            ISMOWNPRO_GTM,

            //  ISMOWNPRO_GUF's ordinal value */
            ISMOWNPRO_GUF,

            //  ISMOWNPRO_GUM's ordinal value */
            ISMOWNPRO_GUM,

            //  ISMOWNPRO_GUY's ordinal value */
            ISMOWNPRO_GUY,

            //  ISMOWNPRO_HKG's ordinal value */
            ISMOWNPRO_HKG,

            //  ISMOWNPRO_HMD's ordinal value */
            ISMOWNPRO_HMD,

            //  ISMOWNPRO_HND's ordinal value */
            ISMOWNPRO_HND,

            //  ISMOWNPRO_HRV's ordinal value */
            ISMOWNPRO_HRV,

            //  ISMOWNPRO_HTI's ordinal value */
            ISMOWNPRO_HTI,

            //  ISMOWNPRO_HUN's ordinal value */
            ISMOWNPRO_HUN,

            //  ISMOWNPRO_IDN's ordinal value */
            ISMOWNPRO_IDN,

            //  ISMOWNPRO_IMN's ordinal value */
            ISMOWNPRO_IMN,

            //  ISMOWNPRO_IND's ordinal value */
            ISMOWNPRO_IND,

            //  ISMOWNPRO_IOT's ordinal value */
            ISMOWNPRO_IOT,

            //  ISMOWNPRO_IRL's ordinal value */
            ISMOWNPRO_IRL,

            //  ISMOWNPRO_IRN's ordinal value */
            ISMOWNPRO_IRN,

            //  ISMOWNPRO_IRQ's ordinal value */
            ISMOWNPRO_IRQ,

            //  ISMOWNPRO_ISL's ordinal value */
            ISMOWNPRO_ISL,

            //  ISMOWNPRO_ISR's ordinal value */
            ISMOWNPRO_ISR,

            //  ISMOWNPRO_ITA's ordinal value */
            ISMOWNPRO_ITA,

            //  ISMOWNPRO_JAM's ordinal value */
            ISMOWNPRO_JAM,

            //  ISMOWNPRO_JEY's ordinal value */
            ISMOWNPRO_JEY,

            //  ISMOWNPRO_JOR's ordinal value */
            ISMOWNPRO_JOR,

            //  ISMOWNPRO_JPN's ordinal value */
            ISMOWNPRO_JPN,

            //  ISMOWNPRO_KAZ's ordinal value */
            ISMOWNPRO_KAZ,

            //  ISMOWNPRO_KEN's ordinal value */
            ISMOWNPRO_KEN,

            //  ISMOWNPRO_KGZ's ordinal value */
            ISMOWNPRO_KGZ,

            //  ISMOWNPRO_KHM's ordinal value */
            ISMOWNPRO_KHM,

            //  ISMOWNPRO_KIR's ordinal value */
            ISMOWNPRO_KIR,

            //  ISMOWNPRO_KNA's ordinal value */
            ISMOWNPRO_KNA,

            //  ISMOWNPRO_KOR's ordinal value */
            ISMOWNPRO_KOR,

            //  ISMOWNPRO_KWT's ordinal value */
            ISMOWNPRO_KWT,

            //  ISMOWNPRO_LAO's ordinal value */
            ISMOWNPRO_LAO,

            //  ISMOWNPRO_LBN's ordinal value */
            ISMOWNPRO_LBN,

            //  ISMOWNPRO_LBR's ordinal value */
            ISMOWNPRO_LBR,

            //  ISMOWNPRO_LBY's ordinal value */
            ISMOWNPRO_LBY,

            //  ISMOWNPRO_LCA's ordinal value */
            ISMOWNPRO_LCA,

            //  ISMOWNPRO_LIE's ordinal value */
            ISMOWNPRO_LIE,

            //  ISMOWNPRO_LKA's ordinal value */
            ISMOWNPRO_LKA,

            //  ISMOWNPRO_LSO's ordinal value */
            ISMOWNPRO_LSO,

            //  ISMOWNPRO_LTU's ordinal value */
            ISMOWNPRO_LTU,

            //  ISMOWNPRO_LUX's ordinal value */
            ISMOWNPRO_LUX,

            //  ISMOWNPRO_LVA's ordinal value */
            ISMOWNPRO_LVA,

            //  ISMOWNPRO_MAC's ordinal value */
            ISMOWNPRO_MAC,

            //  ISMOWNPRO_MAF's ordinal value */
            ISMOWNPRO_MAF,

            //  ISMOWNPRO_MAR's ordinal value */
            ISMOWNPRO_MAR,

            //  ISMOWNPRO_MCO's ordinal value */
            ISMOWNPRO_MCO,

            //  ISMOWNPRO_MDA's ordinal value */
            ISMOWNPRO_MDA,

            //  ISMOWNPRO_MDG's ordinal value */
            ISMOWNPRO_MDG,

            //  ISMOWNPRO_MDV's ordinal value */
            ISMOWNPRO_MDV,

            //  ISMOWNPRO_MEX's ordinal value */
            ISMOWNPRO_MEX,

            //  ISMOWNPRO_MHL's ordinal value */
            ISMOWNPRO_MHL,

            //  ISMOWNPRO_MKD's ordinal value */
            ISMOWNPRO_MKD,

            //  ISMOWNPRO_MLI's ordinal value */
            ISMOWNPRO_MLI,

            //  ISMOWNPRO_MLT's ordinal value */
            ISMOWNPRO_MLT,

            //  ISMOWNPRO_MMR's ordinal value */
            ISMOWNPRO_MMR,

            //  ISMOWNPRO_MNE's ordinal value */
            ISMOWNPRO_MNE,

            //  ISMOWNPRO_MNG's ordinal value */
            ISMOWNPRO_MNG,

            //  ISMOWNPRO_MNP's ordinal value */
            ISMOWNPRO_MNP,

            //  ISMOWNPRO_MOZ's ordinal value */
            ISMOWNPRO_MOZ,

            //  ISMOWNPRO_MRT's ordinal value */
            ISMOWNPRO_MRT,

            //  ISMOWNPRO_MSR's ordinal value */
            ISMOWNPRO_MSR,

            //  ISMOWNPRO_MTQ's ordinal value */
            ISMOWNPRO_MTQ,

            //  ISMOWNPRO_MUS's ordinal value */
            ISMOWNPRO_MUS,

            //  ISMOWNPRO_MWI's ordinal value */
            ISMOWNPRO_MWI,

            //  ISMOWNPRO_MYS's ordinal value */
            ISMOWNPRO_MYS,

            //  ISMOWNPRO_MYT's ordinal value */
            ISMOWNPRO_MYT,

            //  ISMOWNPRO_NAM's ordinal value */
            ISMOWNPRO_NAM,

            //  ISMOWNPRO_NCL's ordinal value */
            ISMOWNPRO_NCL,

            //  ISMOWNPRO_NER's ordinal value */
            ISMOWNPRO_NER,

            //  ISMOWNPRO_NFK's ordinal value */
            ISMOWNPRO_NFK,

            //  ISMOWNPRO_NGA's ordinal value */
            ISMOWNPRO_NGA,

            //  ISMOWNPRO_NIC's ordinal value */
            ISMOWNPRO_NIC,

            //  ISMOWNPRO_NIU's ordinal value */
            ISMOWNPRO_NIU,

            //  ISMOWNPRO_NLD's ordinal value */
            ISMOWNPRO_NLD,

            //  ISMOWNPRO_NOR's ordinal value */
            ISMOWNPRO_NOR,

            //  ISMOWNPRO_NPL's ordinal value */
            ISMOWNPRO_NPL,

            //  ISMOWNPRO_NRU's ordinal value */
            ISMOWNPRO_NRU,

            //  ISMOWNPRO_NZL's ordinal value */
            ISMOWNPRO_NZL,

            //  ISMOWNPRO_OMN's ordinal value */
            ISMOWNPRO_OMN,

            //  ISMOWNPRO_PAK's ordinal value */
            ISMOWNPRO_PAK,

            //  ISMOWNPRO_PAN's ordinal value */
            ISMOWNPRO_PAN,

            //  ISMOWNPRO_PCN's ordinal value */
            ISMOWNPRO_PCN,

            //  ISMOWNPRO_PER's ordinal value */
            ISMOWNPRO_PER,

            //  ISMOWNPRO_PHL's ordinal value */
            ISMOWNPRO_PHL,

            //  ISMOWNPRO_PLW's ordinal value */
            ISMOWNPRO_PLW,

            //  ISMOWNPRO_PNG's ordinal value */
            ISMOWNPRO_PNG,

            //  ISMOWNPRO_POL's ordinal value */
            ISMOWNPRO_POL,

            //  ISMOWNPRO_PRI's ordinal value */
            ISMOWNPRO_PRI,

            //  ISMOWNPRO_PRK's ordinal value */
            ISMOWNPRO_PRK,

            //  ISMOWNPRO_PRT's ordinal value */
            ISMOWNPRO_PRT,

            //  ISMOWNPRO_PRY's ordinal value */
            ISMOWNPRO_PRY,

            //  ISMOWNPRO_PSE's ordinal value */
            ISMOWNPRO_PSE,

            //  ISMOWNPRO_PYF's ordinal value */
            ISMOWNPRO_PYF,

            //  ISMOWNPRO_QAT's ordinal value */
            ISMOWNPRO_QAT,

            //  ISMOWNPRO_REU's ordinal value */
            ISMOWNPRO_REU,

            //  ISMOWNPRO_ROU's ordinal value */
            ISMOWNPRO_ROU,

            //  ISMOWNPRO_RUS's ordinal value */
            ISMOWNPRO_RUS,

            //  ISMOWNPRO_RWA's ordinal value */
            ISMOWNPRO_RWA,

            //  ISMOWNPRO_SAU's ordinal value */
            ISMOWNPRO_SAU,

            //  ISMOWNPRO_SDN's ordinal value */
            ISMOWNPRO_SDN,

            //  ISMOWNPRO_SEN's ordinal value */
            ISMOWNPRO_SEN,

            //  ISMOWNPRO_SGP's ordinal value */
            ISMOWNPRO_SGP,

            //  ISMOWNPRO_SGS's ordinal value */
            ISMOWNPRO_SGS,

            //  ISMOWNPRO_SHN's ordinal value */
            ISMOWNPRO_SHN,

            //  ISMOWNPRO_SJM's ordinal value */
            ISMOWNPRO_SJM,

            //  ISMOWNPRO_SLB's ordinal value */
            ISMOWNPRO_SLB,

            //  ISMOWNPRO_SLE's ordinal value */
            ISMOWNPRO_SLE,

            //  ISMOWNPRO_SLV's ordinal value */
            ISMOWNPRO_SLV,

            //  ISMOWNPRO_SMR's ordinal value */
            ISMOWNPRO_SMR,

            //  ISMOWNPRO_SOM's ordinal value */
            ISMOWNPRO_SOM,

            //  ISMOWNPRO_SPM's ordinal value */
            ISMOWNPRO_SPM,

            //  ISMOWNPRO_SRB's ordinal value */
            ISMOWNPRO_SRB,

            //  ISMOWNPRO_SSD's ordinal value */
            ISMOWNPRO_SSD,

            //  ISMOWNPRO_STP's ordinal value */
            ISMOWNPRO_STP,

            //  ISMOWNPRO_SUR's ordinal value */
            ISMOWNPRO_SUR,

            //  ISMOWNPRO_SVK's ordinal value */
            ISMOWNPRO_SVK,

            //  ISMOWNPRO_SVN's ordinal value */
            ISMOWNPRO_SVN,

            //  ISMOWNPRO_SWE's ordinal value */
            ISMOWNPRO_SWE,

            //  ISMOWNPRO_SWZ's ordinal value */
            ISMOWNPRO_SWZ,

            //  ISMOWNPRO_SXM's ordinal value */
            ISMOWNPRO_SXM,

            //  ISMOWNPRO_SYC's ordinal value */
            ISMOWNPRO_SYC,

            //  ISMOWNPRO_SYR's ordinal value */
            ISMOWNPRO_SYR,

            //  ISMOWNPRO_TCA's ordinal value */
            ISMOWNPRO_TCA,

            //  ISMOWNPRO_TCD's ordinal value */
            ISMOWNPRO_TCD,

            //  ISMOWNPRO_TGO's ordinal value */
            ISMOWNPRO_TGO,

            //  ISMOWNPRO_THA's ordinal value */
            ISMOWNPRO_THA,

            //  ISMOWNPRO_TJK's ordinal value */
            ISMOWNPRO_TJK,

            //  ISMOWNPRO_TKL's ordinal value */
            ISMOWNPRO_TKL,

            //  ISMOWNPRO_TKM's ordinal value */
            ISMOWNPRO_TKM,

            //  ISMOWNPRO_TLS's ordinal value */
            ISMOWNPRO_TLS,

            //  ISMOWNPRO_TON's ordinal value */
            ISMOWNPRO_TON,

            //  ISMOWNPRO_TTO's ordinal value */
            ISMOWNPRO_TTO,

            //  ISMOWNPRO_TUN's ordinal value */
            ISMOWNPRO_TUN,

            //  ISMOWNPRO_TUR's ordinal value */
            ISMOWNPRO_TUR,

            //  ISMOWNPRO_TUV's ordinal value */
            ISMOWNPRO_TUV,

            //  ISMOWNPRO_TWN's ordinal value */
            ISMOWNPRO_TWN,

            //  ISMOWNPRO_TZA's ordinal value */
            ISMOWNPRO_TZA,

            //  ISMOWNPRO_UGA's ordinal value */
            ISMOWNPRO_UGA,

            //  ISMOWNPRO_UKR's ordinal value */
            ISMOWNPRO_UKR,

            //  ISMOWNPRO_UMI's ordinal value */
            ISMOWNPRO_UMI,

            //  ISMOWNPRO_URY's ordinal value */
            ISMOWNPRO_URY,

            //  ISMOWNPRO_USA's ordinal value */
            ISMOWNPRO_USA,

            //  ISMOWNPRO_UZB's ordinal value */
            ISMOWNPRO_UZB,

            //  ISMOWNPRO_VAT's ordinal value */
            ISMOWNPRO_VAT,

            //  ISMOWNPRO_VCT's ordinal value */
            ISMOWNPRO_VCT,

            //  ISMOWNPRO_VEN's ordinal value */
            ISMOWNPRO_VEN,

            //  ISMOWNPRO_VGB's ordinal value */
            ISMOWNPRO_VGB,

            //  ISMOWNPRO_VIR's ordinal value */
            ISMOWNPRO_VIR,

            //  ISMOWNPRO_VNM's ordinal value */
            ISMOWNPRO_VNM,

            //  ISMOWNPRO_VUT's ordinal value */
            ISMOWNPRO_VUT,

            //  ISMOWNPRO_WLF's ordinal value */
            ISMOWNPRO_WLF,

            //  ISMOWNPRO_WSM's ordinal value */
            ISMOWNPRO_WSM,

            //  ISMOWNPRO_YEM's ordinal value */
            ISMOWNPRO_YEM,

            //  ISMOWNPRO_ZAF's ordinal value */
            ISMOWNPRO_ZAF,

            //  ISMOWNPRO_ZMB's ordinal value */
            ISMOWNPRO_ZMB,

            //  ISMOWNPRO_ZWE's ordinal value */
            ISMOWNPRO_ZWE,

            //  ISMOWNPRO_ACGU's ordinal value */
            ISMOWNPRO_ACGU,

            //  ISMOWNPRO_APFS's ordinal value */
            ISMOWNPRO_APFS,

            //  ISMOWNPRO_BWCS's ordinal value */
            ISMOWNPRO_BWCS,

            //  ISMOWNPRO_CFCK's ordinal value */
            ISMOWNPRO_CFCK,

            //  ISMOWNPRO_CFUP's ordinal value */
            ISMOWNPRO_CFUP,

            //  ISMOWNPRO_CMFC's ordinal value */
            ISMOWNPRO_CMFC,

            //  ISMOWNPRO_CMFP's ordinal value */
            ISMOWNPRO_CMFP,

            //  ISMOWNPRO_CPMT's ordinal value */
            ISMOWNPRO_CPMT,

            //  ISMOWNPRO_CWCS's ordinal value */
            ISMOWNPRO_CWCS,

            //  ISMOWNPRO_EFOR's ordinal value */
            ISMOWNPRO_EFOR,

            //  ISMOWNPRO_EUDA's ordinal value */
            ISMOWNPRO_EUDA,

            //  ISMOWNPRO_FVEY's ordinal value */
            ISMOWNPRO_FVEY,

            //  ISMOWNPRO_GCTF's ordinal value */
            ISMOWNPRO_GCTF,

            //  ISMOWNPRO_GMIF's ordinal value */
            ISMOWNPRO_GMIF,

            //  ISMOWNPRO_IESC's ordinal value */
            ISMOWNPRO_IESC,

            //  ISMOWNPRO_ISAF's ordinal value */
            ISMOWNPRO_ISAF,

            //  ISMOWNPRO_KFOR's ordinal value */
            ISMOWNPRO_KFOR,

            //  ISMOWNPRO_MIFH's ordinal value */
            ISMOWNPRO_MIFH,

            //  ISMOWNPRO_MLEC's ordinal value */
            ISMOWNPRO_MLEC,

            //  ISMOWNPRO_NACT's ordinal value */
            ISMOWNPRO_NACT,

            //  ISMOWNPRO_NATO's ordinal value */
            ISMOWNPRO_NATO,

            //  ISMOWNPRO_OSTY's ordinal value */
            ISMOWNPRO_OSTY,

            //  ISMOWNPRO_SPAA's ordinal value */
            ISMOWNPRO_SPAA,

            //  ISMOWNPRO_TEYE's ordinal value */
            ISMOWNPRO_TEYE,

            //  ISMOWNPRO_UNCK's ordinal value */
            ISMOWNPRO_UNCK,

            //  Enumerated item that indicates maximum enumerated item exclusive of this item
            enumMaxExclusive
         };


         /** The destructor */
         virtual ~CVEnumISMOwnerProducerValues()
         { }

         /** Returns this accessor's type constant, i.e. cVEnumISMOwnerProducerValues
           *
           * @return This accessor's type constant, i.e. cVEnumISMOwnerProducerValues
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return ismownpro::type::accessorType::cVEnumISMOwnerProducerValues;
         }


         /** The assignment operator. Sets the contents of this CVEnumISMOwnerProducerValues to the contents of the
           * CVEnumISMOwnerProducerValues on the right hand side (rhs) of the assignment operator.CVEnumISMOwnerProducerValues
           *
           * @param rhs The CVEnumISMOwnerProducerValues on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this CVEnumISMOwnerProducerValues
           * @return A constant reference to this CVEnumISMOwnerProducerValues.
           */
         const CVEnumISMOwnerProducerValues& operator=(const CVEnumISMOwnerProducerValues& rhs)
         {
            setValue(rhs.getValue());
            return *this;
         }


         /** The assignment operator. Sets the value of this CVEnumISMOwnerProducerValues enumeration to the EnumerationItem on
           * the right hand side of the assignment operator.
           *
           * @param rhs The EnumerationItemwhose value is to be assigned to this CVEnumISMOwnerProducerValues enumeration
           * @return A reference to this CVEnumISMOwnerProducerValues enumeration
           */
         const CVEnumISMOwnerProducerValues& operator=(EnumerationItem rhs)
         {
            setValue(rhs);
            return *this;
         }


         /** Sets the value of this CVEnumISMOwnerProducerValues enumeration to the value of the specified enumeration item.
           *
           * @param item The EnumerationItemwhose value is to be used to set the value of this CVEnumISMOwnerProducerValues
           *      enumeration
           */
         virtual void setValue(EnumerationItem item)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of this CVEnumISMOwnerProducerValues enumeration.
           *
           * @param testForValidity=true Specifies whether this CVEnumISMOwnerProducerValues enumeration should be validated
           *      (true) or not (false) as part of retrieving its value.
           * @return The value of this CVEnumISMOwnerProducerValues enumeration.
           */
         virtual EnumerationItem getValue(bool testForValidity=true) const
            throw(uci::base::UCIException) = 0;


         /** Returns the number of items in this CVEnumISMOwnerProducerValues enumeration.
           *
           * @return The number of items in this CVEnumISMOwnerProducerValues enumeration.
           */
         int getNumberOfItems() const
         {
            return 275;
         }


         /** Returns whether this CVEnumISMOwnerProducerValues enumeration is valid or not. A CVEnumISMOwnerProducerValues
           * enumeration is valid if its value is one of the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @return A flag indicating whether this CVEnumISMOwnerProducerValues enumeration is valid (true) or not (false).
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


         /** Tests whether the value of the CVEnumISMOwnerProducerValues enumeration on the right hand side of the == operator is
           * equal to the value as this CVEnumISMOwnerProducerValues enumeration.
           *
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMOwnerProducerValues enumeration.
           * @return True if the value of the specified CVEnumISMOwnerProducerValues enumeration (rhs) is equal to the value of
           *      this CVEnumISMOwnerProducerValues enumeration, false otherwise
           */
         bool operator==(const CVEnumISMOwnerProducerValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() == rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMOwnerProducerValues enumeration on the right hand side of the != operator is
           * not equal to the value as this CVEnumISMOwnerProducerValues enumeration.
           *
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMOwnerProducerValues enumeration.
           * @return True if the value of the specified CVEnumISMOwnerProducerValues enumeration (rhs) is not equal to the value
           *      of this CVEnumISMOwnerProducerValues enumeration, false otherwise
           */
         bool operator!=(const CVEnumISMOwnerProducerValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() != rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMOwnerProducerValues enumeration on the right hand side of the < operator is
           * less than the value as this CVEnumISMOwnerProducerValues enumeration.
           *
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMOwnerProducerValues enumeration.
           * @return True if the value of the specified CVEnumISMOwnerProducerValues enumeration (rhs) is less than the value of
           *      this CVEnumISMOwnerProducerValues enumeration, false otherwise
           */
         bool operator<(const CVEnumISMOwnerProducerValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() < rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMOwnerProducerValues enumeration on the right hand side of the <= operator is
           * less than or equal to the value as this CVEnumISMOwnerProducerValues enumeration.
           *
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMOwnerProducerValues enumeration.
           * @return True if the value of the specified CVEnumISMOwnerProducerValues enumeration (rhs) is less than or equal to
           *      the value of this CVEnumISMOwnerProducerValues enumeration, false otherwise
           */
         bool operator<=(const CVEnumISMOwnerProducerValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() <= rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMOwnerProducerValues enumeration on the right hand side of the > operator is
           * greater than the value as this CVEnumISMOwnerProducerValues enumeration.
           *
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMOwnerProducerValues enumeration.
           * @return True if the value of the specified CVEnumISMOwnerProducerValues enumeration (rhs) is greater than the value
           *      of this CVEnumISMOwnerProducerValues enumeration, false otherwise
           */
         bool operator>(const CVEnumISMOwnerProducerValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() > rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMOwnerProducerValues enumeration on the right hand side of the >= operator is
           * greater than or equal to the value as this CVEnumISMOwnerProducerValues enumeration.
           *
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMOwnerProducerValues enumeration.
           * @return True if the value of the specified CVEnumISMOwnerProducerValues enumeration (rhs) is greater than or equal to
           *      the value of this CVEnumISMOwnerProducerValues enumeration, false otherwise
           */
         bool operator>=(const CVEnumISMOwnerProducerValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() >= rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMOwnerProducerValues enumeration is equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMOwnerProducerValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is equal to the value of this CVEnumISMOwnerProducerValues
           *      enumeration, false otherwise
           */
         bool operator==(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() == rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified
           * CVEnumISMOwnerProducerValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMOwnerProducerValues enumeration.
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is equal to the value of the CVEnumISMOwnerProducerValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator==(EnumerationItem lhs, const CVEnumISMOwnerProducerValues& rhs)
         {
            testForValidItem(lhs);
            return lhs == rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMOwnerProducerValues enumeration is not equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMOwnerProducerValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this CVEnumISMOwnerProducerValues
           *      enumeration, false otherwise
           */
         bool operator!=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() != rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified
           * CVEnumISMOwnerProducerValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMOwnerProducerValues enumeration.
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the CVEnumISMOwnerProducerValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator!=(EnumerationItem lhs, const CVEnumISMOwnerProducerValues& rhs)
         {
            testForValidItem(lhs);
            return lhs != rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMOwnerProducerValues enumeration is less than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMOwnerProducerValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than the value of this CVEnumISMOwnerProducerValues
           *      enumeration, false otherwise
           */
         bool operator<(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() < rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than the value of the specified
           * CVEnumISMOwnerProducerValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMOwnerProducerValues enumeration.
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than the value of the CVEnumISMOwnerProducerValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator<(EnumerationItem lhs, const CVEnumISMOwnerProducerValues& rhs)
         {
            testForValidItem(lhs);
            return lhs < rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMOwnerProducerValues enumeration is less than or equal to the value of the
           * specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMOwnerProducerValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this
           *      CVEnumISMOwnerProducerValues enumeration, false otherwise
           */
         bool operator<=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() <= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
           * CVEnumISMOwnerProducerValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMOwnerProducerValues enumeration.
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the
           *      CVEnumISMOwnerProducerValues enumeration (rhs), false otherwise
           */
         friend bool operator<=(EnumerationItem lhs, const CVEnumISMOwnerProducerValues& rhs)
         {
            testForValidItem(lhs);
            return lhs <= rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMOwnerProducerValues enumeration is greater than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMOwnerProducerValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than the value of this CVEnumISMOwnerProducerValues
           *      enumeration, false otherwise
           */
         bool operator>(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() > rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified
           * CVEnumISMOwnerProducerValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMOwnerProducerValues enumeration.
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than the value of the CVEnumISMOwnerProducerValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator>(EnumerationItem lhs, const CVEnumISMOwnerProducerValues& rhs)
         {
            testForValidItem(lhs);
            return lhs > rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMOwnerProducerValues enumeration is greater than or equal to the value of the
           * specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMOwnerProducerValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this
           *      CVEnumISMOwnerProducerValues enumeration, false otherwise
           */
         bool operator>=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() >= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
           * CVEnumISMOwnerProducerValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMOwnerProducerValues enumeration.
           * @param rhs The CVEnumISMOwnerProducerValues enumeration whose value is to be compared against the value of the
           *      specified EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the
           *      CVEnumISMOwnerProducerValues enumeration (rhs), false otherwise
           */
         friend bool operator>=(EnumerationItem lhs, const CVEnumISMOwnerProducerValues& rhs)
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


         /** Return the name of the enumeration item that the value of this CVEnumISMOwnerProducerValues enumeration is set to.
           *
           * @return The name of the enumeration item.
           */
         virtual const std::string& toName() const
            throw(uci::base::UCIException)
         {
            return toName(getValue());
         }


         /** Sets the value of this CVEnumISMOwnerProducerValues enumeration to the value of the enumeration item having the
           * specified name.
           *
           * @param itemName The name of the enumeration item whose value is to be used to set this CVEnumISMOwnerProducerValues
           *      enumeration to
           */
         virtual void setValueFromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            setValue(fromName(itemName));
         }



      protected:

         /** The default constructor. In addition to constructing a new CVEnumISMOwnerProducerValues, this constructor also makes
           * sure the conversion tables are initialized
           */
         CVEnumISMOwnerProducerValues()
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
           * @param rhs The CVEnumISMOwnerProducerValues to copy from
           */
         CVEnumISMOwnerProducerValues(const CVEnumISMOwnerProducerValues& rhs)
         {
            (void)rhs;
         }

      }; // CVEnumISMOwnerProducerValues


   } // Namespace: type
} // Namespace: ismownpro
template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& oStream, const ismownpro::type::CVEnumISMOwnerProducerValues& enumeration)
{
   oStream << enumeration.toName();
   return oStream;
}

#endif // Ismownpro__Type__CVEnumISMOwnerProducerValues_h

