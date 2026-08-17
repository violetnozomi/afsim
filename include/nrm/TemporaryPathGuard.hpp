#ifndef NRM_TEMPORARY_PATH_GUARD_HPP
#define NRM_TEMPORARY_PATH_GUARD_HPP

// Scope guard for temporary files or staging paths. A successful atomic
// rename calls Commit(); every other exit invokes the supplied cleanup.

#include <cstdio>
#include <functional>
#include <string>
#include <utility>

namespace nrm
{
class TemporaryPathGuard
{
public:
   using Cleanup = std::function<void(const std::string&)>;

   explicit TemporaryPathGuard(std::string aPath)
      : TemporaryPathGuard(
           std::move(aPath),
           [](const std::string& aTemporaryPath)
           {
              std::remove(aTemporaryPath.c_str());
           })
   {
   }

   TemporaryPathGuard(std::string aPath, Cleanup aCleanup)
      : mPath(std::move(aPath))
      , mCleanup(std::move(aCleanup))
      , mActive(!mPath.empty() && static_cast<bool>(mCleanup))
   {
   }

   ~TemporaryPathGuard() noexcept { CleanupNow(); }

   TemporaryPathGuard(const TemporaryPathGuard&) = delete;
   TemporaryPathGuard& operator=(const TemporaryPathGuard&) = delete;

   TemporaryPathGuard(TemporaryPathGuard&& aSource) noexcept
      : mPath(std::move(aSource.mPath))
      , mCleanup(std::move(aSource.mCleanup))
      , mActive(aSource.mActive)
   {
      aSource.mActive = false;
   }

   TemporaryPathGuard& operator=(TemporaryPathGuard&& aSource) noexcept
   {
      if (this != &aSource)
      {
         CleanupNow();
         mPath = std::move(aSource.mPath);
         mCleanup = std::move(aSource.mCleanup);
         mActive = aSource.mActive;
         aSource.mActive = false;
      }
      return *this;
   }

   void Commit() noexcept { mActive = false; }
   bool Active() const noexcept { return mActive; }

private:
   void CleanupNow() noexcept
   {
      if (!mActive) return;
      mActive = false;
      try { mCleanup(mPath); } catch (...) {}
   }

   std::string mPath;
   Cleanup mCleanup;
   bool mActive = false;
};
}

#endif
