/******************************************************************************
 * Copyright 2019 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file
 **/

#pragma once

#include <utility>
#include <vector>

#include "modules/planning/tasks/optimizers/trajectory_optimizer.h"

#include "modules/canbus/proto/chassis.pb.h"
#include "modules/common/configs/vehicle_config_helper.h"
#include "modules/common/math/linear_interpolation.h"
#include "modules/common/status/status.h"
#include "modules/planning/common/trajectory/discretized_trajectory.h"

namespace apollo {
namespace planning {
class OpenSpaceTrajectoryPartition : public TrajectoryOptimizer {
 public:
  explicit OpenSpaceTrajectoryPartition(const TaskConfig& config);

  ~OpenSpaceTrajectoryPartition() = default;

  void Restart();

 private:
  common::Status Process() override;

  void InterpolateTrajectory(const DiscretizedTrajectory& trajectory,
                             DiscretizedTrajectory* interpolated_trajectory);

  void UpdateVehicleInfo();

  void PartitionTrajectory(DiscretizedTrajectory* interpolated_trajectory,
                           std::vector<TrajGearPair>* paritioned_trajectories);

  bool CheckReachTrajectoryEnd(const DiscretizedTrajectory& trajectory,
                               const canbus::Chassis::GearPosition& gear,
                               const size_t trajectories_size,
                               const size_t trajectories_index,
                               size_t* current_trajectory_index,
                               size_t* current_trajectory_point_index);

  bool UseFailSafeSearch(
      const std::vector<TrajGearPair>& paritioned_trajectories,
      size_t* current_trajectory_index, size_t* current_trajectory_point_index);

  bool InsertGearShiftTrajectory(
      const bool flag_change_to_next, const size_t current_trajectory_index,
      const std::vector<TrajGearPair>& paritioned_trajectories,
      TrajGearPair* gear_switch_idle_time_trajectory);

  void GenerateGearShiftTrajectory(
      const canbus::Chassis::GearPosition& gear_position,
      TrajGearPair* gear_switch_idle_time_trajectory);

  void AdjustRelativeTimeAndS(
      const std::vector<TrajGearPair>& paritioned_trajectories,
      const size_t current_trajectory_index,
      const size_t closest_trajectory_point_index,
      DiscretizedTrajectory* stitched_trajectory_result,
      TrajGearPair* current_paritioned_trajectory);

 private:
  OpenSpaceTrajectoryPartitionConfig open_space_trajectory_partition_config_;
  double distance_search_range_ = 0.0;
  double distance_to_midpoint_ = 0.0;
  double heading_search_range_ = 0.0;
  double heading_track_range_ = 0.0;

  common::VehicleParam vehicle_param_;
  double ego_length_ = 0.0;
  double ego_width_ = 0.0;
  double shift_distance_ = 0.0;
  double ego_theta_ = 0.0;
  double ego_x_ = 0.0;
  double ego_y_ = 0.0;
  common::math::Box2d ego_box_;
  double vehicle_moving_direction_ = 0.0;

  struct pair_comp_ {
    bool operator()(
        const std::pair<std::pair<size_t, size_t>, double>& left,
        const std::pair<std::pair<size_t, size_t>, double>& right) const {
      return left.second <= right.second;
    }
  };
  struct comp_ {
    bool operator()(const std::pair<size_t, double>& left,
                    const std::pair<size_t, double>& right) {
      return left.second <= right.second;
    }
  };
};
}  // namespace planning
}  // namespace apollo
