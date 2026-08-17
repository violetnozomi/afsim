#ifndef NRM_NETWORK_PLAN_DISTRIBUTION_SERVICE_HPP
#define NRM_NETWORK_PLAN_DISTRIBUTION_SERVICE_HPP

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>

#include <cerrno>
#include <sys/stat.h>
#include <unistd.h>

#include "nrm/NetworkPlanRepository.hpp"
#include "nrm/NetworkPlanSerialization.hpp"
#include "nrm/TemporaryPathGuard.hpp"

namespace nrm
{
class NetworkPlanDistributionService
{
public:
   DistributionPackageResult Generate(
      const NetworkPlanDocument& aPlan,
      const PlanValidationResult& aValidation,
      const NetworkPlanEvaluationResult& aEvaluation,
      const std::string& aOutputRoot = std::string()) const
   {
      DistributionPackageResult result;
      result.planId = aPlan.planId;
      result.revision = aPlan.revision;
      result.planFingerprint = network_plan_detail::PlanContentFingerprint(aPlan);
      if (!network_plan_detail::IsSafePlanId(aPlan.planId) || aPlan.revision == 0)
      {
         result.reason = PlanValidationReason::cOUTPUT_PATH_INVALID;
         return result;
      }
      if (!InputsMatch(aPlan, aValidation, aEvaluation))
      {
         result.reason = PlanValidationReason::cPLAN_IDENTITY_MISMATCH;
         return result;
      }
      if (!aValidation.passed ||
          aEvaluation.overallStatus != PlanEvaluationStatus::cPASS ||
          aEvaluation.resultingState != NetworkPlanState::cVALIDATED ||
          !AllDemandsPassed(aPlan, aEvaluation))
      {
         result.reason = PlanValidationReason::cPLAN_NOT_VALIDATED;
         return result;
      }

      const std::string outputRoot = ResolveOutputRoot(aOutputRoot);
      if (outputRoot.empty())
      {
         result.reason = PlanValidationReason::cOUTPUT_PATH_INVALID;
         return result;
      }
      const std::string packageRoot = Join(outputRoot, "distribution_packages");
      const std::string planRoot = Join(packageRoot, aPlan.planId);
      const std::string finalRoot = Join(planRoot, std::to_string(aPlan.revision));
      if (!CreateDirectories(planRoot))
      {
         result.reason = PlanValidationReason::cFILE_WRITE_FAILED;
         return result;
      }
      if (PathExists(finalRoot))
      {
         result.reason = PlanValidationReason::cPACKAGE_ALREADY_EXISTS;
         return result;
      }

      const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
      const std::string stagingRoot = Join(
         planRoot, ".revision-" + std::to_string(aPlan.revision) +
                      ".tmp-" + std::to_string(stamp));
      if (mkdir(stagingRoot.c_str(), 0700) != 0)
      {
         result.reason = PlanValidationReason::cFILE_WRITE_FAILED;
         return result;
      }

      const std::string planPath = Join(stagingRoot, "network_plan.nrm");
      const std::string validationPath = Join(stagingRoot, "validation_result.json");
      const std::string evaluationPath = Join(stagingRoot, "evaluation_result.json");
      const std::string manifestPath = Join(stagingRoot, "manifest.json");
      TemporaryPathGuard stagingGuard(
         stagingRoot,
         [planPath, validationPath, evaluationPath, manifestPath](
            const std::string& aRoot) {
            CleanupStaging(aRoot, planPath, validationPath,
                           evaluationPath, manifestPath);
         });
      NetworkPlanDocument packagePlan = aPlan;
      packagePlan.state = NetworkPlanState::cREADY_FOR_DISTRIBUTION;
      const PlanRepositoryResult saveResult =
         NetworkPlanRepository::SaveDocumentAtomic(packagePlan, planPath, true);
      const std::string generatedTime = UtcTimestamp();
      const bool filesWritten = saveResult.success &&
         WriteValidationFile(validationPath, aValidation) &&
         WriteEvaluationFile(evaluationPath, aEvaluation) &&
         WriteManifest(manifestPath, packagePlan, generatedTime);
      if (!filesWritten)
      {
         result.reason = saveResult.success
                            ? PlanValidationReason::cFILE_WRITE_FAILED
                            : saveResult.reason;
         return result;
      }
      if (std::rename(stagingRoot.c_str(), finalRoot.c_str()) != 0)
      {
         result.reason = PlanValidationReason::cATOMIC_RENAME_FAILED;
         return result;
      }
      stagingGuard.Commit();

      result.generated = true;
      result.outputPath = finalRoot;
      result.resultingState = NetworkPlanState::cREADY_FOR_DISTRIBUTION;
      result.reason = PlanValidationReason::cNONE;
      return result;
   }

private:
   static bool InputsMatch(const NetworkPlanDocument& aPlan,
                           const PlanValidationResult& aValidation,
                           const NetworkPlanEvaluationResult& aEvaluation)
   {
      const std::string fingerprint =
         network_plan_detail::PlanContentFingerprint(aPlan);
      return aValidation.planId == aPlan.planId &&
             aValidation.revision == aPlan.revision &&
             !fingerprint.empty() &&
             aValidation.planFingerprint == fingerprint &&
             aEvaluation.planId == aPlan.planId &&
             aEvaluation.revision == aPlan.revision &&
             aEvaluation.planFingerprint == fingerprint &&
             aEvaluation.validation.planId == aPlan.planId &&
             aEvaluation.validation.revision == aPlan.revision &&
             aEvaluation.validation.planFingerprint == fingerprint;
   }

   static bool AllDemandsPassed(const NetworkPlanDocument& aPlan,
                                const NetworkPlanEvaluationResult& aEvaluation)
   {
      if (aPlan.demands.empty() || aEvaluation.demands.size() != aPlan.demands.size())
         return false;
      for (std::size_t index = 0; index < aEvaluation.demands.size(); ++index)
      {
         const PlanDemandEvaluation& demand = aEvaluation.demands[index];
         if (demand.demandId != aPlan.demands[index].demandId) return false;
         if (demand.status != PlanEvaluationStatus::cPASS) return false;
      }
      return true;
   }

   static std::string ResolveOutputRoot(const std::string& aOutputRoot)
   {
      if (!aOutputRoot.empty()) return aOutputRoot;
      const char* store = std::getenv("NRM_PLAN_STORE_DIR");
      return store == nullptr ? std::string() : std::string(store);
   }

   static std::string Join(const std::string& aLeft, const std::string& aRight)
   {
      if (aLeft.empty()) return aRight;
      return aLeft.back() == '/' ? aLeft + aRight : aLeft + '/' + aRight;
   }

   static bool PathExists(const std::string& aPath)
   {
      struct stat info;
      return stat(aPath.c_str(), &info) == 0;
   }

   static bool EnsureDirectory(const std::string& aPath)
   {
      if (aPath.empty()) return false;
      struct stat info;
      if (stat(aPath.c_str(), &info) == 0) return S_ISDIR(info.st_mode);
      return mkdir(aPath.c_str(), 0700) == 0 || errno == EEXIST;
   }

   static bool CreateDirectories(const std::string& aPath)
   {
      if (aPath.empty()) return false;
      std::string current = aPath.front() == '/' ? "/" : std::string();
      std::size_t start = aPath.front() == '/' ? 1 : 0;
      while (start <= aPath.size())
      {
         const std::size_t end = aPath.find('/', start);
         const std::string component =
            aPath.substr(start, end == std::string::npos ? std::string::npos : end - start);
         if (!component.empty())
         {
            current = Join(current, component);
            if (!EnsureDirectory(current)) return false;
         }
         if (end == std::string::npos) break;
         start = end + 1;
      }
      return true;
   }

   static bool WriteValidationFile(const std::string& aPath,
                                   const PlanValidationResult& aValidation)
   {
      std::ofstream output(aPath, std::ios::out | std::ios::trunc);
      if (!output) return false;
      network_plan_serialization::WriteValidation(output, aValidation);
      output << '\n';
      output.flush();
      return output.good();
   }

   static bool WriteEvaluationFile(const std::string& aPath,
                                   const NetworkPlanEvaluationResult& aEvaluation)
   {
      std::ofstream output(aPath, std::ios::out | std::ios::trunc);
      if (!output) return false;
      network_plan_serialization::WriteEvaluation(output, aEvaluation);
      output << '\n';
      output.flush();
      return output.good();
   }

   static bool WriteManifest(const std::string& aPath,
                             const NetworkPlanDocument& aPlan,
                             const std::string& aGeneratedTime)
   {
      std::ofstream output(aPath, std::ios::out | std::ios::trunc);
      if (!output) return false;
      output << "{\"schemaVersion\":\"nrm.network_plan_distribution.v1\","
                "\"planId\":\""
             << network_plan_serialization::EscapeJson(aPlan.planId)
             << "\",\"revision\":" << aPlan.revision
             << ",\"planFingerprint\":\""
             << network_plan_detail::PlanContentFingerprint(aPlan) << '"'
             << ",\"configVersion\":\""
             << network_plan_serialization::EscapeJson(aPlan.configVersion)
             << "\",\"planFile\":\"network_plan.nrm\","
                "\"validationResultFile\":\"validation_result.json\","
                "\"evaluationResultFile\":\"evaluation_result.json\","
                "\"generatedTime\":\""
             << network_plan_serialization::EscapeJson(aGeneratedTime)
             << "\",\"readyForDistribution\":true}\n";
      output.flush();
      return output.good();
   }

   static std::string UtcTimestamp()
   {
      const std::time_t now = std::time(nullptr);
      std::tm utc{};
      gmtime_r(&now, &utc);
      char buffer[32];
      std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &utc);
      return buffer;
   }

   static void CleanupStaging(const std::string& aRoot,
                              const std::string& aPlanPath,
                              const std::string& aValidationPath,
                              const std::string& aEvaluationPath,
                              const std::string& aManifestPath)
   {
      std::remove(aPlanPath.c_str());
      std::remove(aValidationPath.c_str());
      std::remove(aEvaluationPath.c_str());
      std::remove(aManifestPath.c_str());
      rmdir(aRoot.c_str());
   }
};
} // namespace nrm

#endif
