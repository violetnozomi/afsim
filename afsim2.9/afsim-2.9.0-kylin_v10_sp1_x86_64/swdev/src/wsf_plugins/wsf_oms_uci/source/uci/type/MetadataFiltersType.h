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
#ifndef Uci__Type__MetadataFiltersType_h
#define Uci__Type__MetadataFiltersType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

#if !defined(Uci__Type__ProcessingTypeEnum_h)
# include "uci/type/ProcessingTypeEnum.h"
#endif

#if !defined(Uci__Type__ProductTypeEnum_h)
# include "uci/type/ProductTypeEnum.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityCommandID_Type_h)
# include "uci/type/CapabilityCommandID_Type.h"
#endif

#if !defined(Uci__Type__TraceabilityType_h)
# include "uci/type/TraceabilityType.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MetadataFiltersType sequence accessor class */
      class MetadataFiltersType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MetadataFiltersType()
         { }

         /** Returns this accessor's type constant, i.e. MetadataFiltersType
           *
           * @return This accessor's type constant, i.e. MetadataFiltersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::metadataFiltersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MetadataFiltersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filterable element identifier from the ProductMetadata message [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductMetadataID_Type, uci::type::accessorType::productMetadataID_Type> ProductMetadataID;

         /** Identifier of the source for the Metadata filterable element [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductMetadataID_Type, uci::type::accessorType::productMetadataID_Type> DerivedFromID;

         /** Enumerates the filter processing type for the Metadata element [Minimum occurrences: 0] [Maximum occurrences: 32] */
         typedef uci::base::BoundedList<uci::type::ProcessingTypeEnum, uci::type::accessorType::processingTypeEnum> ProcessingType;

         /** Enumerates the filter product type for the Metadata element [Minimum occurrences: 0] [Maximum occurrences: 78] */
         typedef uci::base::BoundedList<uci::type::ProductTypeEnum, uci::type::accessorType::productTypeEnum> ProductType;

         /** Identifies the task associated with the filterable element [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TaskID_Type, uci::type::accessorType::taskID_Type> TaskID;

         /** Identifies the CapabilityCommand associated with the filterable element [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityCommandID_Type, uci::type::accessorType::capabilityCommandID_Type> CapabilityCommandID;

         /** Identifies the tasking source associated with the filterable element [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TraceabilityType, uci::type::accessorType::traceabilityType> Traceability;

         /** The classification label associated with the filterable element [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SecurityInformationType, uci::type::accessorType::securityInformationType> SecurityInformation;

         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual const uci::type::MetadataFiltersType::ProductMetadataID& getProductMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual uci::type::MetadataFiltersType::ProductMetadataID& getProductMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductMetadataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductMetadataID(const uci::type::MetadataFiltersType::ProductMetadataID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DerivedFromID.
           *
           * @return The bounded list identified by DerivedFromID.
           */
         virtual const uci::type::MetadataFiltersType::DerivedFromID& getDerivedFromID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DerivedFromID.
           *
           * @return The bounded list identified by DerivedFromID.
           */
         virtual uci::type::MetadataFiltersType::DerivedFromID& getDerivedFromID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DerivedFromID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDerivedFromID(const uci::type::MetadataFiltersType::DerivedFromID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProcessingType.
           *
           * @return The bounded list identified by ProcessingType.
           */
         virtual const uci::type::MetadataFiltersType::ProcessingType& getProcessingType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProcessingType.
           *
           * @return The bounded list identified by ProcessingType.
           */
         virtual uci::type::MetadataFiltersType::ProcessingType& getProcessingType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProcessingType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProcessingType(const uci::type::MetadataFiltersType::ProcessingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductType.
           *
           * @return The bounded list identified by ProductType.
           */
         virtual const uci::type::MetadataFiltersType::ProductType& getProductType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductType.
           *
           * @return The bounded list identified by ProductType.
           */
         virtual uci::type::MetadataFiltersType::ProductType& getProductType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductType(const uci::type::MetadataFiltersType::ProductType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual const uci::type::MetadataFiltersType::TaskID& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskID.
           *
           * @return The bounded list identified by TaskID.
           */
         virtual uci::type::MetadataFiltersType::TaskID& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskID(const uci::type::MetadataFiltersType::TaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityCommandID.
           *
           * @return The bounded list identified by CapabilityCommandID.
           */
         virtual const uci::type::MetadataFiltersType::CapabilityCommandID& getCapabilityCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityCommandID.
           *
           * @return The bounded list identified by CapabilityCommandID.
           */
         virtual uci::type::MetadataFiltersType::CapabilityCommandID& getCapabilityCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityCommandID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityCommandID(const uci::type::MetadataFiltersType::CapabilityCommandID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Traceability.
           *
           * @return The bounded list identified by Traceability.
           */
         virtual const uci::type::MetadataFiltersType::Traceability& getTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Traceability.
           *
           * @return The bounded list identified by Traceability.
           */
         virtual uci::type::MetadataFiltersType::Traceability& getTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Traceability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTraceability(const uci::type::MetadataFiltersType::Traceability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecurityInformation.
           *
           * @return The bounded list identified by SecurityInformation.
           */
         virtual const uci::type::MetadataFiltersType::SecurityInformation& getSecurityInformation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecurityInformation.
           *
           * @return The bounded list identified by SecurityInformation.
           */
         virtual uci::type::MetadataFiltersType::SecurityInformation& getSecurityInformation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SecurityInformation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSecurityInformation(const uci::type::MetadataFiltersType::SecurityInformation& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MetadataFiltersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MetadataFiltersType to copy from
           */
         MetadataFiltersType(const MetadataFiltersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MetadataFiltersType to the contents of the MetadataFiltersType on
           * the right hand side (rhs) of the assignment operator.MetadataFiltersType [only available to derived classes]
           *
           * @param rhs The MetadataFiltersType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::MetadataFiltersType
           * @return A constant reference to this MetadataFiltersType.
           */
         const MetadataFiltersType& operator=(const MetadataFiltersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MetadataFiltersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MetadataFiltersType_h

