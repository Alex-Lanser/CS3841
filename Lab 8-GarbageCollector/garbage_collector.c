#include "garbage_collector.h"
#include <memory.h>
#include <stdlib.h>


void register_root_object(object_db_t *object_db, void *objptr, char *struct_name, unsigned int units)
{
    struct_db_rec_t *struct_rec = struct_db_look_up(object_db->struct_db, struct_name);
    assert(struct_rec);

    add_object_to_object_db(object_db, objptr, units, struct_rec, MLD_TRUE);
}

void set_object_as_global_root(object_db_t *object_db, void *obj_ptr)
{
    object_db_rec_t *obj_rec = object_db_look_up(object_db, obj_ptr);
    assert(obj_rec);

    obj_rec->is_root = MLD_TRUE;
}