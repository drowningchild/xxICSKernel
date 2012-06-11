#!/bin/sh
export KERNELDIR=`readlink -f .`
export INITRAMFS_SOURCE=`readlink -f $KERNELDIR/../initramfs_root`
export PARENT_DIR=`readlink -f ..`
export USE_SEC_FIPS_MODE=true
export ARCH=arm
export CROSS_COMPILE=/home/tony/android-toolchain-eabi/bin/arm-eabi-

#Copy the initramfs
echo "Remove old zImage"
rm zImage
echo "Create initramfs dir"
mkdir -p kernel/usr/initramfs
echo "Remove old initramfs dir"
rm -rf kernel/usr/initramfs/*
echo "Copy new initramfs dir"
cp -R $PARENT_DIR/initramfs_root/* kernel/usr/initramfs
#echo "Remove .o files"
#rm -rf kernel/*.o
echo "chmod initramfs dir"
chmod -R g-w kernel/usr/initramfs/*
rm $(find kernel/usr/initramfs -name EMPTY_DIRECTORY -print)
rm -rf $(find kernel/usr/initramfs -name .git -print)
#Enable FIPS mode
export USE_SEC_FIPS_MODE=true
#make xxKernel_i777_defconfig
make -j`grep 'processor' /proc/cpuinfo | wc -l`
echo "Copying Modules"
cp -a $(find . -name *.ko -print |grep -v initramfs) kernel/usr/initramfs/lib/modules/
echo "Modules Copied"
sleep 5
touch kernel/usr/initramfs
echo "Rebuilding kernel with new initramfs"
make -j8
cp arch/arm/boot/zImage zImage
# adb shell reboot download
# sleep 5
# heimdall flash --kernel arch/arm/boot/zImage
