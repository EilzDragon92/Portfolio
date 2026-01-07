# Introduction

This is a GUI file encryption/decryption program using AES-256-GCM algorithm.

* Language: C++
* OS: Windows, Linux


## Features

* AES-256-GCM authenticated encryption
* Argon2id password-based key derivation
* Qt-based graphical user interface
* Real-time progress tracking with cancellation support
* Securely wipe sensitive data using RAII pattern
* Asynchronous, multithread processing for non-blocking UI
* Cross-platform support for Windows and Linux


### Why This Project?

* **AES-GCM** 
  * Most used encryption algorithm, de facto industry standard
  * Chosen by NIST, trusted by many governments and cooperations
  * Provides both confidentiality and integrity in a single operation (AEAD)

* **Argon2id**
  * Winner of 2015 Password Hashing Competition
  * Hybrid of Argon2i and Argon2d, balances strength of both algorithms
    * Argon2i: Data independant memory access, resistant to side channel attack
    * Argon2d: Memory hard function, resistant to brute force attack using GPU or ASIC


## Source File Architecture

```
AES-GCM
├── Common
│   ├── header.h                # Common includes, macros, function definitions
│   └── main.cpp                # Application entry point
├── Core
│   ├── AES_GCM.h               # Encryption/decryption engine
│   ├── AES_GCM.h/cpp           # Core implementation
│   ├── AES_GCM_enc.cpp  		# Encryption implementation
│   ├── AES_GCM_dec.cpp  		# Decryption implementation
│   └── Worker.h/cpp            # Asynchronous worker thread
├── GUI
│   ├── MainGUI.h/cpp           # Main workflow controller
│   ├── InputGUI.h/cpp          # User input GUI
│   ├── ProgressGUI.h/cpp       # Progress tracking GUI
│   ├── ModeButton.h/cpp        # Encrypt/Decrypt mode selection component
│   └── PWLineEdit.h/cpp        # Password input component
└── Utils
    ├── Password.h/cpp          # Secure password container
    └── library.cpp             # Utility functions
```


## Security Design

### Encryption Scheme

| Component | Specification |
|-----------|---------------|
| Algorithm | AES-256-GCM |
| Key Size | 256 bits |
| IV Size | 96 bits (random) |
| Auth Tag | 128 bits |


### Key Derivation

| Parameter | Value | Rationale |
|-----------|-------|-----------|
| Algorithm | Argon2id | Provides both side-channel resistance and GPU attack resistance |
| Memory Cost | 512 MB | High memory requirement makes GPU/ASIC attacks impractical |
| Time Cost | 4 iterations | Increases computational cost |
| Parallelism | Auto-detected | Uses all available CPU cores |
| Salt Size | 16 bytes | Randomly generated per encryption |


### Memory Security

* Sensitive data (keys, passwords) are securely wiped after use
* RAII classes ensure data wipe on error, exception or forced stop


### Encrypted File Format

```
│ Salt (16 bytes) │ IV (12 bytes) │ Ciphertext │ Tag (16 bytes) │
```


## Technical Specifications

| Specification | Value |
|---------------|-------|
| Max File Size | 64 GB |
| Buffer Size | 64 KB |
| Block Size | 16 bytes |
| Language Standard | C++20 |


## Limitations

* Maximum file size: 64 GB (AES-GCM counter limit)
* Password-based: Security depends on password strength
* No key file support (password only)


## Dependencies

* **Qt 6** (Core, GUI, Widgets)
* **OpenSSL 3.x** (libcrypto)
* **Argon2** (reference implementation)


## Important Notes

* Must use strong password
* Keep the encrypted file and password separate
* The original file is not modified or deleted


## License

MIT License

