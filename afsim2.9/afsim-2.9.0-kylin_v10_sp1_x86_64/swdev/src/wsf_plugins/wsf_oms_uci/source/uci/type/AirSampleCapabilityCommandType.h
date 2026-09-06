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
#ifndef Uci__Type__AirSampleCapabilityCommandType_h
#define Uci__Type__AirSampleCapabilityCommandType_h 1

#if !defined(Uci__Type__CapabilityCommandBaseType_h)
# include "uci/type/CapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Type__AirTargetVolumeType_h)
# include "uci/type/AirTargetVolumeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductOutputCommandBasicType_h)
# include "uci/type/ProductOutputCommandBasicType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AirSampleCapabilityCommandType sequence accessor class */
      class AirSampleCapabilityCommandType : public virtual uci::type::CapabilityCommandBaseType {
      public:

         /** The destructor */
         virtual ~AirSampleCapabilityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. AirSampleCapabilityCommandType
           *
           * @return This accessor's type constant, i.e. AirSampleCapabilityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::airSampleCapabilityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AirSampleCapabilityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a required output Product of the Command. Only Products consistent with AirSample and supported by the
           * commanded AirSample Capability should be requested. If omitted, no output Product should be produced. See
           * ProductTypeEnum for further details. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputCommandBasicType, uci::type::accessorType::productOutputCommandBasicType> ProductOutput;

         /** Returns the accessor that provides access to the complex content that is identified by the AirVolume.
           *
           * @return The acecssor that provides access to the complex content that is identified by AirVolume.
           */
         virtual const uci::type::AirTargetVolumeType& getAirVolume() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AirVolume.
           *
           * @return The acecssor that provides access to the complex content that is identified by AirVolume.
           */
         virtual uci::type::AirTargetVolumeType& getAirVolume()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AirVolume to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AirVolume
           */
         virtual void setAirVolume(const uci::type::AirTargetVolumeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual const uci::type::AirSampleCapabilityCommandType::ProductOutput& getProductOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual uci::type::AirSampleCapabilityCommandType::ProductOutput& getProductOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutput(const uci::type::AirSampleCapabilityCommandType::ProductOutput& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AirSampleCapabilityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AirSampleCapabilityCommandType to copy from
           */
         AirSampleCapabilityCommandType(const AirSampleCapabilityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AirSampleCapabilityCommandType to the contents of the
           * AirSampleCapabilityCommandType on the right hand side (rhs) of the assignment operator.AirSampleCapabilityCommandType
           * [only available to derived classes]
           *
           * @param rhs The AirSampleCapabilityCommandType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::AirSampleCapabilityCommandType
           * @return A constant reference to this AirSampleCapabilityCommandType.
           */
         const AirSampleCapabilityCommandType& operator=(const AirSampleCapabilityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AirSampleCapabilityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AirSampleCapabilityCommandType_h

