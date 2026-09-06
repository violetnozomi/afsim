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
#ifndef Uci__Type__SubsystemStateCommandMDT_h
#define Uci__Type__SubsystemStateCommandMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemCommandID_Type_h)
# include "uci/type/SubsystemCommandID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemStateEnum_h)
# include "uci/type/SubsystemStateEnum.h"
#endif

#if !defined(Uci__Type__SubsystemEraseCommandEnum_h)
# include "uci/type/SubsystemEraseCommandEnum.h"
#endif

#if !defined(Uci__Type__BIT_ID_Type_h)
# include "uci/type/BIT_ID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SubsystemStateCommandMDT sequence accessor class */
      class SubsystemStateCommandMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemStateCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemStateCommandMDT
           *
           * @return This accessor's type constant, i.e. SubsystemStateCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemStateCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemStateCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual const uci::type::SubsystemCommandID_Type& getCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual uci::type::SubsystemCommandID_Type& getCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommandID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommandID
           */
         virtual void setCommandID(const uci::type::SubsystemCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


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


         /** Returns the value of the enumeration that is identified by the CommandedState.
           *
           * @return The value of the enumeration identified by CommandedState.
           */
         virtual const uci::type::SubsystemStateEnum& getCommandedState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandedState.
           *
           * @return The value of the enumeration identified by CommandedState.
           */
         virtual uci::type::SubsystemStateEnum& getCommandedState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandedState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandedState(const uci::type::SubsystemStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandedState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandedState(uci::type::SubsystemStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EraseParameter.
           *
           * @return The value of the enumeration identified by EraseParameter.
           */
         virtual const uci::type::SubsystemEraseCommandEnum& getEraseParameter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EraseParameter.
           *
           * @return The value of the enumeration identified by EraseParameter.
           */
         virtual uci::type::SubsystemEraseCommandEnum& getEraseParameter()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EraseParameter.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEraseParameter(const uci::type::SubsystemEraseCommandEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EraseParameter.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEraseParameter(uci::type::SubsystemEraseCommandEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield EraseParameteris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasEraseParameter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getEraseParameter will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::subsystemEraseCommandEnum This Accessor's accessor type
           */
         virtual void enableEraseParameter(uci::base::accessorType::AccessorType type = uci::type::accessorType::subsystemEraseCommandEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearEraseParameter()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BIT_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by BIT_ID.
           */
         virtual const uci::type::BIT_ID_Type& getBIT_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BIT_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by BIT_ID.
           */
         virtual uci::type::BIT_ID_Type& getBIT_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BIT_ID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BIT_ID
           */
         virtual void setBIT_ID(const uci::type::BIT_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BIT_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BIT_ID is emabled or not
           */
         virtual bool hasBIT_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BIT_ID
           *
           * @param type = uci::type::accessorType::bIT_ID_Type This Accessor's accessor type
           */
         virtual void enableBIT_ID(uci::base::accessorType::AccessorType type = uci::type::accessorType::bIT_ID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BIT_ID */
         virtual void clearBIT_ID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CalibrationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CalibrationID.
           */
         virtual const uci::type::BIT_ID_Type& getCalibrationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CalibrationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CalibrationID.
           */
         virtual uci::type::BIT_ID_Type& getCalibrationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CalibrationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CalibrationID
           */
         virtual void setCalibrationID(const uci::type::BIT_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CalibrationID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CalibrationID is emabled or not
           */
         virtual bool hasCalibrationID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CalibrationID
           *
           * @param type = uci::type::accessorType::bIT_ID_Type This Accessor's accessor type
           */
         virtual void enableCalibrationID(uci::base::accessorType::AccessorType type = uci::type::accessorType::bIT_ID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CalibrationID */
         virtual void clearCalibrationID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemStateCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemStateCommandMDT to copy from
           */
         SubsystemStateCommandMDT(const SubsystemStateCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemStateCommandMDT to the contents of the
           * SubsystemStateCommandMDT on the right hand side (rhs) of the assignment operator.SubsystemStateCommandMDT [only
           * available to derived classes]
           *
           * @param rhs The SubsystemStateCommandMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SubsystemStateCommandMDT
           * @return A constant reference to this SubsystemStateCommandMDT.
           */
         const SubsystemStateCommandMDT& operator=(const SubsystemStateCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemStateCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemStateCommandMDT_h

