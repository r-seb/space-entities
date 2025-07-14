#include "game/ecs.h"
#include <stdio.h>
#include <string.h>

static inline uint8_t entity_get_dense_idx(ecs_entity_t entity)
{
    return entity >> ENTITY_BITSHIFT;
}

static inline uint8_t entity_get_id(ecs_entity_t entity)
{
    return entity >> ENTITY_BITSHIFT;
}

static inline uint8_t entity_get_tag(ecs_entity_t entity)
{
    return entity & ENTITY_BITMASK;
}

static inline uint8_t entity_get_version(ecs_entity_t entity)
{
    return entity & ENTITY_BITMASK;
}

bool ecs_is_entity_alive(ecs_world_t* ecs, ecs_entity_t entity)
{
    uint8_t id = entity_get_id(entity);
    uint8_t version = entity_get_version(entity);
    // Compare with our cached index of entities
    return entity_get_version(ecs->entities.sparse[id]) == version;
}

ecs_entity_t ecs_get_entity_version(ecs_entity_t entity)
{
    return entity_get_version(entity);
}

ecs_entity_t ecs_get_entity_tag(ecs_world_t* ecs, ecs_entity_t entity)
{
    if (ecs->entities.count > 0 && ecs_is_entity_alive(ecs, entity)) {
        uint8_t dense_idx = entity_get_dense_idx(ecs->entities.sparse[entity_get_id(entity)]);
        return entity_get_tag(ecs->entities.dense[dense_idx]);
    }

    return UINT16_MAX;
}

ecs_entity_t ecs_get_entity_id(ecs_entity_t entity)
{
    return entity_get_id(entity);
}

// https://bitsquid.blogspot.com/2014/08/building-data-oriented-entity-system.html
// https://ajmmertens.medium.com/doing-a-lot-with-a-little-ecs-identifiers-25a72bd2647
ecs_entity_t ecs_create_entity(ecs_world_t* ecs, uint8_t tag)
{
    // find or create entity
    uint8_t dense_idx = ecs->entities.count;
    uint8_t id = dense_idx; // if recycle_entity_count == 0

    if (ecs->recycle_entity_count > 0) {
        id = ecs->next_entity_id;
        // Fetch next id to recycle
        ecs->next_entity_id =
            (--ecs->recycle_entity_count) ? entity_get_id(ecs->entities.sparse[id]) : 0;
    }

    ecs->entities.dense[dense_idx] = (id << ENTITY_BITSHIFT) | tag;
    ++(ecs->entities.count);

    uint8_t version = entity_get_version(ecs->entities.sparse[id]);
    ecs->entities.sparse[id] = (dense_idx << ENTITY_BITSHIFT) | version;

    return (id << ENTITY_BITSHIFT) | version;
}

// https://skypjack.github.io/2019-05-06-ecs-baf-part-3/#the-entity-and-the-version
bool ecs_delete_entity(ecs_world_t* ecs, ecs_entity_t entity)
{
    uint8_t id = entity_get_id(entity);
    uint8_t dense_idx = entity_get_dense_idx(ecs->entities.sparse[id]);

    if ((ecs->entities.count == 0) || (entity_get_id(ecs->entities.dense[dense_idx]) != id)) {
        return false;
    }

    // Delete all associated components
    for (int comp_id = 0; comp_id < COMP_ID_MAX; ++comp_id) {
        // Cast to suppress lsp warning
        ecs_remove_component(ecs, entity, (ecs_component_id_t)comp_id);
    }

    uint8_t version = entity_get_version(ecs->entities.sparse[id]);

    ecs->recycle_entity_count++;
    uint8_t last_dense_idx = --ecs->entities.count;
    uint8_t last_id = entity_get_id(ecs->entities.dense[last_dense_idx]);
    uint8_t last_version = entity_get_version(ecs->entities.sparse[last_id]);

    /* Store last element at the deleted index
     *                ___________
     *               |          |
     *              \/          |
     * [0][1][2][3][4][5][6][7][8] // before
     * ---------------------------
     * [0][1][2][3][8][5][6][7][X] // after
     */
    ecs->entities.dense[dense_idx] = ecs->entities.dense[last_dense_idx];
    /* Update dense_idx based on last_element in dense
     *               _____________
     *              |            |
     *              |           \/
     * [0][1][2][3][4][5][6][7][8] // before
     * ---------------------------
     * [0][1][2][3][X][5][6][7][4] // after
     */
    ecs->entities.sparse[last_id] = dense_idx << ENTITY_BITSHIFT | last_version;

    /* Cache the previous deleted EntityID
     * As consecutive deletion happens without recycling, this will build a simple linked list that
     * points the next EntityID to recycle by re-using the EntityID as an index for storage:
     *
     * EXAMPLE:
     * next_entity_id = 0;
     * -- Delete[4], 1st delete
     * [0][1][2][3][0][5][6][7][8] // previous next_entity_id gets stored at Index 4
     *             /\
     *             |_____
     *                  |
     * next_entity_id = 4
     * -- Delete[2], 2nd delete
     *          _______
     *         |      |
     *         |     \/
     * [0][1][4*][3][0][5][6][7][8] // previous next_entity_id gets stored at Index 2
     *        /\
     *        |__________
     *                  |
     * next_entity_id = 2
     * -- Delete[6], 3rd delete
     *          _____________
     *         |            |
     *        \/            |
     * [0][1][4*][3][0][5][2*][7][8] // previous next_entity_id gets stored at Index 6
     *        |     /\     /\
     *        |_____|    __|
     *                  |
     * next_entity_id = 6
     *
     * 3 deletions, so we'll have 6, 2 and 4 to recycle. This is stored in the sparse array
     */
    ecs->entities.sparse[id] = ecs->next_entity_id << ENTITY_BITSHIFT;
    // Update current EntityID to recycle
    ecs->next_entity_id = id;

    /* Increment version
     * To keep track of whether an entity is alive/dead given that the entityIDs are recycled, a
     * version/generation is used as an indication.
     *
     * EXAMPLE:
     * [0-v0][1-v0][4-v1][3-v0][0-v1][5-v0][2-v1][7-v0][4-v0] // Stored in sparse
     *
     * So if the version of a previously stored entity differs from the current one, it means the
     * entity has been deleted/recycled.
     */
    ecs->entities.sparse[id] &= ~(ENTITY_BITMASK); // Clear the version bits
    ecs->entities.sparse[id] |= ++version;

    return true;
}

void* ecs_get_component_from_idx(ecs_world_t* ecs, ecs_component_id_t comp_id, uint16_t idx)
{
    if (ecs->components[comp_id].set.count > 0) {
        return (uint8_t*)ecs->components[comp_id].data + idx * ecs->components[comp_id].data_size;
    }

    return NULL;
}

void* ecs_get_component_from_entity(ecs_world_t* ecs, ecs_component_id_t comp_id,
                                    ecs_entity_t entity)
{
    if (ecs->components[comp_id].set.count > 0 && ecs_is_entity_alive(ecs, entity)) {
        uint8_t idx = ecs->components[comp_id].set.sparse[entity_get_id(entity)];
        return (uint8_t*)ecs->components[comp_id].data + idx * ecs->components[comp_id].data_size;
    }

    return NULL;
}

// TODO: Experiment with variadic function
void ecs_register_component(ecs_world_t* ecs, ecs_component_id_t comp_id, size_t component_size)
{
    // ecs->components[comp_id].data = malloc(MAX_ENTITIES * component_size);
    ecs->components[comp_id].data_size = component_size;
}

bool ecs_add_component(ecs_world_t* ecs, ecs_entity_t entity, ecs_component_id_t comp_id,
                       void* component)
{
    if (ecs->components[comp_id].set.count > MAX_ENTITIES || !ecs_is_entity_alive(ecs, entity)) {
        return false;
    }

    /* An example of what this function is trying to build for each component:
     *
     *                    sparse_array
     *    dense_array        | X |          data_array
     *       | e1 |<-------->| 0 |--------->| value1 |
     *       | e3 |<----\_   | X |   _/---->| value2 |
     *       | e4 |<---\_ \->| 1 |--/ _/--->| value3 |
     *                   \-->| 2 |---/
     */

    uint8_t id = entity_get_id(entity);
    uint8_t data_idx = ecs->components[comp_id].set.count;
    uint8_t data_size = ecs->components[comp_id].data_size;

    ecs->components[comp_id].set.sparse[id] = data_idx;
    ecs->components[comp_id].set.dense[data_idx] = entity;

    void* data_position = (uint8_t*)ecs->components[comp_id].data + (data_idx * data_size);
    memcpy(data_position, component, data_size);

    ++(ecs->components[comp_id].set.count);

    return true;
}

bool ecs_remove_component(ecs_world_t* ecs, ecs_entity_t entity, ecs_component_id_t comp_id)
{
    if (ecs->components[comp_id].set.count == 0 || !ecs_is_entity_alive(ecs, entity)) {
        return false;
    }

    uint8_t last_data_idx = --(ecs->components[comp_id].set.count);
    uint8_t last_id = entity_get_id(ecs->entities.dense[last_data_idx]);
    uint8_t id = entity_get_id(entity);
    uint8_t data_idx = ecs->components[comp_id].set.sparse[id];
    uint8_t data_size = ecs->components[comp_id].data_size;

    /* Store last element at the deleted index
     *                ___________
     *               |          |
     *              \/          |
     * [0][1][2][3][4][5][6][7][8] // before
     * ---------------------------
     * [0][1][2][3][8][5][6][7][X] // after
     */
    ecs->components[comp_id].set.dense[data_idx] =
        ecs->components[comp_id].set.dense[last_data_idx];
    /* Update data_idx based on last_element in dense
     *               _____________
     *              |            |
     *              |           \/
     * [0][1][2][3][4][5][6][7][8] // before
     * ---------------------------
     * [0][1][2][3][X][5][6][7][4] // after
     */
    ecs->components[comp_id].set.sparse[last_id] = data_idx;

    void* data_position = (uint8_t*)ecs->components[comp_id].data + (data_idx * data_size);
    void* last_component = (uint8_t*)ecs->components[comp_id].data + (last_data_idx * data_size);
    memcpy(data_position, last_component, data_size);

    return true;
}
