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
#ifndef Uci__Type__EmergencyReferencePointType_h
#define Uci__Type__EmergencyReferencePointType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OpPointEmergencyEnum_h)
# include "uci/type/OpPointEmergencyEnum.h"
#endif

#if !defined(Uci__Type__OpPointReferenceType_h)
# include "uci/type/OpPointReferenceType.h"
#endif

#if !defined(Uci__Type__StrengthRangeType_h)
# include "uci/type/StrengthRangeType.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EmergencyReferencePointType sequence accessor class */
      class EmergencyReferencePointType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EmergencyReferencePointType()
         { }

         /** Returns this accessor's type constant, i.e. EmergencyReferencePointType
           *
           * @return This accessor's type constant, i.e. EmergencyReferencePointType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::emergencyReferencePointType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EmergencyReferencePointType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EmergencyCategory.
           *
           * @return The value of the enumeration identified by EmergencyCategory.
           */
         virtual const uci::type::OpPointEmergencyEnum& getEmergencyCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EmergencyCategory.
           *
           * @return The value of the enumeration identified by EmergencyCategory.
           */
         virtual uci::type::OpPointEmergencyEnum& getEmergencyCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EmergencyCategory.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEmergencyCategory(const uci::type::OpPointEmergencyEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EmergencyCategory.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEmergencyCategory(uci::type::OpPointEmergencyEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Reference.
           *
           * @return The acecssor that provides access to the complex content that is identified by Reference.
           */
         virtual const uci::type::OpPointReferenceType& getReference() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Reference.
           *
           * @return The acecssor that provides access to the complex content that is identified by Reference.
           */
         virtual uci::type::OpPointReferenceType& getReference()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Reference to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Reference
           */
         virtual void setReference(const uci::type::OpPointReferenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PersonnelCount.
           *
           * @return The acecssor that provides access to the complex content that is identified by PersonnelCount.
           */
         virtual const uci::type::StrengthRangeType& getPersonnelCount() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PersonnelCount.
           *
           * @return The acecssor that provides access to the complex content that is identified by PersonnelCount.
           */
         virtual uci::type::StrengthRangeType& getPersonnelCount()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PersonnelCount to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PersonnelCount
           */
         virtual void setPersonnelCount(const uci::type::StrengthRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PersonnelCount exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PersonnelCount is emabled or not
           */
         virtual bool hasPersonnelCount() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PersonnelCount
           *
           * @param type = uci::type::accessorType::strengthRangeType This Accessor's accessor type
           */
         virtual void enablePersonnelCount(uci::base::accessorType::AccessorType type = uci::type::accessorType::strengthRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PersonnelCount */
         virtual void clearPersonnelCount()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the LastKnownPosition.
           *
           * @return The value of the string data type identified by LastKnownPosition.
           */
         virtual const uci::type::EmptyType& getLastKnownPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the LastKnownPosition.
           *
           * @return The value of the string data type identified by LastKnownPosition.
           */
         virtual uci::type::EmptyType& getLastKnownPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the LastKnownPosition to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setLastKnownPosition(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the LastKnownPosition to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setLastKnownPosition(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the LastKnownPosition to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setLastKnownPosition(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LastKnownPosition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LastKnownPosition is emabled or not
           */
         virtual bool hasLastKnownPosition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LastKnownPosition
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableLastKnownPosition(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LastKnownPosition */
         virtual void clearLastKnownPosition()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EmergencyReferencePointType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EmergencyReferencePointType to copy from
           */
         EmergencyReferencePointType(const EmergencyReferencePointType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EmergencyReferencePointType to the contents of the
           * EmergencyReferencePointType on the right hand side (rhs) of the assignment operator.EmergencyReferencePointType [only
           * available to derived classes]
           *
           * @param rhs The EmergencyReferencePointType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EmergencyReferencePointType
           * @return A constant reference to this EmergencyReferencePointType.
           */
         const EmergencyReferencePointType& operator=(const EmergencyReferencePointType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EmergencyReferencePointType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EmergencyReferencePointType_h

