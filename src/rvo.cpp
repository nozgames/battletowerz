//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

// Simple collision avoidance using repulsion forces

#include "rvo.h"


Vec2 ComputeRVOVelocity(
    const RVOAgent& agent,
    const RVOAgent* obstacles,
    int obstacle_count,
    float time_horizon
) {
    (void)time_horizon; // Unused in this simplified version

    // Start with preferred velocity
    Vec2 result = agent.preferred_velocity;

    // Clamp to max speed
    float speed_sq = LengthSqr(result);
    if (speed_sq > Sqr(agent.max_speed)) {
        result = result * (agent.max_speed / sqrtf(speed_sq));
    }

    if (obstacle_count == 0) {
        return result;
    }

    // Add repulsion forces from nearby obstacles
    Vec2 repulsion = VEC2_ZERO;

    for (int i = 0; i < obstacle_count; i++) {
        const RVOAgent& obstacle = obstacles[i];

        Vec2 to_obstacle = obstacle.position - agent.position;
        float dist_sq = LengthSqr(to_obstacle);

        // Combined radius with extra margin
        float combined_radius = agent.radius + obstacle.radius + 0.1f;
        float threshold_sq = Sqr(combined_radius);

        // Only avoid if too close
        if (dist_sq < threshold_sq && dist_sq > 0.01f) {
            float dist = sqrtf(dist_sq);
            Vec2 direction = to_obstacle / dist;

            // Repulsion strength increases as distance decreases
            float strength = (combined_radius - dist) / combined_radius;
            strength = strength * strength * strength; // Cubic for even more aggressive close-range avoidance

            // Push away from obstacle with stronger force
            repulsion -= direction * strength * agent.max_speed * 4.0f;
        }
    }

    // Combine preferred velocity with repulsion
    result = result + repulsion;

    // If repulsion is significant but result is too slow, boost it to minimum speed
    float repulsion_strength = Length(repulsion);
    if (repulsion_strength > 0.1f) {
        float result_speed = Length(result);
        float min_speed = agent.max_speed * 0.7f; // Minimum 70% of max speed when avoiding

        if (result_speed < min_speed && result_speed > 0.01f) {
            result = result * (min_speed / result_speed);
        }
    }

    // Clamp final result to max speed
    speed_sq = LengthSqr(result);
    if (speed_sq > Sqr(agent.max_speed)) {
        result = result * (agent.max_speed / sqrtf(speed_sq));
    }

    return result;
}
