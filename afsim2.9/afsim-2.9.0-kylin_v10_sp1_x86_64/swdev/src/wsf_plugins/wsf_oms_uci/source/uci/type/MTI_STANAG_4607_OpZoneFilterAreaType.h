// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__MTI_STANAG_4607_OpZoneFilterAreaType_h
#define Uci__Type__MTI_STANAG_4607_OpZoneFilterAreaType_h 1

#if !defined(Uci__Type__MTI_OpZoneFilterAreaType_h)
# include "uci/type/MTI_OpZoneFilterAreaType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__STANAG_4607_TargetClassificationEnum_h)
# include "uci/type/STANAG_4607_TargetClassificationEnum.h"
#endif

#if !defined(Uci__Type__STANAG_4607_MissionType_h)
# include "uci/type/STANAG_4607_MissionType.h"
#endif

#if !defined(Uci__Type__STANAG_4607_PlatformTypeEnum_h)
# include "uci/type/STANAG_4607_PlatformTypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This element represents a filtered area for raw STANAG 4607 measurements. This type extends MTI_OpZoneFilterAreaType
        * to provide Moving Target Indicator (MTI) parameters specific to STANAG 4607.
        */
      class MTI_STANAG_4607_OpZoneFilterAreaType : public virtual uci::type::MTI_OpZoneFilterAreaType {
      public:

         /** The destructor */
         virtual ~MTI_STANAG_4607_OpZoneFilterAreaType()
         { }

         /** Returns this accessor's type constant, i.e. MTI_STANAG_4607_OpZoneFilterAreaType
           *
           * @return This accessor's type constant, i.e. MTI_STANAG_4607_OpZoneFilterAreaType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::mTI_STANAG_4607_OpZoneFilterAreaType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MTI_STANAG_4607_OpZoneFilterAreaType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on the target classification [Minimum occurrences: 0] [Maximum occurrences: 32] */
         typedef uci::base::BoundedList<uci::type::STANAG_4607_TargetClassificationEnum, uci::type::accessorType::sTANAG_4607_TargetClassificationEnum> TargetClassification;

         /** Field M3 from the STANAG 4607 Mission Packet [Minimum occurrences: 0] [Maximum occurrences: 41] */
         typedef uci::base::BoundedList<uci::type::STANAG_4607_PlatformTypeEnum, uci::type::accessorType::sTANAG_4607_PlatformTypeEnum> PlatformType;

         /** Returns the bounded list that is identified by the TargetClassification.
           *
           * @return The bounded list identified by TargetClassification.
           */
         virtual const uci::type::MTI_STANAG_4607_OpZoneFilterAreaType::TargetClassification& getTargetClassification() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TargetClassification.
           *
           * @return The bounded list identified by TargetClassification.
           */
         virtual uci::type::MTI_STANAG_4607_OpZoneFilterAreaType::TargetClassification& getTargetClassification()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TargetClassification.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTargetClassification(const uci::type::MTI_STANAG_4607_OpZoneFilterAreaType::TargetClassification& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the STANAG_4607_Mission.
           *
           * @return The acecssor that provides access to the complex content that is identified by STANAG_4607_Mission.
           */
         virtual const uci::type::STANAG_4607_MissionType& getSTANAG_4607_Mission() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the STANAG_4607_Mission.
           *
           * @return The acecssor that provides access to the complex content that is identified by STANAG_4607_Mission.
           */
         virtual uci::type::STANAG_4607_MissionType& getSTANAG_4607_Mission()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the STANAG_4607_Mission to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by STANAG_4607_Mission
           */
         virtual void setSTANAG_4607_Mission(const uci::type::STANAG_4607_MissionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by STANAG_4607_Mission exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by STANAG_4607_Mission is emabled or not
           */
         virtual bool hasSTANAG_4607_Mission() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by STANAG_4607_Mission
           *
           * @param type = uci::type::accessorType::sTANAG_4607_MissionType This Accessor's accessor type
           */
         virtual void enableSTANAG_4607_Mission(uci::base::accessorType::AccessorType type = uci::type::accessorType::sTANAG_4607_MissionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by STANAG_4607_Mission */
         virtual void clearSTANAG_4607_Mission()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PlatformType.
           *
           * @return The bounded list identified by PlatformType.
           */
         virtual const uci::type::MTI_STANAG_4607_OpZoneFilterAreaType::PlatformType& getPlatformType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PlatformType.
           *
           * @return The bounded list identified by PlatformType.
           */
         virtual uci::type::MTI_STANAG_4607_OpZoneFilterAreaType::PlatformType& getPlatformType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PlatformType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPlatformType(const uci::type::MTI_STANAG_4607_OpZoneFilterAreaType::PlatformType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MTI_STANAG_4607_OpZoneFilterAreaType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MTI_STANAG_4607_OpZoneFilterAreaType to copy from
           */
         MTI_STANAG_4607_OpZoneFilterAreaType(const MTI_STANAG_4607_OpZoneFilterAreaType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MTI_STANAG_4607_OpZoneFilterAreaType to the contents of the
           * MTI_STANAG_4607_OpZoneFilterAreaType on the right hand side (rhs) of the assignment
           * operator.MTI_STANAG_4607_OpZoneFilterAreaType [only available to derived classes]
           *
           * @param rhs The MTI_STANAG_4607_OpZoneFilterAreaType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::MTI_STANAG_4607_OpZoneFilterAreaType
           * @return A constant reference to this MTI_STANAG_4607_OpZoneFilterAreaType.
           */
         const MTI_STANAG_4607_OpZoneFilterAreaType& operator=(const MTI_STANAG_4607_OpZoneFilterAreaType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MTI_STANAG_4607_OpZoneFilterAreaType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MTI_STANAG_4607_OpZoneFilterAreaType_h

