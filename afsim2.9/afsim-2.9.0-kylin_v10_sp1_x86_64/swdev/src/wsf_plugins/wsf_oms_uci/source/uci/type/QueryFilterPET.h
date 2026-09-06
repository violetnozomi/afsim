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
#ifndef Uci__Type__QueryFilterPET_h
#define Uci__Type__QueryFilterPET_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MissionID_Type_h)
# include "uci/type/MissionID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__SecurityInformationFilterRangeType_h)
# include "uci/type/SecurityInformationFilterRangeType.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the abstract base for all QueryFilterType (QFT) complexTypes. It is a Polymorphic Extension Type (PET). In
        * UCI, PETs are used in select instances to allow a controlled set of complexTypes, defined elsewhere in the schema, to
        * be used to define an element without creating a large choice statement or other unwieldy XSD definition.
        */
      class QueryFilterPET : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~QueryFilterPET()
         { }

         /** Returns this accessor's type constant, i.e. QueryFilterPET
           *
           * @return This accessor's type constant, i.e. QueryFilterPET
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::queryFilterPET;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const QueryFilterPET& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on the Mission ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::MissionID_Type, uci::type::accessorType::missionID_Type> MissionID;

         /** Filter on the range of time when the object was created [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeType, uci::type::accessorType::dateTimeRangeType> CreationDateRange;

         /** Filter on ranges of security information. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SecurityInformationFilterRangeType, uci::type::accessorType::securityInformationFilterRangeType> SecurityInformationRange;

         /** Filter on the security information markings of the object to be the specified security information. This filter will
           * be treated as an exact match filter. When including multiple elements, it is considered to be a logical OR [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SecurityInformationType, uci::type::accessorType::securityInformationType> SecurityInformation;

         /** Returns the bounded list that is identified by the MissionID.
           *
           * @return The bounded list identified by MissionID.
           */
         virtual const uci::type::QueryFilterPET::MissionID& getMissionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionID.
           *
           * @return The bounded list identified by MissionID.
           */
         virtual uci::type::QueryFilterPET::MissionID& getMissionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionID(const uci::type::QueryFilterPET::MissionID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CreationDateRange.
           *
           * @return The bounded list identified by CreationDateRange.
           */
         virtual const uci::type::QueryFilterPET::CreationDateRange& getCreationDateRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CreationDateRange.
           *
           * @return The bounded list identified by CreationDateRange.
           */
         virtual uci::type::QueryFilterPET::CreationDateRange& getCreationDateRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CreationDateRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCreationDateRange(const uci::type::QueryFilterPET::CreationDateRange& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecurityInformationRange.
           *
           * @return The bounded list identified by SecurityInformationRange.
           */
         virtual const uci::type::QueryFilterPET::SecurityInformationRange& getSecurityInformationRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecurityInformationRange.
           *
           * @return The bounded list identified by SecurityInformationRange.
           */
         virtual uci::type::QueryFilterPET::SecurityInformationRange& getSecurityInformationRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SecurityInformationRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSecurityInformationRange(const uci::type::QueryFilterPET::SecurityInformationRange& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecurityInformation.
           *
           * @return The bounded list identified by SecurityInformation.
           */
         virtual const uci::type::QueryFilterPET::SecurityInformation& getSecurityInformation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecurityInformation.
           *
           * @return The bounded list identified by SecurityInformation.
           */
         virtual uci::type::QueryFilterPET::SecurityInformation& getSecurityInformation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SecurityInformation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSecurityInformation(const uci::type::QueryFilterPET::SecurityInformation& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         QueryFilterPET()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The QueryFilterPET to copy from
           */
         QueryFilterPET(const QueryFilterPET& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this QueryFilterPET to the contents of the QueryFilterPET on the right
           * hand side (rhs) of the assignment operator.QueryFilterPET [only available to derived classes]
           *
           * @param rhs The QueryFilterPET on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::QueryFilterPET
           * @return A constant reference to this QueryFilterPET.
           */
         const QueryFilterPET& operator=(const QueryFilterPET& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // QueryFilterPET


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__QueryFilterPET_h

