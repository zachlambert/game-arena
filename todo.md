# Objective 1

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
- Have enemies only get hit when you click on them.

# Objective 3

Basic animation and fog of war.

- Basic walk cycle animation (two frames), when not holding gun
- Add fog of war, where terrain out of view is darker and entities are not visible (can avoid drawing if completely in fog of war. If partially, draw, then overwrite the obscured part with the polygon).

# Objective 4

Gun logic and particles.

- Have the gun ray narrow if you hold it still and don't move.
- Add a particle effect when an enemy is hit.
- Have a random hit change with gun, which increases with a narrower gun ray and closer distance. (visually: width of ray at target)
- Give entities health and reduce this health on hit. Remove when depleted
- Block the bullet if it hits a wall.
- Add a different particle effect for when a wall is hit.

# Objective 5



# Objective 6


