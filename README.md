Keywords: C++, Open SSL, Hash functions

# 📝 Task Objective:

Your task is to implement a function (or a set of functions, not a full program) that finds any message whose hash (SHA-512) starts with a specified sequence of zero bits from the left.
The bit order is big-endian: Byte 0 from MSB to LSB, Byte 1 from MSB to LSB, ..., last byte from MSB to LSB.
In other words, two zero bits correspond to a byte like 0010 0111 (0x27).

The function is required in two variants:
1. Basic solution (findHash function). The implementation of this function is mandatory.
2. Enhanced solution (findHashEx function). The implementation of this function is optional, but a dummy implementation must be provided to compile the task. Implement this function if you aim for a bonus.

#### 📥 Function Parameters:

```
int findHash(int bits, string &message, string &hash)
```

- **bits:** The required number of zero bits in the hash of the message.
- **message:** Output parameter. This parameter will contain the data for which the corresponding hash was found. The result is stored as a hexadecimal string.
- **hash:** Output parameter. This is the hash of the message from the previous parameter, also stored as a hexadecimal string.
- **Return value:** The function returns 1 on success, 0 on failure or invalid parameters (typically, an unreasonable number of zero bits).

```
int findHashEx(int bits, string &message, string &hash, string_view hashFunction)
```

- Extension of findHash: All parameters and return value remain the same as in the basic variant.
- **hashFunction:** A new parameter that specifies which hash function should be used to find the sequence of zero bits. The provided hash function name must be compatible with EVP_get_digestbyname.
