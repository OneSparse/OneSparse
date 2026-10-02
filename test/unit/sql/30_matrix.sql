BEGIN;

CREATE EXTENSION IF NOT EXISTS onesparse;
CREATE EXTENSION IF NOT EXISTS pgtap;
SET LOCAL search_path = public, onesparse;

SELECT plan(1);
SELECT pass('matrix test skeleton is available');

-- Construction, elements, transpose, algebra, and assignment.

SELECT * FROM finish();

ROLLBACK;
