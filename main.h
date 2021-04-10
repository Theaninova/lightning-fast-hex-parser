/**
 * This is a stupidly optimized version of a hex-to-int parser.
 * It is capable of parsing a full 32-bit unsigned int at once,
 * and is substantially faster than using a lookup table, however
 * it also requires using a 64-bit machine
 *
 * @param c the input string (must be 8 chars long)
 * @return the parsed integer
 */
inline unsigned long parse_uint32_3(const char c[8]) {
    // ascii '0' is            0011 0000           1100 1111
    // ascii '1' subsequently  0011 0001           1100 1110
    // ascii '9' is            0011 1001           1100 0110
    // so we can mask out just the last four bits
    // ascii 'a' is            0100 0001           1011 1110
    // ascii 'A' is            0110 0001           1001 1110
    // ascii 'f' is            0100 0110           1011 1001
    // ascii 'F' is            0110 0110           1001 1001
    // by masking out the first two bits and by shifting right by 3, we get
    // 0000 in the case of 0..9, and 1000 for [aA]..[fF]
    // 1001 is the magic number, composed out of the second bit from the left,
    // mask it out and shift to the 5th place, the other bit can just be shifted to the last place.

    // first interpret the string as an unsigned long long
    unsigned long long f = *((unsigned long long *) (void*) &c[0]);
    // mask out the second bit from the left for each char, which is 1 for [aA]..[fF] and 0 for 0..9
    unsigned long long t = f & 0x4040404040404040;
    // compose a set of 9s (0b1001) for each char, by shifting our previous value,
    // so we only get 9 when we have a value over 'a'
    // then we add 9 'conditionally' to last bits of the char, 0..9 for '0'..'9' and 1..6 for [aA]..[fF]
    unsigned long long s = (f & 0xF0F0F0F0F0F0F0F) + ((t >> 3) | (t >> 6));

    // now we need to combine all the bit groups into a full 32 bit int
    // first we shift the value by four
    // '=' -> junk
    // '*' -> valued bits
    // ' ' -> guaranteed 0

    // in addition to compressing the bytes together, we also need to swap the order of the
    // four bit groups. We can do both of those in one go

    // shifting first by four to the left prompted the compiler to generate vastly
    // faster code compared to shifting in the end.
    // also note that using an initial shift, and then applying a 12 right shift on top
    // would be slower because of processor pipelining

    //     | * * * * * * * *|    IN
    // or  |   * = * = * = *|    SHIFT RIGHT
    //     |= * * * * * * * |    IN SHIFT LEFT
    //     |==**==**==**==**|    OR
    //     |  **  **  **  **|    MASK CLEANUP
    s = ((s << 4) | (s >> 8)) & 0x00FF00FF00FF00FF;
    //     |**  ==  **  ==  |    SHIFT LEFT
    //     |  **  ==  **  ==|    IN
    //     |****====****====|    OR
    // because of processor pipelining, it is faster to apply the mask afterwards to just one part,
    // as the right shift will be allowed to execute at the same time as the mask is applied
    s |= s << 24;
    //     |            ****|    SHIFT RIGHT
    //     |========****====|    IN
    //     |====    ****    |    MASK CLEANUP // this mask will be optimized to just xor the lower part of the register
    //     |====    ********|    OR
    return (s & 0xFFFF0000FFFF0000) | (s >> 48);
}
