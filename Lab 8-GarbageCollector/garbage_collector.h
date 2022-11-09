#ifndef garbage_collector
#include <assert.h>

#define MAX_STRUCTURE_NAME_SIZE 128
#define MAX_FIELD_NAME_SIZE 128

typedef enum
{
    UINT8,
    UINT32,
    INT32,
    CHAR,
    OBJ_PTR,
    FLOAT,
    DOUBLE,
    OBJ_STRUCT
} data_type_t;

#define OFFSETOF(structname, fieldname) \
    (unsigned int)&(((structname *)0)->fieldname);

#define FIELD_SIZE(structname, fieldname) \
    sizeof(((structname *)0)->fieldname);

typedef struct struct_db_rec_t struct_db_rec_t;

typedef struct field_info
{
    char fieldname[MAX_FIELD_NAME_SIZE];
    data_type_t datatype;
    unsigned int size;
    unsigned int offset;
    char nested_string_name[MAX_STRUCTURE_NAME_SIZE];
} field_info_t;

struct struct_db_rec_t
{
    struct_db_rec_t *next;
    char structname[MAX_STRUCTURE_NAME_SIZE];
    unsigned int struct_size;
    unsigned int nfields;
};

typedef struct struct_db
{
    struct_db_rec_t *head;
    unsigned int count; // Number of recorded structures in the linked list
} struct_db_t;

// Return 0 on success, -1 on failure
int add_structure_to_struct_db(struct_db_t *struct_db, struct_db_rec_t *struct _rec);

// Struct registration helping APIs
#define FIELD_INFO(structname, fieldname, datatype, nested_struct_name) \
    {                                                                   \
#fieldname, datatype, FIELD_SIZE(structname, fieldname),                \  
        OFFSETOF(structname, fieldname),                                \
            #nested_struct_name                                         \
    }

#define REG_STRUCT(struct_db, st_name, fields_arr)                      \
    do                                                                  \
    {                                                                   \
        struct_db_rec_t *rec = calloc(1, sizeof(struct_db_rec_t));      \
        strncpy(rec->structname, #st_name, MAX_STRUCTURE_NAME_SIZE);    \
        rec->ds_size = sizeof(st_name);                                 \
        rec->fields = fields_arr;                                       \
        if (add_structure_to_struct_db(struct_db, rec))                 \
        {                                                               \
            assert(0);                                                  \
        }                                                               \
    } while (0);

typedef struct object_db_rec object_db_rec_t;

struct object_db_rec
{
    object_db_rec_t *next;
    void *ptr;
    unsigned int units;
    struct_db_rec_t *struct_rec;
};

typedef struct object_db
{
    struct_db_t *struct_db;
    object_db_rec_t *head;
    unsigned int count;
} object_db_t;

static void add_object_to_object_db(object_db_t *object_db, void *ptr, int units, struct_db_rec_t *struct_rec)
{
    object_db_rec_t *object_rec = object_db_look_up(object_db, ptr);
    assert(!object_rec); // Check to make sure the object being added to the database has not already been added

    object_rec = calloc(1, sizeof(object_db_rec_t));

    object_rec->next = NULL;
    object_rec->ptr = ptr;
    object_rec->units = units;
    object_rec->struct_rec = struct_rec;

    object_db_rec_t *head = object_db->head;

    if (!head)
    {
        object_db->head = object_rec;
        object_rec->next = NULL;
        object_db->count++;
        return;
    }

    object_rec->next = head;
    object_db->head = NULL;
    object_db->count++;
}
void *gc_malloc(object_db_t *object_db, char *structname, int units)
{
    struct_db_rec_t *struct_rec = struct_db_look_up(object_db->struct_db, stuctname);
    assert(struct_rec);
    void *ptr = calloc(units, struct_rec->ds_size);
    add_object_to_object_db(object_db, ptr, units, struct_rec);
    return ptr;
}
#endif garbage_collector