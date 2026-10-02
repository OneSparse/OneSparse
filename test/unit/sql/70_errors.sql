BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(1);
SELECT pass('error test skeleton is available');

-- Bounds, literals, incompatible dimensions, and unsupported operations.

SELECT * FROM finish();

ROLLBACK;
