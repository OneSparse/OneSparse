# GitHub Release Workflow Design

## Overview

This document describes the design and architecture of the automated release workflow for OneSparse.

## Architecture

The workflow consists of three sequential jobs that run when version tags are pushed:

1. **Test Job**: Validates code quality by running the full test suite
2. **Docker Job**: Builds and publishes the Docker image to Docker Hub
3. **Release Job**: Creates a GitHub release with auto-generated notes

## Design Decisions

### Sequential Job Dependencies

Jobs are configured with explicit dependencies (`needs:` key) to ensure:
- Docker images are only built if tests pass
- GitHub releases are only created if the Docker image is successfully published
- Failed jobs prevent subsequent jobs from running

### Docker Image Strategy

Uses `Dockerfile-slim` for production releases to minimize image size and attack surface.

### Pre-release Detection

Tags containing "alpha", "beta", or "rc" are automatically marked as pre-releases in GitHub.

### Release Notes

Uses GitHub's auto-generated release notes based on commit messages and pull requests since the last release.

## Technical Specifications

**Workflow File**: `.github/workflows/release.yml`

**Trigger Pattern**: `v*.*.*` (semantic versioning tags)

**Docker Image Tags**:
- Version-specific: `onesparse/onesparse-slim:1.2.3`
- Latest: `onesparse/onesparse-slim:latest`

**Required Secrets**:
- `DOCKERHUB_USERNAME`: Docker Hub account username
- `DOCKERHUB_TOKEN`: Docker Hub access token

**Permissions**:
- `contents: write` (for creating releases)

---

## Usage Instructions

### Creating a Release

1. **Ensure all changes are merged to main branch**

2. **Create and push a version tag:**

   ```bash
   # For regular release
   git tag v1.2.3
   git push origin v1.2.3

   # For pre-release
   git tag v1.2.3-beta.1
   git push origin v1.2.3-beta.1
   ```

3. **Monitor workflow progress:**

   - Navigate to: Repository → Actions tab
   - Click on the "Release" workflow run
   - Watch test → docker → release jobs execute

4. **Verify release created:**

   - Navigate to: Repository → Releases
   - Confirm new release appears with auto-generated notes

### Troubleshooting

**Tests fail:**
- Check Actions logs for test output
- Run `./test.sh` locally to reproduce
- Fix issues, create new tag after merge

**Docker build fails:**
- Verify `DOCKERHUB_USERNAME` and `DOCKERHUB_TOKEN` secrets are set
- Check Dockerfile-slim syntax
- Review Docker build logs in Actions

**Release creation fails:**
- Verify workflow has `contents: write` permission (should be automatic)
- Check if release already exists for tag
- Review gh CLI output in Actions logs

**Image not appearing on Docker Hub:**
- Verify Docker Hub credentials are correct
- Check if repository `onesparse/onesparse-slim` exists on Docker Hub
- Ensure Docker Hub account has write access

### Deleting/Re-running Failed Releases

If a release fails partway through:

```bash
# Delete the GitHub release (if created)
gh release delete v1.2.3 --yes

# Delete the tag locally and remotely
git tag -d v1.2.3
git push origin :refs/tags/v1.2.3

# Fix the issue, then recreate the tag
git tag v1.2.3
git push origin v1.2.3
```
