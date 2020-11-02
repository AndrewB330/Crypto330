# Crypto330

![XLOCC](https://europe-west6-xlocc-badge.cloudfunctions.net/XLOCC/AndrewB330/Crypto330?caption=Lines&color=blue&kill_cache=3)

University project. Implementation of:
- AES block cipher (128, 196, 256 key sizes)
- Kalyna block cipher (128/128, 256/128, 256/256, 512/256, 512/512 types)
- RC4 stream cipher (n = 8)
- ECB, CBC, CFB, OFB, CTR block cipher mode of operation
- Salsa

## Usage

CMake:
```cmake
add_subdirectory(crypto330)
target_link_libraries(<TARGET_NAME> crypto330)
```
Code:
```c++
#include <crypto330/block/aes.hpp>
#include <crypto330/block/kalyna.hpp>
#include <crypto330/stream/block_stream.hpp>
#include <crypto330/stream/rc4.hpp>

// ...

AES cipher(AES::Type::AES128, "0022446688AACCEE", /*key represented as hex?*/ true);
Kalyna cipher2(Kalyna::Type::K128_128, "0022446688AACCEE", true);

cipher.EncryptFile("my_precious_file.txt", "encrypted.txt");
cipher.DecryptFile("encrypted.txt", "decrypted.txt");

// OR

std::vector<uint8_t> data = {0x01, 0xb2, ..., 0x32, 0xfa}; // any bytes
cipher.Encrypt(data); // in-place encryption
cipher.Decrypt(data); // in-place decryption

BlockStreamEncryption enc(std::make_unique<AES>(AES::Type::AES128, "Cool AES Key"));
enc.Encrypt(data, BlockStreamEncryption::Mode::CFB);
enc.Decrypt(data, BlockStreamEncryption::Mode::CFB);

RC4 rc4("Cool RC4 Key");
rc4.Encrypt(data);
rc4.Decrypt(data);
```

## Benchmarks
This implementation uses OpenMP if available, so it will be parallel by default. Also, 
Kalyna implementation is not completely optimal.

| Algorithm | Encrypt+Decrypt 1GB (OMP disabled) | Encrypt+Decrypt 1GB (OMP enabled) |
| ------------- | ------------- | ------------- |
| AES128  | 35.6s  | 11.3s  | 
| Kalyna 128/128  | 242.0s  | 79.5s  |
| AES256  | 59.1s  | 18.4s  | 
| Kalyna 512/512  | 342.1s  | 113.7s  |

## Benchmarks2

| Algorithm | 128MB |
| ------------- | ------------- | 
| Salsa  | 1.53s  |
| RC4  | 0.77s  | 
| ECB AES  | 5.3s | 
| CBC AES  | 5.3s  | 
| CFB AES  | 7.6s  | 
| OFB AES  | 6.4s | 
| CTR AES  | 6.8s  | 

## Hashing Results
#### SHA256
Prefix of 20 zero bits found in: `~1'008'000` iterations and `0.8` seconds on average.

With current Bitcoin difficulty, you can mine one block in `728'760'254'630` days, or `2'024'334'040` years. 
- On one core CPU it can make `0.00004$` per year. 
- After code optimization it can make `0.00008$` per year.
- With 8 cores of my CPU it can make `0.0012$` per year.
- With GPU it can make `0.12$` per year.

**PR0FIT**

#### Kupyna
Prefix of 20 zero bits found in: `~1'250'000` iterations and `20.22` seconds on average.

