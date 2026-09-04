#!/usr/bin/env bash
set -euo pipefail

REPO="shadowmkj/kmprs"
GITHUB_API="https://api.github.com/repos/${REPO}/releases/latest"

echo "==> Detecting system architecture..."
OS="$(uname -s)"
ARCH="$(uname -m)"

TARGET=""
case "${OS}" in
    Linux)
        case "${ARCH}" in
            x86_64|amd64)
                TARGET="x86_64-unknown-linux-gnu"
                ;;
            *)
                echo "Error: Unsupported Linux architecture: ${ARCH}" >&2
                echo "Supported Linux architectures: x86_64" >&2
                exit 1
                ;;
        esac
        ;;
    Darwin)
        case "${ARCH}" in
            arm64|aarch64)
                TARGET="aarch64-apple-darwin"
                ;;
            *)
                echo "Error: Unsupported macOS architecture: ${ARCH}" >&2
                echo "Supported macOS architectures: Apple Silicon (arm64)" >&2
                exit 1
                ;;
        esac
        ;;
    *)
        echo "Error: Unsupported operating system: ${OS}" >&2
        echo "Supported operating systems: Linux, macOS" >&2
        exit 1
        ;;
esac

echo "==> Fetching latest release info for ${REPO}..."
RELEASE_JSON="$(curl -fsSL -H "Accept: application/vnd.github.v3+json" "${GITHUB_API}" 2>/dev/null)" || true

TAG=""
if [ -n "${RELEASE_JSON}" ]; then
    TAG="$(echo "${RELEASE_JSON}" | grep '"tag_name":' | head -n 1 | sed -E 's/.*"tag_name":[[:space:]]*"([^"]+)".*/\1/')"
fi

if [ -z "${TAG}" ]; then
    echo "Error: Failed to fetch latest release tag from GitHub API." >&2
    echo "Please visit https://github.com/${REPO}/releases to download manually." >&2
    exit 1
fi

echo "==> Found latest version: ${TAG} (${TARGET})"

ARCHIVE_NAME="kmprs-${TAG}-${TARGET}.tar.gz"
DOWNLOAD_URL="https://github.com/${REPO}/releases/download/${TAG}/${ARCHIVE_NAME}"

TMP_DIR="$(mktemp -d)"
trap 'rm -rf "${TMP_DIR}"' EXIT

echo "==> Downloading ${DOWNLOAD_URL}..."
curl -fsSL "${DOWNLOAD_URL}" -o "${TMP_DIR}/${ARCHIVE_NAME}"

echo "==> Extracting archive..."
tar -xzf "${TMP_DIR}/${ARCHIVE_NAME}" -C "${TMP_DIR}"

if [ -f "${TMP_DIR}/kmprs" ]; then
    :
else
    echo "Error: Binary 'kmprs' not found in downloaded archive." >&2
    exit 1
fi

# Determine install directory
INSTALL_DIR="/usr/local/bin"
USE_SUDO=0

if [ -w "${INSTALL_DIR}" ]; then
    USE_SUDO=0
elif [ "$(id -u)" -eq 0 ]; then
    USE_SUDO=0
else
    # Try ~/.local/bin if /usr/local/bin requires root
    if [ -d "${HOME}/.local/bin" ] || mkdir -p "${HOME}/.local/bin" 2>/dev/null; then
        INSTALL_DIR="${HOME}/.local/bin"
        USE_SUDO=0
    elif command -v sudo >/dev/null 2>&1; then
        USE_SUDO=1
    fi
fi

echo "==> Installing kmprs to ${INSTALL_DIR}..."
if [ "${USE_SUDO}" -eq 1 ]; then
    sudo install -m 755 "${TMP_DIR}/kmprs" "${INSTALL_DIR}/kmprs"
else
    mkdir -p "${INSTALL_DIR}"
    install -m 755 "${TMP_DIR}/kmprs" "${INSTALL_DIR}/kmprs"
fi

echo "==> Successfully installed kmprs ${TAG} to ${INSTALL_DIR}/kmprs!"

# Check if INSTALL_DIR is in PATH
case ":${PATH}:" in
    *:"${INSTALL_DIR}":*)
        ;;
    *)
        echo ""
        echo "Notice: '${INSTALL_DIR}' is not in your PATH."
        echo "Add it to your shell configuration (e.g. ~/.bashrc, ~/.zshrc):"
        echo "  export PATH=\"${INSTALL_DIR}:\$PATH\""
        ;;
esac
