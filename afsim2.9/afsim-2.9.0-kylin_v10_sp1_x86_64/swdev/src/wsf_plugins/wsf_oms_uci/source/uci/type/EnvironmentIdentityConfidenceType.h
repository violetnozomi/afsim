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
#ifndef Uci__Type__EnvironmentIdentityConfidenceType_h
#define Uci__Type__EnvironmentIdentityConfidenceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EnvironmentEnum_h)
# include "uci/type/EnvironmentEnum.h"
#endif

#if !defined(Uci__Type__PointTrackEnum_h)
# include "uci/type/PointTrackEnum.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EnvironmentIdentityConfidenceType sequence accessor class */
      class EnvironmentIdentityConfidenceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EnvironmentIdentityConfidenceType()
         { }

         /** Returns this accessor's type constant, i.e. EnvironmentIdentityConfidenceType
           *
           * @return This accessor's type constant, i.e. EnvironmentIdentityConfidenceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::environmentIdentityConfidenceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EnvironmentIdentityConfidenceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Environment.
           *
           * @return The value of the enumeration identified by Environment.
           */
         virtual const uci::type::EnvironmentEnum& getEnvironment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Environment.
           *
           * @return The value of the enumeration identified by Environment.
           */
         virtual uci::type::EnvironmentEnum& getEnvironment()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Environment.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEnvironment(const uci::type::EnvironmentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Environment.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEnvironment(uci::type::EnvironmentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PointTrack.
           *
           * @return The value of the enumeration identified by PointTrack.
           */
         virtual const uci::type::PointTrackEnum& getPointTrack() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PointTrack.
           *
           * @return The value of the enumeration identified by PointTrack.
           */
         virtual uci::type::PointTrackEnum& getPointTrack()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PointTrack.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPointTrack(const uci::type::PointTrackEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PointTrack.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPointTrack(uci::type::PointTrackEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PointTrackis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPointTrack() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPointTrack will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::pointTrackEnum This Accessor's accessor type
           */
         virtual void enablePointTrack(uci::base::accessorType::AccessorType type = uci::type::accessorType::pointTrackEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPointTrack()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Confidence.
           *
           * @return The value of the simple primitive data type identified by Confidence.
           */
         virtual uci::type::PercentTypeValue getConfidence() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Confidence.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setConfidence(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EnvironmentIdentityConfidenceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EnvironmentIdentityConfidenceType to copy from
           */
         EnvironmentIdentityConfidenceType(const EnvironmentIdentityConfidenceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EnvironmentIdentityConfidenceType to the contents of the
           * EnvironmentIdentityConfidenceType on the right hand side (rhs) of the assignment
           * operator.EnvironmentIdentityConfidenceType [only available to derived classes]
           *
           * @param rhs The EnvironmentIdentityConfidenceType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::EnvironmentIdentityConfidenceType
           * @return A constant reference to this EnvironmentIdentityConfidenceType.
           */
         const EnvironmentIdentityConfidenceType& operator=(const EnvironmentIdentityConfidenceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EnvironmentIdentityConfidenceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EnvironmentIdentityConfidenceType_h

