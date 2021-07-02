# TODO

# Objective 4

Animation.

- Create animation component and system which changes sprite id.
- Basic walk cycle animation (two frames), when not holding gun
- Add the E key to swap between gun out and gun away, with an animation between them.
- Only allow shooting, and only display gun ray, when gun is out.

# Objective 4

Gun logic.

- Have enemies only get hit when you click on them (using the mesh, instead of just distance away), and there is a collision-free line of sight.
- Have the gun ray narrow if you hold it still and don't move.
- Have a random hit change with gun, which increases with a narrower gun ray and closer distance. (visually: width of ray at target)
- Give entities health and reduce this health on hit. Remove when depleted

# Objective 5

Particles.

- Create a particle renderer.
- Add a particle effect when an enemy is hit.
- Add a different particle effect for when a wall is hit.

# Objective 6

Basic enemy logic and basic level.

- Give the enemies guns and have them aim and shoot at you.
- Have the enemies walk in a straight line towards you.
- Create a simple level for testing, with walls and enemy spawners.

# Objective 7

Level 1 AI.

- Enemies have a set of waypoints they follow, which by default, they navigate between.
- Give the enemies lines of sight. If they spot the player, they set their target to some distance away from  the player (a good spot to shoot) and navigate towards this.
- Whenever they can see the player, they shoot towards the player at some rate.
- When the player hasn't been spotted for a certain period of time (eg: 10 seconds), the enemy resumes default behaviour.

# Objective 8

Level 2 AI.

- Enemies hear gunshots and navigate towards gunshots when in earshot.
- After losing the player, will continue to explore randomly for a bit.

# COMPLETED

## Objective 1

Basic movement control and rendering

- Walk around
- Focus the camera on the player
- Point towards the camera
- Draw a ray for the direction of the gun.
- On click, produce a bullet ray which decays then disappears
- If you click on an enemy (just check distance for now), then enemy
  disappears.
- Enemies randomly spawn at the origin and move around randomly.

# Objective 2

Walls and collisions

- Add in static walls, defined by polygons
- Add a collision system which holds collision polygon for entities.
- Handle wall collisions such that entities slide along the edge instead
  of stopping completely.
- Also: Add a collision renderer for debugging, which renders collision polygons and the
  terrain collision octree.

# Objective 3

Add occlusion polygon
