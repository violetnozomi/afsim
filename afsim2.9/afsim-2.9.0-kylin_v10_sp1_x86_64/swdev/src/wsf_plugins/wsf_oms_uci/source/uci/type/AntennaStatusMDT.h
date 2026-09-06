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
#ifndef Uci__Type__AntennaStatusMDT_h
#define Uci__Type__AntennaStatusMDT_h 1

#if !defined(Uci__Type__SupportCapabilityStatusBaseType_h)
# include "uci/type/SupportCapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__PowerType_h)
# include "uci/type/PowerType.h"
#endif

#if !defined(Uci__Type__AntennaPerformanceProfileType_h)
# include "uci/type/AntennaPerformanceProfileType.h"
#endif

#if !defined(Uci__Type__AntennaResourceInstanceStatusType_h)
# include "uci/type/AntennaResourceInstanceStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type represents the status and settings for an Antenna that is relevant to mission operations. */
      class AntennaStatusMDT : public virtual uci::type::SupportCapabilityStatusBaseType {
      public:

         /** The destructor */
         virtual ~AntennaStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. AntennaStatusMDT
           *
           * @return This accessor's type constant, i.e. AntennaStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::antennaStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AntennaStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the ID of a Capability whose use requires access to an antenna resource. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Provides performance feedback on a given AntennaResourceInstance [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::AntennaResourceInstanceStatusType, uci::type::accessorType::antennaResourceInstanceStatusType> AntennaResourceInstanceStatus;

         /** Returns the value of the SimplePrimitive data type that is identified by the BeamConfigSupport.
           *
           * @return The value of the simple primitive data type identified by BeamConfigSupport.
           */
         virtual xs::Boolean getBeamConfigSupport() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BeamConfigSupport.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBeamConfigSupport(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::AntennaStatusMDT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::AntennaStatusMDT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::AntennaStatusMDT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PowerConsumption.
           *
           * @return The value of the simple primitive data type identified by PowerConsumption.
           */
         virtual uci::type::PowerTypeValue getPowerConsumption() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PowerConsumption.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPowerConsumption(uci::type::PowerTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PowerConsumption exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PowerConsumption is emabled or not
           */
         virtual bool hasPowerConsumption() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PowerConsumption
           *
           * @param type = uci::type::accessorType::powerType This Accessor's accessor type
           */
         virtual void enablePowerConsumption(uci::base::accessorType::AccessorType type = uci::type::accessorType::powerType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PowerConsumption */
         virtual void clearPowerConsumption()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AntennaPerformanceProfile.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaPerformanceProfile.
           */
         virtual const uci::type::AntennaPerformanceProfileType& getAntennaPerformanceProfile() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AntennaPerformanceProfile.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaPerformanceProfile.
           */
         virtual uci::type::AntennaPerformanceProfileType& getAntennaPerformanceProfile()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AntennaPerformanceProfile to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AntennaPerformanceProfile
           */
         virtual void setAntennaPerformanceProfile(const uci::type::AntennaPerformanceProfileType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AntennaPerformanceProfile exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AntennaPerformanceProfile is emabled or not
           */
         virtual bool hasAntennaPerformanceProfile() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AntennaPerformanceProfile
           *
           * @param type = uci::type::accessorType::antennaPerformanceProfileType This Accessor's accessor type
           */
         virtual void enableAntennaPerformanceProfile(uci::base::accessorType::AccessorType type = uci::type::accessorType::antennaPerformanceProfileType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AntennaPerformanceProfile */
         virtual void clearAntennaPerformanceProfile()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaResourceInstanceStatus.
           *
           * @return The bounded list identified by AntennaResourceInstanceStatus.
           */
         virtual const uci::type::AntennaStatusMDT::AntennaResourceInstanceStatus& getAntennaResourceInstanceStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaResourceInstanceStatus.
           *
           * @return The bounded list identified by AntennaResourceInstanceStatus.
           */
         virtual uci::type::AntennaStatusMDT::AntennaResourceInstanceStatus& getAntennaResourceInstanceStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AntennaResourceInstanceStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAntennaResourceInstanceStatus(const uci::type::AntennaStatusMDT::AntennaResourceInstanceStatus& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AntennaStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AntennaStatusMDT to copy from
           */
         AntennaStatusMDT(const AntennaStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AntennaStatusMDT to the contents of the AntennaStatusMDT on the
           * right hand side (rhs) of the assignment operator.AntennaStatusMDT [only available to derived classes]
           *
           * @param rhs The AntennaStatusMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AntennaStatusMDT
           * @return A constant reference to this AntennaStatusMDT.
           */
         const AntennaStatusMDT& operator=(const AntennaStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AntennaStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AntennaStatusMDT_h

