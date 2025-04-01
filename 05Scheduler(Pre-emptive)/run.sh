# Clean previous builds
date +"%Y-%m-%d %H:%M:%S"
echo " "
echo "Cleaning previous builds..."
make clean

# Build the project
echo " "
echo "Building the project..."
make

# Run QEMU
echo " "
echo "Starting QEMU with the specified parameters..."
qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial null -serial stdio

echo "QEMU has been started."