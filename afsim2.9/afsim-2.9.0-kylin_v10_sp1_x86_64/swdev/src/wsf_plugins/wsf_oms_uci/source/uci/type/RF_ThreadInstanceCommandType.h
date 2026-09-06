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
#ifndef Uci__Type__RF_ThreadInstanceCommandType_h
#define Uci__Type__RF_ThreadInstanceCommandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SetupRF_ThreadInstanceType_h)
# include "uci/type/SetupRF_ThreadInstanceType.h"
#endif

#if !defined(Uci__Type__ModifyRF_ThreadInstanceType_h)
# include "uci/type/ModifyRF_ThreadInstanceType.h"
#endif

#if !defined(Uci__Type__RemoveRF_ThreadInstanceType_h)
# include "uci/type/RemoveRF_ThreadInstanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RF_ThreadInstanceCommandType choice accessor class */
      class RF_ThreadInstanceCommandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RF_ThreadInstanceCommandType()
         { }

         /** Returns this accessor's type constant, i.e. RF_ThreadInstanceCommandType
           *
           * @return This accessor's type constant, i.e. RF_ThreadInstanceCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_ThreadInstanceCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_ThreadInstanceCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Sets up the allocated RF Thread Instance that will be utilized by the subsystem (e.g. EA, ESM). This information is
           * passed to the Real Time Arbitration and Control component of the RFDC for configuration for future use. [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SetupRF_ThreadInstanceType, uci::type::accessorType::setupRF_ThreadInstanceType> SetupRF_ThreadInstance;

         /** Modify RF Thread Instances that has been setup. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ModifyRF_ThreadInstanceType, uci::type::accessorType::modifyRF_ThreadInstanceType> ModifyRF_ThreadInstance;

         /** Remove RF Thread Instances that has been setup. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::RemoveRF_ThreadInstanceType, uci::type::accessorType::removeRF_ThreadInstanceType> RemoveRF_ThreadInstance;

         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum RF_ThreadInstanceCommandTypeChoice {
            RF_THREADINSTANCECOMMANDTYPE_CHOICE_NONE,
            RF_THREADINSTANCECOMMANDTYPE_CHOICE_SETUPRF_THREADINSTANCE,
            RF_THREADINSTANCECOMMANDTYPE_CHOICE_MODIFYRF_THREADINSTANCE,
            RF_THREADINSTANCECOMMANDTYPE_CHOICE_REMOVERF_THREADINSTANCE
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual RF_ThreadInstanceCommandTypeChoice getRF_ThreadInstanceCommandTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getRF_ThreadInstanceCommandTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In
           * addition, the access methods associated with the chosen element will be enabled and will provide access to the chosen
           * element.
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
         virtual void setRF_ThreadInstanceCommandTypeChoiceOrdinal(RF_ThreadInstanceCommandTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SetupRF_ThreadInstance.
           *
           * @return The bounded list identified by SetupRF_ThreadInstance.
           */
         virtual const uci::type::RF_ThreadInstanceCommandType::SetupRF_ThreadInstance& getSetupRF_ThreadInstance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SetupRF_ThreadInstance.
           *
           * @return The bounded list identified by SetupRF_ThreadInstance.
           */
         virtual uci::type::RF_ThreadInstanceCommandType::SetupRF_ThreadInstance& getSetupRF_ThreadInstance()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SetupRF_ThreadInstance.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSetupRF_ThreadInstance(const uci::type::RF_ThreadInstanceCommandType::SetupRF_ThreadInstance& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ModifyRF_ThreadInstance.
           *
           * @return The bounded list identified by ModifyRF_ThreadInstance.
           */
         virtual const uci::type::RF_ThreadInstanceCommandType::ModifyRF_ThreadInstance& getModifyRF_ThreadInstance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ModifyRF_ThreadInstance.
           *
           * @return The bounded list identified by ModifyRF_ThreadInstance.
           */
         virtual uci::type::RF_ThreadInstanceCommandType::ModifyRF_ThreadInstance& getModifyRF_ThreadInstance()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ModifyRF_ThreadInstance.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setModifyRF_ThreadInstance(const uci::type::RF_ThreadInstanceCommandType::ModifyRF_ThreadInstance& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RemoveRF_ThreadInstance.
           *
           * @return The bounded list identified by RemoveRF_ThreadInstance.
           */
         virtual const uci::type::RF_ThreadInstanceCommandType::RemoveRF_ThreadInstance& getRemoveRF_ThreadInstance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RemoveRF_ThreadInstance.
           *
           * @return The bounded list identified by RemoveRF_ThreadInstance.
           */
         virtual uci::type::RF_ThreadInstanceCommandType::RemoveRF_ThreadInstance& getRemoveRF_ThreadInstance()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RemoveRF_ThreadInstance.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRemoveRF_ThreadInstance(const uci::type::RF_ThreadInstanceCommandType::RemoveRF_ThreadInstance& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_ThreadInstanceCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_ThreadInstanceCommandType to copy from
           */
         RF_ThreadInstanceCommandType(const RF_ThreadInstanceCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_ThreadInstanceCommandType to the contents of the
           * RF_ThreadInstanceCommandType on the right hand side (rhs) of the assignment operator.RF_ThreadInstanceCommandType
           * [only available to derived classes]
           *
           * @param rhs The RF_ThreadInstanceCommandType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::RF_ThreadInstanceCommandType
           * @return A constant reference to this RF_ThreadInstanceCommandType.
           */
         const RF_ThreadInstanceCommandType& operator=(const RF_ThreadInstanceCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_ThreadInstanceCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_ThreadInstanceCommandType_h

