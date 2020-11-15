# Crypto330

University project. Implementation of:
- AES block cipher (128, 196, 256 key sizes)
- Kalyna block cipher (128/128, 256/128, 256/256, 512/256, 512/512 types)
- RC4 stream cipher (n = 8)
- ECB, CBC, CFB, OFB, CTR block cipher mode of operation
- Salsa
- RSA + OAEP

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

## Benchmarks Block Encryption
This implementation uses OpenMP if available, so it will be parallel by default. Also, 
Kalyna implementation is not completely optimal.

| Algorithm      | Encrypt+Decrypt 1GB (OMP disabled) | Encrypt+Decrypt 1GB (OMP enabled) |
| -------------- | ------------: | ------------: |
| AES128         | 35.6s  | 11.3s  | 
| Kalyna 128/128 | 242.0s | 79.5s  |
| AES256         | 59.1s  | 18.4s  | 
| Kalyna 512/512 | 342.1s | 113.7s |

## Benchmarks Stream Encryption

| Algorithm| 128MB  |
| -------- | -----: | 
| Salsa    | 1.53s  |
| RC4      | 0.77s  | 
| ECB AES  | 5.3s   | 
| CBC AES  | 5.3s   | 
| CFB AES  | 7.6s   | 
| OFB AES  | 6.4s   | 
| CTR AES  | 6.8s   | 

## Benchmarks RSA with OAEP Encryption

| Public key size* | KeyGen | Encrypt 1KB | Decrypt 1KB | Decrypt 1KB  + Optimization |
| ---------------  | -----: | ----------: | ----------: | --------------------------: |
| 1024 Bit         | ~2.38 s|      0.02 s |      1.64 s |                      0.61 s |
| 2048 Bit         | ~20.1 s|      0.03 s |      3.23 s |                      1.20 s |

*Size of the product of two primes, both prime numbers have half this size


