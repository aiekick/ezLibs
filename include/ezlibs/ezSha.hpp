#pragma once

/*
MIT License

Copyright (c) 2014-2024 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ezSha is part of the ezLibs project : https://github.com/aiekick/ezLibs.git
// and base on https://github.com/983/SHA1.git - Unlicense

#include <cstdint>
#include <cstring>
#include <string>
#include <array>

namespace ez {

class sha1 {
public:
    static size_t constexpr SHA1_HEX_SIZE{40};
    static size_t constexpr SHA1_BASE64_SIZE{28};

private:
    std::array<uint32_t, 5> m_state{};
    std::array<uint8_t, 64> m_buf{};
    uint32_t m_index{};
    uint64_t m_countBits{};

public:
    sha1() : m_index(0), m_countBits(0) {
        m_state[0] = 0x67452301;
        m_state[1] = 0xEFCDAB89;
        m_state[2] = 0x98BADCFE;
        m_state[3] = 0x10325476;
        m_state[4] = 0xC3D2E1F0;
    }
    explicit sha1(const std::string &vText) : sha1() { add(vText); }
    explicit sha1(const char *vText) : sha1() { add(vText); }

    sha1 &add(uint8_t x) {
        m_addByteDontCountBits(x);
        m_countBits += 8;
        return *this;
    }

    sha1 &add(char c) { return add(*(uint8_t *)&c); }

    sha1 &add(const void *data, uint32_t n) {
        if (!data) {
            return *this;
        }

        const uint8_t *ptr = (const uint8_t *)data;

        // fill up block if not full
        for (; n && m_index % sizeof(m_buf); n--)
            add(*ptr++);

        // process full blocks
        for (; n >= sizeof(m_buf); n -= sizeof(m_buf)) {
            m_processBlock(ptr);
            ptr += sizeof(m_buf);
            m_countBits += sizeof(m_buf) * 8;
        }

        // process remaining part of block
        for (; n; n--) {
            add(*ptr++);
        }

        return *this;
    }

    sha1 &add(const std::string &vText) {
        if (!vText.empty()) {
            return *this;
        }
        return add(vText.data(), static_cast<uint32_t>(vText.size()));
    }

    sha1 &add(const char *text) {
        if (!text) {
            return *this;
        }
        return add(text, static_cast<uint32_t>(strlen(text)));
    }

    sha1 &finalize() {
        // hashed text ends with 0x80, some padding 0x00 and the length in bits
        m_addByteDontCountBits(0x80);
        while (m_index % 64 != 56) {
            m_addByteDontCountBits(0x00);
        }
        for (int32_t j = 7; j >= 0; j--) {
            m_addByteDontCountBits(static_cast<uint8_t>(m_countBits >> j * 8));
        }

        return *this;
    }

    const std::string getHex(const char *alphabet = "0123456789abcdef") {
        std::string ret(SHA1_HEX_SIZE, 0);
        int k = 0;
        for (int m_index = 0; m_index < 5; m_index++) {
            for (int j = 7; j >= 0; j--) {
                ret[k++] = alphabet[(m_state[m_index] >> j * 4) & 0xf];
            }
        }
        return ret;
    }

private:
    void m_addByteDontCountBits(uint8_t x) {
        m_buf[m_index++] = x;
        if (m_index >= sizeof(m_buf)) {
            m_index = 0;
            m_processBlock(m_buf.data());
        }
    }

    static uint32_t m_rol32(uint32_t x, uint32_t n) { return (x << n) | (x >> (32 - n)); }

    static uint32_t m_makeWord(const uint8_t *p) { return ((uint32_t)p[0] << 3 * 8) | ((uint32_t)p[1] << 2 * 8) | ((uint32_t)p[2] << 1 * 8) | ((uint32_t)p[3] << 0 * 8); }

    void m_processBlock(const uint8_t *ptr) {
        const uint32_t c0 = 0x5a827999;
        const uint32_t c1 = 0x6ed9eba1;
        const uint32_t c2 = 0x8f1bbcdc;
        const uint32_t c3 = 0xca62c1d6;

        uint32_t a = m_state[0];
        uint32_t b = m_state[1];
        uint32_t c = m_state[2];
        uint32_t d = m_state[3];
        uint32_t e = m_state[4];

        uint32_t w[16];

        for (int m_index = 0; m_index < 16; m_index++) {
            w[m_index] = m_makeWord(ptr + m_index * 4);
        }

#define SHA1_LOAD(m_index) w[m_index & 15] = m_rol32(w[(m_index + 13) & 15] ^ w[(m_index + 8) & 15] ^ w[(m_index + 2) & 15] ^ w[m_index & 15], 1);
#define SHA1_ROUND_0(v, u, x, y, z, m_index)                         \
    z += ((u & (x ^ y)) ^ y) + w[m_index & 15] + c0 + m_rol32(v, 5); \
    u = m_rol32(u, 30);
#define SHA1_ROUND_1(v, u, x, y, z, m_index)                                            \
    SHA1_LOAD(m_index) z += ((u & (x ^ y)) ^ y) + w[m_index & 15] + c0 + m_rol32(v, 5); \
    u = m_rol32(u, 30);
#define SHA1_ROUND_2(v, u, x, y, z, m_index)                                    \
    SHA1_LOAD(m_index) z += (u ^ x ^ y) + w[m_index & 15] + c1 + m_rol32(v, 5); \
    u = m_rol32(u, 30);
#define SHA1_ROUND_3(v, u, x, y, z, m_index)                                                  \
    SHA1_LOAD(m_index) z += (((u | x) & y) | (u & x)) + w[m_index & 15] + c2 + m_rol32(v, 5); \
    u = m_rol32(u, 30);
#define SHA1_ROUND_4(v, u, x, y, z, m_index)                                    \
    SHA1_LOAD(m_index) z += (u ^ x ^ y) + w[m_index & 15] + c3 + m_rol32(v, 5); \
    u = m_rol32(u, 30);

        SHA1_ROUND_0(a, b, c, d, e, 0);
        SHA1_ROUND_0(e, a, b, c, d, 1);
        SHA1_ROUND_0(d, e, a, b, c, 2);
        SHA1_ROUND_0(c, d, e, a, b, 3);
        SHA1_ROUND_0(b, c, d, e, a, 4);
        SHA1_ROUND_0(a, b, c, d, e, 5);
        SHA1_ROUND_0(e, a, b, c, d, 6);
        SHA1_ROUND_0(d, e, a, b, c, 7);
        SHA1_ROUND_0(c, d, e, a, b, 8);
        SHA1_ROUND_0(b, c, d, e, a, 9);
        SHA1_ROUND_0(a, b, c, d, e, 10);
        SHA1_ROUND_0(e, a, b, c, d, 11);
        SHA1_ROUND_0(d, e, a, b, c, 12);
        SHA1_ROUND_0(c, d, e, a, b, 13);
        SHA1_ROUND_0(b, c, d, e, a, 14);
        SHA1_ROUND_0(a, b, c, d, e, 15);
        SHA1_ROUND_1(e, a, b, c, d, 16);
        SHA1_ROUND_1(d, e, a, b, c, 17);
        SHA1_ROUND_1(c, d, e, a, b, 18);
        SHA1_ROUND_1(b, c, d, e, a, 19);
        SHA1_ROUND_2(a, b, c, d, e, 20);
        SHA1_ROUND_2(e, a, b, c, d, 21);
        SHA1_ROUND_2(d, e, a, b, c, 22);
        SHA1_ROUND_2(c, d, e, a, b, 23);
        SHA1_ROUND_2(b, c, d, e, a, 24);
        SHA1_ROUND_2(a, b, c, d, e, 25);
        SHA1_ROUND_2(e, a, b, c, d, 26);
        SHA1_ROUND_2(d, e, a, b, c, 27);
        SHA1_ROUND_2(c, d, e, a, b, 28);
        SHA1_ROUND_2(b, c, d, e, a, 29);
        SHA1_ROUND_2(a, b, c, d, e, 30);
        SHA1_ROUND_2(e, a, b, c, d, 31);
        SHA1_ROUND_2(d, e, a, b, c, 32);
        SHA1_ROUND_2(c, d, e, a, b, 33);
        SHA1_ROUND_2(b, c, d, e, a, 34);
        SHA1_ROUND_2(a, b, c, d, e, 35);
        SHA1_ROUND_2(e, a, b, c, d, 36);
        SHA1_ROUND_2(d, e, a, b, c, 37);
        SHA1_ROUND_2(c, d, e, a, b, 38);
        SHA1_ROUND_2(b, c, d, e, a, 39);
        SHA1_ROUND_3(a, b, c, d, e, 40);
        SHA1_ROUND_3(e, a, b, c, d, 41);
        SHA1_ROUND_3(d, e, a, b, c, 42);
        SHA1_ROUND_3(c, d, e, a, b, 43);
        SHA1_ROUND_3(b, c, d, e, a, 44);
        SHA1_ROUND_3(a, b, c, d, e, 45);
        SHA1_ROUND_3(e, a, b, c, d, 46);
        SHA1_ROUND_3(d, e, a, b, c, 47);
        SHA1_ROUND_3(c, d, e, a, b, 48);
        SHA1_ROUND_3(b, c, d, e, a, 49);
        SHA1_ROUND_3(a, b, c, d, e, 50);
        SHA1_ROUND_3(e, a, b, c, d, 51);
        SHA1_ROUND_3(d, e, a, b, c, 52);
        SHA1_ROUND_3(c, d, e, a, b, 53);
        SHA1_ROUND_3(b, c, d, e, a, 54);
        SHA1_ROUND_3(a, b, c, d, e, 55);
        SHA1_ROUND_3(e, a, b, c, d, 56);
        SHA1_ROUND_3(d, e, a, b, c, 57);
        SHA1_ROUND_3(c, d, e, a, b, 58);
        SHA1_ROUND_3(b, c, d, e, a, 59);
        SHA1_ROUND_4(a, b, c, d, e, 60);
        SHA1_ROUND_4(e, a, b, c, d, 61);
        SHA1_ROUND_4(d, e, a, b, c, 62);
        SHA1_ROUND_4(c, d, e, a, b, 63);
        SHA1_ROUND_4(b, c, d, e, a, 64);
        SHA1_ROUND_4(a, b, c, d, e, 65);
        SHA1_ROUND_4(e, a, b, c, d, 66);
        SHA1_ROUND_4(d, e, a, b, c, 67);
        SHA1_ROUND_4(c, d, e, a, b, 68);
        SHA1_ROUND_4(b, c, d, e, a, 69);
        SHA1_ROUND_4(a, b, c, d, e, 70);
        SHA1_ROUND_4(e, a, b, c, d, 71);
        SHA1_ROUND_4(d, e, a, b, c, 72);
        SHA1_ROUND_4(c, d, e, a, b, 73);
        SHA1_ROUND_4(b, c, d, e, a, 74);
        SHA1_ROUND_4(a, b, c, d, e, 75);
        SHA1_ROUND_4(e, a, b, c, d, 76);
        SHA1_ROUND_4(d, e, a, b, c, 77);
        SHA1_ROUND_4(c, d, e, a, b, 78);
        SHA1_ROUND_4(b, c, d, e, a, 79);

#undef SHA1_LOAD
#undef SHA1_ROUND_0
#undef SHA1_ROUND_1
#undef SHA1_ROUND_2
#undef SHA1_ROUND_3
#undef SHA1_ROUND_4

        m_state[0] += a;
        m_state[1] += b;
        m_state[2] += c;
        m_state[3] += d;
        m_state[4] += e;
    }
};

}  // namespace ez
