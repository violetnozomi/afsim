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
#ifndef Uci__Type__EOB_CorrelatedToType_h
#define Uci__Type__EOB_CorrelatedToType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EOB_RecordID_Type_h)
# include "uci/type/EOB_RecordID_Type.h"
#endif

#if !defined(Uci__Type__IdentityType_h)
# include "uci/type/IdentityType.h"
#endif

#if !defined(Uci__Type__EOB_EmitterID_Type_h)
# include "uci/type/EOB_EmitterID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EOB_CorrelatedToType sequence accessor class */
      class EOB_CorrelatedToType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EOB_CorrelatedToType()
         { }

         /** Returns this accessor's type constant, i.e. EOB_CorrelatedToType
           *
           * @return This accessor's type constant, i.e. EOB_CorrelatedToType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eOB_CorrelatedToType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EOB_CorrelatedToType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_RecordID.
           */
         virtual const uci::type::EOB_RecordID_Type& getEOB_RecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_RecordID.
           */
         virtual uci::type::EOB_RecordID_Type& getEOB_RecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EOB_RecordID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EOB_RecordID
           */
         virtual void setEOB_RecordID(const uci::type::EOB_RecordID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EOB_RecordID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EOB_RecordID is emabled or not
           */
         virtual bool hasEOB_RecordID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EOB_RecordID
           *
           * @param type = uci::type::accessorType::eOB_RecordID_Type This Accessor's accessor type
           */
         virtual void enableEOB_RecordID(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_RecordID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EOB_RecordID */
         virtual void clearEOB_RecordID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::IdentityType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::IdentityType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::IdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Identity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Identity is emabled or not
           */
         virtual bool hasIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Identity
           *
           * @param type = uci::type::accessorType::identityType This Accessor's accessor type
           */
         virtual void enableIdentity(uci::base::accessorType::AccessorType type = uci::type::accessorType::identityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Identity */
         virtual void clearIdentity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterID.
           */
         virtual const uci::type::EOB_EmitterID_Type& getEmitterID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterID.
           */
         virtual uci::type::EOB_EmitterID_Type& getEmitterID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmitterID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmitterID
           */
         virtual void setEmitterID(const uci::type::EOB_EmitterID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmitterID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmitterID is emabled or not
           */
         virtual bool hasEmitterID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmitterID
           *
           * @param type = uci::type::accessorType::eOB_EmitterID_Type This Accessor's accessor type
           */
         virtual void enableEmitterID(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_EmitterID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmitterID */
         virtual void clearEmitterID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_CorrelatedToType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_CorrelatedToType to copy from
           */
         EOB_CorrelatedToType(const EOB_CorrelatedToType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_CorrelatedToType to the contents of the EOB_CorrelatedToType
           * on the right hand side (rhs) of the assignment operator.EOB_CorrelatedToType [only available to derived classes]
           *
           * @param rhs The EOB_CorrelatedToType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::EOB_CorrelatedToType
           * @return A constant reference to this EOB_CorrelatedToType.
           */
         const EOB_CorrelatedToType& operator=(const EOB_CorrelatedToType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_CorrelatedToType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EOB_CorrelatedToType_h

