# Navigate to the build directory
cd build

# Remove any existing compiled binaries
rm -f ./master ./drone ./input ./obstacles ./server ./targets ./watchdog ./window

# Navigate back to the parent directory
cd ..

# Compile all programs
make all

# Navigate back to the build directory
cd build

# Run the master program
./master