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
#ifndef Uci__Type__CommMADL_PeerNodeDataType_h
#define Uci__Type__CommMADL_PeerNodeDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommMADL_NodeRangeType_h)
# include "uci/type/CommMADL_NodeRangeType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__CommMADL_AircraftTypeEnum_h)
# include "uci/type/CommMADL_AircraftTypeEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommMADL_PeerNodeDataType sequence accessor class */
      class CommMADL_PeerNodeDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommMADL_PeerNodeDataType()
         { }

         /** Returns this accessor's type constant, i.e. CommMADL_PeerNodeDataType
           *
           * @return This accessor's type constant, i.e. CommMADL_PeerNodeDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commMADL_PeerNodeDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommMADL_PeerNodeDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Specifies lowest Node ID in the MADL network.; [Minimum inclusive: 1] [Maximum inclusive: 65535] */
         typedef uci::base::UnsignedIntAccessor PeerMADL_UniqueID;
         //  The type of primitive data accessed by PeerMADL_UniqueID
         typedef xs::UnsignedInt PeerMADL_UniqueIDValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the PeerNodeID.
           *
           * @return The value of the simple primitive data type identified by PeerNodeID.
           */
         virtual uci::type::CommMADL_NodeRangeTypeValue getPeerNodeID() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeerNodeID.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeerNodeID(uci::type::CommMADL_NodeRangeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PeerMADL_UniqueID.
           *
           * @return The value of the simple primitive data type identified by PeerMADL_UniqueID.
           */
         virtual uci::type::CommMADL_PeerNodeDataType::PeerMADL_UniqueIDValue getPeerMADL_UniqueID() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeerMADL_UniqueID.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeerMADL_UniqueID(uci::type::CommMADL_PeerNodeDataType::PeerMADL_UniqueIDValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PeerAircraftType.
           *
           * @return The value of the enumeration identified by PeerAircraftType.
           */
         virtual const uci::type::CommMADL_AircraftTypeEnum& getPeerAircraftType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PeerAircraftType.
           *
           * @return The value of the enumeration identified by PeerAircraftType.
           */
         virtual uci::type::CommMADL_AircraftTypeEnum& getPeerAircraftType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PeerAircraftType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPeerAircraftType(const uci::type::CommMADL_AircraftTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PeerAircraftType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPeerAircraftType(uci::type::CommMADL_AircraftTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DataCapable.
           *
           * @return The value of the simple primitive data type identified by DataCapable.
           */
         virtual xs::Boolean getDataCapable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DataCapable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDataCapable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceCapable.
           *
           * @return The value of the simple primitive data type identified by VoiceCapable.
           */
         virtual xs::Boolean getVoiceCapable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceCapable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceCapable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommMADL_PeerNodeDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommMADL_PeerNodeDataType to copy from
           */
         CommMADL_PeerNodeDataType(const CommMADL_PeerNodeDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommMADL_PeerNodeDataType to the contents of the
           * CommMADL_PeerNodeDataType on the right hand side (rhs) of the assignment operator.CommMADL_PeerNodeDataType [only
           * available to derived classes]
           *
           * @param rhs The CommMADL_PeerNodeDataType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommMADL_PeerNodeDataType
           * @return A constant reference to this CommMADL_PeerNodeDataType.
           */
         const CommMADL_PeerNodeDataType& operator=(const CommMADL_PeerNodeDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommMADL_PeerNodeDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommMADL_PeerNodeDataType_h

