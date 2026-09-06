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
#ifndef Uci__Type__DataPlanActivationCommandMDT_h
#define Uci__Type__DataPlanActivationCommandMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DataPlanActivationCommandID_Type_h)
# include "uci/type/DataPlanActivationCommandID_Type.h"
#endif

#if !defined(Uci__Type__DataPlanID_Type_h)
# include "uci/type/DataPlanID_Type.h"
#endif

#if !defined(Uci__Type__DataPlanActivationCommandEnum_h)
# include "uci/type/DataPlanActivationCommandEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DataPlanEffectivityActivationType_h)
# include "uci/type/DataPlanEffectivityActivationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** See the annotation in the associated message for an overall description of the message and this type. */
      class DataPlanActivationCommandMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DataPlanActivationCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. DataPlanActivationCommandMDT
           *
           * @return This accessor's type constant, i.e. DataPlanActivationCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dataPlanActivationCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DataPlanActivationCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Commands a Data Plan effectivity to a state. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DataPlanEffectivityActivationType, uci::type::accessorType::dataPlanEffectivityActivationType> EffectivityActivation;

         /** Returns the accessor that provides access to the complex content that is identified by the
           * DataPlanActivationCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataPlanActivationCommandID.
           */
         virtual const uci::type::DataPlanActivationCommandID_Type& getDataPlanActivationCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * DataPlanActivationCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataPlanActivationCommandID.
           */
         virtual uci::type::DataPlanActivationCommandID_Type& getDataPlanActivationCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DataPlanActivationCommandID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DataPlanActivationCommandID
           */
         virtual void setDataPlanActivationCommandID(const uci::type::DataPlanActivationCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


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


         /** Returns the value of the enumeration that is identified by the CommandType.
           *
           * @return The value of the enumeration identified by CommandType.
           */
         virtual const uci::type::DataPlanActivationCommandEnum& getCommandType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandType.
           *
           * @return The value of the enumeration identified by CommandType.
           */
         virtual uci::type::DataPlanActivationCommandEnum& getCommandType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandType(const uci::type::DataPlanActivationCommandEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandType(uci::type::DataPlanActivationCommandEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EffectivityActivation.
           *
           * @return The bounded list identified by EffectivityActivation.
           */
         virtual const uci::type::DataPlanActivationCommandMDT::EffectivityActivation& getEffectivityActivation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EffectivityActivation.
           *
           * @return The bounded list identified by EffectivityActivation.
           */
         virtual uci::type::DataPlanActivationCommandMDT::EffectivityActivation& getEffectivityActivation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EffectivityActivation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEffectivityActivation(const uci::type::DataPlanActivationCommandMDT::EffectivityActivation& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DataPlanActivationCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DataPlanActivationCommandMDT to copy from
           */
         DataPlanActivationCommandMDT(const DataPlanActivationCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DataPlanActivationCommandMDT to the contents of the
           * DataPlanActivationCommandMDT on the right hand side (rhs) of the assignment operator.DataPlanActivationCommandMDT
           * [only available to derived classes]
           *
           * @param rhs The DataPlanActivationCommandMDT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::DataPlanActivationCommandMDT
           * @return A constant reference to this DataPlanActivationCommandMDT.
           */
         const DataPlanActivationCommandMDT& operator=(const DataPlanActivationCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DataPlanActivationCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DataPlanActivationCommandMDT_h

