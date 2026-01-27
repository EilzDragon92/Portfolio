## 1. Introduction

This is the GUI, password-based file encryption/decryption tool using AES-256-GCM.

![Build](https://github.com/EilzDragon92/Portfolio/actions/workflows/build.yml/badge.svg)
![License](https://img.shields.io/badge/License-MIT-blue.svg) ![C++](https://img.shields.io/badge/C++-20-00599C?logo=cplusplus) ![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey)
![CMake](https://img.shields.io/badge/CMake-3.16+-064F8C?logo=cmake&logoColor=white) ![OpenSSL](https://img.shields.io/badge/OpenSSL-3.0-721412?logo=openssl&logoColor=white) ![Qt](https://img.shields.io/badge/Qt-6-41CD52?logo=qt&logoColor=white)

## 2. Features

* AES-256-GCM algorithm for file encryption and integrity check
* Argon2id for key derivation from password
* Qt library for graphical user interface
* Double buffering and asynchronous write for better performance 
* Asynchronous, multithread processing for non-blocking UI
* Real-time progress tracking and cancellation support
* Error report and automatic stop when error occurs
* Cross-platform support for Windows and Linux

### 2-1. Why use this?

* **AES-GCM**
	* **AES**
		* Most used encryption algorithm, de facto industry standard
		* Chosen by NIST, trusted by many governments and corporations
	* **GCM
		* Does not require padding, immune to padding oracle attack
		* Can be parallelized in both encryption and decryption process 
		* Provides both confidentiality and integrity using AEAD

* **Argon2id**
	* Winner of 2015 Password Hashing Competition
	* Hybrid of Argon2i and Argon2d, balances strength of both algorithms
		* Argon2i: Data independent memory access, resistant to side channel attack
		* Argon2d: Memory hard function, resistant to brute force attack using GPU or ASIC

### 2-2. Security Considerations

* Keys are locked in memory using `VirtualLock`/`mlock` to prevent swapping to disk
* RAII class for secure password handling ensures data wipe on error or cancellation
* Salt and IV are generated using OS-provided CSPRNG (`BCryptGenRandom`/`getrandom`)
* Sensitive data (passwords, keys, buffers) are wiped using `SecureZeroMemory`/`explicit_bzero`
* Tampered ciphertext files are rejected before any plaintext is output

## 3. Specifications

* **Maximum File Size:** 64 GiB (2 ^ 32 blocks, limitation of 32-bit counter)

* **AES-256-GCM**
	* **IV Size:** 96 bits (recommended for AES-256-GCM)
	* **Key Size:** 256 bits (using AES-256)
	* **Block Size:** 128 bits (using AES)
	* **Authentication Tag Size:** 128 bits

* **Argon2id**
	* **Memory Cost:** 512 MiB
	* **Time Cost:** 4 iterations
	* **Parallelism:** All available CPU cores
	* **Salt Size:** 128 bits

* **Buffer Size:** 4096 blocks (64 KiB)

### 3-1. Encrypted File Format

```
│ Salt (16 bytes) │ IV (12 bytes) │ Ciphertext │ Tag (16 bytes) │
```

### 3-2. Source Code Architecture

```
AES-GCM
├── Common
│   ├── header.h           # Common includes, macros, function definitions
│   └── main.cpp           # Application entry point
├── Core
│   ├── AES_GCM.h/cpp      # AES-GCM engine
│   ├── AES_GCM_enc.cpp    # Encryption implementation
│   ├── AES_GCM_dec.cpp    # Decryption implementation
│   └── Worker.h/cpp       # Asynchronous worker thread
├── GUI
│   ├── MainGUI.h/cpp      # Main workflow controller
│   ├── InputGUI.h/cpp     # User input GUI
│   ├── ProgressGUI.h/cpp  # Progress tracking GUI
│   ├── ModeButton.h/cpp   # Encrypt/Decrypt mode selection component
│   └── PWLineEdit.h/cpp   # Password input component
└── Utils
    ├── Password.h/cpp     # Secure password container
    └── library.cpp        # Utility functions
```

### 3-3. Limitations

* Maximum 64 GiB file
* No batch encryption (Single file only)
* No CLI mode (GUI only)
* No key file support (Password-based key only)
* No log file (GUI message and progress bar only)
* No original file removal

## 4. Build and Usage
### 4-1. Prerequisites

**Windows:**
* Visual Studio 2022+ with C++ workload
* CMake 3.16+
* vcpkg
* Qt 6.7+

**Linux:**
* GCC 11+ or Clang 14+
* CMake 3.16+
* Qt6 development packages

### 4-2. Build

**Windows:**
```bash
# Install dependencies
vcpkg install openssl:x64-windows argon2:x64-windows gtest:x64-windows

# Configure and build
cd Projects/AES-GCM
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

**Linux:**
```bash
# Install dependencies
sudo apt-get install qt6-base-dev libssl-dev libargon2-dev libgtest-dev

# Configure and build
cd Projects/AES-GCM
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### 4-3. Usage

![Screenshot](Screenshot.png)

1. Run the executable `AES-GCM.exe` or `AES-GCM`
2. Select mode
3. Enter source file path
4. Enter destination file path
5. Enter password
6. Click Start

## 5. Testing and Code Quality
### 5-1. Running Tests

**Windows:**
```bash
cd Projects/AES-GCM
ctest --test-dir build -C Release --output-on-failure
```

**Linux:**
```bash
cd Projects/AES-GCM
ctest --test-dir build --output-on-failure
```

### 5-2. Test Coverage

![Coverage](https://codecov.io/gh/EilzDragon92/Portfolio/branch/main/graph/badge.svg)

| Module   | Test File          | Test Cases                                                                                                                                                                                                                                                       |
| -------- | ------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| AES_GCM  | `AES_GCM_Test.cpp` | Basic encryption/decryption, wrong password rejection, tampered or corrupted ciphertext detection, empty file handling, exact buffer size file handling, progress callback invocation, error callback invocation, cancellation                                   |
| Password | `PasswordTest.cpp` | Default constructor, `setData` with C-string and another `Password` class, data replacement, deep copy for copy constructor/assignment, ownership transfer for move constructor/assignment, self-assignment safety, null pointer handling, destructor after move |
| Utils    | `UtilsTest.cpp`    | `GetFileSize`, `FileExists`, `Argon2id` determinism and uniqueness, `GetProcNum`, `OpenFile`, `Random` non-zero and uniqueness, `RemoveFile`, `Seek`, `Wipe`                                                                                                     |

Note: GUI files, error reports for external libraries and system calls are excluded from tests.

### 5-3. Static Analysis

The project uses cppcheck for static code analysis:
```bash
cppcheck --library=qt --enable=warning,style,performance,portability \
    --error-exitcode=1 --suppress=missingIncludeSystem --inline-suppr \
    -I Projects/AES-GCM \
    Projects/AES-GCM/Core \
    Projects/AES-GCM/GUI \
    Projects/AES-GCM/Utils \
    Projects/AES-GCM/Common
```

### 5-4. Continuous Integration

All commits are automatically validated via GitHub Actions:

| Platform | Compiler  | Build System  | Checks                      |
| -------- | --------- | ------------- | --------------------------- |
| Windows  | MSVC 2022 | CMake + vcpkg | Build, Unit Tests           |
| Linux    | GCC       | CMake + apt   | Build, Unit Tests, cppcheck |

### 5-5. Performance Benchmark

Benchmarks are run automatically via GitHub Actions on every push. 

* **Test Environment**
	* **CPU:** 4 Cores, 3.5 GHz
	* **L3 Cache:** 48 MiB
	* **File Size:** 4 GiB

* **Results**
	* **Encryption:** 1.96 GB/s
	* **Decryption:** 1.87 GB/s
	* **Argon2id Key Derivation:** 538ms

Running Benchmarks Locally: 
```bash
cd Projects/AES-GCM
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target AES-GCM-bench
./build/AES-GCM-bench
```

## 6. License

This project is licensed under the MIT License. See [LICENSE.md](LICENSE.md) for more details.

This project uses the following third-party libraries. See [LICENSES-THIRD-PARTY.md](LICENSES-THIRD-PARTY.md) for more details.
- OpenSSL (Apache 2.0)
- Argon2 (CC0/Apache 2.0)
- Qt (LGPL v3)
- Google Test (BSD 3-Clause)
