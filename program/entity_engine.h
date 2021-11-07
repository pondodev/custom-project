#ifndef ENTITY_ENGINE_H
#define ENTITY_ENGINE_H

#include <queue>
#include <cstdint>
#include <optional>

#include "components.h"

#define MAX_ENTITIES 0xFF
typedef uint8_t Entity;

class EntityEngine {
public:
    EntityEngine();
    ~EntityEngine();

    std::optional<Entity> register_entity();
    void unregister_entity( const Entity id );

    MovementComponent* get_movement_component( const Entity id ) const;
    EnemyTypeComponent* get_enemy_type_component( const Entity id ) const;
    DistanceComponent* get_distance_component( const Entity id ) const;

private:
    std::queue<Entity> available_ids;
    MovementComponent* movement_components;
    EnemyTypeComponent* enemy_type_components;
    DistanceComponent* distance_components;
};

#endif
