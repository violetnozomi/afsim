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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "DisPdu.hpp"

#include <iostream>

#include "Dis.hpp"
#include "DisActionEnums.hpp"
#include "DisAttribute.hpp"
#include "DisEntityId.hpp"
#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "DisPduFactory.hpp"
#include "DisTime.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtMemory.hpp"
#include "math.h"

namespace
{
static const DisUint16 BaseLength = 12;
}

DisPdu::DisPdu()
   : mProtocolVersion(Dis::GetProtocolVersion())
   , mExerciseIdentifier(Dis::GetExercise())
   , mPduType(DisEnum::Pdu::Type::Other)
   , mProtocolFamily(DisEnum::Pdu::Family::Other)
   , mTimestamp(0)
   , mLength(BaseLength)
   , mPduStatus(0)
   , mCoupledExtension(nullptr)
{
}

DisPdu::DisPdu(const std::vector<std::string>& aLoggerResultVector)
   : mCoupledExtension(nullptr)
{
   for (const std::string& line : aLoggerResultVector)
   {
      if (line.find("PDU Protocol") != std::string::npos)
      {
         mProtocolVersion = (std::atoi(line.substr(line.find("=") + 2).c_str()));
      }
      else if (line.find("Exercise") != std::string::npos)
      {
         mExerciseIdentifier = (std::atoi(line.substr(line.find("=") + 2).c_str()));
      }
      else if (line.find("Type") != std::string::npos)
      {
         mPduType = (std::atoi(line.substr(line.find("=") + 2).c_str()));
      }
      else if (line.find("Family") != std::string::npos)
      {
         mProtocolFamily = (std::atoi(line.substr(line.find("=") + 2).c_str()));
      }
      else if (line.find("Time Stamp") != std::string::npos)
      {
         mTimestamp = (std::atoi(line.substr(line.find("=") + 2).c_str()));
      }
      else if (line.find("Length") != std::string::npos)
      {
         mLength = (std::atoi(line.substr(line.find("=") + 2).c_str()));
      }
   }
}


DisPdu::DisPdu(const DisPdu& aSrc)
   : mProtocolVersion(aSrc.mProtocolVersion)
   , mExerciseIdentifier(aSrc.mExerciseIdentifier)
   , mPduType(aSrc.mPduType)
   , mProtocolFamily(aSrc.mProtocolFamily)
   , mTimestamp(aSrc.mTimestamp)
   , mLength(aSrc.mLength)
   , mPduStatus(aSrc.mPduStatus)
   , mCoupledExtension(nullptr)
{
   if (aSrc.mCoupledExtension)
   {
      DisAttribute* ext = static_cast<DisAttribute*>(aSrc.mCoupledExtension->Clone());
      CoupleExtension(ext);
   }
}

DisPdu::DisPdu(GenI& aGenI)
   : mCoupledExtension(nullptr)
{
   DisPdu::Get(aGenI);
}

DisPdu::~DisPdu()
{
   delete mCoupledExtension;
}

// virtual
DisPdu* DisPdu::Clone() const
{
   return new DisPdu(*this);
}

// virtual
const char* DisPdu::GetScriptClassName() const
{
   return "DisBasePdu";
}

// virtual
int DisPdu::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Other;
}

int DisPdu::GetFamily() const
{
   return (int)DisEnum::Pdu::Family::Other;
}

bool DisPdu::IsInitiator() const
{
   using namespace DisEnum::Pdu::Type;
   bool result = false;

   switch (mPduType)
   {
   case ServiceRequest:
   case ResupplyOffer:
   case ResupplyCancel:
   case CreateEntity:
   case RemoveEntity:
   case StartResume:
   case StopFreeze:
   case ActionRequest:
   case DataQuery:
   case SetData:
   case EventReport:
   case TransferOwnership:
   case CreateEntityR:
   case RemoveEntityR:
   case StartResumeR:
   case StopFreezeR:
   case ActionRequestR:
   case DataQueryR:
   case SetDataR:
   case SetRecordR:
   case EventReportR:
   case RecordQueryR:
      result = true;
      break;
   default:;
   }
   return result;
}

bool DisPdu::IsRespondent() const
{
   // It is possible that in the future some implementations may desire
   // that a DIS PDU "Reliable" Initiator be satisfiable by a non-reliable
   // response equivalent.  The DIS standard permits this... for instance
   // a DisSetRecordR is responded to by a DisRecordR PDU, and the reliability
   // service bit in the DisRecordR PDU is explicitly requested to be set false.

   using namespace DisEnum::Pdu::Type;
   bool result = false;
   switch (mPduType)
   {
   case ActionResponse:
   case ActionResponseR:
   case Acknowledge:
   case AcknowledgeR:
   case Data:
   case DataR:
   case RecordR:
   case ResupplyOffer:
   case RepairResponse:
      result = true;
      break;
   default:;
   }
   return result;
}

bool DisPdu::IsReliability() const
{
   bool result = false;
   if ((mPduType >= DisEnum::Pdu::Type::CreateEntityR) && (mPduType <= DisEnum::Pdu::Type::RecordQueryR))
   {
      result = true;
   }

   return result;
}

// virtual
int DisPdu::Process()
{
   const int USER_CAN_DELETE_PDU = 1;
   return USER_CAN_DELETE_PDU;
}

// virtual
void DisPdu::TimedOut()
{
   // Do nothing in the base class.
   ;
}

// virtual
DisUint32 DisPdu::GetRequestId() const
{
   return 0;
}

// virtual
DisEnum8 DisPdu::GetReliabilityService() const
{
   return DisEnum::Pdu::Reliability::Unacknowledged;
}

// virtual
const DisEntityId& DisPdu::GetOriginatingEntity() const
{
   // Return an empty Id for now
   return Dis::cNO_SPECIFIC_ENTITY;
}

// virtual
const DisEntityId& DisPdu::GetReceivingEntity() const
{
   // Return an empty Id for now
   return Dis::cNO_SPECIFIC_ENTITY;
}

// 'Create' is a static method that uses a factory to create a PDU
// from the provided input stream. If no factory is provided a
// default one is created.
//
// This is a static function

DisPdu* DisPdu::Create(GenI& aGenI, DisPduFactory* aDisPduFactory)
{
   DisPdu* pduPtr = nullptr;

   // Use the correct factory
   if (aDisPduFactory != nullptr)
   {
      pduPtr = aDisPduFactory->GetMsg(aGenI);
   }
   else
   {
      pduPtr = GetLocalDisPduFactory()->GetMsg(aGenI);
   }
   return pduPtr;
}

// virtual
void DisPdu::HandleReadError(ut::log::MessageStream& aStream)
{
   if (Dis::ShowError(Dis::cIO_ERRORS))
   {
      aStream.AddNote() << "Length: " << GetLengthToRead();
      aStream.AddNote() << "Version: " << static_cast<unsigned int>(mProtocolVersion);
      aStream.AddNote() << "Type: " << static_cast<unsigned int>(mPduType);
      aStream.AddNote() << "Family: " << static_cast<unsigned int>(mProtocolFamily);
      aStream.AddNote() << "Exercise: " << static_cast<int>(mExerciseIdentifier);
   }
}

// virtual
bool DisPdu::IsValid() const
{
   // Skip PDU Version test to allow forward compatibility with future versions
   // if(! DisEnum::Pdu::Version::IsValid(mProtocolVersion) ) return false;
   if (!DisEnum::Pdu::Type::IsValid(mPduType))
   {
      return false;
   }
   if (!DisEnum::Pdu::Family::IsValid(mProtocolFamily))
   {
      return false;
   }
   if (mLength > Dis::GetMAX_PDU_SIZE_OCTETS())
   {
      return false;
   }
   if (mProtocolVersion > DisEnum::Pdu::Version::IEEE_1278_1A_1998 &&
       ((GetCoupledExtensionIndicator() && !mCoupledExtension) || (!GetCoupledExtensionIndicator() && mCoupledExtension)))
   {
      return false;
   }
   return true;
}

void DisPdu::Get(GenI& aGenI)
{
   aGenI >> mProtocolVersion;
   aGenI >> mExerciseIdentifier;
   aGenI >> mPduType;
   aGenI >> mProtocolFamily;
   aGenI >> mTimestamp;
   aGenI >> mLength;
   aGenI >> mPduStatus;
   // Skip 8 bits of padding (1 octet)
   aGenI >> mPaddingEightBits;
}

// static
DisUint16 DisPdu::GetBaseLength()
{
   return static_cast<int>(BaseLength);
}

// virtual
DisUint16 DisPdu::GetLength()
{
   return mLength;
}

// Virtual
int DisPdu::DoIt()
{
   // Defalut is to return a zero so the receiver does not take ownership
   return 0;
}


void DisPdu::Put(GenO& aGenO)
{
   aGenO << mProtocolVersion;
   aGenO << mExerciseIdentifier;
   aGenO << mPduType;
   aGenO << mProtocolFamily;
   aGenO << mTimestamp;
   aGenO << mLength;
   aGenO << mPduStatus;
   // aGenO << DisUint8(0); // 8 bits of padding
   aGenO << mPaddingEightBits;
}

// The following method is called by the specific PDU reading
// routines to read any 'extra' data.  This occurs when the
// length declared in the header is greater that actually
// required for the PDU.
void DisPdu::ReadExtraData(GenI& aGenI, DisUint16 aLengthToRead)
{
   // Check that length declared in the header is equal to
   // requirement for the PDU.  If size is mismatched,
   // the status is set.
   if (aLengthToRead != 0)
   {
      if (Dis::ShowError(Dis::cIO_ERRORS))
      {
         auto out = ut::log::error() << "Mismatched size reading data.";
         out.AddNote() << "Interface Name: " << aGenI.GetName();
         out.AddNote() << "PDU Type:" << static_cast<unsigned int>(mPduType);
         aGenI.SetInputStatus(GenBuf::MismatchedSize);
      }
   }
}

std::string DisPdu::GetEntityPartCountStringValue(int enumValue)
{
   return "No Parts attached";
}

DisPduProcessI& DisPdu::GetPduProcess() const
{
   return *mPduProcess;
}

void DisPdu::SetPduProcess(DisPduProcessI* aNewPduProcess)
{
   mPduProcess.reset(aNewPduProcess);
}

//
// Private
//

std::string DisPdu::ToString() const
{
   using namespace std;
   using namespace DisEnum::Pdu;
   std::ostringstream ss("");

   Version::Enum version = static_cast<Version::Enum>(GetProtocolVersion());
   Type::Enum    type    = static_cast<Type::Enum>(GetPduType());
   Family::Enum  family  = static_cast<Family::Enum>(GetProtocolFamily());

   static const unsigned int TICKS_PER_HOUR = 0x80000000;
   double       seconds  = static_cast<double>(GetTimestamp() >> 1) * (3600.0 / static_cast<double>(TICKS_PER_HOUR));
   unsigned int min      = static_cast<unsigned int>(seconds) / 60;
   unsigned int absolute = GetTimestamp() & 0x00000001;
   seconds -= 60.0 * min;
   ss << "***** PDU Header *****\n"
      << "Protocol Version:    " << version << " (" << Version::ToString(version) << ")\n"
      << "Exercise Identifier: " << static_cast<DisUint16>(GetExerciseIdentifier()) << '\n'
      << "PDU Type:            " << type << " (" << Type::ToString(type) << ")\n"
      << "Protocol Family:     " << family << " (" << Family::ToString(family) << ")\n"
      << "Timestamp:           " << static_cast<DisUint32>(GetTimestamp()) << " (" << setw(2) << right << setfill('0')
      << min << ":" << setw(9) << fixed << setprecision(6) << right << seconds << " MM:SS past hour, ";
   ss << setfill(' ');
   if (absolute == 1)
   {
      ss << "Absolute)\n";
   }
   else
   {
      ss << "Relative)\n";
   }
   if (GetPduStatus() != 0)
   {
      ss << "PDU Status:          " << hex << showbase << setw(2) << setfill('0') << GetPduStatus() << dec << noshowbase
         << setfill(' ') << '\n';
      ss << DisEnum::Pdu::Status::ToString(GetPduStatus(), GetPduType());
   }
   ss << "***** PDU Header End *****";
   return ss.str();
}
// Static
DisPduFactory* DisPdu::GetLocalDisPduFactory()
{
   static DisPduFactory theFactory;
   return &theFactory;
}

const DisPdu& DisPdu::operator=(const DisPdu& aPdu)
{
   mProtocolVersion    = aPdu.mProtocolVersion;
   mExerciseIdentifier = aPdu.mExerciseIdentifier;
   mPduType            = aPdu.mPduType;
   mProtocolFamily     = aPdu.mProtocolFamily;
   mTimestamp          = aPdu.mTimestamp;
   mLength             = aPdu.mLength;
   mPduStatus          = aPdu.mPduStatus;

   if (aPdu.mCoupledExtension)
   {
      DisAttribute* ext = static_cast<DisAttribute*>(aPdu.mCoupledExtension->Clone());
      CoupleExtension(ext);
   }
   else
   {
      delete mCoupledExtension;
      mCoupledExtension = nullptr;
   }

   return *this;
}

// virtual
void DisPdu::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

bool DisPdu::CoupleExtension(DisAttribute* aAttribute)
{
   static const DisEnum8 CEI_BIT = 0x08;
   if (!aAttribute)
   {
      return false;
   }
   size_t total_length = GetLength() + aAttribute->GetLength();
   if (total_length > Dis::GetMAX_PDU_SIZE_OCTETS())
   {
      return false;
   }
   DisAttribute* tmp = mCoupledExtension;
   mCoupledExtension = aAttribute;
   delete tmp;
   mPduStatus |= CEI_BIT;
   return true;
}

// Time helper functions

double DisPdu::GetSecsAfterHour(unsigned int& aAbsoluteFlag) const
{
   return DisTime::GetSecsAfterHour(mTimestamp, aAbsoluteFlag);
}

double DisPdu::GetAbsoluteTime() const
{
   return DisTime::GetAbsoluteTime(mTimestamp);
}

double DisPdu::GetBestAvailableTime(double aSimTime, unsigned int aAdjustDelta) const
{
   return DisTime::GetBestAvailableTime(mTimestamp, GetOriginatingEntity(), aSimTime, aAdjustDelta);
}

void DisPdu::SetTime(double aTime)
{
   DisTime::SetTimeStamp(mTimestamp, aTime);
}
