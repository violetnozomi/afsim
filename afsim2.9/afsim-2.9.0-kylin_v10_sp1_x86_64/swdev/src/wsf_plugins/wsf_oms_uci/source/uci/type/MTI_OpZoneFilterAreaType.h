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
#ifndef Uci__Type__MTI_OpZoneFilterAreaType_h
#define Uci__Type__MTI_OpZoneFilterAreaType_h 1

#if !defined(Uci__Type__OpZoneFilterAreaPET_h)
# include "uci/type/OpZoneFilterAreaPET.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This element represents a filtered area for raw Moving Target Indicator (MTI) measurements. This type extends the
        * base OpZoneFilterArea_PET to provide generic MTI parameters. Specialized MTI will have an extended type for
        * additional parameters.
        */
      class MTI_OpZoneFilterAreaType : public virtual uci::type::OpZoneFilterAreaPET {
      public:

         /** The destructor */
         virtual ~MTI_OpZoneFilterAreaType()
         { }

         /** Returns this accessor's type constant, i.e. MTI_OpZoneFilterAreaType
           *
           * @return This accessor's type constant, i.e. MTI_OpZoneFilterAreaType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::mTI_OpZoneFilterAreaType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MTI_OpZoneFilterAreaType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on the specific Job Identifier. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::base::IntAccessor, uci::base::accessorType::intAccessor> Job;

         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumTargetCountInDwell.
           *
           * @return The value of the simple primitive data type identified by MinimumTargetCountInDwell.
           */
         virtual xs::Int getMinimumTargetCountInDwell() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumTargetCountInDwell.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumTargetCountInDwell(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MinimumTargetCountInDwell exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MinimumTargetCountInDwell is emabled or not
           */
         virtual bool hasMinimumTargetCountInDwell() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MinimumTargetCountInDwell
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableMinimumTargetCountInDwell(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MinimumTargetCountInDwell */
         virtual void clearMinimumTargetCountInDwell()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumTargetCountInDwell.
           *
           * @return The value of the simple primitive data type identified by MaximumTargetCountInDwell.
           */
         virtual xs::Int getMaximumTargetCountInDwell() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumTargetCountInDwell.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumTargetCountInDwell(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaximumTargetCountInDwell exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaximumTargetCountInDwell is emabled or not
           */
         virtual bool hasMaximumTargetCountInDwell() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaximumTargetCountInDwell
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableMaximumTargetCountInDwell(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaximumTargetCountInDwell */
         virtual void clearMaximumTargetCountInDwell()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumSNR.
           *
           * @return The value of the simple primitive data type identified by MinimumSNR.
           */
         virtual xs::Double getMinimumSNR() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumSNR.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumSNR(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MinimumSNR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MinimumSNR is emabled or not
           */
         virtual bool hasMinimumSNR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MinimumSNR
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableMinimumSNR(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MinimumSNR */
         virtual void clearMinimumSNR()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumSNR.
           *
           * @return The value of the simple primitive data type identified by MaximumSNR.
           */
         virtual xs::Double getMaximumSNR() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumSNR.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumSNR(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaximumSNR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaximumSNR is emabled or not
           */
         virtual bool hasMaximumSNR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaximumSNR
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableMaximumSNR(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaximumSNR */
         virtual void clearMaximumSNR()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumRCS.
           *
           * @return The value of the simple primitive data type identified by MinimumRCS.
           */
         virtual xs::Double getMinimumRCS() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumRCS.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumRCS(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MinimumRCS exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MinimumRCS is emabled or not
           */
         virtual bool hasMinimumRCS() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MinimumRCS
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableMinimumRCS(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MinimumRCS */
         virtual void clearMinimumRCS()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumRCS.
           *
           * @return The value of the simple primitive data type identified by MaximumRCS.
           */
         virtual xs::Double getMaximumRCS() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumRCS.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumRCS(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaximumRCS exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaximumRCS is emabled or not
           */
         virtual bool hasMaximumRCS() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaximumRCS
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableMaximumRCS(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaximumRCS */
         virtual void clearMaximumRCS()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumRadialVelocity.
           *
           * @return The value of the simple primitive data type identified by MinimumRadialVelocity.
           */
         virtual xs::Double getMinimumRadialVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumRadialVelocity.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumRadialVelocity(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MinimumRadialVelocity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MinimumRadialVelocity is emabled or not
           */
         virtual bool hasMinimumRadialVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MinimumRadialVelocity
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableMinimumRadialVelocity(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MinimumRadialVelocity */
         virtual void clearMinimumRadialVelocity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumRadialVelocity.
           *
           * @return The value of the simple primitive data type identified by MaximumRadialVelocity.
           */
         virtual xs::Double getMaximumRadialVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumRadialVelocity.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumRadialVelocity(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaximumRadialVelocity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaximumRadialVelocity is emabled or not
           */
         virtual bool hasMaximumRadialVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaximumRadialVelocity
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableMaximumRadialVelocity(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaximumRadialVelocity */
         virtual void clearMaximumRadialVelocity()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Job.
           *
           * @return The bounded list identified by Job.
           */
         virtual const uci::type::MTI_OpZoneFilterAreaType::Job& getJob() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Job.
           *
           * @return The bounded list identified by Job.
           */
         virtual uci::type::MTI_OpZoneFilterAreaType::Job& getJob()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Job.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setJob(const uci::type::MTI_OpZoneFilterAreaType::Job& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MTI_OpZoneFilterAreaType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MTI_OpZoneFilterAreaType to copy from
           */
         MTI_OpZoneFilterAreaType(const MTI_OpZoneFilterAreaType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MTI_OpZoneFilterAreaType to the contents of the
           * MTI_OpZoneFilterAreaType on the right hand side (rhs) of the assignment operator.MTI_OpZoneFilterAreaType [only
           * available to derived classes]
           *
           * @param rhs The MTI_OpZoneFilterAreaType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::MTI_OpZoneFilterAreaType
           * @return A constant reference to this MTI_OpZoneFilterAreaType.
           */
         const MTI_OpZoneFilterAreaType& operator=(const MTI_OpZoneFilterAreaType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MTI_OpZoneFilterAreaType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MTI_OpZoneFilterAreaType_h

