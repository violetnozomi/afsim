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
#ifndef Uci__Type__ReleaseConsentType_h
#define Uci__Type__ReleaseConsentType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** ReleaseConsentType expresses that release is authorized and to express special aspects of the launch. */
      class ReleaseConsentType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ReleaseConsentType()
         { }

         /** Returns this accessor's type constant, i.e. ReleaseConsentType
           *
           * @return This accessor's type constant, i.e. ReleaseConsentType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::releaseConsentType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ReleaseConsentType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ReleaseConsent.
           *
           * @return The value of the simple primitive data type identified by ReleaseConsent.
           */
         virtual xs::Boolean getReleaseConsent() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ReleaseConsent.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setReleaseConsent(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RepresentationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RepresentationID.
           */
         virtual const uci::type::ForeignKeyType& getRepresentationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RepresentationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RepresentationID.
           */
         virtual uci::type::ForeignKeyType& getRepresentationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RepresentationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RepresentationID
           */
         virtual void setRepresentationID(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RepresentationID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RepresentationID is emabled or not
           */
         virtual bool hasRepresentationID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RepresentationID
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableRepresentationID(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RepresentationID */
         virtual void clearRepresentationID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ReleaseConsentType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ReleaseConsentType to copy from
           */
         ReleaseConsentType(const ReleaseConsentType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ReleaseConsentType to the contents of the ReleaseConsentType on
           * the right hand side (rhs) of the assignment operator.ReleaseConsentType [only available to derived classes]
           *
           * @param rhs The ReleaseConsentType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ReleaseConsentType
           * @return A constant reference to this ReleaseConsentType.
           */
         const ReleaseConsentType& operator=(const ReleaseConsentType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ReleaseConsentType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ReleaseConsentType_h

