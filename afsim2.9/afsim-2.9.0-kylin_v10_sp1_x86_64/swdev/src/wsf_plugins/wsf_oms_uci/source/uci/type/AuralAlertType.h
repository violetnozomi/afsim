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
#ifndef Uci__Type__AuralAlertType_h
#define Uci__Type__AuralAlertType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__VisibleString64Type_h)
# include "uci/type/VisibleString64Type.h"
#endif

#if !defined(Uci__Type__AuralAlertRepetitionType_h)
# include "uci/type/AuralAlertRepetitionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AuralAlertType sequence accessor class */
      class AuralAlertType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AuralAlertType()
         { }

         /** Returns this accessor's type constant, i.e. AuralAlertType
           *
           * @return This accessor's type constant, i.e. AuralAlertType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::auralAlertType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AuralAlertType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AuralAlert.
           *
           * @return The value of the string data type identified by AuralAlert.
           */
         virtual const uci::type::VisibleString64Type& getAuralAlert() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AuralAlert.
           *
           * @return The value of the string data type identified by AuralAlert.
           */
         virtual uci::type::VisibleString64Type& getAuralAlert()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AuralAlert to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setAuralAlert(const uci::type::VisibleString64Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AuralAlert to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAuralAlert(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AuralAlert to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAuralAlert(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Repetition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Repetition.
           */
         virtual const uci::type::AuralAlertRepetitionType& getRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Repetition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Repetition.
           */
         virtual uci::type::AuralAlertRepetitionType& getRepetition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Repetition to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Repetition
           */
         virtual void setRepetition(const uci::type::AuralAlertRepetitionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Repetition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Repetition is emabled or not
           */
         virtual bool hasRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Repetition
           *
           * @param type = uci::type::accessorType::auralAlertRepetitionType This Accessor's accessor type
           */
         virtual void enableRepetition(uci::base::accessorType::AccessorType type = uci::type::accessorType::auralAlertRepetitionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Repetition */
         virtual void clearRepetition()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AuralAlertType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AuralAlertType to copy from
           */
         AuralAlertType(const AuralAlertType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AuralAlertType to the contents of the AuralAlertType on the right
           * hand side (rhs) of the assignment operator.AuralAlertType [only available to derived classes]
           *
           * @param rhs The AuralAlertType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::AuralAlertType
           * @return A constant reference to this AuralAlertType.
           */
         const AuralAlertType& operator=(const AuralAlertType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AuralAlertType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AuralAlertType_h

