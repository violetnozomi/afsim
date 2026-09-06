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
#ifndef Uci__Type__MissionDefinitionQFT_h
#define Uci__Type__MissionDefinitionQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Type__MissionStateEnum_h)
# include "uci/type/MissionStateEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__UserID_Type_h)
# include "uci/type/UserID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__MissionTypeCategory_h)
# include "uci/type/MissionTypeCategory.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionDefinitionQFT sequence accessor class */
      class MissionDefinitionQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~MissionDefinitionQFT()
         { }

         /** Returns this accessor's type constant, i.e. MissionDefinitionQFT
           *
           * @return This accessor's type constant, i.e. MissionDefinitionQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionDefinitionQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionDefinitionQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a query filter to specify an authorized user. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::UserID_Type, uci::type::accessorType::userID_Type> AuthorizedUserID;

         /** Indicates a query filter to specify an authorized system. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> AuthorizedSystemID;

         /** An operator specified purpose or category of the mission. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MissionTypeCategory, uci::type::accessorType::missionTypeCategory> MissionType;

         /** Returns the value of the enumeration that is identified by the Active.
           *
           * @return The value of the enumeration identified by Active.
           */
         virtual const uci::type::MissionStateEnum& getActive() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Active.
           *
           * @return The value of the enumeration identified by Active.
           */
         virtual uci::type::MissionStateEnum& getActive()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Active.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setActive(const uci::type::MissionStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Active.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setActive(uci::type::MissionStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Activeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasActive() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getActive will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::missionStateEnum This Accessor's accessor type
           */
         virtual void enableActive(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionStateEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearActive()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AuthorizedUserID.
           *
           * @return The bounded list identified by AuthorizedUserID.
           */
         virtual const uci::type::MissionDefinitionQFT::AuthorizedUserID& getAuthorizedUserID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AuthorizedUserID.
           *
           * @return The bounded list identified by AuthorizedUserID.
           */
         virtual uci::type::MissionDefinitionQFT::AuthorizedUserID& getAuthorizedUserID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AuthorizedUserID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAuthorizedUserID(const uci::type::MissionDefinitionQFT::AuthorizedUserID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AuthorizedSystemID.
           *
           * @return The bounded list identified by AuthorizedSystemID.
           */
         virtual const uci::type::MissionDefinitionQFT::AuthorizedSystemID& getAuthorizedSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AuthorizedSystemID.
           *
           * @return The bounded list identified by AuthorizedSystemID.
           */
         virtual uci::type::MissionDefinitionQFT::AuthorizedSystemID& getAuthorizedSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AuthorizedSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAuthorizedSystemID(const uci::type::MissionDefinitionQFT::AuthorizedSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionType.
           *
           * @return The bounded list identified by MissionType.
           */
         virtual const uci::type::MissionDefinitionQFT::MissionType& getMissionType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionType.
           *
           * @return The bounded list identified by MissionType.
           */
         virtual uci::type::MissionDefinitionQFT::MissionType& getMissionType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionType(const uci::type::MissionDefinitionQFT::MissionType& value)
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


         /** Returns whether the Element that is identified by Expired exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Expired is emabled or not
           */
         virtual bool hasExpired() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Expired
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableExpired(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Expired */
         virtual void clearExpired()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionDefinitionQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionDefinitionQFT to copy from
           */
         MissionDefinitionQFT(const MissionDefinitionQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionDefinitionQFT to the contents of the MissionDefinitionQFT
           * on the right hand side (rhs) of the assignment operator.MissionDefinitionQFT [only available to derived classes]
           *
           * @param rhs The MissionDefinitionQFT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::MissionDefinitionQFT
           * @return A constant reference to this MissionDefinitionQFT.
           */
         const MissionDefinitionQFT& operator=(const MissionDefinitionQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionDefinitionQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionDefinitionQFT_h

