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
#ifndef Uci__Type__POST_ComponentWindowType_h
#define Uci__Type__POST_ComponentWindowType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__POST_WindowID_Type_h)
# include "uci/type/POST_WindowID_Type.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ComponentWindowType sequence accessor class */
      class POST_ComponentWindowType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_ComponentWindowType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ComponentWindowType
           *
           * @return This accessor's type constant, i.e. POST_ComponentWindowType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ComponentWindowType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ComponentWindowType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the number of discrete color values in each band, e.g. 8-bit color representation will require a value of
           * 256 [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::base::UnsignedIntAccessor, uci::base::accessorType::unsignedIntAccessor> ColorDepth;

         /** Returns the accessor that provides access to the complex content that is identified by the POST_WindowID.
           *
           * @return The acecssor that provides access to the complex content that is identified by POST_WindowID.
           */
         virtual const uci::type::POST_WindowID_Type& getPOST_WindowID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the POST_WindowID.
           *
           * @return The acecssor that provides access to the complex content that is identified by POST_WindowID.
           */
         virtual uci::type::POST_WindowID_Type& getPOST_WindowID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the POST_WindowID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by POST_WindowID
           */
         virtual void setPOST_WindowID(const uci::type::POST_WindowID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the WindowSizeX.
           *
           * @return The value of the simple primitive data type identified by WindowSizeX.
           */
         virtual xs::UnsignedInt getWindowSizeX() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the WindowSizeX.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setWindowSizeX(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the WindowSizeY.
           *
           * @return The value of the simple primitive data type identified by WindowSizeY.
           */
         virtual xs::UnsignedInt getWindowSizeY() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the WindowSizeY.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setWindowSizeY(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LowerLeftX.
           *
           * @return The value of the simple primitive data type identified by LowerLeftX.
           */
         virtual xs::UnsignedInt getLowerLeftX() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LowerLeftX.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLowerLeftX(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LowerLeftY.
           *
           * @return The value of the simple primitive data type identified by LowerLeftY.
           */
         virtual xs::UnsignedInt getLowerLeftY() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LowerLeftY.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLowerLeftY(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ColorDepth.
           *
           * @return The bounded list identified by ColorDepth.
           */
         virtual const uci::type::POST_ComponentWindowType::ColorDepth& getColorDepth() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ColorDepth.
           *
           * @return The bounded list identified by ColorDepth.
           */
         virtual uci::type::POST_ComponentWindowType::ColorDepth& getColorDepth()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ColorDepth.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setColorDepth(const uci::type::POST_ComponentWindowType::ColorDepth& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ComponentWindowType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ComponentWindowType to copy from
           */
         POST_ComponentWindowType(const POST_ComponentWindowType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ComponentWindowType to the contents of the
           * POST_ComponentWindowType on the right hand side (rhs) of the assignment operator.POST_ComponentWindowType [only
           * available to derived classes]
           *
           * @param rhs The POST_ComponentWindowType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::POST_ComponentWindowType
           * @return A constant reference to this POST_ComponentWindowType.
           */
         const POST_ComponentWindowType& operator=(const POST_ComponentWindowType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ComponentWindowType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ComponentWindowType_h

