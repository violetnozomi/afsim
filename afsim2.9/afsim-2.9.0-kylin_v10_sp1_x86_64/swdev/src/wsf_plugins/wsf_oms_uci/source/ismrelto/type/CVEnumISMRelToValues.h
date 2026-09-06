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
#ifndef Ismrelto__Type__CVEnumISMRelToValues_h
#define Ismrelto__Type__CVEnumISMRelToValues_h 1

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
namespace ismrelto {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** (U) USA followed by all currently valid ISO Trigraphs except USA in alphabetical order by Trigraph, followed by all
        * currently valid CAPCO Coalition tetragraphs in alphabetical order by tetragraph. PERMISSIBLE VALUES The permissible
        * values for this simple type are defined in the Controlled Value Enumeration: CVEnumISMRelTo.xml
        */
      class CVEnumISMRelToValues : public virtual uci::base::Accessor {
      public:

         //  The following are the enumerated items that comprise this enumeration
         enum EnumerationItem {
            //  Enumerated item that indicates that the enumeration has not been set
            enumNotSet,
            //  ISMRELTO_USA's ordinal value */
            ISMRELTO_USA,

            //  ISMRELTO_ABW's ordinal value */
            ISMRELTO_ABW,

            //  ISMRELTO_AFG's ordinal value */
            ISMRELTO_AFG,

            //  ISMRELTO_AGO's ordinal value */
            ISMRELTO_AGO,

            //  ISMRELTO_AIA's ordinal value */
            ISMRELTO_AIA,

            //  ISMRELTO_ALA's ordinal value */
            ISMRELTO_ALA,

            //  ISMRELTO_ALB's ordinal value */
            ISMRELTO_ALB,

            //  ISMRELTO_AND's ordinal value */
            ISMRELTO_AND,

            //  ISMRELTO_ARE's ordinal value */
            ISMRELTO_ARE,

            //  ISMRELTO_ARG's ordinal value */
            ISMRELTO_ARG,

            //  ISMRELTO_ARM's ordinal value */
            ISMRELTO_ARM,

            //  ISMRELTO_ASM's ordinal value */
            ISMRELTO_ASM,

            //  ISMRELTO_ATA's ordinal value */
            ISMRELTO_ATA,

            //  ISMRELTO_ATF's ordinal value */
            ISMRELTO_ATF,

            //  ISMRELTO_ATG's ordinal value */
            ISMRELTO_ATG,

            //  ISMRELTO_AUS's ordinal value */
            ISMRELTO_AUS,

            //  ISMRELTO_AUT's ordinal value */
            ISMRELTO_AUT,

            //  ISMRELTO_AZE's ordinal value */
            ISMRELTO_AZE,

            //  ISMRELTO_BDI's ordinal value */
            ISMRELTO_BDI,

            //  ISMRELTO_BEL's ordinal value */
            ISMRELTO_BEL,

            //  ISMRELTO_BEN's ordinal value */
            ISMRELTO_BEN,

            //  ISMRELTO_BES's ordinal value */
            ISMRELTO_BES,

            //  ISMRELTO_BFA's ordinal value */
            ISMRELTO_BFA,

            //  ISMRELTO_BGD's ordinal value */
            ISMRELTO_BGD,

            //  ISMRELTO_BGR's ordinal value */
            ISMRELTO_BGR,

            //  ISMRELTO_BHR's ordinal value */
            ISMRELTO_BHR,

            //  ISMRELTO_BHS's ordinal value */
            ISMRELTO_BHS,

            //  ISMRELTO_BIH's ordinal value */
            ISMRELTO_BIH,

            //  ISMRELTO_BLM's ordinal value */
            ISMRELTO_BLM,

            //  ISMRELTO_BLR's ordinal value */
            ISMRELTO_BLR,

            //  ISMRELTO_BLZ's ordinal value */
            ISMRELTO_BLZ,

            //  ISMRELTO_BMU's ordinal value */
            ISMRELTO_BMU,

            //  ISMRELTO_BOL's ordinal value */
            ISMRELTO_BOL,

            //  ISMRELTO_BRA's ordinal value */
            ISMRELTO_BRA,

            //  ISMRELTO_BRB's ordinal value */
            ISMRELTO_BRB,

            //  ISMRELTO_BRN's ordinal value */
            ISMRELTO_BRN,

            //  ISMRELTO_BTN's ordinal value */
            ISMRELTO_BTN,

            //  ISMRELTO_BVT's ordinal value */
            ISMRELTO_BVT,

            //  ISMRELTO_BWA's ordinal value */
            ISMRELTO_BWA,

            //  ISMRELTO_CAF's ordinal value */
            ISMRELTO_CAF,

            //  ISMRELTO_CAN's ordinal value */
            ISMRELTO_CAN,

            //  ISMRELTO_CCK's ordinal value */
            ISMRELTO_CCK,

            //  ISMRELTO_CHE's ordinal value */
            ISMRELTO_CHE,

            //  ISMRELTO_CHL's ordinal value */
            ISMRELTO_CHL,

            //  ISMRELTO_CHN's ordinal value */
            ISMRELTO_CHN,

            //  ISMRELTO_CIV's ordinal value */
            ISMRELTO_CIV,

            //  ISMRELTO_CMR's ordinal value */
            ISMRELTO_CMR,

            //  ISMRELTO_COD's ordinal value */
            ISMRELTO_COD,

            //  ISMRELTO_COG's ordinal value */
            ISMRELTO_COG,

            //  ISMRELTO_COK's ordinal value */
            ISMRELTO_COK,

            //  ISMRELTO_COL's ordinal value */
            ISMRELTO_COL,

            //  ISMRELTO_COM's ordinal value */
            ISMRELTO_COM,

            //  ISMRELTO_CPV's ordinal value */
            ISMRELTO_CPV,

            //  ISMRELTO_CRI's ordinal value */
            ISMRELTO_CRI,

            //  ISMRELTO_CUB's ordinal value */
            ISMRELTO_CUB,

            //  ISMRELTO_CUW's ordinal value */
            ISMRELTO_CUW,

            //  ISMRELTO_CXR's ordinal value */
            ISMRELTO_CXR,

            //  ISMRELTO_CYM's ordinal value */
            ISMRELTO_CYM,

            //  ISMRELTO_CYP's ordinal value */
            ISMRELTO_CYP,

            //  ISMRELTO_CZE's ordinal value */
            ISMRELTO_CZE,

            //  ISMRELTO_DEU's ordinal value */
            ISMRELTO_DEU,

            //  ISMRELTO_DJI's ordinal value */
            ISMRELTO_DJI,

            //  ISMRELTO_DMA's ordinal value */
            ISMRELTO_DMA,

            //  ISMRELTO_DNK's ordinal value */
            ISMRELTO_DNK,

            //  ISMRELTO_DOM's ordinal value */
            ISMRELTO_DOM,

            //  ISMRELTO_DZA's ordinal value */
            ISMRELTO_DZA,

            //  ISMRELTO_ECU's ordinal value */
            ISMRELTO_ECU,

            //  ISMRELTO_EGY's ordinal value */
            ISMRELTO_EGY,

            //  ISMRELTO_ERI's ordinal value */
            ISMRELTO_ERI,

            //  ISMRELTO_ESH's ordinal value */
            ISMRELTO_ESH,

            //  ISMRELTO_ESP's ordinal value */
            ISMRELTO_ESP,

            //  ISMRELTO_EST's ordinal value */
            ISMRELTO_EST,

            //  ISMRELTO_ETH's ordinal value */
            ISMRELTO_ETH,

            //  ISMRELTO_FIN's ordinal value */
            ISMRELTO_FIN,

            //  ISMRELTO_FJI's ordinal value */
            ISMRELTO_FJI,

            //  ISMRELTO_FLK's ordinal value */
            ISMRELTO_FLK,

            //  ISMRELTO_FRA's ordinal value */
            ISMRELTO_FRA,

            //  ISMRELTO_FRO's ordinal value */
            ISMRELTO_FRO,

            //  ISMRELTO_FSM's ordinal value */
            ISMRELTO_FSM,

            //  ISMRELTO_GAB's ordinal value */
            ISMRELTO_GAB,

            //  ISMRELTO_GBR's ordinal value */
            ISMRELTO_GBR,

            //  ISMRELTO_GEO's ordinal value */
            ISMRELTO_GEO,

            //  ISMRELTO_GGY's ordinal value */
            ISMRELTO_GGY,

            //  ISMRELTO_GHA's ordinal value */
            ISMRELTO_GHA,

            //  ISMRELTO_GIB's ordinal value */
            ISMRELTO_GIB,

            //  ISMRELTO_GIN's ordinal value */
            ISMRELTO_GIN,

            //  ISMRELTO_GLP's ordinal value */
            ISMRELTO_GLP,

            //  ISMRELTO_GMB's ordinal value */
            ISMRELTO_GMB,

            //  ISMRELTO_GNB's ordinal value */
            ISMRELTO_GNB,

            //  ISMRELTO_GNQ's ordinal value */
            ISMRELTO_GNQ,

            //  ISMRELTO_GRC's ordinal value */
            ISMRELTO_GRC,

            //  ISMRELTO_GRD's ordinal value */
            ISMRELTO_GRD,

            //  ISMRELTO_GRL's ordinal value */
            ISMRELTO_GRL,

            //  ISMRELTO_GTM's ordinal value */
            ISMRELTO_GTM,

            //  ISMRELTO_GUF's ordinal value */
            ISMRELTO_GUF,

            //  ISMRELTO_GUM's ordinal value */
            ISMRELTO_GUM,

            //  ISMRELTO_GUY's ordinal value */
            ISMRELTO_GUY,

            //  ISMRELTO_HKG's ordinal value */
            ISMRELTO_HKG,

            //  ISMRELTO_HMD's ordinal value */
            ISMRELTO_HMD,

            //  ISMRELTO_HND's ordinal value */
            ISMRELTO_HND,

            //  ISMRELTO_HRV's ordinal value */
            ISMRELTO_HRV,

            //  ISMRELTO_HTI's ordinal value */
            ISMRELTO_HTI,

            //  ISMRELTO_HUN's ordinal value */
            ISMRELTO_HUN,

            //  ISMRELTO_IDN's ordinal value */
            ISMRELTO_IDN,

            //  ISMRELTO_IMN's ordinal value */
            ISMRELTO_IMN,

            //  ISMRELTO_IND's ordinal value */
            ISMRELTO_IND,

            //  ISMRELTO_IOT's ordinal value */
            ISMRELTO_IOT,

            //  ISMRELTO_IRL's ordinal value */
            ISMRELTO_IRL,

            //  ISMRELTO_IRN's ordinal value */
            ISMRELTO_IRN,

            //  ISMRELTO_IRQ's ordinal value */
            ISMRELTO_IRQ,

            //  ISMRELTO_ISL's ordinal value */
            ISMRELTO_ISL,

            //  ISMRELTO_ISR's ordinal value */
            ISMRELTO_ISR,

            //  ISMRELTO_ITA's ordinal value */
            ISMRELTO_ITA,

            //  ISMRELTO_JAM's ordinal value */
            ISMRELTO_JAM,

            //  ISMRELTO_JEY's ordinal value */
            ISMRELTO_JEY,

            //  ISMRELTO_JOR's ordinal value */
            ISMRELTO_JOR,

            //  ISMRELTO_JPN's ordinal value */
            ISMRELTO_JPN,

            //  ISMRELTO_KAZ's ordinal value */
            ISMRELTO_KAZ,

            //  ISMRELTO_KEN's ordinal value */
            ISMRELTO_KEN,

            //  ISMRELTO_KGZ's ordinal value */
            ISMRELTO_KGZ,

            //  ISMRELTO_KHM's ordinal value */
            ISMRELTO_KHM,

            //  ISMRELTO_KIR's ordinal value */
            ISMRELTO_KIR,

            //  ISMRELTO_KNA's ordinal value */
            ISMRELTO_KNA,

            //  ISMRELTO_KOR's ordinal value */
            ISMRELTO_KOR,

            //  ISMRELTO_KWT's ordinal value */
            ISMRELTO_KWT,

            //  ISMRELTO_LAO's ordinal value */
            ISMRELTO_LAO,

            //  ISMRELTO_LBN's ordinal value */
            ISMRELTO_LBN,

            //  ISMRELTO_LBR's ordinal value */
            ISMRELTO_LBR,

            //  ISMRELTO_LBY's ordinal value */
            ISMRELTO_LBY,

            //  ISMRELTO_LCA's ordinal value */
            ISMRELTO_LCA,

            //  ISMRELTO_LIE's ordinal value */
            ISMRELTO_LIE,

            //  ISMRELTO_LKA's ordinal value */
            ISMRELTO_LKA,

            //  ISMRELTO_LSO's ordinal value */
            ISMRELTO_LSO,

            //  ISMRELTO_LTU's ordinal value */
            ISMRELTO_LTU,

            //  ISMRELTO_LUX's ordinal value */
            ISMRELTO_LUX,

            //  ISMRELTO_LVA's ordinal value */
            ISMRELTO_LVA,

            //  ISMRELTO_MAC's ordinal value */
            ISMRELTO_MAC,

            //  ISMRELTO_MAF's ordinal value */
            ISMRELTO_MAF,

            //  ISMRELTO_MAR's ordinal value */
            ISMRELTO_MAR,

            //  ISMRELTO_MCO's ordinal value */
            ISMRELTO_MCO,

            //  ISMRELTO_MDA's ordinal value */
            ISMRELTO_MDA,

            //  ISMRELTO_MDG's ordinal value */
            ISMRELTO_MDG,

            //  ISMRELTO_MDV's ordinal value */
            ISMRELTO_MDV,

            //  ISMRELTO_MEX's ordinal value */
            ISMRELTO_MEX,

            //  ISMRELTO_MHL's ordinal value */
            ISMRELTO_MHL,

            //  ISMRELTO_MKD's ordinal value */
            ISMRELTO_MKD,

            //  ISMRELTO_MLI's ordinal value */
            ISMRELTO_MLI,

            //  ISMRELTO_MLT's ordinal value */
            ISMRELTO_MLT,

            //  ISMRELTO_MMR's ordinal value */
            ISMRELTO_MMR,

            //  ISMRELTO_MNE's ordinal value */
            ISMRELTO_MNE,

            //  ISMRELTO_MNG's ordinal value */
            ISMRELTO_MNG,

            //  ISMRELTO_MNP's ordinal value */
            ISMRELTO_MNP,

            //  ISMRELTO_MOZ's ordinal value */
            ISMRELTO_MOZ,

            //  ISMRELTO_MRT's ordinal value */
            ISMRELTO_MRT,

            //  ISMRELTO_MSR's ordinal value */
            ISMRELTO_MSR,

            //  ISMRELTO_MTQ's ordinal value */
            ISMRELTO_MTQ,

            //  ISMRELTO_MUS's ordinal value */
            ISMRELTO_MUS,

            //  ISMRELTO_MWI's ordinal value */
            ISMRELTO_MWI,

            //  ISMRELTO_MYS's ordinal value */
            ISMRELTO_MYS,

            //  ISMRELTO_MYT's ordinal value */
            ISMRELTO_MYT,

            //  ISMRELTO_NAM's ordinal value */
            ISMRELTO_NAM,

            //  ISMRELTO_NCL's ordinal value */
            ISMRELTO_NCL,

            //  ISMRELTO_NER's ordinal value */
            ISMRELTO_NER,

            //  ISMRELTO_NFK's ordinal value */
            ISMRELTO_NFK,

            //  ISMRELTO_NGA's ordinal value */
            ISMRELTO_NGA,

            //  ISMRELTO_NIC's ordinal value */
            ISMRELTO_NIC,

            //  ISMRELTO_NIU's ordinal value */
            ISMRELTO_NIU,

            //  ISMRELTO_NLD's ordinal value */
            ISMRELTO_NLD,

            //  ISMRELTO_NOR's ordinal value */
            ISMRELTO_NOR,

            //  ISMRELTO_NPL's ordinal value */
            ISMRELTO_NPL,

            //  ISMRELTO_NRU's ordinal value */
            ISMRELTO_NRU,

            //  ISMRELTO_NZL's ordinal value */
            ISMRELTO_NZL,

            //  ISMRELTO_OMN's ordinal value */
            ISMRELTO_OMN,

            //  ISMRELTO_PAK's ordinal value */
            ISMRELTO_PAK,

            //  ISMRELTO_PAN's ordinal value */
            ISMRELTO_PAN,

            //  ISMRELTO_PCN's ordinal value */
            ISMRELTO_PCN,

            //  ISMRELTO_PER's ordinal value */
            ISMRELTO_PER,

            //  ISMRELTO_PHL's ordinal value */
            ISMRELTO_PHL,

            //  ISMRELTO_PLW's ordinal value */
            ISMRELTO_PLW,

            //  ISMRELTO_PNG's ordinal value */
            ISMRELTO_PNG,

            //  ISMRELTO_POL's ordinal value */
            ISMRELTO_POL,

            //  ISMRELTO_PRI's ordinal value */
            ISMRELTO_PRI,

            //  ISMRELTO_PRK's ordinal value */
            ISMRELTO_PRK,

            //  ISMRELTO_PRT's ordinal value */
            ISMRELTO_PRT,

            //  ISMRELTO_PRY's ordinal value */
            ISMRELTO_PRY,

            //  ISMRELTO_PSE's ordinal value */
            ISMRELTO_PSE,

            //  ISMRELTO_PYF's ordinal value */
            ISMRELTO_PYF,

            //  ISMRELTO_QAT's ordinal value */
            ISMRELTO_QAT,

            //  ISMRELTO_REU's ordinal value */
            ISMRELTO_REU,

            //  ISMRELTO_ROU's ordinal value */
            ISMRELTO_ROU,

            //  ISMRELTO_RUS's ordinal value */
            ISMRELTO_RUS,

            //  ISMRELTO_RWA's ordinal value */
            ISMRELTO_RWA,

            //  ISMRELTO_SAU's ordinal value */
            ISMRELTO_SAU,

            //  ISMRELTO_SDN's ordinal value */
            ISMRELTO_SDN,

            //  ISMRELTO_SEN's ordinal value */
            ISMRELTO_SEN,

            //  ISMRELTO_SGP's ordinal value */
            ISMRELTO_SGP,

            //  ISMRELTO_SGS's ordinal value */
            ISMRELTO_SGS,

            //  ISMRELTO_SHN's ordinal value */
            ISMRELTO_SHN,

            //  ISMRELTO_SJM's ordinal value */
            ISMRELTO_SJM,

            //  ISMRELTO_SLB's ordinal value */
            ISMRELTO_SLB,

            //  ISMRELTO_SLE's ordinal value */
            ISMRELTO_SLE,

            //  ISMRELTO_SLV's ordinal value */
            ISMRELTO_SLV,

            //  ISMRELTO_SMR's ordinal value */
            ISMRELTO_SMR,

            //  ISMRELTO_SOM's ordinal value */
            ISMRELTO_SOM,

            //  ISMRELTO_SPM's ordinal value */
            ISMRELTO_SPM,

            //  ISMRELTO_SRB's ordinal value */
            ISMRELTO_SRB,

            //  ISMRELTO_SSD's ordinal value */
            ISMRELTO_SSD,

            //  ISMRELTO_STP's ordinal value */
            ISMRELTO_STP,

            //  ISMRELTO_SUR's ordinal value */
            ISMRELTO_SUR,

            //  ISMRELTO_SVK's ordinal value */
            ISMRELTO_SVK,

            //  ISMRELTO_SVN's ordinal value */
            ISMRELTO_SVN,

            //  ISMRELTO_SWE's ordinal value */
            ISMRELTO_SWE,

            //  ISMRELTO_SWZ's ordinal value */
            ISMRELTO_SWZ,

            //  ISMRELTO_SXM's ordinal value */
            ISMRELTO_SXM,

            //  ISMRELTO_SYC's ordinal value */
            ISMRELTO_SYC,

            //  ISMRELTO_SYR's ordinal value */
            ISMRELTO_SYR,

            //  ISMRELTO_TCA's ordinal value */
            ISMRELTO_TCA,

            //  ISMRELTO_TCD's ordinal value */
            ISMRELTO_TCD,

            //  ISMRELTO_TGO's ordinal value */
            ISMRELTO_TGO,

            //  ISMRELTO_THA's ordinal value */
            ISMRELTO_THA,

            //  ISMRELTO_TJK's ordinal value */
            ISMRELTO_TJK,

            //  ISMRELTO_TKL's ordinal value */
            ISMRELTO_TKL,

            //  ISMRELTO_TKM's ordinal value */
            ISMRELTO_TKM,

            //  ISMRELTO_TLS's ordinal value */
            ISMRELTO_TLS,

            //  ISMRELTO_TON's ordinal value */
            ISMRELTO_TON,

            //  ISMRELTO_TTO's ordinal value */
            ISMRELTO_TTO,

            //  ISMRELTO_TUN's ordinal value */
            ISMRELTO_TUN,

            //  ISMRELTO_TUR's ordinal value */
            ISMRELTO_TUR,

            //  ISMRELTO_TUV's ordinal value */
            ISMRELTO_TUV,

            //  ISMRELTO_TWN's ordinal value */
            ISMRELTO_TWN,

            //  ISMRELTO_TZA's ordinal value */
            ISMRELTO_TZA,

            //  ISMRELTO_UGA's ordinal value */
            ISMRELTO_UGA,

            //  ISMRELTO_UKR's ordinal value */
            ISMRELTO_UKR,

            //  ISMRELTO_UMI's ordinal value */
            ISMRELTO_UMI,

            //  ISMRELTO_URY's ordinal value */
            ISMRELTO_URY,

            //  ISMRELTO_UZB's ordinal value */
            ISMRELTO_UZB,

            //  ISMRELTO_VAT's ordinal value */
            ISMRELTO_VAT,

            //  ISMRELTO_VCT's ordinal value */
            ISMRELTO_VCT,

            //  ISMRELTO_VEN's ordinal value */
            ISMRELTO_VEN,

            //  ISMRELTO_VGB's ordinal value */
            ISMRELTO_VGB,

            //  ISMRELTO_VIR's ordinal value */
            ISMRELTO_VIR,

            //  ISMRELTO_VNM's ordinal value */
            ISMRELTO_VNM,

            //  ISMRELTO_VUT's ordinal value */
            ISMRELTO_VUT,

            //  ISMRELTO_WLF's ordinal value */
            ISMRELTO_WLF,

            //  ISMRELTO_WSM's ordinal value */
            ISMRELTO_WSM,

            //  ISMRELTO_YEM's ordinal value */
            ISMRELTO_YEM,

            //  ISMRELTO_ZAF's ordinal value */
            ISMRELTO_ZAF,

            //  ISMRELTO_ZMB's ordinal value */
            ISMRELTO_ZMB,

            //  ISMRELTO_ZWE's ordinal value */
            ISMRELTO_ZWE,

            //  ISMRELTO_ACGU's ordinal value */
            ISMRELTO_ACGU,

            //  ISMRELTO_APFS's ordinal value */
            ISMRELTO_APFS,

            //  ISMRELTO_BWCS's ordinal value */
            ISMRELTO_BWCS,

            //  ISMRELTO_CFCK's ordinal value */
            ISMRELTO_CFCK,

            //  ISMRELTO_CFUP's ordinal value */
            ISMRELTO_CFUP,

            //  ISMRELTO_CMFC's ordinal value */
            ISMRELTO_CMFC,

            //  ISMRELTO_CMFP's ordinal value */
            ISMRELTO_CMFP,

            //  ISMRELTO_CPMT's ordinal value */
            ISMRELTO_CPMT,

            //  ISMRELTO_CWCS's ordinal value */
            ISMRELTO_CWCS,

            //  ISMRELTO_EFOR's ordinal value */
            ISMRELTO_EFOR,

            //  ISMRELTO_EUDA's ordinal value */
            ISMRELTO_EUDA,

            //  ISMRELTO_FVEY's ordinal value */
            ISMRELTO_FVEY,

            //  ISMRELTO_GCTF's ordinal value */
            ISMRELTO_GCTF,

            //  ISMRELTO_GMIF's ordinal value */
            ISMRELTO_GMIF,

            //  ISMRELTO_IESC's ordinal value */
            ISMRELTO_IESC,

            //  ISMRELTO_ISAF's ordinal value */
            ISMRELTO_ISAF,

            //  ISMRELTO_KFOR's ordinal value */
            ISMRELTO_KFOR,

            //  ISMRELTO_MIFH's ordinal value */
            ISMRELTO_MIFH,

            //  ISMRELTO_MLEC's ordinal value */
            ISMRELTO_MLEC,

            //  ISMRELTO_NACT's ordinal value */
            ISMRELTO_NACT,

            //  ISMRELTO_NATO's ordinal value */
            ISMRELTO_NATO,

            //  ISMRELTO_OSTY's ordinal value */
            ISMRELTO_OSTY,

            //  ISMRELTO_SPAA's ordinal value */
            ISMRELTO_SPAA,

            //  ISMRELTO_TEYE's ordinal value */
            ISMRELTO_TEYE,

            //  ISMRELTO_UNCK's ordinal value */
            ISMRELTO_UNCK,

            //  Enumerated item that indicates maximum enumerated item exclusive of this item
            enumMaxExclusive
         };


         /** The destructor */
         virtual ~CVEnumISMRelToValues()
         { }

         /** Returns this accessor's type constant, i.e. cVEnumISMRelToValues
           *
           * @return This accessor's type constant, i.e. cVEnumISMRelToValues
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return ismrelto::type::accessorType::cVEnumISMRelToValues;
         }


         /** The assignment operator. Sets the contents of this CVEnumISMRelToValues to the contents of the CVEnumISMRelToValues
           * on the right hand side (rhs) of the assignment operator.CVEnumISMRelToValues
           *
           * @param rhs The CVEnumISMRelToValues on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this CVEnumISMRelToValues
           * @return A constant reference to this CVEnumISMRelToValues.
           */
         const CVEnumISMRelToValues& operator=(const CVEnumISMRelToValues& rhs)
         {
            setValue(rhs.getValue());
            return *this;
         }


         /** The assignment operator. Sets the value of this CVEnumISMRelToValues enumeration to the EnumerationItem on the right
           * hand side of the assignment operator.
           *
           * @param rhs The EnumerationItemwhose value is to be assigned to this CVEnumISMRelToValues enumeration
           * @return A reference to this CVEnumISMRelToValues enumeration
           */
         const CVEnumISMRelToValues& operator=(EnumerationItem rhs)
         {
            setValue(rhs);
            return *this;
         }


         /** Sets the value of this CVEnumISMRelToValues enumeration to the value of the specified enumeration item.
           *
           * @param item The EnumerationItemwhose value is to be used to set the value of this CVEnumISMRelToValues enumeration
           */
         virtual void setValue(EnumerationItem item)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of this CVEnumISMRelToValues enumeration.
           *
           * @param testForValidity=true Specifies whether this CVEnumISMRelToValues enumeration should be validated (true) or not
           *      (false) as part of retrieving its value.
           * @return The value of this CVEnumISMRelToValues enumeration.
           */
         virtual EnumerationItem getValue(bool testForValidity=true) const
            throw(uci::base::UCIException) = 0;


         /** Returns the number of items in this CVEnumISMRelToValues enumeration.
           *
           * @return The number of items in this CVEnumISMRelToValues enumeration.
           */
         int getNumberOfItems() const
         {
            return 274;
         }


         /** Returns whether this CVEnumISMRelToValues enumeration is valid or not. A CVEnumISMRelToValues enumeration is valid if
           * its value is one of the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @return A flag indicating whether this CVEnumISMRelToValues enumeration is valid (true) or not (false).
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


         /** Tests whether the value of the CVEnumISMRelToValues enumeration on the right hand side of the == operator is equal to
           * the value as this CVEnumISMRelToValues enumeration.
           *
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMRelToValues enumeration.
           * @return True if the value of the specified CVEnumISMRelToValues enumeration (rhs) is equal to the value of this
           *      CVEnumISMRelToValues enumeration, false otherwise
           */
         bool operator==(const CVEnumISMRelToValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() == rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMRelToValues enumeration on the right hand side of the != operator is not
           * equal to the value as this CVEnumISMRelToValues enumeration.
           *
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMRelToValues enumeration.
           * @return True if the value of the specified CVEnumISMRelToValues enumeration (rhs) is not equal to the value of this
           *      CVEnumISMRelToValues enumeration, false otherwise
           */
         bool operator!=(const CVEnumISMRelToValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() != rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMRelToValues enumeration on the right hand side of the < operator is less than
           * the value as this CVEnumISMRelToValues enumeration.
           *
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMRelToValues enumeration.
           * @return True if the value of the specified CVEnumISMRelToValues enumeration (rhs) is less than the value of this
           *      CVEnumISMRelToValues enumeration, false otherwise
           */
         bool operator<(const CVEnumISMRelToValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() < rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMRelToValues enumeration on the right hand side of the <= operator is less
           * than or equal to the value as this CVEnumISMRelToValues enumeration.
           *
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMRelToValues enumeration.
           * @return True if the value of the specified CVEnumISMRelToValues enumeration (rhs) is less than or equal to the value
           *      of this CVEnumISMRelToValues enumeration, false otherwise
           */
         bool operator<=(const CVEnumISMRelToValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() <= rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMRelToValues enumeration on the right hand side of the > operator is greater
           * than the value as this CVEnumISMRelToValues enumeration.
           *
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMRelToValues enumeration.
           * @return True if the value of the specified CVEnumISMRelToValues enumeration (rhs) is greater than the value of this
           *      CVEnumISMRelToValues enumeration, false otherwise
           */
         bool operator>(const CVEnumISMRelToValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() > rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMRelToValues enumeration on the right hand side of the >= operator is greater
           * than or equal to the value as this CVEnumISMRelToValues enumeration.
           *
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMRelToValues enumeration.
           * @return True if the value of the specified CVEnumISMRelToValues enumeration (rhs) is greater than or equal to the
           *      value of this CVEnumISMRelToValues enumeration, false otherwise
           */
         bool operator>=(const CVEnumISMRelToValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() >= rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMRelToValues enumeration is equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMRelToValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is equal to the value of this CVEnumISMRelToValues enumeration,
           *      false otherwise
           */
         bool operator==(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() == rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified CVEnumISMRelToValues
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified CVEnumISMRelToValues
           *      enumeration.
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is equal to the value of the CVEnumISMRelToValues enumeration
           *      (rhs), false otherwise
           */
         friend bool operator==(EnumerationItem lhs, const CVEnumISMRelToValues& rhs)
         {
            testForValidItem(lhs);
            return lhs == rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMRelToValues enumeration is not equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMRelToValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this CVEnumISMRelToValues
           *      enumeration, false otherwise
           */
         bool operator!=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() != rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified
           * CVEnumISMRelToValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified CVEnumISMRelToValues
           *      enumeration.
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the CVEnumISMRelToValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator!=(EnumerationItem lhs, const CVEnumISMRelToValues& rhs)
         {
            testForValidItem(lhs);
            return lhs != rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMRelToValues enumeration is less than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMRelToValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than the value of this CVEnumISMRelToValues
           *      enumeration, false otherwise
           */
         bool operator<(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() < rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than the value of the specified CVEnumISMRelToValues
           * enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified CVEnumISMRelToValues
           *      enumeration.
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than the value of the CVEnumISMRelToValues enumeration
           *      (rhs), false otherwise
           */
         friend bool operator<(EnumerationItem lhs, const CVEnumISMRelToValues& rhs)
         {
            testForValidItem(lhs);
            return lhs < rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMRelToValues enumeration is less than or equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMRelToValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this CVEnumISMRelToValues
           *      enumeration, false otherwise
           */
         bool operator<=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() <= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
           * CVEnumISMRelToValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified CVEnumISMRelToValues
           *      enumeration.
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the CVEnumISMRelToValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator<=(EnumerationItem lhs, const CVEnumISMRelToValues& rhs)
         {
            testForValidItem(lhs);
            return lhs <= rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMRelToValues enumeration is greater than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMRelToValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than the value of this CVEnumISMRelToValues
           *      enumeration, false otherwise
           */
         bool operator>(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() > rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified
           * CVEnumISMRelToValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified CVEnumISMRelToValues
           *      enumeration.
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than the value of the CVEnumISMRelToValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator>(EnumerationItem lhs, const CVEnumISMRelToValues& rhs)
         {
            testForValidItem(lhs);
            return lhs > rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMRelToValues enumeration is greater than or equal to the value of the
           * specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMRelToValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this
           *      CVEnumISMRelToValues enumeration, false otherwise
           */
         bool operator>=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() >= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
           * CVEnumISMRelToValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified CVEnumISMRelToValues
           *      enumeration.
           * @param rhs The CVEnumISMRelToValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the
           *      CVEnumISMRelToValues enumeration (rhs), false otherwise
           */
         friend bool operator>=(EnumerationItem lhs, const CVEnumISMRelToValues& rhs)
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


         /** Return the name of the enumeration item that the value of this CVEnumISMRelToValues enumeration is set to.
           *
           * @return The name of the enumeration item.
           */
         virtual const std::string& toName() const
            throw(uci::base::UCIException)
         {
            return toName(getValue());
         }


         /** Sets the value of this CVEnumISMRelToValues enumeration to the value of the enumeration item having the specified
           * name.
           *
           * @param itemName The name of the enumeration item whose value is to be used to set this CVEnumISMRelToValues
           *      enumeration to
           */
         virtual void setValueFromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            setValue(fromName(itemName));
         }



      protected:

         /** The default constructor. In addition to constructing a new CVEnumISMRelToValues, this constructor also makes sure the
           * conversion tables are initialized
           */
         CVEnumISMRelToValues()
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
           * @param rhs The CVEnumISMRelToValues to copy from
           */
         CVEnumISMRelToValues(const CVEnumISMRelToValues& rhs)
         {
            (void)rhs;
         }

      }; // CVEnumISMRelToValues


   } // Namespace: type
} // Namespace: ismrelto
template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& oStream, const ismrelto::type::CVEnumISMRelToValues& enumeration)
{
   oStream << enumeration.toName();
   return oStream;
}

#endif // Ismrelto__Type__CVEnumISMRelToValues_h

