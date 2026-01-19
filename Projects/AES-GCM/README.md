## 1. Introduction

This is the GUI, password-based file encryption/decryption program.

* **Language:** C++ 20
* **OS:** Windows, Linux

![Build](https://github.com/EilzDragon92/Portfolio/actions/workflows/build.yml/badge.svg)

## 2. Features

* AES-256-GCM algorithm for authenticated file encryption
* Argon2id for key derivation from password
* Qt library for graphical user interface
* RAII pattern for secure password handling
* Randomly generated salt and initial vector for each session
* Sensitive data are wiped after use
* Asynchronous, multithread processing for non-blocking UI
* Real-time progress tracking and cancellation support
* Error report and automatic stop when error occurs

### 2-1. Why use this?

* **AES-GCM**
	* Most used encryption algorithm, de facto industry standard
	* Chosen by NIST, trusted by many governments and corporations
	* Provides both confidentiality and integrity using AEAD

* **Argon2id**
	* Winner of 2015 Password Hashing Competition
	* Hybrid of Argon2i and Argon2d, balances strength of both algorithms
		* Argon2i: Data independent memory access, resistant to side channel attacks
		* Argon2d: Memory hard function, resistant to brute force attacks using GPU or ASIC

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

## 4. Limitations

* Does not support CLI mode
* Does not support key file (GUI-input password only)
* No log file (GUI message only)

