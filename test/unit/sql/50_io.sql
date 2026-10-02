BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(1);
SELECT pass('I/O test skeleton is available');

-- Serialization, file round trips, large objects, and invalid inputs.

SELECT * FROM finish();

ROLLBACK;
