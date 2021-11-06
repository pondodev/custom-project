#include "entity_engine.h"

EntityEngine::EntityEngine() {
    for ( size_t i = 0; i < MAX_ENTITIES; i++ ) {
        available_ids.push( i );
    }
}

EntityEngine::~EntityEngine() {
    // TODO: destroy all the things !!
}

std::optional<Entity> EntityEngine::register_entity() {
    std::optional<Entity> to_return;

    if ( available_ids.size() > 0 ) {
        to_return = available_ids.front();
        available_ids.pop();
    }

    // clean the data up from anything that might have been there previously
    if ( to_return.has_value() ) {
        auto id = to_return.value();
        movement_components[ id ] = MovementComponent { 0.0f, 0.0f, 0.0f };
        enemy_type_components[ id ] = EnemyTypeComponent { EnemyType::YeeHaw };
        distance_components[ id ] = DistanceComponent { 0.0f };
    }

    return to_return;
}

void EntityEngine::unregister_entity( Entity id ) {
    available_ids.push( id );
}

MovementComponent* EntityEngine::get_movement_component( Entity id ) {
    return &movement_components[ id ];
}

EnemyTypeComponent* EntityEngine::get_enemy_type_component( Entity id ) {
    return &enemy_type_components[ id ];
}

DistanceComponent* EntityEngine::get_distance_component( Entity id ) {
    return &distance_components[ id ];
}
