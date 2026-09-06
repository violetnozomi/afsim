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
#ifndef Uci__Type__IFF_Type_h
#define Uci__Type__IFF_Type_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__IFF_OctalModeType_h)
# include "uci/type/IFF_OctalModeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__IFF_Mode4Type_h)
# include "uci/type/IFF_Mode4Type.h"
#endif

#if !defined(Uci__Type__IFF_Mode5Type_h)
# include "uci/type/IFF_Mode5Type.h"
#endif

#if !defined(Uci__Type__IFF_ModeS_Type_h)
# include "uci/type/IFF_ModeS_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the IFF_Type sequence accessor class */
      class IFF_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~IFF_Type()
         { }

         /** Returns this accessor's type constant, i.e. IFF_Type
           *
           * @return This accessor's type constant, i.e. IFF_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::iFF_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const IFF_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element represents Mode 3A beacon reply information. See MIL-STD-6016 for detailed definition. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::IFF_OctalModeType, uci::type::accessorType::iFF_OctalModeType> Mode3A;

         /** Returns the accessor that provides access to the complex content that is identified by the Mode1.
           *
           * @return The acecssor that provides access to the complex content that is identified by Mode1.
           */
         virtual const uci::type::IFF_OctalModeType& getMode1() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Mode1.
           *
           * @return The acecssor that provides access to the complex content that is identified by Mode1.
           */
         virtual uci::type::IFF_OctalModeType& getMode1()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Mode1 to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Mode1
           */
         virtual void setMode1(const uci::type::IFF_OctalModeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Mode1 exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Mode1 is emabled or not
           */
         virtual bool hasMode1() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Mode1
           *
           * @param type = uci::type::accessorType::iFF_OctalModeType This Accessor's accessor type
           */
         virtual void enableMode1(uci::base::accessorType::AccessorType type = uci::type::accessorType::iFF_OctalModeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Mode1 */
         virtual void clearMode1()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Mode2.
           *
           * @return The acecssor that provides access to the complex content that is identified by Mode2.
           */
         virtual const uci::type::IFF_OctalModeType& getMode2() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Mode2.
           *
           * @return The acecssor that provides access to the complex content that is identified by Mode2.
           */
         virtual uci::type::IFF_OctalModeType& getMode2()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Mode2 to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Mode2
           */
         virtual void setMode2(const uci::type::IFF_OctalModeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Mode2 exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Mode2 is emabled or not
           */
         virtual bool hasMode2() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Mode2
           *
           * @param type = uci::type::accessorType::iFF_OctalModeType This Accessor's accessor type
           */
         virtual void enableMode2(uci::base::accessorType::AccessorType type = uci::type::accessorType::iFF_OctalModeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Mode2 */
         virtual void clearMode2()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Mode3A.
           *
           * @return The bounded list identified by Mode3A.
           */
         virtual const uci::type::IFF_Type::Mode3A& getMode3A() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Mode3A.
           *
           * @return The bounded list identified by Mode3A.
           */
         virtual uci::type::IFF_Type::Mode3A& getMode3A()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Mode3A.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMode3A(const uci::type::IFF_Type::Mode3A& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Mode3LC.
           *
           * @return The acecssor that provides access to the complex content that is identified by Mode3LC.
           */
         virtual const uci::type::IFF_OctalModeType& getMode3LC() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Mode3LC.
           *
           * @return The acecssor that provides access to the complex content that is identified by Mode3LC.
           */
         virtual uci::type::IFF_OctalModeType& getMode3LC()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Mode3LC to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Mode3LC
           */
         virtual void setMode3LC(const uci::type::IFF_OctalModeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Mode3LC exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Mode3LC is emabled or not
           */
         virtual bool hasMode3LC() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Mode3LC
           *
           * @param type = uci::type::accessorType::iFF_OctalModeType This Accessor's accessor type
           */
         virtual void enableMode3LC(uci::base::accessorType::AccessorType type = uci::type::accessorType::iFF_OctalModeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Mode3LC */
         virtual void clearMode3LC()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Mode4.
           *
           * @return The acecssor that provides access to the complex content that is identified by Mode4.
           */
         virtual const uci::type::IFF_Mode4Type& getMode4() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Mode4.
           *
           * @return The acecssor that provides access to the complex content that is identified by Mode4.
           */
         virtual uci::type::IFF_Mode4Type& getMode4()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Mode4 to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Mode4
           */
         virtual void setMode4(const uci::type::IFF_Mode4Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Mode4 exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Mode4 is emabled or not
           */
         virtual bool hasMode4() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Mode4
           *
           * @param type = uci::type::accessorType::iFF_Mode4Type This Accessor's accessor type
           */
         virtual void enableMode4(uci::base::accessorType::AccessorType type = uci::type::accessorType::iFF_Mode4Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Mode4 */
         virtual void clearMode4()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Mode5.
           *
           * @return The acecssor that provides access to the complex content that is identified by Mode5.
           */
         virtual const uci::type::IFF_Mode5Type& getMode5() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Mode5.
           *
           * @return The acecssor that provides access to the complex content that is identified by Mode5.
           */
         virtual uci::type::IFF_Mode5Type& getMode5()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Mode5 to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Mode5
           */
         virtual void setMode5(const uci::type::IFF_Mode5Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Mode5 exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Mode5 is emabled or not
           */
         virtual bool hasMode5() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Mode5
           *
           * @param type = uci::type::accessorType::iFF_Mode5Type This Accessor's accessor type
           */
         virtual void enableMode5(uci::base::accessorType::AccessorType type = uci::type::accessorType::iFF_Mode5Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Mode5 */
         virtual void clearMode5()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ModeS.
           *
           * @return The acecssor that provides access to the complex content that is identified by ModeS.
           */
         virtual const uci::type::IFF_ModeS_Type& getModeS() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ModeS.
           *
           * @return The acecssor that provides access to the complex content that is identified by ModeS.
           */
         virtual uci::type::IFF_ModeS_Type& getModeS()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ModeS to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ModeS
           */
         virtual void setModeS(const uci::type::IFF_ModeS_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ModeS exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ModeS is emabled or not
           */
         virtual bool hasModeS() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ModeS
           *
           * @param type = uci::type::accessorType::iFF_ModeS_Type This Accessor's accessor type
           */
         virtual void enableModeS(uci::base::accessorType::AccessorType type = uci::type::accessorType::iFF_ModeS_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ModeS */
         virtual void clearModeS()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ModeC.
           *
           * @return The acecssor that provides access to the complex content that is identified by ModeC.
           */
         virtual const uci::type::IFF_OctalModeType& getModeC() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ModeC.
           *
           * @return The acecssor that provides access to the complex content that is identified by ModeC.
           */
         virtual uci::type::IFF_OctalModeType& getModeC()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ModeC to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ModeC
           */
         virtual void setModeC(const uci::type::IFF_OctalModeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ModeC exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ModeC is emabled or not
           */
         virtual bool hasModeC() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ModeC
           *
           * @param type = uci::type::accessorType::iFF_OctalModeType This Accessor's accessor type
           */
         virtual void enableModeC(uci::base::accessorType::AccessorType type = uci::type::accessorType::iFF_OctalModeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ModeC */
         virtual void clearModeC()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IFF_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IFF_Type to copy from
           */
         IFF_Type(const IFF_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IFF_Type to the contents of the IFF_Type on the right hand side
           * (rhs) of the assignment operator.IFF_Type [only available to derived classes]
           *
           * @param rhs The IFF_Type on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::IFF_Type
           * @return A constant reference to this IFF_Type.
           */
         const IFF_Type& operator=(const IFF_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IFF_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__IFF_Type_h

