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
#ifndef Uci__Type__CompletionStatusType_h
#define Uci__Type__CompletionStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** A generic type that allows the specification of incremental status. Usually populated when reporting a status of
        * 'Processing'.
        */
      class CompletionStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CompletionStatusType()
         { }

         /** Returns this accessor's type constant, i.e. CompletionStatusType
           *
           * @return This accessor's type constant, i.e. CompletionStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::completionStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CompletionStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedTimeComplete.
           *
           * @return The value of the simple primitive data type identified by EstimatedTimeComplete.
           */
         virtual uci::type::DateTimeTypeValue getEstimatedTimeComplete() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedTimeComplete.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedTimeComplete(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedTimeComplete exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedTimeComplete is emabled or not
           */
         virtual bool hasEstimatedTimeComplete() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedTimeComplete
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableEstimatedTimeComplete(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedTimeComplete */
         virtual void clearEstimatedTimeComplete()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedPercentComplete.
           *
           * @return The value of the simple primitive data type identified by EstimatedPercentComplete.
           */
         virtual uci::type::PercentTypeValue getEstimatedPercentComplete() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedPercentComplete.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedPercentComplete(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedPercentComplete exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedPercentComplete is emabled or not
           */
         virtual bool hasEstimatedPercentComplete() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedPercentComplete
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableEstimatedPercentComplete(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedPercentComplete */
         virtual void clearEstimatedPercentComplete()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualTimeStarted.
           *
           * @return The value of the simple primitive data type identified by ActualTimeStarted.
           */
         virtual uci::type::DateTimeTypeValue getActualTimeStarted() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualTimeStarted.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualTimeStarted(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualTimeStarted exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualTimeStarted is emabled or not
           */
         virtual bool hasActualTimeStarted() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualTimeStarted
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableActualTimeStarted(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualTimeStarted */
         virtual void clearActualTimeStarted()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualTimeComplete.
           *
           * @return The value of the simple primitive data type identified by ActualTimeComplete.
           */
         virtual uci::type::DateTimeTypeValue getActualTimeComplete() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualTimeComplete.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualTimeComplete(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualTimeComplete exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualTimeComplete is emabled or not
           */
         virtual bool hasActualTimeComplete() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualTimeComplete
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableActualTimeComplete(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualTimeComplete */
         virtual void clearActualTimeComplete()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Description.
           *
           * @return The value of the string data type identified by Description.
           */
         virtual const uci::type::VisibleString1024Type& getDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Description.
           *
           * @return The value of the string data type identified by Description.
           */
         virtual uci::type::VisibleString1024Type& getDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Description to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDescription(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Description to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDescription(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Description to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDescription(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Description exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Description is emabled or not
           */
         virtual bool hasDescription() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Description
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableDescription(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Description */
         virtual void clearDescription()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CompletionStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CompletionStatusType to copy from
           */
         CompletionStatusType(const CompletionStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CompletionStatusType to the contents of the CompletionStatusType
           * on the right hand side (rhs) of the assignment operator.CompletionStatusType [only available to derived classes]
           *
           * @param rhs The CompletionStatusType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::CompletionStatusType
           * @return A constant reference to this CompletionStatusType.
           */
         const CompletionStatusType& operator=(const CompletionStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CompletionStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CompletionStatusType_h

