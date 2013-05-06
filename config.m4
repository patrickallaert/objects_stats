dnl config.m4 for object_stats

PHP_ARG_ENABLE(object_stats, whether to enable object_stats support,
[  --enable-object-stats          Enable object_stats support])

if test "$PHP_OBJECT_STATS" != "no"; then
    PHP_NEW_EXTENSION(object_stats, object_stats.c, $ext_shared)
fi
