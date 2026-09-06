// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#undef GetClassName
#ifndef GEN_MSG_HPP
#define GEN_MSG_HPP

// WARNING:  GetClassName is a macro that is defined when running in windows.  Using the /CLR compiler option in
// Visual studio 2005 will set _M_CEE which should #undef GetClassName for you.  However, in Visual Studio 2003,
// this will not work - so you will receive a linking error about GetClassName.  You can fix this by #undef GetClassName
// here.
//#ifdef WIN32
//#undef GetClassName
//#endif

/// \file  GenMsg.hpp
/// \brief GenMsg
/// \class GenMsg GenMsg.hpp
/// \brief This class serves as base class for messages.
///
/// It provides the code most users would otherwise implement to send and messages through the GenIO framework.
/// Most users find it advantages to use this class. However, it's use is not mandatory.
/// The DIS family of classes for example do not use this base class.

#include "genio_export.h"

#include <string>

#include "UtLog.hpp"

class GenI;
class GenO;
class GenMsgFactory;

class GENIO_EXPORT GenMsg
{
public:
   GenMsg(GenI& genIA);
   virtual ~GenMsg();

   /// Get the length of just this class.
   /// \return The lenght in bytes of this class.
   static int GetBaseLength();

   /// Get the derived class name.
   /// It is highly recommned that users deriving from this class implement
   /// their own version of this function that returns the correct name.
   /// \return The name of the class.
   virtual std::string GetClassName() const;

   /// Allow the user the opportunity to do something with the derived class.
   /// This method might typically be called when a message is received to push the
   /// contents of the class into the receiving program.
   /// \return The default behavior for this class is to return a zero
   virtual int DoIt();

   /// Allow the user of the derived class to ensure the data is valid.
   /// This method is typically called right after a message is received.
   /// \return A non zero if valid.
   virtual unsigned int IsValid();

   /// Allow the user of the derived class to at least print
   /// what they can do help debug the read error
   virtual void HandleReadError(ut::log::MessageStream& aStream);

   /// Get time.
   /// \return Time in seconds.
   double GetTime() const;

   /// Get version of a message.
   /// \return Version number.
   int GetVersion() const;

   /// Get message type.
   /// \return message type number.
   int GetType() const;

   /// Compute length of entire message.  This function will call GetClassLength() to obtain length
   /// of the derived class.
   /// \return Message length in bytes.
   int GetLength() const;

   /// Get overall version of message families.
   /// \return Family version number.
   int GetFamilyVersion() const;

   /// Get family type.
   /// \return Family type number
   int GetFamilyType() const;

   /// Get length that was received with the message.
   /// \return Value in bytes as read off of a GenI stream.
   int GetLengthInHeader() const;

   /// Set time.
   /// \param Time in seconds.
   void SetTime(double timeA);

   /// Change the version number.
   /// This is handy when a received object needs to be sent with a different version,
   /// otherwise most people will not have need to change the version number.
   /// \param versionA The version number.
   void SetVersion(int versionA);

   /// Change the computed length stored in the header.
   /// This is handy when a received object needs to be sent with a different length,
   /// otherwise most people will not have need to change the length with this message.
   /// \lengthInheaderA Message length in bytes.
   void SetLengthInHeader(int lengthInheaderA);

   /// Get a message from a GenI derived class. This function will call GetClassData() so the derived class
   /// can read its own data from GenI.
   /// \param A GenI derived class.
   void Get(GenI& genIA);

   /// Get a message from a GenO derived class. This function will call PutClassData() so the derived class
   /// can write its own data to GenO.  It will also call GetLength() prior to calling PutClassData() to
   /// compute an accurate length for this class to send to GenO.
   /// \param A GenO derived class.
   void Put(GenO& genOA) const;

   /// FamilyType enum for message families.
   enum FamilyType
   {
      TypeUnknown = 0, ///<  Unknown
      TypeGenIO   = 1  ///<  GenIO Internal
   };

protected:
   // The constructor and several of the mutators are protected since
   // only a derived class should be constructing the base class or
   // modifying certain key pieces of information.

   GenMsg();

   GenMsg(short familyTypeA, short typeA, short versionA);
   GenMsg(double timeA, short familyTypeA, short typeA, short versionA);

   /// Check data sizes from a GenI stream.
   /// Most derived classes will not need this function, since the Get() method
   /// already calls this function.
   /// \param genIA A GenI stream.
   void CheckMismatchedSize(GenI& genIA);

   /// Get any needed padding in bytes.
   /// \param Message length in bytes.
   /// \return The number of bytes to be added to get to a double word boundary.
   int GetPad1(int lengthInBytesA) const;

   /// Get any needed padding in shorts.
   /// \param Message length in shorts.
   /// \return The number of shorts to be added to get to a double word boundary.
   int GetPad2(int lengthInShortsA) const;

   /// Get any needed padding in words.
   /// \param Message length in words.
   /// \return The number of words to be added to get to a double word boundary.
   int GetPad4(int lengthInWordsA) const;

private:
   void GetGenMsgData(GenI& genIA);

   /// It is expected that the dervied class will implement this function.
   /// It should compute the length of the derived class and not include the length
   /// of this base class.
   /// \return Length in bytes of the derived class.
   virtual int GetClassLength() const;

   /// It is expected that the derived class will implemnt this function.
   /// It should read the derived classes data from a GenI stream.
   /// \param genIA A GenI stream.
   virtual void GetClassData(GenI& genIA);

   /// It is expected that the derived class will implemnt this function.
   /// It should write the derived classes data to a GenO stream.
   /// \param genOA A GenO stream.
   virtual void PutClassData(GenO& genOA) const;

   static const int baseLengthS;

   double timeM;

   // Length is not really needed, but we will keep it just for
   // debugging purposes
   mutable int lengthM;

   int   lengthInHeaderM;
   short familyVersionM;
   short familyTypeM;
   short versionM;
   short typeM;
};

// Accessors
inline int GenMsg::GetVersion() const
{
   return versionM;
}

inline int GenMsg::GetType() const
{
   return typeM;
}

inline int GenMsg::GetFamilyVersion() const
{
   return familyVersionM;
}

inline int GenMsg::GetFamilyType() const
{
   return familyTypeM;
}

inline double GenMsg::GetTime() const
{
   return timeM;
}

inline int GenMsg::GetLengthInHeader() const
{
   return lengthInHeaderM;
}

inline int GenMsg::GetPad1(int lengthInBytesA) const
{
   int tmp = (lengthInBytesA + 7) / 8;
   return tmp * 8 - lengthInBytesA;
}

inline int GenMsg::GetPad2(int lengthInShortsA) const
{
   int tmp = (lengthInShortsA + 3) / 4;
   return tmp * 4 - lengthInShortsA;
}

inline int GenMsg::GetPad4(int lengthInWordsA) const
{
   int tmp = (lengthInWordsA + 1) / 2;
   return tmp * 2 - lengthInWordsA;
}

// Mutators

inline void GenMsg::SetTime(double timeA)
{
   timeM = timeA;
}

inline void GenMsg::SetVersion(int versionA)
{
   versionM = versionA;
}

inline void GenMsg::SetLengthInHeader(int lengthInHeaderA)
{
   lengthInHeaderM = lengthInHeaderA;
}

#endif
