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
#ifndef Uci__Type__POST_CapabilityMDT_h
#define Uci__Type__POST_CapabilityMDT_h 1

#if !defined(Uci__Type__CapabilityBaseType_h)
# include "uci/type/CapabilityBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_CapabilityType_h)
# include "uci/type/POST_CapabilityType.h"
#endif

#if !defined(Uci__Type__PointingTypeEnum_h)
# include "uci/type/PointingTypeEnum.h"
#endif

#if !defined(Uci__Type__POST_SubCapabilityType_h)
# include "uci/type/POST_SubCapabilityType.h"
#endif

#if !defined(Uci__Type__POST_ComponentType_h)
# include "uci/type/POST_ComponentType.h"
#endif

#if !defined(Uci__Type__POST_TrackCapabilityType_h)
# include "uci/type/POST_TrackCapabilityType.h"
#endif

#if !defined(Uci__Type__PO_CollectionConstraintsType_h)
# include "uci/type/PO_CollectionConstraintsType.h"
#endif

#if !defined(Uci__Type__FOR_Type_h)
# include "uci/type/FOR_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_CapabilityMDT sequence accessor class */
      class POST_CapabilityMDT : public virtual uci::type::CapabilityBaseType {
      public:

         /** The destructor */
         virtual ~POST_CapabilityMDT()
         { }

         /** Returns this accessor's type constant, i.e. POST_CapabilityMDT
           *
           * @return This accessor's type constant, i.e. POST_CapabilityMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_CapabilityMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_CapabilityMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an installed POST Capability, its configurable characteristics and its controllability. [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_CapabilityType, uci::type::accessorType::pOST_CapabilityType> Capability;

         /** This indicates the type of pointing options that the POST Subsystem can support. [Maximum occurrences: 6] */
         typedef uci::base::BoundedList<uci::type::PointingTypeEnum, uci::type::accessorType::pointingTypeEnum> PointingCapability;

         /** Indicates all of the subcapabilities that a POST subsystem has available for command and control. Unique
           * configurations can appear multiple times as individual IDs which allows for both: 1. One ID associated with multiple
           * capabilities 2. A unique ID associated with one capability [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_SubCapabilityType, uci::type::accessorType::pOST_SubCapabilityType> SubCapability;

         /** Indicates a Subsystem Component that is relevant to command and control of the associated Capability. A Component's
           * characteristics and performance directly influence those of the associated Capability. One or more defined components
           * may have settings/configuration independent with the associated Capability. Independent control of subsystem
           * components as related to capabilities also adds flexibility in the command and control of independent components
           * hosted within a single subsystem. Unique configurations can appear multiple times as individual IDs which allows for
           * both: 1. One ID associated with multiple capabilities 2. A unique ID associated with one capability [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_ComponentType, uci::type::accessorType::pOST_ComponentType> POST_Component;

         /** Allowable range of values for Passive Optical Search and Track parameters that can be used to customize target
           * detections. Indicates all of the track options that a POST subsystem has available for command and control. Unique
           * configurations can appear multiple times as individual IDs which allows for both: 1. One ID associated with multiple
           * capabilities 2. A unique ID associated with one capability [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_TrackCapabilityType, uci::type::accessorType::pOST_TrackCapabilityType> TrackOption;

         /** This field specifies which type of collection constraints are supported by this sensor. If multiple instances are
           * given, each should be of a different constraint type as indicated by the child element. [Minimum occurrences: 0]
           * [Maximum occurrences: 9]
           */
         typedef uci::base::BoundedList<uci::type::PO_CollectionConstraintsType, uci::type::accessorType::pO_CollectionConstraintsType> CollectionConstraints;

         /** Returns the bounded list that is identified by the Capability.
           *
           * @return The bounded list identified by Capability.
           */
         virtual const uci::type::POST_CapabilityMDT::Capability& getCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Capability.
           *
           * @return The bounded list identified by Capability.
           */
         virtual uci::type::POST_CapabilityMDT::Capability& getCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Capability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapability(const uci::type::POST_CapabilityMDT::Capability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PointingCapability.
           *
           * @return The bounded list identified by PointingCapability.
           */
         virtual const uci::type::POST_CapabilityMDT::PointingCapability& getPointingCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PointingCapability.
           *
           * @return The bounded list identified by PointingCapability.
           */
         virtual uci::type::POST_CapabilityMDT::PointingCapability& getPointingCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PointingCapability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPointingCapability(const uci::type::POST_CapabilityMDT::PointingCapability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual const uci::type::POST_CapabilityMDT::SubCapability& getSubCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual uci::type::POST_CapabilityMDT::SubCapability& getSubCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapability(const uci::type::POST_CapabilityMDT::SubCapability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the POST_Component.
           *
           * @return The bounded list identified by POST_Component.
           */
         virtual const uci::type::POST_CapabilityMDT::POST_Component& getPOST_Component() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the POST_Component.
           *
           * @return The bounded list identified by POST_Component.
           */
         virtual uci::type::POST_CapabilityMDT::POST_Component& getPOST_Component()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the POST_Component.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPOST_Component(const uci::type::POST_CapabilityMDT::POST_Component& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TrackOption.
           *
           * @return The bounded list identified by TrackOption.
           */
         virtual const uci::type::POST_CapabilityMDT::TrackOption& getTrackOption() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TrackOption.
           *
           * @return The bounded list identified by TrackOption.
           */
         virtual uci::type::POST_CapabilityMDT::TrackOption& getTrackOption()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TrackOption.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTrackOption(const uci::type::POST_CapabilityMDT::TrackOption& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CollectionConstraints.
           *
           * @return The bounded list identified by CollectionConstraints.
           */
         virtual const uci::type::POST_CapabilityMDT::CollectionConstraints& getCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CollectionConstraints.
           *
           * @return The bounded list identified by CollectionConstraints.
           */
         virtual uci::type::POST_CapabilityMDT::CollectionConstraints& getCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CollectionConstraints.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCollectionConstraints(const uci::type::POST_CapabilityMDT::CollectionConstraints& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityFOR.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityFOR.
           */
         virtual const uci::type::FOR_Type& getCapabilityFOR() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityFOR.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityFOR.
           */
         virtual uci::type::FOR_Type& getCapabilityFOR()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityFOR to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityFOR
           */
         virtual void setCapabilityFOR(const uci::type::FOR_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CapabilityFOR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CapabilityFOR is emabled or not
           */
         virtual bool hasCapabilityFOR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CapabilityFOR
           *
           * @param type = uci::type::accessorType::fOR_Type This Accessor's accessor type
           */
         virtual void enableCapabilityFOR(uci::base::accessorType::AccessorType type = uci::type::accessorType::fOR_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CapabilityFOR */
         virtual void clearCapabilityFOR()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * CapabilityThermalBlankingFOR.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityThermalBlankingFOR.
           */
         virtual const uci::type::FOR_Type& getCapabilityThermalBlankingFOR() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * CapabilityThermalBlankingFOR.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityThermalBlankingFOR.
           */
         virtual uci::type::FOR_Type& getCapabilityThermalBlankingFOR()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityThermalBlankingFOR to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityThermalBlankingFOR
           */
         virtual void setCapabilityThermalBlankingFOR(const uci::type::FOR_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CapabilityThermalBlankingFOR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CapabilityThermalBlankingFOR is emabled or not
           */
         virtual bool hasCapabilityThermalBlankingFOR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CapabilityThermalBlankingFOR
           *
           * @param type = uci::type::accessorType::fOR_Type This Accessor's accessor type
           */
         virtual void enableCapabilityThermalBlankingFOR(uci::base::accessorType::AccessorType type = uci::type::accessorType::fOR_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CapabilityThermalBlankingFOR */
         virtual void clearCapabilityThermalBlankingFOR()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_CapabilityMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_CapabilityMDT to copy from
           */
         POST_CapabilityMDT(const POST_CapabilityMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_CapabilityMDT to the contents of the POST_CapabilityMDT on
           * the right hand side (rhs) of the assignment operator.POST_CapabilityMDT [only available to derived classes]
           *
           * @param rhs The POST_CapabilityMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::POST_CapabilityMDT
           * @return A constant reference to this POST_CapabilityMDT.
           */
         const POST_CapabilityMDT& operator=(const POST_CapabilityMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_CapabilityMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_CapabilityMDT_h

