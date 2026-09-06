// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ArchiveRequestType_h
#define Uci__Type__ArchiveRequestType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ArchiveRequestType choice accessor class */
      class ArchiveRequestType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ArchiveRequestType()
         { }

         /** Returns this accessor's type constant, i.e. ArchiveRequestType
           *
           * @return This accessor's type constant, i.e. ArchiveRequestType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::archiveRequestType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ArchiveRequestType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum ArchiveRequestTypeChoice {
            ARCHIVEREQUESTTYPE_CHOICE_NONE,
            ARCHIVEREQUESTTYPE_CHOICE_TIME,
            ARCHIVEREQUESTTYPE_CHOICE_NUMBEROFDAYS,
            ARCHIVEREQUESTTYPE_CHOICE_SPACENEEDED,
            ARCHIVEREQUESTTYPE_CHOICE_MANUALLYDELETED,
            ARCHIVEREQUESTTYPE_CHOICE_NUMBEROFMISSIONS
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual ArchiveRequestTypeChoice getArchiveRequestTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getArchiveRequestTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
           * access methods associated with the chosen element will be enabled and will provide access to the chosen element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setArchiveRequestTypeChoiceOrdinal(ArchiveRequestTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Time.
           *
           * @return The value of the simple primitive data type identified by Time.
           */
         virtual uci::type::DateTimeTypeValue getTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Time.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumberOfDays.
           *
           * @return The value of the simple primitive data type identified by NumberOfDays.
           */
         virtual xs::UnsignedInt getNumberOfDays() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumberOfDays.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumberOfDays(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SpaceNeeded.
           *
           * @return The value of the simple primitive data type identified by SpaceNeeded.
           */
         virtual xs::Boolean getSpaceNeeded() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SpaceNeeded.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSpaceNeeded(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ManuallyDeleted.
           *
           * @return The value of the simple primitive data type identified by ManuallyDeleted.
           */
         virtual xs::Boolean getManuallyDeleted() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ManuallyDeleted.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setManuallyDeleted(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumberOfMissions.
           *
           * @return The value of the simple primitive data type identified by NumberOfMissions.
           */
         virtual xs::UnsignedInt getNumberOfMissions() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumberOfMissions.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumberOfMissions(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ArchiveRequestType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ArchiveRequestType to copy from
           */
         ArchiveRequestType(const ArchiveRequestType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ArchiveRequestType to the contents of the ArchiveRequestType on
           * the right hand side (rhs) of the assignment operator.ArchiveRequestType [only available to derived classes]
           *
           * @param rhs The ArchiveRequestType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ArchiveRequestType
           * @return A constant reference to this ArchiveRequestType.
           */
         const ArchiveRequestType& operator=(const ArchiveRequestType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ArchiveRequestType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ArchiveRequestType_h

