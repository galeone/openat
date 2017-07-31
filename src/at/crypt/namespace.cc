#include <at/crypt/namespace.hpp>

namespace at::crypt {

std::vector<unsigned char> sha256(const std::string& data)
{
    std::vector<unsigned char> digest(SHA256_DIGEST_LENGTH);

    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data.c_str(), data.length());
    SHA256_Final(digest.data(), &ctx);

    return digest;
}

std::vector<unsigned char> base64_decode(const std::string& data)
{
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    BIO* bmem = BIO_new_mem_buf((void*)data.c_str(), data.length());
    bmem = BIO_push(b64, bmem);

    std::vector<unsigned char> output(data.length());
    int decoded_size = BIO_read(bmem, output.data(), output.size());
    BIO_free_all(bmem);

    if (decoded_size < 0) {
        throw std::runtime_error("failed while decoding base64.");
    }

    return output;
}

std::string base64_encode(const std::vector<unsigned char>& data)
{
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    BIO* bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);

    BIO_write(b64, data.data(), data.size());
    BIO_flush(b64);

    BUF_MEM* bptr = NULL;
    BIO_get_mem_ptr(b64, &bptr);

    std::string output(bptr->data, bptr->length);
    BIO_free_all(b64);

    return output;
}

std::vector<unsigned char> hmac_sha512(const std::vector<unsigned char>& data,
                                       const std::vector<unsigned char>& key)
{
    unsigned int len = EVP_MAX_MD_SIZE;
    std::vector<unsigned char> digest(len);

    auto ctx = HMAC_CTX_new();

    HMAC_Init_ex(ctx, key.data(), key.size(), EVP_sha512(), NULL);
    HMAC_Update(ctx, data.data(), data.size());
    HMAC_Final(ctx, digest.data(), &len);

    HMAC_CTX_free(ctx);

    return digest;
}

}  // end at::crypt namespace
