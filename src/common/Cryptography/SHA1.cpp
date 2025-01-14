/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "SHA1.h"
#include "BigNumber.h"
#include "Util.h"
#include <cstring>
#include <stdarg.h>

SHA1Hash::SHA1Hash()
{
    SHA1_Init(&mC);
    memset(mDigest, 0, SHA_DIGEST_LENGTH * sizeof(uint8));
}

SHA1Hash::~SHA1Hash()
{
    SHA1_Init(&mC);
}

void SHA1Hash::UpdateData(const uint8 *dta, int len)
{
    SHA1_Update(&mC, dta, len);
}

void SHA1Hash::UpdateData(const std::string &str)
{
    UpdateData((uint8 const*)str.c_str(), str.length());
}

void SHA1Hash::UpdateBigNumbers(BigNumber* bn0, ...)
{
    va_list v;
    BigNumber* bn;

    va_start(v, bn0);
    bn = bn0;
    while (bn)
    {
        UpdateData(bn->AsByteArray().get(), bn->GetNumBytes());
        bn = va_arg(v, BigNumber*);
    }
    va_end(v);
}

void SHA1Hash::Initialize()
{
    SHA1_Init(&mC);
}

void SHA1Hash::Finalize(void)
{
    SHA1_Final(mDigest, &mC);
}

std::string CalculateSHA1Hash(std::string const& content)
{
    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)content.c_str(), content.length(), (unsigned char*)&digest);

    return ByteArrayToHexStr(digest, SHA_DIGEST_LENGTH);
}
