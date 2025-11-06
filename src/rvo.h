//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#pragma once

// RVO (Reciprocal Velocity Obstacles) Algorithm
// Provides collision-free velocity computation for multiple agents
// Each agent cooperatively avoids others by selecting velocities that won't cause future collisions

struct RVOAgent {
    Vec3 position;
    Vec3 velocity;
    Vec3 preferred_velocity;  // Where the agent wants to go
    float radius;
    float max_speed;
};

// Computes a collision-free velocity for an agent given nearby obstacles (other agents)
// Returns a velocity that avoids collisions while staying close to preferred_velocity
Vec3 ComputeRVOVelocity(
    const RVOAgent& agent,
    const RVOAgent* obstacles,
    int obstacle_count,
    float time_horizon = 2.0f  // How far ahead to predict collisions (in seconds)
);
