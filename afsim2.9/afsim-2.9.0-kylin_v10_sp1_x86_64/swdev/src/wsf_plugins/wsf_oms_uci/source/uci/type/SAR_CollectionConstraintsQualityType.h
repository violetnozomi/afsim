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
#ifndef Uci__Type__SAR_CollectionConstraintsQualityType_h
#define Uci__Type__SAR_CollectionConstraintsQualityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__NIIRS_Type_h)
# include "uci/type/NIIRS_Type.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SAR_CollectionConstraintsQualityType sequence accessor class */
      class SAR_CollectionConstraintsQualityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SAR_CollectionConstraintsQualityType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_CollectionConstraintsQualityType
           *
           * @return This accessor's type constant, i.e. SAR_CollectionConstraintsQualityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_CollectionConstraintsQualityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_CollectionConstraintsQualityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DesiredNIIRS.
           *
           * @return The value of the simple primitive data type identified by DesiredNIIRS.
           */
         virtual uci::type::NIIRS_TypeValue getDesiredNIIRS() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DesiredNIIRS.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDesiredNIIRS(uci::type::NIIRS_TypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DesiredNIIRS exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DesiredNIIRS is emabled or not
           */
         virtual bool hasDesiredNIIRS() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DesiredNIIRS
           *
           * @param type = uci::type::accessorType::nIIRS_Type This Accessor's accessor type
           */
         virtual void enableDesiredNIIRS(uci::base::accessorType::AccessorType type = uci::type::accessorType::nIIRS_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DesiredNIIRS */
         virtual void clearDesiredNIIRS()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumNIIRS.
           *
           * @return The value of the simple primitive data type identified by MinimumNIIRS.
           */
         virtual uci::type::NIIRS_TypeValue getMinimumNIIRS() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumNIIRS.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumNIIRS(uci::type::NIIRS_TypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MinimumNIIRS exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MinimumNIIRS is emabled or not
           */
         virtual bool hasMinimumNIIRS() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MinimumNIIRS
           *
           * @param type = uci::type::accessorType::nIIRS_Type This Accessor's accessor type
           */
         virtual void enableMinimumNIIRS(uci::base::accessorType::AccessorType type = uci::type::accessorType::nIIRS_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MinimumNIIRS */
         virtual void clearMinimumNIIRS()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumResolution.
           *
           * @return The value of the simple primitive data type identified by MinimumResolution.
           */
         virtual xs::Double getMinimumResolution() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumResolution.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumResolution(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MinimumResolution exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MinimumResolution is emabled or not
           */
         virtual bool hasMinimumResolution() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MinimumResolution
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableMinimumResolution(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MinimumResolution */
         virtual void clearMinimumResolution()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_CollectionConstraintsQualityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_CollectionConstraintsQualityType to copy from
           */
         SAR_CollectionConstraintsQualityType(const SAR_CollectionConstraintsQualityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_CollectionConstraintsQualityType to the contents of the
           * SAR_CollectionConstraintsQualityType on the right hand side (rhs) of the assignment
           * operator.SAR_CollectionConstraintsQualityType [only available to derived classes]
           *
           * @param rhs The SAR_CollectionConstraintsQualityType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::SAR_CollectionConstraintsQualityType
           * @return A constant reference to this SAR_CollectionConstraintsQualityType.
           */
         const SAR_CollectionConstraintsQualityType& operator=(const SAR_CollectionConstraintsQualityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_CollectionConstraintsQualityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_CollectionConstraintsQualityType_h

