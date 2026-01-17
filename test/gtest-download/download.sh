#!/bin/bash

set -e  # 遇错退出

# 配置
GTEST_VERSION="release-1.8.0"
GTEST_REPO="https://github.com/google/googletest.git"
TARGET_DIR="$(dirname "$0")/../googletest-release-1.8.0"

echo ">>> Downloading GoogleTest $GTEST_VERSION..."

# 如果已存在，跳过下载（避免重复）
if [ -d "$TARGET_DIR" ] && [ -f "$TARGET_DIR/CMakeLists.txt" ]; then
    echo ">>> GoogleTest already exists at $TARGET_DIR. Skipping download."
    exit 0
fi

mkdir -p "$(dirname "$TARGET_DIR")"
git clone --depth=1 --branch="$GTEST_VERSION" "$GTEST_REPO" "$TARGET_DIR"
echo ">>> GoogleTest downloaded to $TARGET_DIR"