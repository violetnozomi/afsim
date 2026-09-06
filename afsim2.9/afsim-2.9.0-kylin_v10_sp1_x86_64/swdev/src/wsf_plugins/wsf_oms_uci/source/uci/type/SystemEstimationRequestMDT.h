// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SystemEstimationRequestMDT_h
#define Uci__Type__SystemEstimationRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemEstimationRequestID_Type_h)
# include "uci/type/SystemEstimationRequestID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EstimationMessageEnum_h)
# include "uci/type/EstimationMessageEnum.h"
#endif

#if !defined(Uci__Type__RepeatEnum_h)
# include "uci/type/RepeatEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__EstimationStartType_h)
# include "uci/type/EstimationStartType.h"
#endif

#if !defined(Uci__Type__EstimationEndType_h)
# include "uci/type/EstimationEndType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SystemEstimationRequestMDT sequence accessor class */
      class SystemEstimationRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SystemEstimationRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. SystemEstimationRequestMDT
           *
           * @return This accessor's type constant, i.e. SystemEstimationRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::systemEstimationRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SystemEstimationRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The type of estimation requested and resulting messages to produce as part of this request [Maximum occurrences: 4] */
         typedef uci::base::BoundedList<uci::type::EstimationMessageEnum, uci::type::accessorType::estimationMessageEnum> RequestType;

         /** Returns the accessor that provides access to the complex content that is identified by the SystemEstimationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemEstimationRequestID.
           */
         virtual const uci::type::SystemEstimationRequestID_Type& getSystemEstimationRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemEstimationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemEstimationRequestID.
           */
         virtual uci::type::SystemEstimationRequestID_Type& getSystemEstimationRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemEstimationRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemEstimationRequestID
           */
         virtual void setSystemEstimationRequestID(const uci::type::SystemEstimationRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RequestType.
           *
           * @return The bounded list identified by RequestType.
           */
         virtual const uci::type::SystemEstimationRequestMDT::RequestType& getRequestType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RequestType.
           *
           * @return The bounded list identified by RequestType.
           */
         virtual uci::type::SystemEstimationRequestMDT::RequestType& getRequestType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RequestType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRequestType(const uci::type::SystemEstimationRequestMDT::RequestType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RequestFrequencyType.
           *
           * @return The value of the enumeration identified by RequestFrequencyType.
           */
         virtual const uci::type::RepeatEnum& getRequestFrequencyType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RequestFrequencyType.
           *
           * @return The value of the enumeration identified by RequestFrequencyType.
           */
         virtual uci::type::RepeatEnum& getRequestFrequencyType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestFrequencyType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestFrequencyType(const uci::type::RepeatEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestFrequencyType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestFrequencyType(uci::type::RepeatEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ResultsInNativeMessage.
           *
           * @return The value of the simple primitive data type identified by ResultsInNativeMessage.
           */
         virtual xs::Boolean getResultsInNativeMessage() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ResultsInNativeMessage.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setResultsInNativeMessage(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TimeStep.
           *
           * @return The value of the simple primitive data type identified by TimeStep.
           */
         virtual uci::type::DurationTypeValue getTimeStep() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TimeStep.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimeStep(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeStep exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeStep is emabled or not
           */
         virtual bool hasTimeStep() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeStep
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableTimeStep(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeStep */
         virtual void clearTimeStep()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EstimationStart.
           *
           * @return The acecssor that provides access to the complex content that is identified by EstimationStart.
           */
         virtual const uci::type::EstimationStartType& getEstimationStart() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EstimationStart.
           *
           * @return The acecssor that provides access to the complex content that is identified by EstimationStart.
           */
         virtual uci::type::EstimationStartType& getEstimationStart()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EstimationStart to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EstimationStart
           */
         virtual void setEstimationStart(const uci::type::EstimationStartType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimationStart exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimationStart is emabled or not
           */
         virtual bool hasEstimationStart() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimationStart
           *
           * @param type = uci::type::accessorType::estimationStartType This Accessor's accessor type
           */
         virtual void enableEstimationStart(uci::base::accessorType::AccessorType type = uci::type::accessorType::estimationStartType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimationStart */
         virtual void clearEstimationStart()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EstimationEnd.
           *
           * @return The acecssor that provides access to the complex content that is identified by EstimationEnd.
           */
         virtual const uci::type::EstimationEndType& getEstimationEnd() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EstimationEnd.
           *
           * @return The acecssor that provides access to the complex content that is identified by EstimationEnd.
           */
         virtual uci::type::EstimationEndType& getEstimationEnd()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EstimationEnd to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EstimationEnd
           */
         virtual void setEstimationEnd(const uci::type::EstimationEndType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimationEnd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimationEnd is emabled or not
           */
         virtual bool hasEstimationEnd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimationEnd
           *
           * @param type = uci::type::accessorType::estimationEndType This Accessor's accessor type
           */
         virtual void enableEstimationEnd(uci::base::accessorType::AccessorType type = uci::type::accessorType::estimationEndType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimationEnd */
         virtual void clearEstimationEnd()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SystemEstimationRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SystemEstimationRequestMDT to copy from
           */
         SystemEstimationRequestMDT(const SystemEstimationRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SystemEstimationRequestMDT to the contents of the
           * SystemEstimationRequestMDT on the right hand side (rhs) of the assignment operator.SystemEstimationRequestMDT [only
           * available to derived classes]
           *
           * @param rhs The SystemEstimationRequestMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SystemEstimationRequestMDT
           * @return A constant reference to this SystemEstimationRequestMDT.
           */
         const SystemEstimationRequestMDT& operator=(const SystemEstimationRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SystemEstimationRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SystemEstimationRequestMDT_h

