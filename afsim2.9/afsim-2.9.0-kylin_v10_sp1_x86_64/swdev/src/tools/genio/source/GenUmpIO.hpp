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

#ifndef GEN_UMP_IO_HPP
#define GEN_UMP_IO_HPP

#include "GenExport.hpp"

/// \file  GenUmpIO.hpp
/// \brief GenUmpIO
/// \class GenUmpIO GenUmpIO.hpp
/// \brief Provides a Ethternet transport mechanism via UMP.
///
/// \anchor receive
/// \anchor send
/// This class inherits from GenIO.  It uses UMP2 to manage the sockets.
/// This class supports UMP named sockets and the UMP agent names in a socket table scheme.
/// Sockets can be UDP, TCP, or Reliable UDP (RUDP).
///
/// This class can be initialized using the detailed constructors or the default constructor followed
/// by the appropriate Init() method.  Using the latter combination of default constructor and Init() method allows
/// the user to see the connect status via the return value from the Init method.  A successful connection must be
/// made before data can be sent or received.
///
/// To receive data this class uses the concept that data is first transferred into the receive buffer by calling
/// Receive(). Then the user calls the various Get() methods as needed to remove all the data from the receive buffer.
///
/// To send data this class uses  the concept that messages or data is "Put" to the send buffer for bundling and then
/// sent at once with the Send() method.  If the send buffer is full, the buffered data will automatically be sent
/// before accepting additional data from any of the Put methods.  Data is not guaranteed to be sent until the Send()
/// method is called. A call to Send() with an empty send buffer is harmless.\n
///
/// The GenOHelper and GenIHelper classes can be used when sending and receiving messages to assist in managing the send
/// and receive buffers.
///
/// There are several references to Reliable over UDP (RUDP) in this header file.  RUDP is too complicated to explain
/// all its details in this file.  See the UMP reference manual for the good stuff.


#include <string>

#include "GenIO.hpp"

class GenBufOManaged;
class GenBufIUser;
struct GenUmpIORudpStats;
struct UMPSockInfoType;


class GEN_EXPORT GenUmpIO : public GenIO
{
public:
   enum
   {
      OptSendOnly = 1 // Do not allocate an input socket
   };

   /// Enum for UMP connection type.
   enum UmpMsgType
   {
      INTERNAL_USE_ONLY  = 0,  /**< .*/
      UDP_SEND_ONLY      = 9,  /**< .*/
      UDP_RCV_ONLY       = 10, /**< .*/
      UDP_BIDIR          = 11, /**< .*/
      UDP_SERVER         = 12, /**< .*/
      UDP_CLIENT         = 13, /**< .*/
      TCP_ACTIVE         = 17, /**< .*/
      TCP_PASSIVE        = 18, /**< .*/
      TCP_SERVER         = 19, /**< .*/
      TCP_CLIENT         = 20, /**< .*/
      TCP_EXISTING       = 21, /**< .*/
      UDP_RELIABLE_BIDIR = 33  /**< .*/
   };

   /// Enum for UMP socket option
   enum UmpSocketOptType
   {
      TCP_CONNECT_NO_WARNING_MSG   = 0x00000001, /**< .*/
      NON_QUEUED                   = 0x00010000, /**< .*/
      QUEUED                       = 0x00020000, /**< .*/
      NO_UMP_PROTOCOL              = 0x00040000, /**< .*/
      TCP_CONNECT_TIMEOUT          = 0x00100000, /**< .*/
      UMP_ALLOW_MULTIPLE_RECEIVERS = 0x00200000, /**< .*/
      UMP_TCP_USE_NAGLE_ALGORITHM =
         0x00400000, /**< Note -- The Nagle Algorithm reduces the number of small TCP packets by
                          combining them, but it also adds latency.  By default, UMP turns off
                          the Nagle Algorithm unless this option is set.  Affects TCP only. */
      UMP_BANNER_SUPPRESS         = 0x00800000, /**< .*/
      UMP_NAMED_MULTICAST         = 0x01000000, /**< .*/
      UMP_NON_BLOCKING_TCP        = 0x02000000, /**< .*/
      UMP1_SOCKET                 = 0x04000000, /**< .*/
      QUEUED_AND_NON_BLOCKING_TCP = 0x02000000  /**< .*/
   };

   /// Default constructor
   GenUmpIO();

   /// This constructor is typically called by GenUmpIOClient to set up a client connection,
   /// since it already has a socket number.
   GenUmpIO(int aClientSocket, const char* aCallingClassName);

   /// Use UMP named addressing with the UMP Arbiter to open the socket.
   /// If it is desired to know the connection status of a TCP client or TCP active connection, it may
   /// be cleaner to use the default constructor and call the Init() method below
   /// that has the same arguments as this constructor.
   /// \param aGroup                 UMP group name.
   /// \param aName                  UMP message name.
   /// \param aMsgType               UMP connection type from UmpMsgType enum. Typically UDP_BIDIR or some form of TCP.
   /// \param aCallingClassName      The name of the calling class.  Used by this class in error messages.
   /// \param aUmpOptions            UMP socket options from UmpSocketOptType enum.  Options can be combined in a bitwise "or".
   ///                               Options that are irrelevant for a particular socket type are ignored.
   ///                               For example "QUEUED" is ignored for TCP sockets.
   /// \param aLocalUpdatePeriodSec  Time on this local computer between calls to Send() or Receive() (generally frame time).
   ///                               This parameter is used only for RUDP connections.
   /// \param aRemoteUpdatePeriodSec Time on remote computer between calls to Send() or Receive() (generally frame time)
   ///                               This parameter is used only for RUDP connections.
   GenUmpIO(const char* aGroup,
            const char* aName,
            UmpMsgType  aMsgType,
            const char* aCallingClassName,
            int         aUmpOptions            = QUEUED | UMP_NON_BLOCKING_TCP | UMP_ALLOW_MULTIPLE_RECEIVERS,
            float       aLocalUpdatePeriodSec  = 0.05F,
            float       aRemoteUpdatePeriodSec = 0.05F);

   /// Use UMP agent name with socket file or IP address to open the socket.
   /// If it is desired to know the connection status of a TCP client or TCP active connection, it may
   /// be cleaner to use the default constructor and call the Init() method below
   /// that has the same arguments as this constructor.
   /// \param aSocketFile[]          An array of socket file names.  Init will traverse the
   ///                               array until it finds a legitimate file and then use that
   ///                               file to build the socket table.  The last entry in the
   ///                               array must be a null zero.  If it is desired to not use
   ///                               a socket table file then the first element of the array
   ///                               should be set to a null zero or aSocketFile
   ///                               should be zero.
   /// \param aSendToAgentName       An agent name that is in the socket table or an IP address
   ///                               of the form "192.168.0.23".
   /// \param aSendToPort            A port number to send the data to on the remote computer.
   /// \param aRecvOnPort            A port number to receive the data on.  If aRecvOnPort is zero,
   ///                               then the function will find a valid receive port.
   /// \param aMsgType               UMP connection type from UmpMsgType enum. Typically UDP_BIDIR or some form of TCP.
   /// \param aCallingClassName      The name of the calling class.  Used by this class in error messages.
   /// \param aUmpOptions            UMP socket options from UmpSocketOptType enum.  Options can be combined in a bitwise "or".
   ///                               Options that are irrelevant for a particular socket type are ignored.
   ///                               For example "QUEUED" is ignored for TCP sockets.
   /// \param aLocalUpdatePeriodSec  Time on this local computer between calls to Send() or Receive() (generally frame time).
   ///                               This parameter is used only for RUDP connections.
   /// \param aRemoteUpdatePeriodSec Time on remote computer between calls to Send() or Receive() (generally frame time)
   ///                               This parameter is used only for RUDP connections.
   GenUmpIO(const char* const aSocketFile[],
            const char*       aSendRecvAgentName,
            unsigned int      aSendToPort,
            unsigned int      aRecvOnPort,
            UmpMsgType        aMsgType,
            const char*       aCallingClassName,
            int               aUmpOptions            = QUEUED | UMP_NON_BLOCKING_TCP | UMP_ALLOW_MULTIPLE_RECEIVERS,
            float             aLocalUpdatePeriodSec  = 0.05F,
            float             aRemoteUpdatePeriodSec = 0.05F);

   /// Destructor
   virtual ~GenUmpIO();

   /// Override RUDP parameters.
   /// Most users will find the LocalUpdatePeriodSec and RemoteUpdatePeriodSec parameters
   /// will set the proper RUDP values, however, if needed this function can be used to
   /// override LocalUpdatePeriodSec and RemoteUpdatePeriodSec.
   /// \warning  This function must be called before calling a function or constructor
   ///           that passes  LocalUpdatePeriodSec and RemoteUpdatePeriodSec.  Therefore,
   ///           if overriding LocalUpdatePeriodSec and RemoteUpdatePeriodSec is desired
   ///           one must use one of the first two constructors, call this function and
   ///           then call one of the Init functions below.
   void SetRudpStatOverrides(int aRetransmitTimeOutMicroSec, int aSendQueueLength, int aRecvQueueLength);

   /// Use UMP named addressing with the UMP Arbiter to open the socket.
   /// Calling this function is not absolutely needed if the group and name were passed
   /// in via a constructor call.  However, calling this function is desired for a
   /// TCP client or a TCP active connection to obtain the status of the connection.
   /// Calling Init repeatedly to obtain a valid  TCP client or a TCP active connection is accetable.
   /// \param aGroup                 UMP group name.
   /// \param aName                  UMP message name.
   /// \param aMsgType               UMP connection type from UmpMsgType enum. Typically UDP_BIDIR or some form of TCP.
   /// \param aCallingClassName      The name of the calling class.  Used by this class in error messages.
   /// \param aUmpOptions            UMP socket options from UmpSocketOptType enum.  Options can be combined in a bitwise "or".
   ///                               Options that are irrelevant for a particular socket type are ignored.
   ///                               For example "QUEUED" is ignored for TCP sockets.
   /// \param aLocalUpdatePeriodSec  Time on this local computer between calls to Send() or Receive() (generally frame time).
   ///                               This parameter is used only for RUDP connections.
   /// \param aRemoteUpdatePeriodSec Time on remote computer between calls to Send() or Receive() (generally frame time)
   ///                               This parameter is used only for RUDP connections.
   /// \return For TCP client and TCP active connections return values are shown below, for
   ///         any other type of connection, a positive number is returned if successful.
   /// \retval   2   Connection was established in this call.
   /// \retval   1   Was previously connected (and still is).
   /// \retval   0   Connection could not be established.
   /// \retval  -1   Connection was previously established, but is now closed.
   int Init(const char* aGroup,
            const char* aName,
            UmpMsgType  aMsgType,
            const char* aCallingClassName,
            int         aUmpOptions            = QUEUED | UMP_NON_BLOCKING_TCP,
            float       aLocalUpdatePeriodSec  = 0.05F,
            float       aRemoteUpdatePeriodSec = 0.05F);

   /// Use UMP agent name with socket file or IP address to open the socket.
   /// Calling this function is not absolutely needed if all the above parameters were passed
   /// in via a constructor call.  However, calling this function is desired for a
   /// TCP client or a TCP active connection to obtain the status of the connection.
   /// Calling Init repeatedly to obtain a valid  TCP client or a TCP active connection is accetable.
   /// \param aSocketFile[]          An array of socket file names.  Init will traverse the
   ///                               array until it finds a legitimate file and then use that
   ///                               file to build the socket table.  The last entry in the
   ///                               array must be a null zero.  If it is desired to not use
   ///                               a socket table file then the first element of the array
   ///                               should be set to a null zero or aSocketFile
   ///                               should be zero.
   /// \param aSendToAgentName       An agent name that is in the socket table or an IP address
   ///                               of the form "192.168.0.23".
   /// \param aSendToPort            A port number to send the data to on the remote computer.
   /// \param aRecvOnPort            A port number to receive the data on.  If aRecvOnPort is zero,
   ///                               then the function will find a valid receive port.
   /// \param aMsgType               UMP connection type from UmpMsgType enum. Typically UDP_BIDIR or some form of TCP.
   /// \param aCallingClassName      The name of the calling class.  Used by this class in error messages.
   /// \param aUmpOptions            UMP socket options from UmpSocketOptType enum.  Options can be combined in a bitwise "or".
   ///                               Options that are irrelevant for a particular socket type are ignored.
   ///                               For example "QUEUED" is ignored for TCP sockets.
   /// \param aLocalUpdatePeriodSec  Time on this local computer between calls to Send() or Receive() (generally frame time).
   ///                               This parameter is used only for RUDP connections.
   /// \param aRemoteUpdatePeriodSec Time on remote computer between calls to Send() or Receive() (generally frame time)
   ///                               This parameter is used only for RUDP connections.
   /// \return For TCP client and TCP active connections return values are shown below, for
   ///         any other type of connection, a positive number is returned if successful.
   /// \retval   2   Connection was established in this call.
   /// \retval   1   Was previously connected (and still is).
   /// \retval   0   Connection could not be established.
   /// \retval  -1   Connection was previously established, but is now closed.
   int Init(const char* const aSocketFile[],
            const char*       aSendRecvAgentName,
            unsigned int      aSendToPort,
            unsigned int      aRecvOnPort,
            UmpMsgType        aMsgType,
            const char*       aCallingClassName,
            int               aUmpOptions            = QUEUED | UMP_NON_BLOCKING_TCP | UMP_ALLOW_MULTIPLE_RECEIVERS,
            float             aLocalUpdatePeriodSec  = 0.05F,
            float             aRemoteUpdatePeriodSec = 0.05F);

   /// Receive the data.
   /// To receive data this class uses the concept that data is first transferred into the receive buffer by calling Receive().
   /// Then the user calls the various Get() methods as needed to remove all the data from the receive buffer.
   /// See the \ref receive "Detailed Description" section for more info on Gets and Receives.
   /// \param aWaitTimeInMicroSec Time period in micro seconds the function will wait for data.
   ///                            If "aWaitTimeInSec" is less than zero, then
   ///                            the function will wait forever.
   /// \retval >= 0     Data received.  This value could optionally be the number of bytes received.
   /// \retval  == -1   No data has been received.
   /// \retval <  -1    An error has occured.
   virtual int Receive(int aWaitTimeInMicroSec);

   /// Send the data.
   /// To send data this class uses the concept that messages or data is "Put" to the send buffer for bundling and then
   /// sent at once with the Send() method.  If the send buffer is full, the buffered data will automatically be sent
   /// before accepting additional data from any of the Put methods.  Data is not guaranteed to be sent until the Send()
   /// method is called. A call to Send() with an empty send buffer is harmless.\n See the \ref send "Detailed
   /// Description" section for more info on Puts and Sends. \retval  >=0   Data was sent and the value is the number of
   /// bytes sent. \retval  <0    An error has occured.
   virtual int Send();

   /// Close all the connections.
   void Terminate();

   /// Check TCP connection status
   /// \retval  1 if connected.
   /// \retval  0 if never connected.
   /// \retval -1 if was connected, but now connection lost.
   int GetTcpConnectState() const;

   /// Change the send to port.  <em>This function is not typically called.</em>
   /// \param aSendToPort Port number on the remote computer.
   void SetSendToPort(unsigned int aSendToPort) { mSendToPort = aSendToPort; }

   /// Get send to address.  <em>This function is not typically called.</em>
   /// \return IP address of where data is being sent.
   unsigned int GetSendToAddr() const;

   /// Get send to address.  <em>This function is not typically called.</em>
   /// \return IP address of where data is being sent.
   std::string GetSendToAddrString() const;

   unsigned short GetRecvFromPort() const;

   /// Get receive from address.  <em>This function is not typically called.</em>
   /// \return IP address of where data is being received from.
   unsigned int GetRecvFromAddr() const;

   /// Get receive from address.  <em>This function is not typically called.</em>
   /// \return IP address of where data is being received from.
   std::string GetRecvFromAddrString() const;

   /// Get send to port.  <em>This function is not typically called.</em>
   /// \return Port number on the remote computer that the data is being sent to.
   int GetSendToPort() const;

   /// Get receive on port.  <em>This function is not typically called.</em>
   /// \return Port number of the local computer that is being used to receive data.
   int GetLocalRecvPort() const;

   /// Get UMP send socket number.  <em>This function is not typically called.</em>
   /// \return UMP send socket number.
   int GetSocketSend() const { return mSocketSend; }

   /// Get UMP receive socket number.  <em>This function is not typically called.</em>
   /// \return UMP receive socket number.
   int GetSocketRecv() const { return mSocketRecv; }

   /// Get max send size.  <em>This function is not typically called.</em>
   /// \return The number of bytes that the send buffer can hold.
   unsigned int GetMaxSendSize();

   /// Get calling class name.  <em>This function is not typically called.</em>
   /// \return Name of the calling class.
   const std::string& GetCallingClassName() const;

   /// Set the name of the calling class.  <em>This function is not typically called.</em>
   /// \param aName Name of the calling class.
   void SetCallingClassName(const std::string& aName);

   /// Change buffer currently being used.  <em>This function is not typically called and will destroy the original Put
   /// buffer.</em> Calling any form of Put after calling this function is not advised. \param aUserBuffer     Pointer
   /// to user buffer. \param aSizeInBytes    Total size of user buffer. \param aNumValidBytes  Number of valid bytes in
   /// user buffer.
   void ChangePutBuffer(unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes);

protected:
   int mSocketRecv;
   int mSocketSend;

private:
   void CreateBuffers(const char* aSendRecvAgentName);
   void DeleteSockets();
   void SetUmpDefaultOptions(int aSocketNumber, UmpMsgType aMsgType);
   int  SetRudpStats(int aSocketNum, float aLocalUpdatePeriodSec, float aRemoteUpdatePeriodSec);
   void CheckForRudpStats();

   static unsigned int sRudpFailedSends;
   static unsigned int sRudpRetrans;
   static unsigned int sRudpUnnecRetrans;
   static unsigned int sRudpRejectedRecvs;

   unsigned int mSendToPort;
   unsigned int mSendFromPort;
   unsigned int mSendError;
   unsigned int mRecvError;
   unsigned int mRudpMaxSendQ;
   unsigned int mRudpMaxRecvQ;
   int          mRudpRetransmitTimeOut; // micro secs
   int          mRudpSendQueueLength;
   int          mRudpRecvQueueLength;

   GenBufIUser*    mRecvBuf;
   GenBufOManaged* mSendBuf;

   GenUmpIORudpStats* mRudpStats;
   unsigned int       mRudpCheckCount;
   UMPSockInfoType*   mSocketRecvInfoPtr;

   std::string mCallingClassName;
};

inline const std::string& GenUmpIO::GetCallingClassName() const
{
   return mCallingClassName;
}

inline void GenUmpIO::SetCallingClassName(const std::string& aName)
{
   mCallingClassName = aName;
}


#endif
