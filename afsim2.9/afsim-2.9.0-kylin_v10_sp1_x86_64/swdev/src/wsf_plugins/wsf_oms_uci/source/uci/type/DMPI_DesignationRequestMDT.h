// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__DMPI_DesignationRequestMDT_h
#define Uci__Type__DMPI_DesignationRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DMPI_DesignationRequestID_Type_h)
# include "uci/type/DMPI_DesignationRequestID_Type.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

#if !defined(Uci__Type__RecommendedWeaponsType_h)
# include "uci/type/RecommendedWeaponsType.h"
#endif

#if !defined(Uci__Type__AvailableWeaponsType_h)
# include "uci/type/AvailableWeaponsType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__PixelReferenceCoordinateType_h)
# include "uci/type/PixelReferenceCoordinateType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DMPI_DesignationRequestMDT sequence accessor class */
      class DMPI_DesignationRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DMPI_DesignationRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. DMPI_DesignationRequestMDT
           *
           * @return This accessor's type constant, i.e. DMPI_DesignationRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dMPI_DesignationRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DMPI_DesignationRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the IDs of the data product from which this DMPI designation should be derived. It should be considered
           * required in cases where the DMPI designation requestor expects specific data products to be used to create the DMPI
           * designation, such as dynamic targeting scenarios with freshly collected sensor products. It is optional to support
           * use cases where products aren't used to create the DMPI designation and/or the selection of products is left to the
           * designator to select. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductMetadataID_Type, uci::type::accessorType::productMetadataID_Type> ProductMetadataID;

         /** Indicates a recommendation on the type and quantity of weapons to use when targeting and creating DMPIs. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RecommendedWeaponsType, uci::type::accessorType::recommendedWeaponsType> RecommendedWeapons;

         /** Indicates a constraint on the type and quantity of weapons available (or to be used) when targeting and creating
           * DMPIs. It is optional for cases when the system/node creating DMPIs has direct awareness of the plans and weapon
           * type/quantity of the strike Systems and/or the requestor has deferred the selection. When specified, this element is
           * considered a targeting constraint to be used regardless of other weapon type/quantity information that may be
           * available. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AvailableWeaponsType, uci::type::accessorType::availableWeaponsType> AvailableWeapons;

         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_DesignationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_DesignationRequestID.
           */
         virtual const uci::type::DMPI_DesignationRequestID_Type& getDMPI_DesignationRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_DesignationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_DesignationRequestID.
           */
         virtual uci::type::DMPI_DesignationRequestID_Type& getDMPI_DesignationRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DMPI_DesignationRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DMPI_DesignationRequestID
           */
         virtual void setDMPI_DesignationRequestID(const uci::type::DMPI_DesignationRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskID.
           */
         virtual const uci::type::TaskID_Type& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskID.
           */
         virtual uci::type::TaskID_Type& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskID
           */
         virtual void setTaskID(const uci::type::TaskID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual const uci::type::DMPI_DesignationRequestMDT::ProductMetadataID& getProductMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual uci::type::DMPI_DesignationRequestMDT::ProductMetadataID& getProductMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductMetadataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductMetadataID(const uci::type::DMPI_DesignationRequestMDT::ProductMetadataID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RecommendedWeapons.
           *
           * @return The bounded list identified by RecommendedWeapons.
           */
         virtual const uci::type::DMPI_DesignationRequestMDT::RecommendedWeapons& getRecommendedWeapons() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RecommendedWeapons.
           *
           * @return The bounded list identified by RecommendedWeapons.
           */
         virtual uci::type::DMPI_DesignationRequestMDT::RecommendedWeapons& getRecommendedWeapons()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RecommendedWeapons.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRecommendedWeapons(const uci::type::DMPI_DesignationRequestMDT::RecommendedWeapons& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AvailableWeapons.
           *
           * @return The bounded list identified by AvailableWeapons.
           */
         virtual const uci::type::DMPI_DesignationRequestMDT::AvailableWeapons& getAvailableWeapons() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AvailableWeapons.
           *
           * @return The bounded list identified by AvailableWeapons.
           */
         virtual uci::type::DMPI_DesignationRequestMDT::AvailableWeapons& getAvailableWeapons()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AvailableWeapons.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAvailableWeapons(const uci::type::DMPI_DesignationRequestMDT::AvailableWeapons& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RespondBy.
           *
           * @return The value of the simple primitive data type identified by RespondBy.
           */
         virtual uci::type::DateTimeTypeValue getRespondBy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RespondBy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRespondBy(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RespondBy exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RespondBy is emabled or not
           */
         virtual bool hasRespondBy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RespondBy
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableRespondBy(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RespondBy */
         virtual void clearRespondBy()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProposedDMPI_Coordinates.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProposedDMPI_Coordinates.
           */
         virtual const uci::type::Point2D_Type& getProposedDMPI_Coordinates() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProposedDMPI_Coordinates.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProposedDMPI_Coordinates.
           */
         virtual uci::type::Point2D_Type& getProposedDMPI_Coordinates()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProposedDMPI_Coordinates to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProposedDMPI_Coordinates
           */
         virtual void setProposedDMPI_Coordinates(const uci::type::Point2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProposedDMPI_Coordinates exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProposedDMPI_Coordinates is emabled or not
           */
         virtual bool hasProposedDMPI_Coordinates() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProposedDMPI_Coordinates
           *
           * @param type = uci::type::accessorType::point2D_Type This Accessor's accessor type
           */
         virtual void enableProposedDMPI_Coordinates(uci::base::accessorType::AccessorType type = uci::type::accessorType::point2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProposedDMPI_Coordinates */
         virtual void clearProposedDMPI_Coordinates()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferencePixelCoordinate.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferencePixelCoordinate.
           */
         virtual const uci::type::PixelReferenceCoordinateType& getReferencePixelCoordinate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferencePixelCoordinate.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferencePixelCoordinate.
           */
         virtual uci::type::PixelReferenceCoordinateType& getReferencePixelCoordinate()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReferencePixelCoordinate to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReferencePixelCoordinate
           */
         virtual void setReferencePixelCoordinate(const uci::type::PixelReferenceCoordinateType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReferencePixelCoordinate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReferencePixelCoordinate is emabled or not
           */
         virtual bool hasReferencePixelCoordinate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReferencePixelCoordinate
           *
           * @param type = uci::type::accessorType::pixelReferenceCoordinateType This Accessor's accessor type
           */
         virtual void enableReferencePixelCoordinate(uci::base::accessorType::AccessorType type = uci::type::accessorType::pixelReferenceCoordinateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReferencePixelCoordinate */
         virtual void clearReferencePixelCoordinate()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DMPI_DesignationRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DMPI_DesignationRequestMDT to copy from
           */
         DMPI_DesignationRequestMDT(const DMPI_DesignationRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DMPI_DesignationRequestMDT to the contents of the
           * DMPI_DesignationRequestMDT on the right hand side (rhs) of the assignment operator.DMPI_DesignationRequestMDT [only
           * available to derived classes]
           *
           * @param rhs The DMPI_DesignationRequestMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DMPI_DesignationRequestMDT
           * @return A constant reference to this DMPI_DesignationRequestMDT.
           */
         const DMPI_DesignationRequestMDT& operator=(const DMPI_DesignationRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DMPI_DesignationRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DMPI_DesignationRequestMDT_h

