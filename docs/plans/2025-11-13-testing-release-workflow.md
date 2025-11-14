# Testing the Release Workflow

## Local Verification (Before Pushing Tag)

### 1. Verify Test Script Works

```bash
./test.sh
```

Expected: All 33 tests pass

### 2. Verify Dockerfile-slim Builds

```bash
docker build -f Dockerfile-slim -t onesparse/onesparse-slim:test .
```

Expected: Build succeeds without errors

### 3. Validate YAML Syntax

```bash
python3 -c "import yaml; yaml.safe_load(open('.github/workflows/release.yml'))"
```

Expected: No output (success)

## Test Release Process

### Option 1: Test Tag (Recommended for First Run)

Create a test tag to verify the workflow without creating an official release:

```bash
# Create test tag
git tag v0.0.1-test
git push origin v0.0.1-test

# Monitor in GitHub Actions
# After verification, delete test release and tag:
gh release delete v0.0.1-test --yes
git tag -d v0.0.1-test
git push origin :refs/tags/v0.0.1-test
```

### Option 2: Branch Testing (Advanced)

Temporarily modify the workflow trigger to test on a branch:

1. Change trigger in `.github/workflows/release.yml`:
   ```yaml
   on:
     push:
       branches:
         - test-release-workflow
   ```

2. Push to test branch and monitor

3. Revert trigger change before merging

## Validation Checklist

After running the workflow, verify:

- [ ] Test job completed successfully
- [ ] All 33 tests passed in test job logs
- [ ] Docker job logged in to Docker Hub
- [ ] Docker image built from Dockerfile-slim
- [ ] Image tagged with version number
- [ ] Image tagged with 'latest'
- [ ] Image pushed to Docker Hub (visible at hub.docker.com)
- [ ] Release job created GitHub release
- [ ] Release has auto-generated notes
- [ ] Release marked as pre-release (if tag contains alpha/beta/rc)
- [ ] Source tarball attached to release

## Common Issues

**"Error: Unable to find Dockerfile-slim"**
- Ensure you're pushing the tag from the correct branch
- Verify Dockerfile-slim exists in repository root

**"Error: authentication required"**
- Verify Docker Hub secrets are configured correctly
- Check secret names match exactly: DOCKERHUB_USERNAME, DOCKERHUB_TOKEN

**"Error: release already exists"**
- Delete existing release before re-running
- Use `gh release delete <tag>` command
