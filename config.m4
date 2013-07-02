dnl config.m4 for objects_stats

PHP_ARG_ENABLE(objects_stats, whether to enable objects_stats support,
[  --enable-objects-stats          Enable objects_stats support])

if test "$PHP_OBJECTS_STATS" != "no"; then
    PHP_NEW_EXTENSION(objects_stats, objects_stats.c, $ext_shared)
fi
