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
#ifndef Uci__Type__SubsystemSettingParameterType_h
#define Uci__Type__SubsystemSettingParameterType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SubsystemSettingParameterType sequence accessor class */
      class SubsystemSettingParameterType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemSettingParameterType()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemSettingParameterType
           *
           * @return This accessor's type constant, i.e. SubsystemSettingParameterType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemSettingParameterType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemSettingParameterType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ParameterNumber.
           *
           * @return The value of the simple primitive data type identified by ParameterNumber.
           */
         virtual xs::UnsignedInt getParameterNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ParameterNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setParameterNumber(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DescriptiveLabel.
           *
           * @return The value of the string data type identified by DescriptiveLabel.
           */
         virtual const uci::type::VisibleString256Type& getDescriptiveLabel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DescriptiveLabel.
           *
           * @return The value of the string data type identified by DescriptiveLabel.
           */
         virtual uci::type::VisibleString256Type& getDescriptiveLabel()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DescriptiveLabel to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDescriptiveLabel(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DescriptiveLabel to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDescriptiveLabel(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DescriptiveLabel to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDescriptiveLabel(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DescriptiveLabel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DescriptiveLabel is emabled or not
           */
         virtual bool hasDescriptiveLabel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DescriptiveLabel
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableDescriptiveLabel(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DescriptiveLabel */
         virtual void clearDescriptiveLabel()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemSettingParameterType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemSettingParameterType to copy from
           */
         SubsystemSettingParameterType(const SubsystemSettingParameterType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemSettingParameterType to the contents of the
           * SubsystemSettingParameterType on the right hand side (rhs) of the assignment operator.SubsystemSettingParameterType
           * [only available to derived classes]
           *
           * @param rhs The SubsystemSettingParameterType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::SubsystemSettingParameterType
           * @return A constant reference to this SubsystemSettingParameterType.
           */
         const SubsystemSettingParameterType& operator=(const SubsystemSettingParameterType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemSettingParameterType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemSettingParameterType_h

