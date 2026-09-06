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
#ifndef Uci__Type__MissionDefinitionMDT_h
#define Uci__Type__MissionDefinitionMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionID_Type_h)
# include "uci/type/MissionID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MissionTypeCategory_h)
# include "uci/type/MissionTypeCategory.h"
#endif

#if !defined(Uci__Type__MissionStateEnum_h)
# include "uci/type/MissionStateEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__UserID_Type_h)
# include "uci/type/UserID_Type.h"
#endif

#if !defined(Uci__Type__ArchiveRequestType_h)
# include "uci/type/ArchiveRequestType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionDefinitionMDT sequence accessor class */
      class MissionDefinitionMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionDefinitionMDT()
         { }

         /** Returns this accessor's type constant, i.e. MissionDefinitionMDT
           *
           * @return This accessor's type constant, i.e. MissionDefinitionMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionDefinitionMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionDefinitionMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** An operator specified purpose or category of the mission. This gives the ability for displays or services to filter
           * missions by their type. The absence of this element indicates that the mission has no special type. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MissionTypeCategory, uci::type::accessorType::missionTypeCategory> MissionType;

         /** Indicates a system authorized to access mission definition. Systems who are listed are associated with planning or
           * execution of the mission. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> AuthorizedSystemID;

         /** Indicates a user authorized to access mission definition. Users who are listed are associated with planning or
           * execution of the mission. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::UserID_Type, uci::type::accessorType::userID_Type> AuthorizedUserID;

         /** Returns the accessor that provides access to the complex content that is identified by the MissionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionID.
           */
         virtual const uci::type::MissionID_Type& getMissionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionID.
           */
         virtual uci::type::MissionID_Type& getMissionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionID
           */
         virtual void setMissionID(const uci::type::MissionID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionType.
           *
           * @return The bounded list identified by MissionType.
           */
         virtual const uci::type::MissionDefinitionMDT::MissionType& getMissionType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionType.
           *
           * @return The bounded list identified by MissionType.
           */
         virtual uci::type::MissionDefinitionMDT::MissionType& getMissionType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionType(const uci::type::MissionDefinitionMDT::MissionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MissionState.
           *
           * @return The value of the enumeration identified by MissionState.
           */
         virtual const uci::type::MissionStateEnum& getMissionState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MissionState.
           *
           * @return The value of the enumeration identified by MissionState.
           */
         virtual uci::type::MissionStateEnum& getMissionState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MissionState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMissionState(const uci::type::MissionStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MissionState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMissionState(uci::type::MissionStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Expired.
           *
           * @return The value of the simple primitive data type identified by Expired.
           */
         virtual xs::Boolean getExpired() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Expired.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setExpired(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AuthorizedSystemID.
           *
           * @return The bounded list identified by AuthorizedSystemID.
           */
         virtual const uci::type::MissionDefinitionMDT::AuthorizedSystemID& getAuthorizedSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AuthorizedSystemID.
           *
           * @return The bounded list identified by AuthorizedSystemID.
           */
         virtual uci::type::MissionDefinitionMDT::AuthorizedSystemID& getAuthorizedSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AuthorizedSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAuthorizedSystemID(const uci::type::MissionDefinitionMDT::AuthorizedSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AuthorizedUserID.
           *
           * @return The bounded list identified by AuthorizedUserID.
           */
         virtual const uci::type::MissionDefinitionMDT::AuthorizedUserID& getAuthorizedUserID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AuthorizedUserID.
           *
           * @return The bounded list identified by AuthorizedUserID.
           */
         virtual uci::type::MissionDefinitionMDT::AuthorizedUserID& getAuthorizedUserID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AuthorizedUserID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAuthorizedUserID(const uci::type::MissionDefinitionMDT::AuthorizedUserID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ArchiveConfiguration.
           *
           * @return The acecssor that provides access to the complex content that is identified by ArchiveConfiguration.
           */
         virtual const uci::type::ArchiveRequestType& getArchiveConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ArchiveConfiguration.
           *
           * @return The acecssor that provides access to the complex content that is identified by ArchiveConfiguration.
           */
         virtual uci::type::ArchiveRequestType& getArchiveConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ArchiveConfiguration to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ArchiveConfiguration
           */
         virtual void setArchiveConfiguration(const uci::type::ArchiveRequestType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ArchiveConfiguration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ArchiveConfiguration is emabled or not
           */
         virtual bool hasArchiveConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ArchiveConfiguration
           *
           * @param type = uci::type::accessorType::archiveRequestType This Accessor's accessor type
           */
         virtual void enableArchiveConfiguration(uci::base::accessorType::AccessorType type = uci::type::accessorType::archiveRequestType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ArchiveConfiguration */
         virtual void clearArchiveConfiguration()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionDefinitionMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionDefinitionMDT to copy from
           */
         MissionDefinitionMDT(const MissionDefinitionMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionDefinitionMDT to the contents of the MissionDefinitionMDT
           * on the right hand side (rhs) of the assignment operator.MissionDefinitionMDT [only available to derived classes]
           *
           * @param rhs The MissionDefinitionMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::MissionDefinitionMDT
           * @return A constant reference to this MissionDefinitionMDT.
           */
         const MissionDefinitionMDT& operator=(const MissionDefinitionMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionDefinitionMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionDefinitionMDT_h

