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
#ifndef Uci__Type__ObservationReportMDT_h
#define Uci__Type__ObservationReportMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__EntityIdentityType_h)
# include "uci/type/EntityIdentityType.h"
#endif

#if !defined(Uci__Type__ActivityByType_h)
# include "uci/type/ActivityByType.h"
#endif

#if !defined(Uci__Type__RelationshipThreatType_h)
# include "uci/type/RelationshipThreatType.h"
#endif

#if !defined(Uci__Type__AmplifyingInformationType_h)
# include "uci/type/AmplifyingInformationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ObservationReportMDT sequence accessor class */
      class ObservationReportMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ObservationReportMDT()
         { }

         /** Returns this accessor's type constant, i.e. ObservationReportMDT
           *
           * @return This accessor's type constant, i.e. ObservationReportMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::observationReportMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ObservationReportMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual const uci::type::EntityID_Type& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual uci::type::EntityID_Type& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EntityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EntityID
           */
         virtual void setEntityID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::EntityIdentityType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::EntityIdentityType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::EntityIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Identity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Identity is emabled or not
           */
         virtual bool hasIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Identity
           *
           * @param type = uci::type::accessorType::entityIdentityType This Accessor's accessor type
           */
         virtual void enableIdentity(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Identity */
         virtual void clearIdentity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityBy.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityBy.
           */
         virtual const uci::type::ActivityByType& getActivityBy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityBy.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityBy.
           */
         virtual uci::type::ActivityByType& getActivityBy()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActivityBy to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActivityBy
           */
         virtual void setActivityBy(const uci::type::ActivityByType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActivityBy exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActivityBy is emabled or not
           */
         virtual bool hasActivityBy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActivityBy
           *
           * @param type = uci::type::accessorType::activityByType This Accessor's accessor type
           */
         virtual void enableActivityBy(uci::base::accessorType::AccessorType type = uci::type::accessorType::activityByType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActivityBy */
         virtual void clearActivityBy()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ThreatData.
           *
           * @return The acecssor that provides access to the complex content that is identified by ThreatData.
           */
         virtual const uci::type::RelationshipThreatType& getThreatData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ThreatData.
           *
           * @return The acecssor that provides access to the complex content that is identified by ThreatData.
           */
         virtual uci::type::RelationshipThreatType& getThreatData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ThreatData to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ThreatData
           */
         virtual void setThreatData(const uci::type::RelationshipThreatType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ThreatData exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ThreatData is emabled or not
           */
         virtual bool hasThreatData() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ThreatData
           *
           * @param type = uci::type::accessorType::relationshipThreatType This Accessor's accessor type
           */
         virtual void enableThreatData(uci::base::accessorType::AccessorType type = uci::type::accessorType::relationshipThreatType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ThreatData */
         virtual void clearThreatData()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AmplifyingInfo.
           *
           * @return The acecssor that provides access to the complex content that is identified by AmplifyingInfo.
           */
         virtual const uci::type::AmplifyingInformationType& getAmplifyingInfo() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AmplifyingInfo.
           *
           * @return The acecssor that provides access to the complex content that is identified by AmplifyingInfo.
           */
         virtual uci::type::AmplifyingInformationType& getAmplifyingInfo()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AmplifyingInfo to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AmplifyingInfo
           */
         virtual void setAmplifyingInfo(const uci::type::AmplifyingInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AmplifyingInfo exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AmplifyingInfo is emabled or not
           */
         virtual bool hasAmplifyingInfo() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AmplifyingInfo
           *
           * @param type = uci::type::accessorType::amplifyingInformationType This Accessor's accessor type
           */
         virtual void enableAmplifyingInfo(uci::base::accessorType::AccessorType type = uci::type::accessorType::amplifyingInformationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AmplifyingInfo */
         virtual void clearAmplifyingInfo()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ObservationReportMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ObservationReportMDT to copy from
           */
         ObservationReportMDT(const ObservationReportMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ObservationReportMDT to the contents of the ObservationReportMDT
           * on the right hand side (rhs) of the assignment operator.ObservationReportMDT [only available to derived classes]
           *
           * @param rhs The ObservationReportMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ObservationReportMDT
           * @return A constant reference to this ObservationReportMDT.
           */
         const ObservationReportMDT& operator=(const ObservationReportMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ObservationReportMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ObservationReportMDT_h

