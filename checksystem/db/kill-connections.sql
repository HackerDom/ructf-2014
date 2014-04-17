SELECT pg_terminate_backend(pg_stat_activity.procpid)
FROM pg_stat_activity
WHERE pg_stat_activity.datname = 'ructf2014'
  AND procpid <> pg_backend_pid();

