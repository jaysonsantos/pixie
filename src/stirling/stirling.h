#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "absl/base/internal/spinlock.h"
#include "src/common/error.h"
#include "src/common/status.h"
#include "src/stirling/data_table.h"
#include "src/stirling/proto/collector_config.pb.h"
#include "src/stirling/pub_sub_manager.h"
#include "src/stirling/source_connector.h"
#include "src/stirling/source_registry.h"

namespace pl {
namespace stirling {

/**
 * @brief Convenience function to subscribe to all info classes of
 * a published proto message. This should actually be in an agent.
 * TODO(kgandhi): Move to agent or common utils for agent when available.
 *
 * @param publish_proto
 * @return stirlingpb::Subscribe
 */
stirlingpb::Subscribe SubscribeToAllInfoClasses(const stirlingpb::Publish& publish_proto);

/**
 * The data collector collects data from various different 'sources',
 * and makes them available via a structured API, where the data can then be used and queried as
 * needed (by Pixie or others). Its function is to unify various, disparate sources of data into a
 * common, structured data format.
 */
class Stirling : public NotCopyable {
 public:
  Stirling() = delete;
  ~Stirling();

  /**
   * @brief Create a Stirling object
   * Factory method to create Stirling with a default registry containing
   * all sources
   *
   * @return std::unique_ptr<Stirling>
   */
  static std::unique_ptr<Stirling> Create() {
    auto registry = std::make_unique<SourceRegistry>();
    RegisterAllSources(registry.get());
    return Create(std::move(registry));
  }
  /**
   * @brief Create a Stirling object
   * Factory method to create Stirling with a source registry.
   *
   * @param registry
   * @return std::unique_ptr<Stirling>
   */
  static std::unique_ptr<Stirling> Create(std::unique_ptr<SourceRegistry> registry) {
    auto stirling = std::unique_ptr<Stirling>(new Stirling(std::move(registry)));
    return stirling;
  }

  /**
   * @brief Initializes Stirling, including bring-up of all the SourceConnectors.
   *
   * @return Status
   */
  Status Init();

  /**
   * @brief Populate the Publish Proto object. Agent calls this function to get the Publish
   * proto message. The proto publish message contains information (InfoClassSchema) on
   * all the Source Connectors that can be run to gather data and information on the types
   * for the data. The agent can then subscribe to a subset of the published message. The proto
   * is defined in //src/stirling/proto/collector_config.proto.
   *
   */
  void GetPublishProto(stirlingpb::Publish* publish_pb);

  /**
   * @brief Get the Subscription object. Receive a Subscribe proto message from the agent.
   * Update the schemas based on the subscription message. Generate the appropriate tables
   * that conform to subscription information.
   *
   * @param subscribe_proto
   * @return Status
   */
  Status SetSubscription(const stirlingpb::Subscribe& subscribe_proto);

  /**
   * @brief Register call-back from Agent. Used to periodically send data.
   *
   * Function signature is:
   *   uint64_t table_id
   *   std::unique_ptr<ColumnWrapperRecordBatch> data
   */
  void RegisterCallback(PushDataCallback f) { agent_callback_ = f; }

  // TODO(oazizi): Get rid of this eventually?
  /**
   * @brief Return a map of table ID to InfoClassManager names.
   */
  std::unordered_map<uint64_t, std::string> TableIDToNameMap() {
    std::unordered_map<uint64_t, std::string> map;

    for (auto& mgr : info_class_mgrs_) {
      map.insert({mgr->id(), mgr->name()});
    }

    return map;
  }

  /**
   * Main data collection call. This version will block, so make sure to wrap a thread around it.
   */
  void Run();

  /**
   * Main data collection call. This version spawns off as an independent thread.
   */
  Status RunAsThread();

  /**
   * Stop the running thread. Return will not be immediate.
   */
  void Stop();

  /**
   * Wait for the running thread to terminate. Assuming you ran RunThread().
   */
  void WaitForThreadJoin();

 protected:
  explicit Stirling(std::unique_ptr<SourceRegistry> registry)
      : run_enable_(false),
        config_(std::make_unique<PubSubManager>()),
        registry_(std::move(registry)) {}

 private:
  /**
   * Create data source connectors from the registered sources.
   */
  Status CreateSourceConnectors();

  /**
   * Adds a source to Stirling, and updates all state accordingly.
   */
  Status AddSourceFromRegistry(const std::string& name,
                               SourceRegistry::RegistryElement registry_element);

  /**
   * Main run implementation.
   */
  void RunCore();

  /**
   * Helper function to figure out how much to sleep between polling iterations.
   */
  void SleepUntilNextTick();

  /**
   * Main thread used to spawn off RunThread().
   */
  std::thread run_thread_;

  /**
   * Whether thread should be running.
   */
  std::atomic<bool> run_enable_;

  /**
   * Vector of all Source Connectors.
   */
  std::vector<std::unique_ptr<SourceConnector>> sources_;

  /**
   * Vector of all Data Tables.
   */
  std::vector<std::unique_ptr<DataTable>> tables_;

  /**
   * Vector of all the InfoClassManagers.
   */
  InfoClassManagerVec info_class_mgrs_;
  /**
   * Spin lock to lock updates to info_class_mgrs_.
   *
   */
  absl::base_internal::SpinLock info_class_mgrs_lock_;

  /**
   * Pointer the config unit that handles sub/pub with agent.
   */
  std::unique_ptr<PubSubManager> config_;

  /**
   * @brief Pointer to data source registry
   *
   */
  std::unique_ptr<SourceRegistry> registry_;

  const std::chrono::milliseconds kMinSleepDuration{1};

  /**
   * Function to call to push data to the agent.
   * Function signature is:
   *   uint64_t table_id
   *   std::unique_ptr<ColumnWrapperRecordBatch> data
   */
  PushDataCallback agent_callback_;
};

}  // namespace stirling
}  // namespace pl
