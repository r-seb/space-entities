#pragma once

/*
 * refs:
 * https://github.com/lonelyfloat/ssecs/
 * https://github.com/richard-egeli/sps
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_ENTITIES 30U

// Entity layout (16 bits):
// Bits 15-8: ID            (8 bits)
// Bits 7-0: version or tag (8 bits)
#define ENTITY_BITMASK 0xFFU
#define ENTITY_BITSHIFT 8
typedef uint16_t ecs_entity_t;

// TODO: Experiment with C11 _Generic
#define ECS_GET_COMP_FROM_IDX(ecs, comp_id, idx, comp_type)                                        \
    ((comp_type*)ecs_get_component_from_idx(ecs, comp_id, idx))

#define ECS_GET_COMP_FROM_ENT(ecs, comp_id, idx, comp_type)                                        \
    ((comp_type*)ecs_get_component_from_entity(ecs, comp_id, idx))

typedef enum {
    // Add Component IDs here
    POSITION_COMP_ID,
    VELOCITY_COMP_ID,
    HEALTH_COMP_ID,
    SPRITE_COMP_ID,
    STATE_COMP_ID,

    COMP_ID_MAX
} ecs_component_id_t;

typedef enum {
    // Add Tags here
    PLAYER_TAG = (1U << 0),
    BULLET_TAG = (1U << 1),
    ENEMY_TAG = (1U << 2),
    OBSTACLE_TAG = (1U << 3),
    KEEP_BOUNDED_TAG = (1U << 4),
    COLLIDED_TAG = (1U << 5),
    INVINCIBLE_TAG = (1U << 6),

    DEAD_TAG = (1U << 7) // Max value
} ecs_entity_tag_t;

typedef struct {
    ecs_entity_t* sparse;
    ecs_entity_t* dense;
    uint8_t count;
} ecs_sparse_t;

// ref: https://ajmmertens.medium.com/building-an-ecs-storage-in-pictures-642b8bfd6e04#b7b0
typedef struct {
    ecs_sparse_t set;  // sparse: Stores the index to an element in the data array
                       //    ex. assignment: sparse[entity_id] = data_count
                       // dense: Stores the entity_id|version in a packed array
                       //    ex. assignment: dense[data_count] = EntityID
                       // count: Number of elements (aka Entities) in the Component data
    void* data;        // Stores Component data in a packed array
                       //    ex. assignment: data[data_count] = ComponentValue
    uint8_t data_size; // Size of Component data
} ecs_component_t;

typedef struct {
    ecs_component_t* components;
    ecs_sparse_t entities; // sparse: Stores the dense_idx|version or next_entity_id|version
                           // dense: Stores the entity_id|tag in a packed array
                           // count: Number of alive Entities
    uint8_t next_entity_id;
    uint8_t recycle_entity_count;
} ecs_world_t;

bool ecs_is_entity_alive(ecs_world_t* ecs, ecs_entity_t entity);
ecs_entity_t ecs_get_entity_version(ecs_entity_t entity);
ecs_entity_t ecs_get_entity_tag(ecs_world_t* ecs, ecs_entity_t entity);
ecs_entity_t ecs_get_entity_id(ecs_entity_t entity);
ecs_entity_t ecs_create_entity(ecs_world_t* ecs, uint8_t tag);
bool ecs_delete_entity(ecs_world_t* ecs, ecs_entity_t entity);
void ecs_register_component(ecs_world_t* ecs, ecs_component_id_t comp_id, size_t component_size);
void* ecs_get_component_from_idx(ecs_world_t* ecs, ecs_component_id_t comp_id, uint16_t idx);
void* ecs_get_component_from_entity(ecs_world_t*, ecs_component_id_t comp_id, ecs_entity_t entity);
bool ecs_add_component(ecs_world_t* ecs, ecs_entity_t entity, ecs_component_id_t comp_id,
                       void* component);
bool ecs_remove_component(ecs_world_t* ecs, ecs_entity_t entity, ecs_component_id_t comp_id);
