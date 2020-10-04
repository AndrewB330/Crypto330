# Crypto330

![XLOCC](https://europe-west6-xlocc-badge.cloudfunctions.net/XLOCC/AndrewB330/Crypto330?caption=Lines&color=blue&kill_cache=1)

University project. Implementation of AES and Kalyna block ciphers.

## Usage

CMake:
```cmake
add_subdirectory(crypto330)
target_link_libraries(<TARGET_NAME> crypto330)
```
Code:
```c++
#include <crypto330/aes.hpp>
#include <crypto330/kalyna.hpp>

// ...

AES cipher(AES::Type::AES128, "0022446688AACCEE", /*key represented as hex?*/ true);
Kalyna cipher2(Kalyna::Type::K128_128, "0022446688AACCEE", true);

cipher.EncryptFile("my_precious_file.txt", "encrypted.txt");
cipher.DecryptFile("encrypted.txt", "decrypted.txt");

// OR

std::vector<uint8_t> data = {0x01, 0xb2, ..., 0x32, 0xfa}; // any bytes
cipher.Encrypt(data); // in-place encryption
cipher.Decrypt(data); // in-place decryption
```

## Benchmarks
This implementation uses OpenMP if available, so it will be parallel by default. Also, 
Kalyna implementation is not completely optimal.

| Algorithm | Encode+Decode 1GB (OMP disabled) | Encode+Decode 1GB (OMP enabled) |
| ------------- | ------------- | ------------- |
| AES128  | 35.6s  | 11.3s  | 
| Kalyna 128/128  | 242.0s  | 79.5s  |
| AES256  | 59.1s  | 18.4s  | 
| Kalyna 512/512  | 342.1s  | 113.7s  |

