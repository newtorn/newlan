#include "utf8.h"
#include "common.h"

// 获取将值编码为UTF-8所占字节数
// 要编码的值超过范围则返回0
uint32_t getByteNumOfEncodeUtf8(int value)
{
    ASSERT(value >= 0, "Can't encode negative value!");

    if (value <= 0x7f)
    {
        // 0~0x7f
        // 0xxxxxxx
        return 1;
    }

    if (value <= 0x7ff)
    {
        // 0x80~0x7ff
        // 110xxxxx 10xxxxxx
        return 2;
    }

    if (value <= 0xffff)
    {
        // 0x800~0xffff
        // 1110xxxx 10xxxxxx 10xxxxxx
        return 3;
    }

    if (value <= 0x10ffff)
    {
        // 0x10000~0x10ffff
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        return 4;
    }

    return 0;
}

// 将值编码为UTF-8写入连续字节缓冲并返回存储字节数
// 缓冲区以大端字节序村粗，高字节在前，低字节在后
uint8_t encodeUtf8(uint8_t *buf, int value)
{
    ASSERT(value >= 0, "Can't encode negative value!");

    if (value <= 0x7f)
    {
        // ascii
        *buf = value & 0x7f;
        return 1;
    }

    if (value <= 0x7ff)
    {
        // 先存入高5位110xxxxx，再存入低6位10xxxxxx

        // 高字节填充过程
        // 先过滤 0x7c0(0000011111111111) & value，再右移6位得出高5位
        // 填充标志位(110)，将高5位和0xc0(11000000)进行或运算，得出高字节
        *buf++ = 0xc0 | ((value & 0x7c0) >> 6);

        // 低字节填充过程
        // 先过滤 0x3f(00111111) & value，得出低6位
        // 填充标志位(10)，将低6位和0x80(10000000)进行或运算，得出低低字节
        *buf = 0x80 | (value & 0x3f);
        return 2;
    }

    if (value <= 0xffff)
    {
        *buf++ = 0xe0 | ((value & 0xf000) >> 12);
        *buf++ = 0x80 | ((value & 0xfc0) >> 6);
        *buf = 0x80 | (value & 0x3f);
        return 3;
    }

    if (value <= 0x10ffff)
    {
        *buf++ = 0xf0 | ((value & 0x1f0000) >> 18);
        *buf++ = 0x80 | ((value & 0x3f000) >> 12);
        *buf++ = 0x80 | ((value & 0xfc0) >> 6);
        *buf = 0x80 | (value & 0x3f);
        return 4;
    }

    NOT_REACHED();
    return 0;
}

// 从UTF-8编码的高字节获取字节数
// 读取到低字节则返回0
uint32_t getByteNumOfDecodeUtf8(uint8_t high_byte)
{
    if (0x80 == (high_byte & 0xc0))
    {
        // 0x02 (10)
        // Or ( (high_byte >> 6) == 0x02 )
        return 0;
    }

    if (0xf0 == (high_byte & 0xf8))
    {
        // 0x1e (11110)
        // Or ( (high_byte >> 3) == 0x1e )
        return 4;
    }

    if (0xe0 == (high_byte & 0xf0))
    {
        // 0x0e (1110)
        // Or ( (high_byte >> 4) == 0x0e )
        return 3;
    }

    if (0xc0 == (high_byte & 0xe0))
    {
        // 0x06 (110)
        // or ( (high_byte >> 5) == 0x06)
        return 2;
    }

    // 0x00 == (high_byte & 0x80)
    // Or ( (high_byte >> 7) == 0x00 )
    return 1;
}

// 从连续字节缓冲解码并返回解码值
// 缓冲区以大端字节序解码
// 如果不是UTF-8编码，返回-1
int decodeUtf8(const uint8_t *buf, uint32_t maxsize)
{
    if (*buf < 0x7f)
    {
        // ascii
        return *buf;
    }

    int value;          // 返回值
    uint32_t restBytes; // 剩余字节数

    // 填充高字节
    if (0xc0 == (*buf & 0xe0))
    {
        value = *buf & 0x1f;
        restBytes = 1;
    }
    else if (0xe0 == (*buf & 0xf0))
    {
        value = *buf & 0x0f;
        restBytes = 2;
    }
    else if (0xf0 == (*buf & 0xf8))
    {
        value = *buf & 0x07;
        restBytes = 3;
    }
    else
    {
        return -1;
    }

    if (restBytes > maxsize - 1)
    {
        return -1;
    }

    // 填充低字节
    while (restBytes-- > 0 & ++buf != NULL)
    {
        // 低字节高2位为10
        if (0x80 != (*buf & 0xc0))
        {
            return -1;
        }
        value = value << 6 | (*buf & 0x3f);
    }

    return value;
}