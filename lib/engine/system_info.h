#pragma once

#include <string>

#include "lib/base/functional/ufunction.h"
#include "lib/engine/update_pipeline.h"

namespace sge {
struct SystemInfo {
  /**
   * \brief The name of this system.
   */
  std::string name;

  /**
   * \brief Actual system function to run.
   */
  UFunction<UpdatePipeline::SystemFn> system_fn;
};
}  // namespace sge
