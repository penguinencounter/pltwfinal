# Build Instructions

## for Raspberry Pi (cross-compiled)

1. On ubuntu-likes, install `gcc-aarch64-linux-gnu` and `g++-12-aarch64-linux-gnu`. Also a recent version of `cmake` if you don't have that yet.
2. Enter `pisrc`.
3. Run (optionally `chmod +x`) `./cmake_setup_toolchain` (Bash script).
5. Run `cmake --build build`.

## for your host machine (tests)

1. Install a recent version of `cmake`, if you don't have those yet.
2. Enter `testsrc`.
3. Run `cmake -B build .`.
4. Run `cmake --build build`.
