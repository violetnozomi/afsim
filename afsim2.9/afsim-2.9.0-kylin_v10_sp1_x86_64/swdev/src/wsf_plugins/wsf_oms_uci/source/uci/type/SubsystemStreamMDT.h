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
#ifndef Uci__Type__SubsystemStreamMDT_h
#define Uci__Type__SubsystemStreamMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemSettingEnum_h)
# include "uci/type/SubsystemSettingEnum.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Xs__Type__BinaryXmlSchemaPrimitives_h)
# include "xs/type/binaryXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SubsystemStreamMDT sequence accessor class */
      class SubsystemStreamMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemStreamMDT()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemStreamMDT
           *
           * @return This accessor's type constant, i.e. SubsystemStreamMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemStreamMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemStreamMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the free-form text data of the stream. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::VisibleString1024Type, uci::type::accessorType::visibleString1024Type> SubsystemStreamString;

         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SubsystemSetting.
           *
           * @return The value of the enumeration identified by SubsystemSetting.
           */
         virtual const uci::type::SubsystemSettingEnum& getSubsystemSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SubsystemSetting.
           *
           * @return The value of the enumeration identified by SubsystemSetting.
           */
         virtual uci::type::SubsystemSettingEnum& getSubsystemSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubsystemSetting.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubsystemSetting(const uci::type::SubsystemSettingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubsystemSetting.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubsystemSetting(uci::type::SubsystemSettingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SubsystemSettingParameter.
           *
           * @return The value of the simple primitive data type identified by SubsystemSettingParameter.
           */
         virtual xs::UnsignedInt getSubsystemSettingParameter() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SubsystemSettingParameter.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSubsystemSettingParameter(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SubsystemSettingParameter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SubsystemSettingParameter is emabled or not
           */
         virtual bool hasSubsystemSettingParameter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SubsystemSettingParameter
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableSubsystemSettingParameter(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SubsystemSettingParameter */
         virtual void clearSubsystemSettingParameter()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemStreamString.
           *
           * @return The bounded list identified by SubsystemStreamString.
           */
         virtual const uci::type::SubsystemStreamMDT::SubsystemStreamString& getSubsystemStreamString() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemStreamString.
           *
           * @return The bounded list identified by SubsystemStreamString.
           */
         virtual uci::type::SubsystemStreamMDT::SubsystemStreamString& getSubsystemStreamString()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubsystemStreamString.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubsystemStreamString(const uci::type::SubsystemStreamMDT::SubsystemStreamString& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor to the SubsystemStreamBinary field contained in this message fragment
           *
           * @return The reference to the accessor to the SubsystemStreamBinary field.
           */
         virtual const xs::HexBinary& getSubsystemStreamBinary() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor to the SubsystemStreamBinary field contained in this message fragment
           *
           * @return The reference to the accessor to the SubsystemStreamBinary field.
           */
         virtual xs::HexBinary& getSubsystemStreamBinary()
            throw(uci::base::UCIException) = 0;


         /** Set the contents of the message fragment that this accesssor provides access to to the contents the specified
           * accessor provides access to. A deep copy will be used by this method in setting the contents
           *
           * @param value The accessor to use to set the contents of the message fragment that this accessor provides access to.
           */
         virtual void setSubsystemStreamBinary(const xs::HexBinary& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SubsystemStreamBinaryis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSubsystemStreamBinary() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSubsystemStreamBinary will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = xs::accessorType::hexBinary This Accessor's accessor type
           */
         virtual void enableSubsystemStreamBinary(uci::base::accessorType::AccessorType type = xs::accessorType::hexBinary)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSubsystemStreamBinary()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemStreamMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemStreamMDT to copy from
           */
         SubsystemStreamMDT(const SubsystemStreamMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemStreamMDT to the contents of the SubsystemStreamMDT on
           * the right hand side (rhs) of the assignment operator.SubsystemStreamMDT [only available to derived classes]
           *
           * @param rhs The SubsystemStreamMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SubsystemStreamMDT
           * @return A constant reference to this SubsystemStreamMDT.
           */
         const SubsystemStreamMDT& operator=(const SubsystemStreamMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemStreamMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemStreamMDT_h

