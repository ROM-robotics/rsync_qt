#!/usr/bin/env bash
set -euo pipefail

# Simple automated Android build for the tuning_app using Qt for Android.
# Adjust paths below to match your installation before first use.

# ---------- Configuration (edit as needed) ----------
QT_VERSION="6.8.3"
QT_ANDROID_ABI="arm64-v8a"              # e.g. "arm64-v8a;armeabi-v7a" for multi-ABI
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_TYPE="${BUILD_TYPE:-Debug}"       # Debug or Release (override via env or --type)
BUILD_DIR="$PROJECT_ROOT/build/android-${QT_ANDROID_ABI}"
ANDROID_SDK_ROOT="${ANDROID_SDK_ROOT:-/home/mr_robot/Android/Sdk}"  # may override via env
ANDROID_NDK_ROOT="${ANDROID_NDK_ROOT:-$ANDROID_SDK_ROOT/ndk/26.1.10909125}" # Qt-supported NDK
JAVA_HOME="${JAVA_HOME:-/usr/lib/jvm/java-17-openjdk-amd64}"
QT_ANDROID_QT_DIR="/home/mr_robot/Qt/${QT_VERSION}/android_arm64_v8a" # Path to Qt for the ABI (change if multi-ABI)
QT_CMAKE_BIN="$QT_ANDROID_QT_DIR/bin/qt-cmake"
GRADLE_WRAPPER="android-build/gradlew"

# Args parsing (very lightweight):
while [[ $# -gt 0 ]]; do
  case "$1" in
    -t|--type)
      BUILD_TYPE="$2"; shift; shift;;
    -a|--abis)
      QT_ANDROID_ABI="$2"; shift; shift;;
    --)
      shift; break;;
    *)
      echo "Unknown arg: $1"; exit 2;;
  esac
done

if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" ]]; then
  echo "BUILD_TYPE must be Debug or Release (got '$BUILD_TYPE')"; exit 2
fi

# ---------- Sanity checks ----------
[[ -x "$QT_CMAKE_BIN" ]] || { echo "qt-cmake not found at $QT_CMAKE_BIN"; exit 1; }
[[ -d "$ANDROID_SDK_ROOT" ]] || { echo "Android SDK missing at $ANDROID_SDK_ROOT"; exit 1; }
[[ -d "$ANDROID_NDK_ROOT" ]] || { echo "Android NDK missing at $ANDROID_NDK_ROOT"; exit 1; }
[[ -d "$JAVA_HOME" ]] || { echo "JAVA_HOME invalid at $JAVA_HOME"; exit 1; }

mkdir -p "$BUILD_DIR"

echo "[1/4] Configuring (CMake) for ABI: $QT_ANDROID_ABI, TYPE: $BUILD_TYPE"
"$QT_CMAKE_BIN" \
  -S "$PROJECT_ROOT" \
  -B "$BUILD_DIR" \
  -GNinja \
  -DQT_ANDROID_ABIS="$QT_ANDROID_ABI" \
  -DANDROID_SDK_ROOT="$ANDROID_SDK_ROOT" \
  -DANDROID_NDK_ROOT="$ANDROID_NDK_ROOT" \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

echo "[2/4] Building native code"
cmake --build "$BUILD_DIR" -j "$(nproc)"

cd "$BUILD_DIR"

if [[ ! -f "$GRADLE_WRAPPER" ]]; then
  echo "[3/4] Generating Gradle android-build directory (first build)"
fi

# The android-build directory is produced after the first CMake build step.
cd android-build

if [[ "$BUILD_TYPE" == "Debug" ]]; then
  echo "[3/4] Assembling debug APK via Gradle"
  ./gradlew assembleDebug
  APK_STD="app/build/outputs/apk/debug/app-debug.apk"
  APK_QT_LEGACY="android-build-debug.apk"
  APK_QT_BUILD="build/outputs/apk/debug/android-build-debug.apk"
  if [[ -f "$APK_STD" ]]; then
    FINAL_APK="$APK_STD"
  elif [[ -f "$APK_QT_LEGACY" ]]; then
    FINAL_APK="$APK_QT_LEGACY"
  elif [[ -f "$APK_QT_BUILD" ]]; then
    FINAL_APK="$APK_QT_BUILD"
  else
    echo "Debug build finished but APK not found; check Gradle output." >&2
    exit 1
  fi
  echo "[4/4] APK ready: $BUILD_DIR/android-build/$FINAL_APK"
  echo "Install on device: adb install -r '$BUILD_DIR/android-build/$FINAL_APK'"
else
  echo "[3/4] Assembling release APK via Gradle"
  ./gradlew assembleRelease
  APK_STD="app/build/outputs/apk/release/app-release-unsigned.apk"
  APK_QT_LEGACY="android-build-release-unsigned.apk"
  APK_QT_BUILD="build/outputs/apk/release/android-build-release-unsigned.apk"
  if [[ -f "$APK_STD" ]]; then
    RELEASE_APK_UNALIGNED="$APK_STD"
  elif [[ -f "$APK_QT_LEGACY" ]]; then
    RELEASE_APK_UNALIGNED="$APK_QT_LEGACY"
  elif [[ -f "$APK_QT_BUILD" ]]; then
    RELEASE_APK_UNALIGNED="$APK_QT_BUILD"
  else
    echo "Release build finished but unsigned APK not found; check Gradle output." >&2
    exit 1
  fi

  # Optional signing if credentials provided
  if [[ -n "${KEYSTORE:-}" && -n "${KEY_ALIAS:-}" && -n "${KEYSTORE_PASS:-}" && -n "${KEY_ALIAS_PASS:-}" ]]; then
    echo "[4/4] Aligning and signing release APK"
    BUILD_TOOLS_DIR=$(ls -1d "$ANDROID_SDK_ROOT"/build-tools/* 2>/dev/null | sort -V | tail -n1)
    ZIPALIGN="$BUILD_TOOLS_DIR/zipalign"
    APKSIGNER="$BUILD_TOOLS_DIR/apksigner"
    [[ -x "$ZIPALIGN" && -x "$APKSIGNER" ]] || { echo "zipalign/apksigner not found in build-tools"; exit 1; }

    ALIGNED_APK="app/build/outputs/apk/release/app-release-aligned.apk"
    SIGNED_APK="app/build/outputs/apk/release/app-release-signed.apk"
    rm -f "$ALIGNED_APK" "$SIGNED_APK"
    "$ZIPALIGN" -f 4 "$RELEASE_APK_UNALIGNED" "$ALIGNED_APK"
    "$APKSIGNER" sign --ks "$KEYSTORE" --ks-pass env:KEYSTORE_PASS --ks-key-alias "$KEY_ALIAS" \
      --key-pass env:KEY_ALIAS_PASS --out "$SIGNED_APK" "$ALIGNED_APK"
    echo "[4/4] Signed APK ready: $BUILD_DIR/android-build/$SIGNED_APK"
  else
    echo "[4/4] Unsigned release APK ready: $BUILD_DIR/android-build/$RELEASE_APK_UNALIGNED"
    echo "To sign: set KEYSTORE, KEY_ALIAS, KEYSTORE_PASS, KEY_ALIAS_PASS and re-run."
  fi
fi
