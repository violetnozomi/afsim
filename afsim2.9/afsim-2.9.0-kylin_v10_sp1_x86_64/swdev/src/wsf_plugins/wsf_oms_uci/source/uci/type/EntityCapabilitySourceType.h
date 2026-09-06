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
#ifndef Uci__Type__EntityCapabilitySourceType_h
#define Uci__Type__EntityCapabilitySourceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ActivityID_Type_h)
# include "uci/type/ActivityID_Type.h"
#endif

#if !defined(Uci__Type__EntityExternalType_h)
# include "uci/type/EntityExternalType.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityCapabilitySourceType sequence accessor class */
      class EntityCapabilitySourceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityCapabilitySourceType()
         { }

         /** Returns this accessor's type constant, i.e. EntityCapabilitySourceType
           *
           * @return This accessor's type constant, i.e. EntityCapabilitySourceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityCapabilitySourceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityCapabilitySourceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an Activity that detected the Entity. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ActivityID_Type, uci::type::accessorType::activityID_Type> ActivityID;

         /** Indicates the unique ID of an Entity, that was an input to the Capability/Subsystem Service, which the
           * Capability/Subsystem has found to correlate closely to this Entity. Declaration of correlation is not an indication
           * of fusion; kinematic, identity and other data in this Capability Entity shall not be fused/blended with data from the
           * correlated input Entity. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> CorrelationID;

         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityID
           */
         virtual void setCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityID.
           *
           * @return The bounded list identified by ActivityID.
           */
         virtual const uci::type::EntityCapabilitySourceType::ActivityID& getActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityID.
           *
           * @return The bounded list identified by ActivityID.
           */
         virtual uci::type::EntityCapabilitySourceType::ActivityID& getActivityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ActivityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivityID(const uci::type::EntityCapabilitySourceType::ActivityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TrackNumberID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackNumberID.
           */
         virtual const uci::type::EntityExternalType& getTrackNumberID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TrackNumberID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TrackNumberID.
           */
         virtual uci::type::EntityExternalType& getTrackNumberID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TrackNumberID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TrackNumberID
           */
         virtual void setTrackNumberID(const uci::type::EntityExternalType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TrackNumberID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TrackNumberID is emabled or not
           */
         virtual bool hasTrackNumberID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TrackNumberID
           *
           * @param type = uci::type::accessorType::entityExternalType This Accessor's accessor type
           */
         virtual void enableTrackNumberID(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityExternalType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TrackNumberID */
         virtual void clearTrackNumberID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CorrelationID.
           *
           * @return The bounded list identified by CorrelationID.
           */
         virtual const uci::type::EntityCapabilitySourceType::CorrelationID& getCorrelationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CorrelationID.
           *
           * @return The bounded list identified by CorrelationID.
           */
         virtual uci::type::EntityCapabilitySourceType::CorrelationID& getCorrelationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CorrelationID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCorrelationID(const uci::type::EntityCapabilitySourceType::CorrelationID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityCapabilitySourceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityCapabilitySourceType to copy from
           */
         EntityCapabilitySourceType(const EntityCapabilitySourceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityCapabilitySourceType to the contents of the
           * EntityCapabilitySourceType on the right hand side (rhs) of the assignment operator.EntityCapabilitySourceType [only
           * available to derived classes]
           *
           * @param rhs The EntityCapabilitySourceType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EntityCapabilitySourceType
           * @return A constant reference to this EntityCapabilitySourceType.
           */
         const EntityCapabilitySourceType& operator=(const EntityCapabilitySourceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityCapabilitySourceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityCapabilitySourceType_h

