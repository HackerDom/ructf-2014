DELETE FROM sla;
DELETE FROM score;
DELETE FROM news;
DELETE FROM rounds;
DELETE FROM checker_run_log;
DELETE FROM access_checks;
DELETE FROM advisories;
ALTER SEQUENCE adv_seq RESTART 1;
DELETE FROM solved_tasks;
ALTER SEQUENCE stask_seq RESTART 1;
DELETE FROM stolen_flags;
DELETE FROM secret_flags;
DELETE FROM flags;
DELETE FROM tasks;
DELETE FROM services;
DELETE FROM teams;
DELETE FROM score;
DELETE FROM sla;
DELETE FROM task_flags;
DELETE FROM stolen_task_flags;

INSERT INTO rounds(n) VALUES(0);
