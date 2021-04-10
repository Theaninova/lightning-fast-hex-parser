# lightning-fast-hex-parser
A stupidly optimized Hex String parser

TL;DR
```c
unsigned long parse_uint32_3(const char c[8]) {
  unsigned long long f = *((unsigned long long *) (void*) &c[0]);
  unsigned long long t = f & 0x4040404040404040;
  unsigned long long s = (f & 0xF0F0F0F0F0F0F0F) + ((t >> 3) | (t >> 6));
  s = ((s << 4) | (s >> 8)) & 0x00FF00FF00FF00FF;
  s |= s << 24;
  return (s & 0xFFFF0000FFFF0000) | (s >> 48);
}
```
