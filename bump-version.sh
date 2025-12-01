#!/bin/bash
set -e

NEW_VERSION="$1"

if [ -z "$NEW_VERSION" ]; then
    echo "Usage: ./bump-version.sh <version>"
    echo "Example: ./bump-version.sh 0.2.0"
    exit 1
fi

# Extract current version from generate.py
CURRENT_VERSION=$(grep "^VERSION = " generate.py | sed "s/VERSION = '\\(.*\\)'/\\1/")

echo "Updating version from $CURRENT_VERSION to $NEW_VERSION"

# Update generate.py
sed -i "s/^VERSION = '.*'/VERSION = '$NEW_VERSION'/" generate.py

# Update onesparse.control
sed -i "s/^default_version = '.*'/default_version = '$NEW_VERSION'/" onesparse.control

# Remove old versioned SQL file if it exists
rm -f "onesparse/onesparse--$CURRENT_VERSION.sql"

# Regenerate versioned SQL file (uses VERSION from generate.py)
python3 generate.py

echo "Version updated to $NEW_VERSION"
echo ""
echo "Next steps:"
echo "  1. Review changes: git diff"
echo "  2. Commit: git add -A && git commit -m 'Bump version to $NEW_VERSION'"
echo "  3. Tag: git tag v$NEW_VERSION"
echo "  4. Push: git push && git push --tags"
