SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Add mkdtimg and dtc to path
export PATH=$SCRIPT_DIR/prebuilts/misc/linux-x86:$PATH
JECLANG=$SCRIPT_DIR/prebuilts/clang-4053586
JEGCC64=$SCRIPT_DIR/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9
JEGCC32=$SCRIPT_DIR/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9

cleanBuild=0
incBuild=0
while getopts "ci" opt; do
    case "$opt" in
    c)
      cleanBuild=1
      ;;
    i)
      incBuild=1
      ;;
  esac
done

if [ $cleanBuild == 0 ] && [ $incBuild == 0 ]; then
  echo Please specify -c for clean build, -i for incremental build.
  exit 1
fi

pushd $SCRIPT_DIR
if [ $cleanBuild != 0 ]; then
  make O=out mrproper
  make -j$(nproc --all) ARCH=arm64 O=out wahoo_defconfig
fi

make -j$(nproc --all) ARCH=arm64 O=out CC=$JECLANG/bin/clang CLANG_TRIPLE=aarch64-linux-gnu- CROSS_COMPILE=$JEGCC64/bin/aarch64-linux-android- CROSS_COMPILE_ARM32=$JEGCC32/bin/arm-linux-androideabi-
echo On success, kernel to copy to build/update boot image is at: $SCRIPT_DIR/out/arch/arm64/boot/Image.lz4-dtb
echo Modules to copy to vendor image /vendor/lib/modules can be found by running find $SCRIPT_DIR/out -name *ko
