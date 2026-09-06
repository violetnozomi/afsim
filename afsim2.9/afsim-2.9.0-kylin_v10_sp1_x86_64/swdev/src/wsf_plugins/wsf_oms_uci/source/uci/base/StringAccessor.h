// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Base__StringAccessor_h
#define Uci__Base__StringAccessor_h 1

#if !defined(StdLibString)
#  include <string>
//  @cond
#  define StdLibString 1
//  @endcond
#endif

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all base data types are declared
   namespace base {

      /** The StringAccessor is used to provide access to all strings. */
      class StringAccessor : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~StringAccessor()
         { }

         /** Returns the string accessed by this StringAccessor as a C++ standard library string
           *
           * @return The string access by this StringAccessor returned as a C++ standard library string.
           */
         virtual const std::string str() const
            throw(uci::base::UCIException) = 0;


         /** Returns the string accessed by this StringAccessor as a C-string (i.e. const char *)
           *
           * @return The string access by this StringAccessor returned as a C-string (i.e. const char*).
           * @note The returned pointer should not be deleted as the memory used to hold the C-string is owned by the CAL.
           */
         virtual const char* c_str() const
            throw(uci::base::UCIException) = 0;


         /** Sets the string accessed by this StringAccessor to the contents of the specified value which is a C++ standard
           * library string
           *
           * @param value The C++ standard library string whose contents are to be used to set the string accessed by this
           *      StringAccessor
           */
         virtual void setStringValue(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the string accessed by this StringAccessor to the specified value which is a C-string (i.e. const char *)
           *
           * @param value The C-string (i.e. const char *) whose contents are to be used to set the string accessed by this
           *      StringAccessor
           */
         virtual void setStringValue(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Assigns the value of the string accessed by the StringAccessor on the right hand side (rhs) of the assignment
           * operator to the string that is accessed by this StringAccessor.
           *
           * @param rhs The StringAccessor on the right hand side (rhs) of the assignment operator which provides access to
           *      thestring whose contents are to be assigned to the string that is accessed by this StringAccessor.
           * @return A constant reference to this StringAccessor.
           */
         virtual const StringAccessor& operator=(const StringAccessor& rhs)
         {
            setStringValue(rhs.str());
            return *this;
         }


         /** Assigns the value of the C++ standard library string on the right hand side (rhs) of the assignment operator to the
           * string that is acccessed by this StringAccessor.
           *
           * @param rhs The C++ standard library string on the right hand side (rhs) of the assignment operator whose contents are
           *      to be assigned to the string accessed by this StringAccessor.
           * @return A constant reference to this StringAccessor.
           */
         virtual const StringAccessor& operator=(const std::string& rhs)
         {
            setStringValue(rhs);
            return *this;
         }


         /** Assigns the value of the C-string (i.e. const char*) on the right hand side (rhs) of the assignment operator to the
           * string that is acccessed by this StringAccessor.
           *
           * @param rhs The C-string (i.e. const char*) on the right hand side (rhs) of the assignment operator whose contents are
           *      to be assigned to the string accessed by this StringAccessor.
           * @return A constant reference to this StringAccessor.
           */
         virtual const StringAccessor& operator=(const char* rhs)
         {
            setStringValue(rhs);
            return *this;
         }


         /** Conversion operator to convert this StringAccessor to the native const std::string type.
           *
           * @return No return specified, this conversion operator returns a const std::string
           */
         virtual  operator const std::string() const
            throw(uci::base::UCIException)
         {
            return str();
         }



      protected:

         /** The constructor */
         StringAccessor()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StringAccessor to copy from
           */
         StringAccessor(const StringAccessor& rhs)
         {
            (void)rhs;
         }

      }; // StringAccessor


   } // Namespace: base
} // Namespace: uci

#endif // Uci__Base__StringAccessor_h

