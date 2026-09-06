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
#ifndef Uci__Type__ReconnaissanceTaskType_h
#define Uci__Type__ReconnaissanceTaskType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__TargetType_h)
# include "uci/type/TargetType.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates a reconnaisance task type. */
      class ReconnaissanceTaskType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ReconnaissanceTaskType()
         { }

         /** Returns this accessor's type constant, i.e. ReconnaissanceTaskType
           *
           * @return This accessor's type constant, i.e. ReconnaissanceTaskType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::reconnaissanceTaskType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ReconnaissanceTaskType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Armed.
           *
           * @return The value of the string data type identified by Armed.
           */
         virtual const uci::type::EmptyType& getArmed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Armed.
           *
           * @return The value of the string data type identified by Armed.
           */
         virtual uci::type::EmptyType& getArmed()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Armed to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setArmed(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Armed to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setArmed(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Armed to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setArmed(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Armed exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Armed is emabled or not
           */
         virtual bool hasArmed() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Armed
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableArmed(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Armed */
         virtual void clearArmed()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual const uci::type::TargetType& getTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual uci::type::TargetType& getTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Target to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Target
           */
         virtual void setTarget(const uci::type::TargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InitialPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by InitialPoint.
           */
         virtual const uci::type::Point3D_Type& getInitialPoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InitialPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by InitialPoint.
           */
         virtual uci::type::Point3D_Type& getInitialPoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the InitialPoint to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by InitialPoint
           */
         virtual void setInitialPoint(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InitialPoint exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InitialPoint is emabled or not
           */
         virtual bool hasInitialPoint() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InitialPoint
           *
           * @param type = uci::type::accessorType::point3D_Type This Accessor's accessor type
           */
         virtual void enableInitialPoint(uci::base::accessorType::AccessorType type = uci::type::accessorType::point3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InitialPoint */
         virtual void clearInitialPoint()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ReconnaissanceTaskType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ReconnaissanceTaskType to copy from
           */
         ReconnaissanceTaskType(const ReconnaissanceTaskType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ReconnaissanceTaskType to the contents of the
           * ReconnaissanceTaskType on the right hand side (rhs) of the assignment operator.ReconnaissanceTaskType [only available
           * to derived classes]
           *
           * @param rhs The ReconnaissanceTaskType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ReconnaissanceTaskType
           * @return A constant reference to this ReconnaissanceTaskType.
           */
         const ReconnaissanceTaskType& operator=(const ReconnaissanceTaskType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ReconnaissanceTaskType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ReconnaissanceTaskType_h

