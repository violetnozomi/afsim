// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************


#ifndef RV_DISTRIBUTED_MERGER_HPP
#define RV_DISTRIBUTED_MERGER_HPP

#include <memory>
#include <string>
#include <vector>

#include <QThread>

#include "RvMergerUtils.hpp"

namespace RvMerger
{
//! Thread wrapper for MergeRecordings.
class MergerThread final : public QThread
{
   Q_OBJECT
public:
   explicit MergerThread(QObject* aParent);
   ~MergerThread() noexcept override;

   //! Adds a recording file to process.
   //! The first recording added will be used as the "reference" recording and should span the entire simulation.
   void AddInputFile(std::string aFileName) noexcept;

   //! Sets the output file destination.
   //! Overwrites the previous setting.
   void SetOutputFile(std::string aFileName) noexcept;

   //! Clears all data
   void Reset() noexcept;

signals:

   //! Emitted if there is an error while running.
   //! \param aErrorMessage The what() of the thrown exception.
   void RunError(const std::string& aErrorMessage);

protected:
   //! Processes the recordings.
   void run() noexcept override;

private:
   std::vector<std::unique_ptr<RvMerger::RecordingFileReader>> InitializeInputs(UtPackSerializer& aSerializer);

   void AddPlatform(PlatformDatabase& aPlatforms, const rv::MsgPlatformInfo& aMessage, size_t aSimIndex);

   void AdjustPlatformIndices(PlatformDatabase& aPlatforms, rv::MsgBase& aMessage, size_t aSimIndex);

   void AdjustSimTimes(rv::MsgBase& aMessage, double aOffset);

   void OutputMessage(RecordingFileWriter& aOutput, size_t aRecordingIndex, const rv::MsgBase& aMessage);

   std::vector<std::string> mInputFiles;
   std::string              mOutputFile;
   double                   mMinTimeOffset;
};
} // namespace RvMerger

#endif
