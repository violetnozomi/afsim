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
#ifndef Ismfgio__Type__CVEnumISMFGIOpenValues_h
#define Ismfgio__Type__CVEnumISMFGIOpenValues_h 1

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
namespace ismfgio {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** (U) UNKNOWN followed by all currently valid ISO Trigraphs except USA in alphabetical order by Trigraph, followed by
        * all currently valid CAPCO Coalition tetragraphs in alphabetical order by tetragraph. PERMISSIBLE VALUES The
        * permissible values for this simple type are defined in the Controlled Value Enumeration: CVEnumISMFGIOpen.xml
        */
      class CVEnumISMFGIOpenValues : public virtual uci::base::Accessor {
      public:

         //  The following are the enumerated items that comprise this enumeration
         enum EnumerationItem {
            //  Enumerated item that indicates that the enumeration has not been set
            enumNotSet,
            //  ISMFGIO_UNKNOWN's ordinal value */
            ISMFGIO_UNKNOWN,

            //  ISMFGIO_ABW's ordinal value */
            ISMFGIO_ABW,

            //  ISMFGIO_AFG's ordinal value */
            ISMFGIO_AFG,

            //  ISMFGIO_AGO's ordinal value */
            ISMFGIO_AGO,

            //  ISMFGIO_AIA's ordinal value */
            ISMFGIO_AIA,

            //  ISMFGIO_ALA's ordinal value */
            ISMFGIO_ALA,

            //  ISMFGIO_ALB's ordinal value */
            ISMFGIO_ALB,

            //  ISMFGIO_AND's ordinal value */
            ISMFGIO_AND,

            //  ISMFGIO_ARE's ordinal value */
            ISMFGIO_ARE,

            //  ISMFGIO_ARG's ordinal value */
            ISMFGIO_ARG,

            //  ISMFGIO_ARM's ordinal value */
            ISMFGIO_ARM,

            //  ISMFGIO_ASM's ordinal value */
            ISMFGIO_ASM,

            //  ISMFGIO_ATA's ordinal value */
            ISMFGIO_ATA,

            //  ISMFGIO_ATF's ordinal value */
            ISMFGIO_ATF,

            //  ISMFGIO_ATG's ordinal value */
            ISMFGIO_ATG,

            //  ISMFGIO_AUS's ordinal value */
            ISMFGIO_AUS,

            //  ISMFGIO_AUT's ordinal value */
            ISMFGIO_AUT,

            //  ISMFGIO_AZE's ordinal value */
            ISMFGIO_AZE,

            //  ISMFGIO_BDI's ordinal value */
            ISMFGIO_BDI,

            //  ISMFGIO_BEL's ordinal value */
            ISMFGIO_BEL,

            //  ISMFGIO_BEN's ordinal value */
            ISMFGIO_BEN,

            //  ISMFGIO_BES's ordinal value */
            ISMFGIO_BES,

            //  ISMFGIO_BFA's ordinal value */
            ISMFGIO_BFA,

            //  ISMFGIO_BGD's ordinal value */
            ISMFGIO_BGD,

            //  ISMFGIO_BGR's ordinal value */
            ISMFGIO_BGR,

            //  ISMFGIO_BHR's ordinal value */
            ISMFGIO_BHR,

            //  ISMFGIO_BHS's ordinal value */
            ISMFGIO_BHS,

            //  ISMFGIO_BIH's ordinal value */
            ISMFGIO_BIH,

            //  ISMFGIO_BLM's ordinal value */
            ISMFGIO_BLM,

            //  ISMFGIO_BLR's ordinal value */
            ISMFGIO_BLR,

            //  ISMFGIO_BLZ's ordinal value */
            ISMFGIO_BLZ,

            //  ISMFGIO_BMU's ordinal value */
            ISMFGIO_BMU,

            //  ISMFGIO_BOL's ordinal value */
            ISMFGIO_BOL,

            //  ISMFGIO_BRA's ordinal value */
            ISMFGIO_BRA,

            //  ISMFGIO_BRB's ordinal value */
            ISMFGIO_BRB,

            //  ISMFGIO_BRN's ordinal value */
            ISMFGIO_BRN,

            //  ISMFGIO_BTN's ordinal value */
            ISMFGIO_BTN,

            //  ISMFGIO_BVT's ordinal value */
            ISMFGIO_BVT,

            //  ISMFGIO_BWA's ordinal value */
            ISMFGIO_BWA,

            //  ISMFGIO_CAF's ordinal value */
            ISMFGIO_CAF,

            //  ISMFGIO_CAN's ordinal value */
            ISMFGIO_CAN,

            //  ISMFGIO_CCK's ordinal value */
            ISMFGIO_CCK,

            //  ISMFGIO_CHE's ordinal value */
            ISMFGIO_CHE,

            //  ISMFGIO_CHL's ordinal value */
            ISMFGIO_CHL,

            //  ISMFGIO_CHN's ordinal value */
            ISMFGIO_CHN,

            //  ISMFGIO_CIV's ordinal value */
            ISMFGIO_CIV,

            //  ISMFGIO_CMR's ordinal value */
            ISMFGIO_CMR,

            //  ISMFGIO_COD's ordinal value */
            ISMFGIO_COD,

            //  ISMFGIO_COG's ordinal value */
            ISMFGIO_COG,

            //  ISMFGIO_COK's ordinal value */
            ISMFGIO_COK,

            //  ISMFGIO_COL's ordinal value */
            ISMFGIO_COL,

            //  ISMFGIO_COM's ordinal value */
            ISMFGIO_COM,

            //  ISMFGIO_CPV's ordinal value */
            ISMFGIO_CPV,

            //  ISMFGIO_CRI's ordinal value */
            ISMFGIO_CRI,

            //  ISMFGIO_CUB's ordinal value */
            ISMFGIO_CUB,

            //  ISMFGIO_CUW's ordinal value */
            ISMFGIO_CUW,

            //  ISMFGIO_CXR's ordinal value */
            ISMFGIO_CXR,

            //  ISMFGIO_CYM's ordinal value */
            ISMFGIO_CYM,

            //  ISMFGIO_CYP's ordinal value */
            ISMFGIO_CYP,

            //  ISMFGIO_CZE's ordinal value */
            ISMFGIO_CZE,

            //  ISMFGIO_DEU's ordinal value */
            ISMFGIO_DEU,

            //  ISMFGIO_DJI's ordinal value */
            ISMFGIO_DJI,

            //  ISMFGIO_DMA's ordinal value */
            ISMFGIO_DMA,

            //  ISMFGIO_DNK's ordinal value */
            ISMFGIO_DNK,

            //  ISMFGIO_DOM's ordinal value */
            ISMFGIO_DOM,

            //  ISMFGIO_DZA's ordinal value */
            ISMFGIO_DZA,

            //  ISMFGIO_ECU's ordinal value */
            ISMFGIO_ECU,

            //  ISMFGIO_EGY's ordinal value */
            ISMFGIO_EGY,

            //  ISMFGIO_ERI's ordinal value */
            ISMFGIO_ERI,

            //  ISMFGIO_ESH's ordinal value */
            ISMFGIO_ESH,

            //  ISMFGIO_ESP's ordinal value */
            ISMFGIO_ESP,

            //  ISMFGIO_EST's ordinal value */
            ISMFGIO_EST,

            //  ISMFGIO_ETH's ordinal value */
            ISMFGIO_ETH,

            //  ISMFGIO_FIN's ordinal value */
            ISMFGIO_FIN,

            //  ISMFGIO_FJI's ordinal value */
            ISMFGIO_FJI,

            //  ISMFGIO_FLK's ordinal value */
            ISMFGIO_FLK,

            //  ISMFGIO_FRA's ordinal value */
            ISMFGIO_FRA,

            //  ISMFGIO_FRO's ordinal value */
            ISMFGIO_FRO,

            //  ISMFGIO_FSM's ordinal value */
            ISMFGIO_FSM,

            //  ISMFGIO_GAB's ordinal value */
            ISMFGIO_GAB,

            //  ISMFGIO_GBR's ordinal value */
            ISMFGIO_GBR,

            //  ISMFGIO_GEO's ordinal value */
            ISMFGIO_GEO,

            //  ISMFGIO_GGY's ordinal value */
            ISMFGIO_GGY,

            //  ISMFGIO_GHA's ordinal value */
            ISMFGIO_GHA,

            //  ISMFGIO_GIB's ordinal value */
            ISMFGIO_GIB,

            //  ISMFGIO_GIN's ordinal value */
            ISMFGIO_GIN,

            //  ISMFGIO_GLP's ordinal value */
            ISMFGIO_GLP,

            //  ISMFGIO_GMB's ordinal value */
            ISMFGIO_GMB,

            //  ISMFGIO_GNB's ordinal value */
            ISMFGIO_GNB,

            //  ISMFGIO_GNQ's ordinal value */
            ISMFGIO_GNQ,

            //  ISMFGIO_GRC's ordinal value */
            ISMFGIO_GRC,

            //  ISMFGIO_GRD's ordinal value */
            ISMFGIO_GRD,

            //  ISMFGIO_GRL's ordinal value */
            ISMFGIO_GRL,

            //  ISMFGIO_GTM's ordinal value */
            ISMFGIO_GTM,

            //  ISMFGIO_GUF's ordinal value */
            ISMFGIO_GUF,

            //  ISMFGIO_GUM's ordinal value */
            ISMFGIO_GUM,

            //  ISMFGIO_GUY's ordinal value */
            ISMFGIO_GUY,

            //  ISMFGIO_HKG's ordinal value */
            ISMFGIO_HKG,

            //  ISMFGIO_HMD's ordinal value */
            ISMFGIO_HMD,

            //  ISMFGIO_HND's ordinal value */
            ISMFGIO_HND,

            //  ISMFGIO_HRV's ordinal value */
            ISMFGIO_HRV,

            //  ISMFGIO_HTI's ordinal value */
            ISMFGIO_HTI,

            //  ISMFGIO_HUN's ordinal value */
            ISMFGIO_HUN,

            //  ISMFGIO_IDN's ordinal value */
            ISMFGIO_IDN,

            //  ISMFGIO_IMN's ordinal value */
            ISMFGIO_IMN,

            //  ISMFGIO_IND's ordinal value */
            ISMFGIO_IND,

            //  ISMFGIO_IOT's ordinal value */
            ISMFGIO_IOT,

            //  ISMFGIO_IRL's ordinal value */
            ISMFGIO_IRL,

            //  ISMFGIO_IRN's ordinal value */
            ISMFGIO_IRN,

            //  ISMFGIO_IRQ's ordinal value */
            ISMFGIO_IRQ,

            //  ISMFGIO_ISL's ordinal value */
            ISMFGIO_ISL,

            //  ISMFGIO_ISR's ordinal value */
            ISMFGIO_ISR,

            //  ISMFGIO_ITA's ordinal value */
            ISMFGIO_ITA,

            //  ISMFGIO_JAM's ordinal value */
            ISMFGIO_JAM,

            //  ISMFGIO_JEY's ordinal value */
            ISMFGIO_JEY,

            //  ISMFGIO_JOR's ordinal value */
            ISMFGIO_JOR,

            //  ISMFGIO_JPN's ordinal value */
            ISMFGIO_JPN,

            //  ISMFGIO_KAZ's ordinal value */
            ISMFGIO_KAZ,

            //  ISMFGIO_KEN's ordinal value */
            ISMFGIO_KEN,

            //  ISMFGIO_KGZ's ordinal value */
            ISMFGIO_KGZ,

            //  ISMFGIO_KHM's ordinal value */
            ISMFGIO_KHM,

            //  ISMFGIO_KIR's ordinal value */
            ISMFGIO_KIR,

            //  ISMFGIO_KNA's ordinal value */
            ISMFGIO_KNA,

            //  ISMFGIO_KOR's ordinal value */
            ISMFGIO_KOR,

            //  ISMFGIO_KWT's ordinal value */
            ISMFGIO_KWT,

            //  ISMFGIO_LAO's ordinal value */
            ISMFGIO_LAO,

            //  ISMFGIO_LBN's ordinal value */
            ISMFGIO_LBN,

            //  ISMFGIO_LBR's ordinal value */
            ISMFGIO_LBR,

            //  ISMFGIO_LBY's ordinal value */
            ISMFGIO_LBY,

            //  ISMFGIO_LCA's ordinal value */
            ISMFGIO_LCA,

            //  ISMFGIO_LIE's ordinal value */
            ISMFGIO_LIE,

            //  ISMFGIO_LKA's ordinal value */
            ISMFGIO_LKA,

            //  ISMFGIO_LSO's ordinal value */
            ISMFGIO_LSO,

            //  ISMFGIO_LTU's ordinal value */
            ISMFGIO_LTU,

            //  ISMFGIO_LUX's ordinal value */
            ISMFGIO_LUX,

            //  ISMFGIO_LVA's ordinal value */
            ISMFGIO_LVA,

            //  ISMFGIO_MAC's ordinal value */
            ISMFGIO_MAC,

            //  ISMFGIO_MAF's ordinal value */
            ISMFGIO_MAF,

            //  ISMFGIO_MAR's ordinal value */
            ISMFGIO_MAR,

            //  ISMFGIO_MCO's ordinal value */
            ISMFGIO_MCO,

            //  ISMFGIO_MDA's ordinal value */
            ISMFGIO_MDA,

            //  ISMFGIO_MDG's ordinal value */
            ISMFGIO_MDG,

            //  ISMFGIO_MDV's ordinal value */
            ISMFGIO_MDV,

            //  ISMFGIO_MEX's ordinal value */
            ISMFGIO_MEX,

            //  ISMFGIO_MHL's ordinal value */
            ISMFGIO_MHL,

            //  ISMFGIO_MKD's ordinal value */
            ISMFGIO_MKD,

            //  ISMFGIO_MLI's ordinal value */
            ISMFGIO_MLI,

            //  ISMFGIO_MLT's ordinal value */
            ISMFGIO_MLT,

            //  ISMFGIO_MMR's ordinal value */
            ISMFGIO_MMR,

            //  ISMFGIO_MNE's ordinal value */
            ISMFGIO_MNE,

            //  ISMFGIO_MNG's ordinal value */
            ISMFGIO_MNG,

            //  ISMFGIO_MNP's ordinal value */
            ISMFGIO_MNP,

            //  ISMFGIO_MOZ's ordinal value */
            ISMFGIO_MOZ,

            //  ISMFGIO_MRT's ordinal value */
            ISMFGIO_MRT,

            //  ISMFGIO_MSR's ordinal value */
            ISMFGIO_MSR,

            //  ISMFGIO_MTQ's ordinal value */
            ISMFGIO_MTQ,

            //  ISMFGIO_MUS's ordinal value */
            ISMFGIO_MUS,

            //  ISMFGIO_MWI's ordinal value */
            ISMFGIO_MWI,

            //  ISMFGIO_MYS's ordinal value */
            ISMFGIO_MYS,

            //  ISMFGIO_MYT's ordinal value */
            ISMFGIO_MYT,

            //  ISMFGIO_NAM's ordinal value */
            ISMFGIO_NAM,

            //  ISMFGIO_NCL's ordinal value */
            ISMFGIO_NCL,

            //  ISMFGIO_NER's ordinal value */
            ISMFGIO_NER,

            //  ISMFGIO_NFK's ordinal value */
            ISMFGIO_NFK,

            //  ISMFGIO_NGA's ordinal value */
            ISMFGIO_NGA,

            //  ISMFGIO_NIC's ordinal value */
            ISMFGIO_NIC,

            //  ISMFGIO_NIU's ordinal value */
            ISMFGIO_NIU,

            //  ISMFGIO_NLD's ordinal value */
            ISMFGIO_NLD,

            //  ISMFGIO_NOR's ordinal value */
            ISMFGIO_NOR,

            //  ISMFGIO_NPL's ordinal value */
            ISMFGIO_NPL,

            //  ISMFGIO_NRU's ordinal value */
            ISMFGIO_NRU,

            //  ISMFGIO_NZL's ordinal value */
            ISMFGIO_NZL,

            //  ISMFGIO_OMN's ordinal value */
            ISMFGIO_OMN,

            //  ISMFGIO_PAK's ordinal value */
            ISMFGIO_PAK,

            //  ISMFGIO_PAN's ordinal value */
            ISMFGIO_PAN,

            //  ISMFGIO_PCN's ordinal value */
            ISMFGIO_PCN,

            //  ISMFGIO_PER's ordinal value */
            ISMFGIO_PER,

            //  ISMFGIO_PHL's ordinal value */
            ISMFGIO_PHL,

            //  ISMFGIO_PLW's ordinal value */
            ISMFGIO_PLW,

            //  ISMFGIO_PNG's ordinal value */
            ISMFGIO_PNG,

            //  ISMFGIO_POL's ordinal value */
            ISMFGIO_POL,

            //  ISMFGIO_PRI's ordinal value */
            ISMFGIO_PRI,

            //  ISMFGIO_PRK's ordinal value */
            ISMFGIO_PRK,

            //  ISMFGIO_PRT's ordinal value */
            ISMFGIO_PRT,

            //  ISMFGIO_PRY's ordinal value */
            ISMFGIO_PRY,

            //  ISMFGIO_PSE's ordinal value */
            ISMFGIO_PSE,

            //  ISMFGIO_PYF's ordinal value */
            ISMFGIO_PYF,

            //  ISMFGIO_QAT's ordinal value */
            ISMFGIO_QAT,

            //  ISMFGIO_REU's ordinal value */
            ISMFGIO_REU,

            //  ISMFGIO_ROU's ordinal value */
            ISMFGIO_ROU,

            //  ISMFGIO_RUS's ordinal value */
            ISMFGIO_RUS,

            //  ISMFGIO_RWA's ordinal value */
            ISMFGIO_RWA,

            //  ISMFGIO_SAU's ordinal value */
            ISMFGIO_SAU,

            //  ISMFGIO_SDN's ordinal value */
            ISMFGIO_SDN,

            //  ISMFGIO_SEN's ordinal value */
            ISMFGIO_SEN,

            //  ISMFGIO_SGP's ordinal value */
            ISMFGIO_SGP,

            //  ISMFGIO_SGS's ordinal value */
            ISMFGIO_SGS,

            //  ISMFGIO_SHN's ordinal value */
            ISMFGIO_SHN,

            //  ISMFGIO_SJM's ordinal value */
            ISMFGIO_SJM,

            //  ISMFGIO_SLB's ordinal value */
            ISMFGIO_SLB,

            //  ISMFGIO_SLE's ordinal value */
            ISMFGIO_SLE,

            //  ISMFGIO_SLV's ordinal value */
            ISMFGIO_SLV,

            //  ISMFGIO_SMR's ordinal value */
            ISMFGIO_SMR,

            //  ISMFGIO_SOM's ordinal value */
            ISMFGIO_SOM,

            //  ISMFGIO_SPM's ordinal value */
            ISMFGIO_SPM,

            //  ISMFGIO_SRB's ordinal value */
            ISMFGIO_SRB,

            //  ISMFGIO_SSD's ordinal value */
            ISMFGIO_SSD,

            //  ISMFGIO_STP's ordinal value */
            ISMFGIO_STP,

            //  ISMFGIO_SUR's ordinal value */
            ISMFGIO_SUR,

            //  ISMFGIO_SVK's ordinal value */
            ISMFGIO_SVK,

            //  ISMFGIO_SVN's ordinal value */
            ISMFGIO_SVN,

            //  ISMFGIO_SWE's ordinal value */
            ISMFGIO_SWE,

            //  ISMFGIO_SWZ's ordinal value */
            ISMFGIO_SWZ,

            //  ISMFGIO_SXM's ordinal value */
            ISMFGIO_SXM,

            //  ISMFGIO_SYC's ordinal value */
            ISMFGIO_SYC,

            //  ISMFGIO_SYR's ordinal value */
            ISMFGIO_SYR,

            //  ISMFGIO_TCA's ordinal value */
            ISMFGIO_TCA,

            //  ISMFGIO_TCD's ordinal value */
            ISMFGIO_TCD,

            //  ISMFGIO_TGO's ordinal value */
            ISMFGIO_TGO,

            //  ISMFGIO_THA's ordinal value */
            ISMFGIO_THA,

            //  ISMFGIO_TJK's ordinal value */
            ISMFGIO_TJK,

            //  ISMFGIO_TKL's ordinal value */
            ISMFGIO_TKL,

            //  ISMFGIO_TKM's ordinal value */
            ISMFGIO_TKM,

            //  ISMFGIO_TLS's ordinal value */
            ISMFGIO_TLS,

            //  ISMFGIO_TON's ordinal value */
            ISMFGIO_TON,

            //  ISMFGIO_TTO's ordinal value */
            ISMFGIO_TTO,

            //  ISMFGIO_TUN's ordinal value */
            ISMFGIO_TUN,

            //  ISMFGIO_TUR's ordinal value */
            ISMFGIO_TUR,

            //  ISMFGIO_TUV's ordinal value */
            ISMFGIO_TUV,

            //  ISMFGIO_TWN's ordinal value */
            ISMFGIO_TWN,

            //  ISMFGIO_TZA's ordinal value */
            ISMFGIO_TZA,

            //  ISMFGIO_UGA's ordinal value */
            ISMFGIO_UGA,

            //  ISMFGIO_UKR's ordinal value */
            ISMFGIO_UKR,

            //  ISMFGIO_UMI's ordinal value */
            ISMFGIO_UMI,

            //  ISMFGIO_URY's ordinal value */
            ISMFGIO_URY,

            //  ISMFGIO_UZB's ordinal value */
            ISMFGIO_UZB,

            //  ISMFGIO_VAT's ordinal value */
            ISMFGIO_VAT,

            //  ISMFGIO_VCT's ordinal value */
            ISMFGIO_VCT,

            //  ISMFGIO_VEN's ordinal value */
            ISMFGIO_VEN,

            //  ISMFGIO_VGB's ordinal value */
            ISMFGIO_VGB,

            //  ISMFGIO_VIR's ordinal value */
            ISMFGIO_VIR,

            //  ISMFGIO_VNM's ordinal value */
            ISMFGIO_VNM,

            //  ISMFGIO_VUT's ordinal value */
            ISMFGIO_VUT,

            //  ISMFGIO_WLF's ordinal value */
            ISMFGIO_WLF,

            //  ISMFGIO_WSM's ordinal value */
            ISMFGIO_WSM,

            //  ISMFGIO_YEM's ordinal value */
            ISMFGIO_YEM,

            //  ISMFGIO_ZAF's ordinal value */
            ISMFGIO_ZAF,

            //  ISMFGIO_ZMB's ordinal value */
            ISMFGIO_ZMB,

            //  ISMFGIO_ZWE's ordinal value */
            ISMFGIO_ZWE,

            //  ISMFGIO_ACGU's ordinal value */
            ISMFGIO_ACGU,

            //  ISMFGIO_APFS's ordinal value */
            ISMFGIO_APFS,

            //  ISMFGIO_BWCS's ordinal value */
            ISMFGIO_BWCS,

            //  ISMFGIO_CFCK's ordinal value */
            ISMFGIO_CFCK,

            //  ISMFGIO_CFUP's ordinal value */
            ISMFGIO_CFUP,

            //  ISMFGIO_CMFC's ordinal value */
            ISMFGIO_CMFC,

            //  ISMFGIO_CMFP's ordinal value */
            ISMFGIO_CMFP,

            //  ISMFGIO_CPMT's ordinal value */
            ISMFGIO_CPMT,

            //  ISMFGIO_CWCS's ordinal value */
            ISMFGIO_CWCS,

            //  ISMFGIO_EFOR's ordinal value */
            ISMFGIO_EFOR,

            //  ISMFGIO_EUDA's ordinal value */
            ISMFGIO_EUDA,

            //  ISMFGIO_FVEY's ordinal value */
            ISMFGIO_FVEY,

            //  ISMFGIO_GCTF's ordinal value */
            ISMFGIO_GCTF,

            //  ISMFGIO_GMIF's ordinal value */
            ISMFGIO_GMIF,

            //  ISMFGIO_IESC's ordinal value */
            ISMFGIO_IESC,

            //  ISMFGIO_ISAF's ordinal value */
            ISMFGIO_ISAF,

            //  ISMFGIO_KFOR's ordinal value */
            ISMFGIO_KFOR,

            //  ISMFGIO_MIFH's ordinal value */
            ISMFGIO_MIFH,

            //  ISMFGIO_MLEC's ordinal value */
            ISMFGIO_MLEC,

            //  ISMFGIO_NACT's ordinal value */
            ISMFGIO_NACT,

            //  ISMFGIO_NATO's ordinal value */
            ISMFGIO_NATO,

            //  ISMFGIO_OSTY's ordinal value */
            ISMFGIO_OSTY,

            //  ISMFGIO_SPAA's ordinal value */
            ISMFGIO_SPAA,

            //  ISMFGIO_TEYE's ordinal value */
            ISMFGIO_TEYE,

            //  ISMFGIO_UNCK's ordinal value */
            ISMFGIO_UNCK,

            //  Enumerated item that indicates maximum enumerated item exclusive of this item
            enumMaxExclusive
         };


         /** The destructor */
         virtual ~CVEnumISMFGIOpenValues()
         { }

         /** Returns this accessor's type constant, i.e. cVEnumISMFGIOpenValues
           *
           * @return This accessor's type constant, i.e. cVEnumISMFGIOpenValues
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return ismfgio::type::accessorType::cVEnumISMFGIOpenValues;
         }


         /** The assignment operator. Sets the contents of this CVEnumISMFGIOpenValues to the contents of the
           * CVEnumISMFGIOpenValues on the right hand side (rhs) of the assignment operator.CVEnumISMFGIOpenValues
           *
           * @param rhs The CVEnumISMFGIOpenValues on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this CVEnumISMFGIOpenValues
           * @return A constant reference to this CVEnumISMFGIOpenValues.
           */
         const CVEnumISMFGIOpenValues& operator=(const CVEnumISMFGIOpenValues& rhs)
         {
            setValue(rhs.getValue());
            return *this;
         }


         /** The assignment operator. Sets the value of this CVEnumISMFGIOpenValues enumeration to the EnumerationItem on the
           * right hand side of the assignment operator.
           *
           * @param rhs The EnumerationItemwhose value is to be assigned to this CVEnumISMFGIOpenValues enumeration
           * @return A reference to this CVEnumISMFGIOpenValues enumeration
           */
         const CVEnumISMFGIOpenValues& operator=(EnumerationItem rhs)
         {
            setValue(rhs);
            return *this;
         }


         /** Sets the value of this CVEnumISMFGIOpenValues enumeration to the value of the specified enumeration item.
           *
           * @param item The EnumerationItemwhose value is to be used to set the value of this CVEnumISMFGIOpenValues enumeration
           */
         virtual void setValue(EnumerationItem item)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of this CVEnumISMFGIOpenValues enumeration.
           *
           * @param testForValidity=true Specifies whether this CVEnumISMFGIOpenValues enumeration should be validated (true) or
           *      not (false) as part of retrieving its value.
           * @return The value of this CVEnumISMFGIOpenValues enumeration.
           */
         virtual EnumerationItem getValue(bool testForValidity=true) const
            throw(uci::base::UCIException) = 0;


         /** Returns the number of items in this CVEnumISMFGIOpenValues enumeration.
           *
           * @return The number of items in this CVEnumISMFGIOpenValues enumeration.
           */
         int getNumberOfItems() const
         {
            return 274;
         }


         /** Returns whether this CVEnumISMFGIOpenValues enumeration is valid or not. A CVEnumISMFGIOpenValues enumeration is
           * valid if its value is one of the enumeration items other than enumNotSet and enumMaxExclusive.
           *
           * @return A flag indicating whether this CVEnumISMFGIOpenValues enumeration is valid (true) or not (false).
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


         /** Tests whether the value of the CVEnumISMFGIOpenValues enumeration on the right hand side of the == operator is equal
           * to the value as this CVEnumISMFGIOpenValues enumeration.
           *
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIOpenValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIOpenValues enumeration (rhs) is equal to the value of this
           *      CVEnumISMFGIOpenValues enumeration, false otherwise
           */
         bool operator==(const CVEnumISMFGIOpenValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() == rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMFGIOpenValues enumeration on the right hand side of the != operator is not
           * equal to the value as this CVEnumISMFGIOpenValues enumeration.
           *
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIOpenValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIOpenValues enumeration (rhs) is not equal to the value of this
           *      CVEnumISMFGIOpenValues enumeration, false otherwise
           */
         bool operator!=(const CVEnumISMFGIOpenValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() != rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMFGIOpenValues enumeration on the right hand side of the < operator is less
           * than the value as this CVEnumISMFGIOpenValues enumeration.
           *
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIOpenValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIOpenValues enumeration (rhs) is less than the value of this
           *      CVEnumISMFGIOpenValues enumeration, false otherwise
           */
         bool operator<(const CVEnumISMFGIOpenValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() < rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMFGIOpenValues enumeration on the right hand side of the <= operator is less
           * than or equal to the value as this CVEnumISMFGIOpenValues enumeration.
           *
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIOpenValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIOpenValues enumeration (rhs) is less than or equal to the
           *      value of this CVEnumISMFGIOpenValues enumeration, false otherwise
           */
         bool operator<=(const CVEnumISMFGIOpenValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() <= rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMFGIOpenValues enumeration on the right hand side of the > operator is greater
           * than the value as this CVEnumISMFGIOpenValues enumeration.
           *
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIOpenValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIOpenValues enumeration (rhs) is greater than the value of this
           *      CVEnumISMFGIOpenValues enumeration, false otherwise
           */
         bool operator>(const CVEnumISMFGIOpenValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() > rhs.getValue();
         }


         /** Tests whether the value of the CVEnumISMFGIOpenValues enumeration on the right hand side of the >= operator is
           * greater than or equal to the value as this CVEnumISMFGIOpenValues enumeration.
           *
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of this
           *      CVEnumISMFGIOpenValues enumeration.
           * @return True if the value of the specified CVEnumISMFGIOpenValues enumeration (rhs) is greater than or equal to the
           *      value of this CVEnumISMFGIOpenValues enumeration, false otherwise
           */
         bool operator>=(const CVEnumISMFGIOpenValues& rhs) const
            throw(uci::base::UCIException)
         {
            return getValue() >= rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIOpenValues enumeration is equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIOpenValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is equal to the value of this CVEnumISMFGIOpenValues
           *      enumeration, false otherwise
           */
         bool operator==(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() == rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is equal to the value of the specified
           * CVEnumISMFGIOpenValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIOpenValues enumeration.
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is equal to the value of the CVEnumISMFGIOpenValues enumeration
           *      (rhs), false otherwise
           */
         friend bool operator==(EnumerationItem lhs, const CVEnumISMFGIOpenValues& rhs)
         {
            testForValidItem(lhs);
            return lhs == rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIOpenValues enumeration is not equal to the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIOpenValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is not equal to the value of this CVEnumISMFGIOpenValues
           *      enumeration, false otherwise
           */
         bool operator!=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() != rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is not equal to the value of the specified
           * CVEnumISMFGIOpenValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIOpenValues enumeration.
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is not equal to the value of the CVEnumISMFGIOpenValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator!=(EnumerationItem lhs, const CVEnumISMFGIOpenValues& rhs)
         {
            testForValidItem(lhs);
            return lhs != rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIOpenValues enumeration is less than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIOpenValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than the value of this CVEnumISMFGIOpenValues
           *      enumeration, false otherwise
           */
         bool operator<(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() < rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than the value of the specified
           * CVEnumISMFGIOpenValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIOpenValues enumeration.
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than the value of the CVEnumISMFGIOpenValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator<(EnumerationItem lhs, const CVEnumISMFGIOpenValues& rhs)
         {
            testForValidItem(lhs);
            return lhs < rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIOpenValues enumeration is less than or equal to the value of the
           * specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIOpenValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is less than or equal to the value of this
           *      CVEnumISMFGIOpenValues enumeration, false otherwise
           */
         bool operator<=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() <= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is less than or equal to the value of the specified
           * CVEnumISMFGIOpenValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIOpenValues enumeration.
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is less than or equal to the value of the
           *      CVEnumISMFGIOpenValues enumeration (rhs), false otherwise
           */
         friend bool operator<=(EnumerationItem lhs, const CVEnumISMFGIOpenValues& rhs)
         {
            testForValidItem(lhs);
            return lhs <= rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIOpenValues enumeration is greater than the value of the specified
           * EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIOpenValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than the value of this CVEnumISMFGIOpenValues
           *      enumeration, false otherwise
           */
         bool operator>(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() > rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than the value of the specified
           * CVEnumISMFGIOpenValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIOpenValues enumeration.
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than the value of the CVEnumISMFGIOpenValues
           *      enumeration (rhs), false otherwise
           */
         friend bool operator>(EnumerationItem lhs, const CVEnumISMFGIOpenValues& rhs)
         {
            testForValidItem(lhs);
            return lhs > rhs.getValue();
         }


         /** Tests whether the value of this CVEnumISMFGIOpenValues enumeration is greater than or equal to the value of the
           * specified EnumerationItem.
           *
           * @param rhs The EnumerationItem whose value is to be compared against the value of this CVEnumISMFGIOpenValues
           *      enumeration.
           * @return True if the value of the EnumerationItem(rhs) is greater than or equal to the value of this
           *      CVEnumISMFGIOpenValues enumeration, false otherwise
           */
         bool operator>=(EnumerationItem rhs) const
            throw(uci::base::UCIException)
         {
            testForValidItem(rhs);
            return getValue() >= rhs;
         }


         /** Tests whether the value of the specified EnumerationItem is greater than or equal to the value of the specified
           * CVEnumISMFGIOpenValues enumeration.
           *
           * @param lhs The EnumerationItem whose value is to be compared against the value of the specified
           *      CVEnumISMFGIOpenValues enumeration.
           * @param rhs The CVEnumISMFGIOpenValues enumeration whose value is to be compared against the value of the specified
           *      EnumerationItem
           * @return True if the value of the EnumerationItem(lhs) is greater than or equal to the value of the
           *      CVEnumISMFGIOpenValues enumeration (rhs), false otherwise
           */
         friend bool operator>=(EnumerationItem lhs, const CVEnumISMFGIOpenValues& rhs)
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


         /** Return the name of the enumeration item that the value of this CVEnumISMFGIOpenValues enumeration is set to.
           *
           * @return The name of the enumeration item.
           */
         virtual const std::string& toName() const
            throw(uci::base::UCIException)
         {
            return toName(getValue());
         }


         /** Sets the value of this CVEnumISMFGIOpenValues enumeration to the value of the enumeration item having the specified
           * name.
           *
           * @param itemName The name of the enumeration item whose value is to be used to set this CVEnumISMFGIOpenValues
           *      enumeration to
           */
         virtual void setValueFromName(const std::string& itemName)
            throw(uci::base::UCIException)
         {
            setValue(fromName(itemName));
         }



      protected:

         /** The default constructor. In addition to constructing a new CVEnumISMFGIOpenValues, this constructor also makes sure
           * the conversion tables are initialized
           */
         CVEnumISMFGIOpenValues()
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
           * @param rhs The CVEnumISMFGIOpenValues to copy from
           */
         CVEnumISMFGIOpenValues(const CVEnumISMFGIOpenValues& rhs)
         {
            (void)rhs;
         }

      }; // CVEnumISMFGIOpenValues


   } // Namespace: type
} // Namespace: ismfgio
template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& oStream, const ismfgio::type::CVEnumISMFGIOpenValues& enumeration)
{
   oStream << enumeration.toName();
   return oStream;
}

#endif // Ismfgio__Type__CVEnumISMFGIOpenValues_h

