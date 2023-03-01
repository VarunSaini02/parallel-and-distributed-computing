#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "spinlock.h"
#include "rc.h"

#define DEFAULT_SLEEP_USECS 10

/***
 * @brief This function is a helper function for dealing with NULL arguments.
 *
 * This function is a helper function for dealing with NULL arguments. It prints
 * out a detailed error message and returns InvalidArgument.
 *
 * @param func The function in which the error occurred.
 * @param var The variable that was incorrectly passed in as NULL.
 *
 * @returns InvalidArgument
 *
 ***/
rc_t invalid_null_argument_error(const char func[], const char var[]) {
    fprintf(stderr, "%s: %s cannot be NULL\n", func, var);
    return InvalidArgument;
}

/***
 * @brief This function initializes a given spinlock_attrs_t struct.
 *
 * This function initializes a given spinlock_attrs_t struct by setting the
 * struct's sleep_usecs field to DEFAULT_SLEEP_USECS.
 *
 * @param attrs A pointer to the spinlock_attrs_t struct to initialize.
 *
 * @returns Success on success and InvalidArgument if attrs is NULL.
 *
 ***/
rc_t spinlock_attr_init(spinlock_attrs_t* attrs) {
    if (attrs == NULL) {
        return invalid_null_argument_error("spinlock_attr_init", "attrs");
    }
    attrs->sleep_usecs = DEFAULT_SLEEP_USECS;
    return Success;
}

/***
 * @brief This function initializes a given spinlock_t struct.
 *
 * This function initializes a given spinlock_t struct. A spinlock_obj_t struct
 * is initialized using the given spinlock_attrs_t struct, and this is 
 * attached to the spinlock_t struct to complete initialization.
 *
 * @param handle A pointer to the spinlock_t struct to initialize.
 * @param obj A pointer to the spinlock_obj_t struct to initialize and attach.
 * @param attrs A pointer to the spinlock_attrs_t struct that is used to initialize obj.
 *
 * @returns Success on success and InvalidArgument if any parameters are NULL.
 *
 ***/
rc_t spinlock_init(spinlock_t* handle, spinlock_obj_t* obj, spinlock_attrs_t* attrs) {
    spinlock_attrs_t default_attrs;
    if (handle == NULL) {
        return invalid_null_argument_error("spinlock_init", "handle");
    }
    if (obj == NULL) {
        return invalid_null_argument_error("spinlock_init", "obj");
    }
    if (attrs == NULL) {
        attrs = &default_attrs;
        rc_t rc = spinlock_attr_init(attrs);
        if (rc != Success) {
            return rc; 
        }
    }

    atomic_flag_clear(&(obj->lock));
    obj->sleep_usecs = attrs->sleep_usecs;
    handle->obj = obj;
    return Success;
}

/***
 * @brief This function creates and initializes a given spinlock_t struct.
 *
 * This function allocates memory on the heap for a spinlock_obj_t struct that is
 * then passed to spinlock_init to initialize a given spinlock_t struct. After calling 
 * this function, spinlock_destroy must be called to free this struct.
 *
 * @param handle A pointer to the spinlock_t struct to initialize.
 * @param attrs A pointer to the spinlock_attrs_t struct that is used to initialize obj
 * in spinlock_init.
 *
 * @returns Success on success and InvalidArgument if any parameters are NULL 
 * (by calling spinlock_init).
 *
 ***/
rc_t spinlock_create(spinlock_t* handle, spinlock_attrs_t* attrs) {
    spinlock_obj_t* obj = malloc(sizeof(spinlock_obj_t));
    if (obj == NULL) {
        fprintf(stderr, "spinlock_create: out of memory for obj\n");
    }
    
    rc_t rc = spinlock_init(handle, obj, attrs);
    if (rc != Success) {
        free(obj);
    }
    return rc;
}

/***
 * @brief This function acquires the given spinlock_t struct's lock.
 *
 * This function acquires the given spinlock_t struct's lock by using the atomic method
 * atomic_flag_test_and_set, sleeping between attempts.
 *
 * @param handle A pointer to the spinlock_t struct.
 *
 * @returns Success on success and InvalidArgument if either handle or handle->obj are NULL.
 *
 ***/
rc_t spinlock_acquire(spinlock_t* handle) {
    if (handle == NULL) {
        return invalid_null_argument_error("spinlock_acquire", "handle");
    }
    if (handle->obj == NULL) {
        return invalid_null_argument_error("spinlock_acquire", "handle->obj");
    }

    while (atomic_flag_test_and_set(&(handle->obj->lock))) {
        usleep(handle->obj->sleep_usecs);
    }
    return Success;
}

/***
 * @brief This function releases the given spinlock_t struct's lock.
 *
 * This function releases the given spinlock_t struct's lock by using the atomic method
 * atomic_flag_clear.
 *
 * @param handle A pointer to the spinlock_t struct.
 *
 * @returns Success on success and InvalidArgument if either handle or handle->obj are NULL.
 *
 ***/
rc_t spinlock_release(spinlock_t* handle) {
    if (handle == NULL) {
        return invalid_null_argument_error("spinlock_release", "handle");
    }
    if (handle->obj == NULL) {
        return invalid_null_argument_error("spinlock_release", "handle->obj");
    }

    atomic_flag_clear(&(handle->obj->lock));
    return Success;
}

/***
 * @brief This function destroys a given spinlock_t struct.
 *
 * This function destroys the given spinlock_t struct by freeing its obj field.
 * This is meant to be paired with spinlock_create.
 *
 * @param handle A pointer to the spinlock_t struct.
 *
 * @returns Success on success and InvalidArgument if either handle or handle->obj are NULL.
 *
 ***/
rc_t spinlock_destroy(spinlock_t* handle) {
    if (handle == NULL) {
        return invalid_null_argument_error("spinlock_destroy", "handle");
    }
    if (handle->obj == NULL) {
        return invalid_null_argument_error("spinlock_destroy", "handle->obj");
    }

    free(handle->obj);
    return Success;
}
