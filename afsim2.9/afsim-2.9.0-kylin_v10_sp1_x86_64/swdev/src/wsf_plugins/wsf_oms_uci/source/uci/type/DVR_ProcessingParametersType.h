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
#ifndef Uci__Type__DVR_ProcessingParametersType_h
#define Uci__Type__DVR_ProcessingParametersType_h 1

#if !defined(Uci__Type__ProcessingParametersPET_h)
# include "uci/type/ProcessingParametersPET.h"
#endif

#if !defined(Uci__Type__DVR_OperationEnum_h)
# include "uci/type/DVR_OperationEnum.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base ProcessingParametersPET. This represents
        * the processing parameters needed for using a DVR-like service to create new Products from existing video Products or
        * streams.
        */
      class DVR_ProcessingParametersType : public virtual uci::type::ProcessingParametersPET {
      public:

         /** The destructor */
         virtual ~DVR_ProcessingParametersType()
         { }

         /** Returns this accessor's type constant, i.e. DVR_ProcessingParametersType
           *
           * @return This accessor's type constant, i.e. DVR_ProcessingParametersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dVR_ProcessingParametersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DVR_ProcessingParametersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Operation.
           *
           * @return The value of the enumeration identified by Operation.
           */
         virtual const uci::type::DVR_OperationEnum& getOperation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Operation.
           *
           * @return The value of the enumeration identified by Operation.
           */
         virtual uci::type::DVR_OperationEnum& getOperation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Operation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperation(const uci::type::DVR_OperationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Operation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperation(uci::type::DVR_OperationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StartTime.
           *
           * @return The value of the simple primitive data type identified by StartTime.
           */
         virtual uci::type::DurationTypeValue getStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StartTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStartTime(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StartTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StartTime is emabled or not
           */
         virtual bool hasStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StartTime
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableStartTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StartTime */
         virtual void clearStartTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EndTime.
           *
           * @return The value of the simple primitive data type identified by EndTime.
           */
         virtual uci::type::DurationTypeValue getEndTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EndTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEndTime(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EndTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EndTime is emabled or not
           */
         virtual bool hasEndTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EndTime
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableEndTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EndTime */
         virtual void clearEndTime()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DVR_ProcessingParametersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DVR_ProcessingParametersType to copy from
           */
         DVR_ProcessingParametersType(const DVR_ProcessingParametersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DVR_ProcessingParametersType to the contents of the
           * DVR_ProcessingParametersType on the right hand side (rhs) of the assignment operator.DVR_ProcessingParametersType
           * [only available to derived classes]
           *
           * @param rhs The DVR_ProcessingParametersType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::DVR_ProcessingParametersType
           * @return A constant reference to this DVR_ProcessingParametersType.
           */
         const DVR_ProcessingParametersType& operator=(const DVR_ProcessingParametersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DVR_ProcessingParametersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DVR_ProcessingParametersType_h

