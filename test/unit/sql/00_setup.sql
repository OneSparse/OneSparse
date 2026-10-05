BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(1);
SELECT pass('pgTAP setup is available');
SELECT * FROM finish();

ROLLBACK;
