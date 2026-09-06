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
#ifndef Uci__Type__AntennaAllocationAndBeamConfigurationsType_h
#define Uci__Type__AntennaAllocationAndBeamConfigurationsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AntennaAndResourceInstanceType_h)
# include "uci/type/AntennaAndResourceInstanceType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AntennaAllocationAndBeamConfigurationsType sequence accessor class */
      class AntennaAllocationAndBeamConfigurationsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AntennaAllocationAndBeamConfigurationsType()
         { }

         /** Returns this accessor's type constant, i.e. AntennaAllocationAndBeamConfigurationsType
           *
           * @return This accessor's type constant, i.e. AntennaAllocationAndBeamConfigurationsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::antennaAllocationAndBeamConfigurationsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AntennaAllocationAndBeamConfigurationsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The allocation includes the actual Beam Configuration Indices to be used for each AntennaID. The number maps to an
           * actual beam buffer in the antenna (or is set to "1" for a simple antenna), so the BeamConfigurationIndex cannot
           * exceed the number of BeamBufferSize identified in the AntennaInformationFile for the associated AntennaID. [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::base::UnsignedIntAccessor, uci::base::accessorType::unsignedIntAccessor> BeamConfigurationIndex;

         /** Returns the accessor that provides access to the complex content that is identified by the AntennaResource.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaResource.
           */
         virtual const uci::type::AntennaAndResourceInstanceType& getAntennaResource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AntennaResource.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaResource.
           */
         virtual uci::type::AntennaAndResourceInstanceType& getAntennaResource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AntennaResource to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AntennaResource
           */
         virtual void setAntennaResource(const uci::type::AntennaAndResourceInstanceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BeamConfigurationIndex.
           *
           * @return The bounded list identified by BeamConfigurationIndex.
           */
         virtual const uci::type::AntennaAllocationAndBeamConfigurationsType::BeamConfigurationIndex& getBeamConfigurationIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BeamConfigurationIndex.
           *
           * @return The bounded list identified by BeamConfigurationIndex.
           */
         virtual uci::type::AntennaAllocationAndBeamConfigurationsType::BeamConfigurationIndex& getBeamConfigurationIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the BeamConfigurationIndex.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBeamConfigurationIndex(const uci::type::AntennaAllocationAndBeamConfigurationsType::BeamConfigurationIndex& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AntennaAllocationAndBeamConfigurationsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AntennaAllocationAndBeamConfigurationsType to copy from
           */
         AntennaAllocationAndBeamConfigurationsType(const AntennaAllocationAndBeamConfigurationsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AntennaAllocationAndBeamConfigurationsType to the contents of the
           * AntennaAllocationAndBeamConfigurationsType on the right hand side (rhs) of the assignment
           * operator.AntennaAllocationAndBeamConfigurationsType [only available to derived classes]
           *
           * @param rhs The AntennaAllocationAndBeamConfigurationsType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::AntennaAllocationAndBeamConfigurationsType
           * @return A constant reference to this AntennaAllocationAndBeamConfigurationsType.
           */
         const AntennaAllocationAndBeamConfigurationsType& operator=(const AntennaAllocationAndBeamConfigurationsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AntennaAllocationAndBeamConfigurationsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AntennaAllocationAndBeamConfigurationsType_h

