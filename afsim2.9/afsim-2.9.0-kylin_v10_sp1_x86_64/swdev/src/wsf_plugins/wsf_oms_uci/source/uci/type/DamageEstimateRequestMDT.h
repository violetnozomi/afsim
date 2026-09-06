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
#ifndef Uci__Type__DamageEstimateRequestMDT_h
#define Uci__Type__DamageEstimateRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DamageEstimateRequestID_Type_h)
# include "uci/type/DamageEstimateRequestID_Type.h"
#endif

#if !defined(Uci__Type__DamageCategoryEnum_h)
# include "uci/type/DamageCategoryEnum.h"
#endif

#if !defined(Uci__Type__DamageEstimateContentsEnum_h)
# include "uci/type/DamageEstimateContentsEnum.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__DamageEstimateTargetType_h)
# include "uci/type/DamageEstimateTargetType.h"
#endif

#if !defined(Uci__Type__VisibleString64Type_h)
# include "uci/type/VisibleString64Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DMPI_DamageEstimateType_h)
# include "uci/type/DMPI_DamageEstimateType.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

#if !defined(Uci__Type__DMPI_BlastPairingType_h)
# include "uci/type/DMPI_BlastPairingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageEstimateRequestMDT sequence accessor class */
      class DamageEstimateRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageEstimateRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. DamageEstimateRequestMDT
           *
           * @return This accessor's type constant, i.e. DamageEstimateRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageEstimateRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageEstimateRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a DMPI associated with this attack. If omitted, DMPI information should be retrieved via the sibling Target
           * element's Task reference. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DMPI_DamageEstimateType, uci::type::accessorType::dMPI_DamageEstimateType> DMPI_Data;

         /** Indicates the ID of a data product from which this damage request should be derived. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductMetadataID_Type, uci::type::accessorType::productMetadataID_Type> ProductMetadataID;

         /** Optional DMPI-to-BlastRadius pairings to be used for fratricide and collateral damage checks. If left unspecified,
           * DMPI(s) will be looked up via TaskID, and blast radius (a.k.a. minimum safe distance) will be a lookup value based on
           * the weapon. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DMPI_BlastPairingType, uci::type::accessorType::dMPI_BlastPairingType> DMPI_BlastPairing;

         /** Returns the accessor that provides access to the complex content that is identified by the DamageEstimateRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageEstimateRequestID.
           */
         virtual const uci::type::DamageEstimateRequestID_Type& getDamageEstimateRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamageEstimateRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageEstimateRequestID.
           */
         virtual uci::type::DamageEstimateRequestID_Type& getDamageEstimateRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DamageEstimateRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DamageEstimateRequestID
           */
         virtual void setDamageEstimateRequestID(const uci::type::DamageEstimateRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EstimateType.
           *
           * @return The value of the enumeration identified by EstimateType.
           */
         virtual const uci::type::DamageCategoryEnum& getEstimateType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EstimateType.
           *
           * @return The value of the enumeration identified by EstimateType.
           */
         virtual uci::type::DamageCategoryEnum& getEstimateType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EstimateType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEstimateType(const uci::type::DamageCategoryEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EstimateType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEstimateType(uci::type::DamageCategoryEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EstimateContents.
           *
           * @return The value of the enumeration identified by EstimateContents.
           */
         virtual const uci::type::DamageEstimateContentsEnum& getEstimateContents() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EstimateContents.
           *
           * @return The value of the enumeration identified by EstimateContents.
           */
         virtual uci::type::DamageEstimateContentsEnum& getEstimateContents()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EstimateContents.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEstimateContents(const uci::type::DamageEstimateContentsEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EstimateContents.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEstimateContents(uci::type::DamageEstimateContentsEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ReportingThreshold.
           *
           * @return The value of the simple primitive data type identified by ReportingThreshold.
           */
         virtual uci::type::PercentTypeValue getReportingThreshold() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ReportingThreshold.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setReportingThreshold(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TargetData.
           *
           * @return The acecssor that provides access to the complex content that is identified by TargetData.
           */
         virtual const uci::type::DamageEstimateTargetType& getTargetData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TargetData.
           *
           * @return The acecssor that provides access to the complex content that is identified by TargetData.
           */
         virtual uci::type::DamageEstimateTargetType& getTargetData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TargetData to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TargetData
           */
         virtual void setTargetData(const uci::type::DamageEstimateTargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TargetName.
           *
           * @return The value of the string data type identified by TargetName.
           */
         virtual const uci::type::VisibleString64Type& getTargetName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TargetName.
           *
           * @return The value of the string data type identified by TargetName.
           */
         virtual uci::type::VisibleString64Type& getTargetName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TargetName to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTargetName(const uci::type::VisibleString64Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TargetName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTargetName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TargetName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTargetName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TargetName exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TargetName is emabled or not
           */
         virtual bool hasTargetName() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TargetName
           *
           * @param type = uci::type::accessorType::visibleString64Type This Accessor's accessor type
           */
         virtual void enableTargetName(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString64Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TargetName */
         virtual void clearTargetName()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DMPI_Data.
           *
           * @return The bounded list identified by DMPI_Data.
           */
         virtual const uci::type::DamageEstimateRequestMDT::DMPI_Data& getDMPI_Data() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DMPI_Data.
           *
           * @return The bounded list identified by DMPI_Data.
           */
         virtual uci::type::DamageEstimateRequestMDT::DMPI_Data& getDMPI_Data()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DMPI_Data.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDMPI_Data(const uci::type::DamageEstimateRequestMDT::DMPI_Data& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the ReleasePoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReleasePoint.
           */
         virtual const uci::type::Point3D_Type& getReleasePoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReleasePoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReleasePoint.
           */
         virtual uci::type::Point3D_Type& getReleasePoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReleasePoint to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReleasePoint
           */
         virtual void setReleasePoint(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReleasePoint exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReleasePoint is emabled or not
           */
         virtual bool hasReleasePoint() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReleasePoint
           *
           * @param type = uci::type::accessorType::point3D_Type This Accessor's accessor type
           */
         virtual void enableReleasePoint(uci::base::accessorType::AccessorType type = uci::type::accessorType::point3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReleasePoint */
         virtual void clearReleasePoint()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual const uci::type::DamageEstimateRequestMDT::ProductMetadataID& getProductMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual uci::type::DamageEstimateRequestMDT::ProductMetadataID& getProductMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductMetadataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductMetadataID(const uci::type::DamageEstimateRequestMDT::ProductMetadataID& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the EstimateZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by EstimateZone.
           */
         virtual const uci::type::ZoneType& getEstimateZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EstimateZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by EstimateZone.
           */
         virtual uci::type::ZoneType& getEstimateZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EstimateZone to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EstimateZone
           */
         virtual void setEstimateZone(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimateZone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimateZone is emabled or not
           */
         virtual bool hasEstimateZone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimateZone
           *
           * @param type = uci::type::accessorType::zoneType This Accessor's accessor type
           */
         virtual void enableEstimateZone(uci::base::accessorType::AccessorType type = uci::type::accessorType::zoneType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimateZone */
         virtual void clearEstimateZone()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DMPI_BlastPairing.
           *
           * @return The bounded list identified by DMPI_BlastPairing.
           */
         virtual const uci::type::DamageEstimateRequestMDT::DMPI_BlastPairing& getDMPI_BlastPairing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DMPI_BlastPairing.
           *
           * @return The bounded list identified by DMPI_BlastPairing.
           */
         virtual uci::type::DamageEstimateRequestMDT::DMPI_BlastPairing& getDMPI_BlastPairing()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DMPI_BlastPairing.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDMPI_BlastPairing(const uci::type::DamageEstimateRequestMDT::DMPI_BlastPairing& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageEstimateRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageEstimateRequestMDT to copy from
           */
         DamageEstimateRequestMDT(const DamageEstimateRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageEstimateRequestMDT to the contents of the
           * DamageEstimateRequestMDT on the right hand side (rhs) of the assignment operator.DamageEstimateRequestMDT [only
           * available to derived classes]
           *
           * @param rhs The DamageEstimateRequestMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DamageEstimateRequestMDT
           * @return A constant reference to this DamageEstimateRequestMDT.
           */
         const DamageEstimateRequestMDT& operator=(const DamageEstimateRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageEstimateRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageEstimateRequestMDT_h

