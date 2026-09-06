// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtoViewerScreenCaptureHandler.hpp"

#include <sstream>

#include <osgDB/WriteFile>

UtoViewerScreenCaptureHandler::UtoViewerScreenCaptureHandler(CaptureOperation* defaultOperation)
   : osgViewer::ScreenCaptureHandler(defaultOperation)
{
   if (defaultOperation)
   {
      mCapOp = defaultOperation;
      setCaptureOperation(defaultOperation);
   }
   else
   {
      mCapOp = new UtoViewerCaptureOperation("screen_shot", "jpg");
      setCaptureOperation(mCapOp);
   }
}

void UtoViewerScreenCaptureHandler::SetCaptureDirectory(const std::string& aDirectory)
{
   UtoViewerCaptureOperation* vco = dynamic_cast<UtoViewerCaptureOperation*>(mCapOp);
   if (nullptr != vco)
   {
      vco->SetCaptureDirectory(aDirectory);
   }
}

void UtoViewerScreenCaptureHandler::SetContextId(int aValue)
{
   UtoViewerCaptureOperation* vco = dynamic_cast<UtoViewerCaptureOperation*>(mCapOp);
   if (nullptr != vco)
   {
      vco->SetContextId(aValue);
   }
}

void UtoViewerScreenCaptureHandler::SetFilename(const std::string& aFilename)
{
   UtoViewerCaptureOperation* vco = dynamic_cast<UtoViewerCaptureOperation*>(mCapOp);
   if (nullptr != vco)
   {
      vco->SetFilename(aFilename);
      if (!aFilename.empty())
      {
         vco->setSavePolicy(UtoViewerCaptureOperation::OVERWRITE);
      }
      else
      {
         vco->setSavePolicy(UtoViewerCaptureOperation::SEQUENTIAL_NUMBER);
      }
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ScreenCaptureHandler::WriteToFile
//
UtoViewerCaptureOperation::UtoViewerCaptureOperation(const std::string& filename,
                                                     const std::string& extension,
                                                     SavePolicy         savePolicy)
   : _filename(filename)
   , _extension(extension)
   , _directory("")
   , _savePolicy(savePolicy)
{
}

void UtoViewerCaptureOperation::operator()(const osg::Image& image, const unsigned int context_id)
{
   if (_savePolicy == SEQUENTIAL_NUMBER)
   {
      if (_contextSaveCounter.size() <= context_id)
      {
         unsigned int oldSize = _contextSaveCounter.size();
         _contextSaveCounter.resize(context_id + 1);
         // Initialize all new values to 0 since context ids may not be consecutive.
         for (unsigned int i = oldSize; i <= context_id; i++)
            _contextSaveCounter[i] = 0;
      }
   }

   bool fileWritten = false;
   while (!fileWritten)
   {
      std::stringstream filename;
      if (!(_filenameForce.empty()))
      {
         filename << _filenameForce;
      }
      else
      {
         filename << _directory << _filename << "_" << mContextId;

         if (_savePolicy == SEQUENTIAL_NUMBER)
            filename << "_" << _contextSaveCounter[context_id];

         filename << "." << _extension;
      }

      if (_savePolicy == SEQUENTIAL_NUMBER)
      {
         FILE* file = fopen(filename.str().c_str(), "r");
         if (file != nullptr)
         {
            _contextSaveCounter[context_id]++;
            fclose(file);
         }
         else
         {
            osgDB::writeImageFile(image, filename.str());
            fileWritten = true;
         }
      }
      else
      {
         osgDB::writeImageFile(image, filename.str());
         fileWritten = true;
      }
   }

#ifdef OSG_3_0_1
   OSG_INFO << "ScreenCaptureHandler: Taking a screenshot, saved as '" << filename.str() << "'" << std::endl;
#endif

   if (_savePolicy == SEQUENTIAL_NUMBER)
   {
      _contextSaveCounter[context_id]++;
   }
}
