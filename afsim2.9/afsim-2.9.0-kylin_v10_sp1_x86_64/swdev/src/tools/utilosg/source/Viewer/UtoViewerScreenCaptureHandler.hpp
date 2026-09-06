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
#ifndef UTOVIEWERSCREENCAPTUREHANDLER_HPP
#define UTOVIEWERSCREENCAPTUREHANDLER_HPP

#include <osgViewer/ViewerEventHandlers>

class UtoViewerScreenCaptureHandler : public osgViewer::ScreenCaptureHandler
{
public:
   UtoViewerScreenCaptureHandler(CaptureOperation* defaultOperation = nullptr);

   void SetCaptureDirectory(const std::string& aDirectory);
   void SetContextId(int aValue);

   // this will override the management done by the previous two methods, and force all write to happen to the
   // provided location.  Setting the filename back to "" will revert to the normal behavior.
   void SetFilename(const std::string& aFilename);

private:
   CaptureOperation* mCapOp;
};

class UtoViewerCaptureOperation : public osgViewer::ScreenCaptureHandler::CaptureOperation
{
public:
   enum SavePolicy
   {
      OVERWRITE,
      SEQUENTIAL_NUMBER
      // ... any others?
   };

   UtoViewerCaptureOperation(const std::string& filename,
                             const std::string& extension,
                             SavePolicy         savePolicy = SEQUENTIAL_NUMBER);

   void operator()(const osg::Image& image, const unsigned int context_id) override;

   void       setSavePolicy(SavePolicy savePolicy) { _savePolicy = savePolicy; }
   SavePolicy getSavePolicy() const { return _savePolicy; }

   void SetCaptureDirectory(const std::string& aDirectory) { _directory = aDirectory; }

   void SetContextId(int aValue) { mContextId = aValue; }

   void SetFilename(const std::string& aFilename) { _filenameForce = aFilename; }

protected:
   UtoViewerCaptureOperation& operator=(const UtoViewerCaptureOperation&) { return *this; }

   const std::string _filename;
   const std::string _extension;
   std::string       _directory;
   std::string       _filenameForce;

   SavePolicy _savePolicy;

   std::vector<unsigned int> _contextSaveCounter;

   int mContextId;
};

#endif
