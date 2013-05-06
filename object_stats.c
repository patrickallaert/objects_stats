#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_object_stats.h"

static PHP_FUNCTION(get_objects_count);

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_objects_count, 0, 0, 0)
    ZEND_ARG_INFO(0, criteria)
ZEND_END_ARG_INFO()

zend_function_entry object_stats_functions[] =
{
    ZEND_FE(get_objects_count, arginfo_get_objects_count)
    {NULL,NULL,NULL} /* Marks the end of function entries */
};

/* {{{ object_stats_module_entry
 */
zend_module_entry object_stats_module_entry = {
    STANDARD_MODULE_HEADER,
    "object_stats",
    object_stats_functions, /* Function entries */
    NULL, /* Module init */
    NULL, /* Module shutdown */
    NULL, /* Request init */
    NULL, /* Request shutdown */
    NULL, /* Module information */
    "0.1", /* Replace with version number for your extension */
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_OBJECT_STATS
ZEND_GET_MODULE(object_stats)
#endif

/* {{{ _zend_hash_index_search */
static inline zend_ulong _zend_hash_index_search(HashTable* ht, zval *pzval, zend_ulong *index TSRMLS_DC) {
    HashPosition position;
    zval **search = NULL;

    /* nothing to do */
    if (Z_TYPE_P(pzval) == IS_NULL)
        return FAILURE;

    for (zend_hash_internal_pointer_reset_ex(ht, &position);
         zend_hash_get_current_data_ex(ht, (void**) &search, &position) == SUCCESS;
         zend_hash_move_forward_ex(ht, &position)) {

         switch (Z_TYPE_P(pzval)) {
            /* perform string comparison */
            case IS_STRING:
                if (strncmp(Z_STRVAL_PP(search), Z_STRVAL_P(pzval), Z_STRLEN_P(pzval)) == SUCCESS) {
                    char *ks;
                    zend_uint kl;

                    if (zend_hash_get_current_key_ex(ht, &ks, &kl, index, 0, &position) == HASH_KEY_IS_LONG) {
                        return SUCCESS;
                    }
                 }
                 break;
         }
    }

    return FAILURE;
} /* }}} */

/* {{{ proto mixed get_objects_count([mixed criteria])
    where no criteria is provided will return the number of all objects in the object store as an associative array of class => count
    where a class name is provided, returns the number of objects of the specified class in the object store
    where an object is provided, returns the number of objects of the specified objects class in the object store
    where an array is provided, the array is treated as an inclusive indexed array of class names, returns an associative array of class => count */
PHP_FUNCTION(get_objects_count)
{
    zend_class_entry *pce = NULL;
    zend_class_entry **ppce;
    zval *pzarg = NULL;
    HashTable *class_list = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &pzarg) != SUCCESS)
        return;

    if (pzarg) {
        switch (Z_TYPE_P(pzarg)) {
            /* take a string argument representing a class name */
            case IS_STRING:
                if (zend_lookup_class(Z_STRVAL_P(pzarg), Z_STRLEN_P(pzarg), &ppce TSRMLS_CC) != SUCCESS) {
                    zend_error(
                        E_WARNING, "%s() could not find the class requested (%s)",
                        get_active_function_name(TSRMLS_C), Z_STRVAL_P(pzarg));
                    return;
                }
                pce = *ppce;
                break;

            /* use the class of this object directly */
            case IS_OBJECT:
                pce = Z_OBJCE_P(pzarg);
                break;

            /* use an array as indexed class names */
            case IS_ARRAY:
                class_list = Z_ARRVAL_P(pzarg);
                break;

            /* woops ! */
            default:
                zend_error(E_WARNING, "%s() expects a string or object argument", get_active_function_name(TSRMLS_C));
                return;
        }

        if (!class_list) {
            ZVAL_LONG(return_value, 0);
            /* skip to entry */
            goto enter;
        }
    }

    /* default initialization of return value/type */
    array_init(return_value);

    /* operation entry point */
enter:
    {
        zend_uint zhandle = 1;

        /* loop over all of objects in store */
        while (zhandle < EG(objects_store).top) {

            /* check the object has not been destroyed */
            if (!EG(objects_store).object_buckets[zhandle].destructor_called) {

                zend_object *zobject = (zend_object*) (EG(objects_store).object_buckets[zhandle].bucket.obj.object);

                if (zobject && zobject->ce) {

                    /* match with specific entry */
                    if (pce) {
                        if (pce == zobject->ce) {
                           ++Z_LVAL_P(return_value);
                        }
                    } else {
                        /* match with strings and arrays */
                        zval *zcount;
                        zval **zcounted;

                        size_t zlength = strlen(zobject->ce->name);

                        if (class_list) {
                            zend_ulong exists;
                            zval zclass;

                            /* make temp zval for searching */
                            ZVAL_STRINGL(&zclass, zobject->ce->name, zlength, 0);

                            /* attempt to find this classes name in the parameter array */
                            if (_zend_hash_index_search(class_list, &zclass, &exists TSRMLS_CC) != SUCCESS) {
                                goto omit;
                            }
                        }

                        MAKE_STD_ZVAL(zcount);
                        /* calculate how to ammed return array */
                        if (zend_hash_find(Z_ARRVAL_P(return_value), zobject->ce->name, zlength+1, (void**)&zcounted) == SUCCESS) {
                            ZVAL_LONG(zcount, Z_LVAL_PP(zcounted)+1);
                        } else {
                            ZVAL_LONG(zcount, 1);
                        }

                        /* ammend return array */
                        zend_hash_update(Z_ARRVAL_P(return_value), zobject->ce->name, zlength+1, (void**)&zcount, sizeof(zval), NULL);
                    }
                }
            }
omit:
            zhandle++;
        }
    }
} /* }}} */
