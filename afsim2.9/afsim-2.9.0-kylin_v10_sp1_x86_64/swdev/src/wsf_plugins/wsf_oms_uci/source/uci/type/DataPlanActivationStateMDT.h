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
#ifndef Uci__Type__DataPlanActivationStateMDT_h
#define Uci__Type__DataPlanActivationStateMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DataPlanID_Type_h)
# include "uci/type/DataPlanID_Type.h"
#endif

#if !defined(Uci__Type__DataPlanActivationStateEnum_h)
# include "uci/type/DataPlanActivationStateEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DataPlanEffectivityStateType_h)
# include "uci/type/DataPlanEffectivityStateType.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** See the annotation in the associated message for an overall description of the message and this type. */
      class DataPlanActivationStateMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DataPlanActivationStateMDT()
         { }

         /** Returns this accessor's type constant, i.e. DataPlanActivationStateMDT
           *
           * @return This accessor's type constant, i.e. DataPlanActivationStateMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dataPlanActivationStateMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DataPlanActivationStateMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The current state of the Data Plan effectivities [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DataPlanEffectivityStateType, uci::type::accessorType::dataPlanEffectivityStateType> EffectivityState;

         /** This element is used to trace a UCI mission plan to a foreign, non-UCI ID. The primary use case for this element is
           * to allow a System’s mission plan adapter service to report the ID of the System specific formatted mission plan
           * generated from the UCI mission plan. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> ForeignDataPlanID;

         /** Returns the accessor that provides access to the complex content that is identified by the DataPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataPlanID.
           */
         virtual const uci::type::DataPlanID_Type& getDataPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DataPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataPlanID.
           */
         virtual uci::type::DataPlanID_Type& getDataPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DataPlanID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DataPlanID
           */
         virtual void setDataPlanID(const uci::type::DataPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the State.
           *
           * @return The value of the enumeration identified by State.
           */
         virtual const uci::type::DataPlanActivationStateEnum& getState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the State.
           *
           * @return The value of the enumeration identified by State.
           */
         virtual uci::type::DataPlanActivationStateEnum& getState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setState(const uci::type::DataPlanActivationStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the State.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setState(uci::type::DataPlanActivationStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EffectivityState.
           *
           * @return The bounded list identified by EffectivityState.
           */
         virtual const uci::type::DataPlanActivationStateMDT::EffectivityState& getEffectivityState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EffectivityState.
           *
           * @return The bounded list identified by EffectivityState.
           */
         virtual uci::type::DataPlanActivationStateMDT::EffectivityState& getEffectivityState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EffectivityState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEffectivityState(const uci::type::DataPlanActivationStateMDT::EffectivityState& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ForeignDataPlanID.
           *
           * @return The bounded list identified by ForeignDataPlanID.
           */
         virtual const uci::type::DataPlanActivationStateMDT::ForeignDataPlanID& getForeignDataPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ForeignDataPlanID.
           *
           * @return The bounded list identified by ForeignDataPlanID.
           */
         virtual uci::type::DataPlanActivationStateMDT::ForeignDataPlanID& getForeignDataPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ForeignDataPlanID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setForeignDataPlanID(const uci::type::DataPlanActivationStateMDT::ForeignDataPlanID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DataPlanActivationStateMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DataPlanActivationStateMDT to copy from
           */
         DataPlanActivationStateMDT(const DataPlanActivationStateMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DataPlanActivationStateMDT to the contents of the
           * DataPlanActivationStateMDT on the right hand side (rhs) of the assignment operator.DataPlanActivationStateMDT [only
           * available to derived classes]
           *
           * @param rhs The DataPlanActivationStateMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DataPlanActivationStateMDT
           * @return A constant reference to this DataPlanActivationStateMDT.
           */
         const DataPlanActivationStateMDT& operator=(const DataPlanActivationStateMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DataPlanActivationStateMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DataPlanActivationStateMDT_h

