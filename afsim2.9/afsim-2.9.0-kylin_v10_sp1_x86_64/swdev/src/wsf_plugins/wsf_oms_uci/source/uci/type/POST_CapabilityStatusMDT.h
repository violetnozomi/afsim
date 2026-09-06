// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__POST_CapabilityStatusMDT_h
#define Uci__Type__POST_CapabilityStatusMDT_h 1

#if !defined(Uci__Type__CapabilityStatusBaseType_h)
# include "uci/type/CapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_CapabilityStatusType_h)
# include "uci/type/POST_CapabilityStatusType.h"
#endif

#if !defined(Uci__Type__POST_SubCapabilitySettingsType_h)
# include "uci/type/POST_SubCapabilitySettingsType.h"
#endif

#if !defined(Uci__Type__POST_TrackStatusType_h)
# include "uci/type/POST_TrackStatusType.h"
#endif

#if !defined(Uci__Type__POST_ComponentStatusType_h)
# include "uci/type/POST_ComponentStatusType.h"
#endif

#if !defined(Uci__Type__PO_CollectionConstraintsStatusType_h)
# include "uci/type/PO_CollectionConstraintsStatusType.h"
#endif

#if !defined(Uci__Type__FOR_Type_h)
# include "uci/type/FOR_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_CapabilityStatusMDT sequence accessor class */
      class POST_CapabilityStatusMDT : public virtual uci::type::CapabilityStatusBaseType {
      public:

         /** The destructor */
         virtual ~POST_CapabilityStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. POST_CapabilityStatusMDT
           *
           * @return This accessor's type constant, i.e. POST_CapabilityStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_CapabilityStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_CapabilityStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the high level availability/state of a Capability instance. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::POST_CapabilityStatusType, uci::type::accessorType::pOST_CapabilityStatusType> CapabilityStatus;

         /** Indicates the status settings associated with a subcapability as commanded in the settings command. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_SubCapabilitySettingsType, uci::type::accessorType::pOST_SubCapabilitySettingsType> SubCapabilityStatus;

         /** Indicates the status of the track settings associated with a track option settings as commanded in the settings
           * command. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_TrackStatusType, uci::type::accessorType::pOST_TrackStatusType> TrackStatus;

         /** The field specified the current status (and settings) of this component. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_ComponentStatusType, uci::type::accessorType::pOST_ComponentStatusType> ComponentStatus;

         /** Returns the bounded list that is identified by the CapabilityStatus.
           *
           * @return The bounded list identified by CapabilityStatus.
           */
         virtual const uci::type::POST_CapabilityStatusMDT::CapabilityStatus& getCapabilityStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityStatus.
           *
           * @return The bounded list identified by CapabilityStatus.
           */
         virtual uci::type::POST_CapabilityStatusMDT::CapabilityStatus& getCapabilityStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityStatus(const uci::type::POST_CapabilityStatusMDT::CapabilityStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityStatus.
           *
           * @return The bounded list identified by SubCapabilityStatus.
           */
         virtual const uci::type::POST_CapabilityStatusMDT::SubCapabilityStatus& getSubCapabilityStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityStatus.
           *
           * @return The bounded list identified by SubCapabilityStatus.
           */
         virtual uci::type::POST_CapabilityStatusMDT::SubCapabilityStatus& getSubCapabilityStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapabilityStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapabilityStatus(const uci::type::POST_CapabilityStatusMDT::SubCapabilityStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TrackStatus.
           *
           * @return The bounded list identified by TrackStatus.
           */
         virtual const uci::type::POST_CapabilityStatusMDT::TrackStatus& getTrackStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TrackStatus.
           *
           * @return The bounded list identified by TrackStatus.
           */
         virtual uci::type::POST_CapabilityStatusMDT::TrackStatus& getTrackStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TrackStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTrackStatus(const uci::type::POST_CapabilityStatusMDT::TrackStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentStatus.
           *
           * @return The bounded list identified by ComponentStatus.
           */
         virtual const uci::type::POST_CapabilityStatusMDT::ComponentStatus& getComponentStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentStatus.
           *
           * @return The bounded list identified by ComponentStatus.
           */
         virtual uci::type::POST_CapabilityStatusMDT::ComponentStatus& getComponentStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ComponentStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComponentStatus(const uci::type::POST_CapabilityStatusMDT::ComponentStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * CollectionConstraintsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraintsStatus.
           */
         virtual const uci::type::PO_CollectionConstraintsStatusType& getCollectionConstraintsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * CollectionConstraintsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraintsStatus.
           */
         virtual uci::type::PO_CollectionConstraintsStatusType& getCollectionConstraintsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionConstraintsStatus to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionConstraintsStatus
           */
         virtual void setCollectionConstraintsStatus(const uci::type::PO_CollectionConstraintsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionConstraintsStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionConstraintsStatus is emabled or not
           */
         virtual bool hasCollectionConstraintsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionConstraintsStatus
           *
           * @param type = uci::type::accessorType::pO_CollectionConstraintsStatusType This Accessor's accessor type
           */
         virtual void enableCollectionConstraintsStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_CollectionConstraintsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionConstraintsStatus */
         virtual void clearCollectionConstraintsStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentFOR.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentFOR.
           */
         virtual const uci::type::FOR_Type& getCurrentFOR() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentFOR.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentFOR.
           */
         virtual uci::type::FOR_Type& getCurrentFOR()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CurrentFOR to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CurrentFOR
           */
         virtual void setCurrentFOR(const uci::type::FOR_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CurrentFOR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CurrentFOR is emabled or not
           */
         virtual bool hasCurrentFOR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CurrentFOR
           *
           * @param type = uci::type::accessorType::fOR_Type This Accessor's accessor type
           */
         virtual void enableCurrentFOR(uci::base::accessorType::AccessorType type = uci::type::accessorType::fOR_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CurrentFOR */
         virtual void clearCurrentFOR()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentThermalBlankingFOR.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentThermalBlankingFOR.
           */
         virtual const uci::type::FOR_Type& getCurrentThermalBlankingFOR() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentThermalBlankingFOR.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentThermalBlankingFOR.
           */
         virtual uci::type::FOR_Type& getCurrentThermalBlankingFOR()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CurrentThermalBlankingFOR to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CurrentThermalBlankingFOR
           */
         virtual void setCurrentThermalBlankingFOR(const uci::type::FOR_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CurrentThermalBlankingFOR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CurrentThermalBlankingFOR is emabled or not
           */
         virtual bool hasCurrentThermalBlankingFOR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CurrentThermalBlankingFOR
           *
           * @param type = uci::type::accessorType::fOR_Type This Accessor's accessor type
           */
         virtual void enableCurrentThermalBlankingFOR(uci::base::accessorType::AccessorType type = uci::type::accessorType::fOR_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CurrentThermalBlankingFOR */
         virtual void clearCurrentThermalBlankingFOR()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_CapabilityStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_CapabilityStatusMDT to copy from
           */
         POST_CapabilityStatusMDT(const POST_CapabilityStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_CapabilityStatusMDT to the contents of the
           * POST_CapabilityStatusMDT on the right hand side (rhs) of the assignment operator.POST_CapabilityStatusMDT [only
           * available to derived classes]
           *
           * @param rhs The POST_CapabilityStatusMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::POST_CapabilityStatusMDT
           * @return A constant reference to this POST_CapabilityStatusMDT.
           */
         const POST_CapabilityStatusMDT& operator=(const POST_CapabilityStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_CapabilityStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_CapabilityStatusMDT_h

