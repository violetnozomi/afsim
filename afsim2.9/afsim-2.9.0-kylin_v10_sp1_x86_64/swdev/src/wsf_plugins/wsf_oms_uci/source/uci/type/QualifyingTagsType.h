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
#ifndef Uci__Type__QualifyingTagsType_h
#define Uci__Type__QualifyingTagsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the QualifyingTagsType sequence accessor class */
      class QualifyingTagsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~QualifyingTagsType()
         { }

         /** Returns this accessor's type constant, i.e. QualifyingTagsType
           *
           * @return This accessor's type constant, i.e. QualifyingTagsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::qualifyingTagsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const QualifyingTagsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: 0] [Maximum inclusive: 4094] */
         typedef uci::base::UnsignedIntAccessor QualifiedSpecial;
         //  The type of primitive data accessed by QualifiedSpecial
         typedef xs::UnsignedInt QualifiedSpecialValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the QualifiedSpecial.
           *
           * @return The value of the simple primitive data type identified by QualifiedSpecial.
           */
         virtual uci::type::QualifyingTagsType::QualifiedSpecialValue getQualifiedSpecial() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the QualifiedSpecial.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setQualifiedSpecial(uci::type::QualifyingTagsType::QualifiedSpecialValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by QualifiedSpecial exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by QualifiedSpecial is emabled or not
           */
         virtual bool hasQualifiedSpecial() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by QualifiedSpecial
           *
           * @param type = uci::type::qualifyingTagsType::accessorType::qualifiedSpecial This Accessor's accessor type
           */
         virtual void enableQualifiedSpecial(uci::base::accessorType::AccessorType type = uci::type::qualifyingTagsType::accessorType::qualifiedSpecial)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by QualifiedSpecial */
         virtual void clearQualifiedSpecial()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ProcessSpecial.
           *
           * @return The value of the string data type identified by ProcessSpecial.
           */
         virtual const uci::type::EmptyType& getProcessSpecial() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ProcessSpecial.
           *
           * @return The value of the string data type identified by ProcessSpecial.
           */
         virtual uci::type::EmptyType& getProcessSpecial()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ProcessSpecial to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setProcessSpecial(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ProcessSpecial to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setProcessSpecial(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ProcessSpecial to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setProcessSpecial(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProcessSpecial exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProcessSpecial is emabled or not
           */
         virtual bool hasProcessSpecial() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProcessSpecial
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableProcessSpecial(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProcessSpecial */
         virtual void clearProcessSpecial()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FilterOverride.
           *
           * @return The value of the string data type identified by FilterOverride.
           */
         virtual const uci::type::EmptyType& getFilterOverride() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FilterOverride.
           *
           * @return The value of the string data type identified by FilterOverride.
           */
         virtual uci::type::EmptyType& getFilterOverride()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FilterOverride to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFilterOverride(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FilterOverride to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFilterOverride(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FilterOverride to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFilterOverride(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FilterOverride exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FilterOverride is emabled or not
           */
         virtual bool hasFilterOverride() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FilterOverride
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableFilterOverride(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FilterOverride */
         virtual void clearFilterOverride()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the HighInterest.
           *
           * @return The value of the string data type identified by HighInterest.
           */
         virtual const uci::type::EmptyType& getHighInterest() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the HighInterest.
           *
           * @return The value of the string data type identified by HighInterest.
           */
         virtual uci::type::EmptyType& getHighInterest()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HighInterest to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setHighInterest(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HighInterest to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setHighInterest(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HighInterest to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setHighInterest(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HighInterest exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HighInterest is emabled or not
           */
         virtual bool hasHighInterest() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HighInterest
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableHighInterest(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HighInterest */
         virtual void clearHighInterest()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the QualifyingSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by QualifyingSystemID.
           */
         virtual const uci::type::SystemID_Type& getQualifyingSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the QualifyingSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by QualifyingSystemID.
           */
         virtual uci::type::SystemID_Type& getQualifyingSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the QualifyingSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by QualifyingSystemID
           */
         virtual void setQualifyingSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         QualifyingTagsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The QualifyingTagsType to copy from
           */
         QualifyingTagsType(const QualifyingTagsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this QualifyingTagsType to the contents of the QualifyingTagsType on
           * the right hand side (rhs) of the assignment operator.QualifyingTagsType [only available to derived classes]
           *
           * @param rhs The QualifyingTagsType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::QualifyingTagsType
           * @return A constant reference to this QualifyingTagsType.
           */
         const QualifyingTagsType& operator=(const QualifyingTagsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // QualifyingTagsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__QualifyingTagsType_h

