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
#ifndef Uci__Type__EntitySignalSummaryType_h
#define Uci__Type__EntitySignalSummaryType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SignalSummaryType_h)
# include "uci/type/SignalSummaryType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__SignalID_Type_h)
# include "uci/type/SignalID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntitySignalSummaryType sequence accessor class */
      class EntitySignalSummaryType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntitySignalSummaryType()
         { }

         /** Returns this accessor's type constant, i.e. EntitySignalSummaryType
           *
           * @return This accessor's type constant, i.e. EntitySignalSummaryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entitySignalSummaryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntitySignalSummaryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalDescription.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalDescription.
           */
         virtual const uci::type::SignalSummaryType& getSignalDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalDescription.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalDescription.
           */
         virtual uci::type::SignalSummaryType& getSignalDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SignalDescription to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SignalDescription
           */
         virtual void setSignalDescription(const uci::type::SignalSummaryType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LatestDetectionTimestamp.
           *
           * @return The value of the simple primitive data type identified by LatestDetectionTimestamp.
           */
         virtual uci::type::DateTimeTypeValue getLatestDetectionTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LatestDetectionTimestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLatestDetectionTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LatestDetectionTimestamp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LatestDetectionTimestamp is emabled or not
           */
         virtual bool hasLatestDetectionTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LatestDetectionTimestamp
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableLatestDetectionTimestamp(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LatestDetectionTimestamp */
         virtual void clearLatestDetectionTimestamp()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalID.
           */
         virtual const uci::type::SignalID_Type& getSignalID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalID.
           */
         virtual uci::type::SignalID_Type& getSignalID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SignalID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SignalID
           */
         virtual void setSignalID(const uci::type::SignalID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SignalID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SignalID is emabled or not
           */
         virtual bool hasSignalID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SignalID
           *
           * @param type = uci::type::accessorType::signalID_Type This Accessor's accessor type
           */
         virtual void enableSignalID(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SignalID */
         virtual void clearSignalID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntitySignalSummaryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntitySignalSummaryType to copy from
           */
         EntitySignalSummaryType(const EntitySignalSummaryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntitySignalSummaryType to the contents of the
           * EntitySignalSummaryType on the right hand side (rhs) of the assignment operator.EntitySignalSummaryType [only
           * available to derived classes]
           *
           * @param rhs The EntitySignalSummaryType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EntitySignalSummaryType
           * @return A constant reference to this EntitySignalSummaryType.
           */
         const EntitySignalSummaryType& operator=(const EntitySignalSummaryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntitySignalSummaryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntitySignalSummaryType_h

