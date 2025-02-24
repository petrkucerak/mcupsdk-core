//! [include]

#include <stdio.h>
#include <string.h>
#include <security/security_common/drivers/crypto/crypto.h>
#include <kernel/dpl/DebugP.h>
#include <security/security_common/drivers/crypto/asym_crypt.h>

/* Openssl command To generate public key : Openssl rsa -pubout -in private.pem -out public.pem*/
static const struct AsymCrypt_RSAPubkey gPkaRsa2kPublicKey = 
{
	{
		64UL,
		0x8C6C85B9UL,0x056953ACUL,0x9F24B6E7UL,0x91D77638UL,
		0x3E28B3C7UL,0x3DDCCC06UL,0xBFD47EB2UL,0x6B606CC6UL,
		0x5175DF58UL,0x7D2A303DUL,0xCB5A09CEUL,0xD9430637UL,
		0x6CCB3436UL,0x6B12FE2FUL,0xAC169F63UL,0xEE112EA4UL,
		0x3FA18B8DUL,0xD35A6216UL,0x9A95897DUL,0x10C853C2UL,
		0x371C8456UL,0xE767B532UL,0x4834F9C3UL,0x8B00B9E8UL,
		0x47DC231DUL,0xC979A0E1UL,0x7FB0CC0DUL,0x2EA266CDUL,
		0x3EDE8C2DUL,0xC78D2E6FUL,0xB27C8E0CUL,0x5C4DC759UL,
		0x72D66874UL,0x589ED84BUL,0x1A23F0B0UL,0xEE420C45UL,
		0x0930D8F4UL,0x9793E7F4UL,0x19586A35UL,0xED664D03UL,
		0xA55EF9D6UL,0x3CA68A5DUL,0x93275527UL,0x210F0365UL,
		0x4336EB94UL,0x3BE61CD6UL,0x330F4E75UL,0x79816748UL,
		0xD1906A36UL,0x9D311DB6UL,0x0BF47E6FUL,0xBE7D5D82UL,
		0xB3DC1598UL,0x6E9F6DB7UL,0xCFCC4A8AUL,0xDFBA2F3EUL,
		0x0CEA1413UL,0xF6E7AE2FUL,0x0429A8F5UL,0xFF21AFADUL,
		0x9E529C69UL,0x75245098UL,0x958033AFUL,0xC56969C2UL,
	},
	{   1UL,
		0x00010001UL, 
	}
};

/* Openssl command To generate private key : Openssl genrsa -out private.pem*/
static const struct AsymCrypt_RSAPrivkey gPkaRsa2kPrivateKey = 
{
	{
		64UL,
		0x8C6C85B9UL,0x056953ACUL,0x9F24B6E7UL,0x91D77638UL,
		0x3E28B3C7UL,0x3DDCCC06UL,0xBFD47EB2UL,0x6B606CC6UL,
		0x5175DF58UL,0x7D2A303DUL,0xCB5A09CEUL,0xD9430637UL,
		0x6CCB3436UL,0x6B12FE2FUL,0xAC169F63UL,0xEE112EA4UL,
		0x3FA18B8DUL,0xD35A6216UL,0x9A95897DUL,0x10C853C2UL,
		0x371C8456UL,0xE767B532UL,0x4834F9C3UL,0x8B00B9E8UL,
		0x47DC231DUL,0xC979A0E1UL,0x7FB0CC0DUL,0x2EA266CDUL,
		0x3EDE8C2DUL,0xC78D2E6FUL,0xB27C8E0CUL,0x5C4DC759UL,
		0x72D66874UL,0x589ED84BUL,0x1A23F0B0UL,0xEE420C45UL,
		0x0930D8F4UL,0x9793E7F4UL,0x19586A35UL,0xED664D03UL,
		0xA55EF9D6UL,0x3CA68A5DUL,0x93275527UL,0x210F0365UL,
		0x4336EB94UL,0x3BE61CD6UL,0x330F4E75UL,0x79816748UL,
		0xD1906A36UL,0x9D311DB6UL,0x0BF47E6FUL,0xBE7D5D82UL,
		0xB3DC1598UL,0x6E9F6DB7UL,0xCFCC4A8AUL,0xDFBA2F3EUL,
		0x0CEA1413UL,0xF6E7AE2FUL,0x0429A8F5UL,0xFF21AFADUL,
		0x9E529C69UL,0x75245098UL,0x958033AFUL,0xC56969C2UL,
	},
	{
		1UL,
		0x00010001UL,
	},
	{
		64UL,
		0x76B24971UL,0xABAE2708UL,0x7090F0B9UL,0xF2B4C776UL,
		0xD30CD9E2UL,0x77A17624UL,0xCC8C2F9CUL,0x205ADBD6UL,
		0x709F9778UL,0xBF700835UL,0x39415524UL,0x2603467DUL,
		0xD19476C9UL,0x57176E0AUL,0xE6EB00C1UL,0x9F2C8188UL,
		0xAEB5AB6CUL,0x3CC340A0UL,0x86C8D4B8UL,0x9ED2DE1AUL,
		0x1DE5126CUL,0x91C518A6UL,0xB2F933DEUL,0x588146D2UL,
		0xCB2D4E92UL,0xD4420D3AUL,0xE9C29055UL,0xAA0E226EUL,
		0x86E83ED8UL,0xBFCB8270UL,0x0BEECA18UL,0x22FEE79EUL,
		0xFB39B96DUL,0x3A461E0AUL,0x5F9CB1A5UL,0x7A3CB7C8UL,
		0x8AD36D97UL,0xDED1073BUL,0x95A4B28CUL,0x0D8F7612UL,
		0xECDBA3A1UL,0xD7B6B2D6UL,0xC493C52CUL,0x30D4DB55UL,
		0x1435D7E4UL,0x6369AA17UL,0xB0F2E9FDUL,0xE6604C1CUL,
		0xF634C564UL,0xD619065AUL,0xCC97DF2AUL,0x4B88C523UL,
		0x3437D542UL,0x48FD52CAUL,0x125BD29EUL,0x0D6C69D7UL,
		0xBDFCA12AUL,0x69BD7BAAUL,0xFD5D8D04UL,0x24E4DFEDUL,
		0x3F3E724FUL,0x9EAD06CAUL,0x0EE1B3A8UL,0x171211ADUL,
	},
	{
		32UL,
		0x1615910DUL,0x896F86ADUL,0x3FA600B5UL,0xBE5CAE1FUL,
		0x431ACC84UL,0xBE0B507FUL,0xE19E3780UL,0xE4B1701BUL,
		0x9C03114EUL,0x5A2C16D7UL,0xFDE1682FUL,0xDBAEE508UL,
		0xD9A2A8A2UL,0x03EFB7A2UL,0xDEAA84EFUL,0x8AEE33C9UL,
		0x0D95FED7UL,0xA3ED1F55UL,0xB0EA72C7UL,0x773291B0UL,
		0x055BBBE7UL,0x50ABE824UL,0x936448DFUL,0x6D96E050UL,
		0x1AE0060CUL,0xF513D05DUL,0x53C29C4CUL,0x5BE4EDE3UL,
		0x53B357C3UL,0xC2624CCFUL,0x508FA4EAUL,0xFA5D6875UL,
	},
	{
		32UL,
		0xB0B5245DUL,0x6B6FBB98UL,0xC9D6FA06UL,0xB35E18FAUL,
		0x180B5D7DUL,0xFF895C08UL,0xF311CE70UL,0x03B9D7F1UL,
		0x17FBC6BAUL,0x5D06779BUL,0x3F7F6FC8UL,0xA84C4879UL,
		0x26516954UL,0xC6A3D297UL,0x70566BEEUL,0x161F2DE1UL,
		0xC1ED432BUL,0xF38BF70CUL,0x2A3BA241UL,0x85256B85UL,
		0x738F5EDAUL,0x54076D5AUL,0x0FC4655CUL,0x44440091UL,
		0xD8D167B7UL,0x2A45134DUL,0xE2D0891EUL,0x7427A577UL,
		0xBC7B661EUL,0x33AFA2F3UL,0x98E19426UL,0xC9DAE439UL,
	},
	{
		32UL,
		0x37383D85UL,0xF81ABE55UL,0xED5CFFEAUL,0x769715C7UL,
		0xEAFF10E5UL,0x64D022FEUL,0xDCF6656FUL,0x20AAC7E2UL,
		0xBDBB9A84UL,0xCA084CA2UL,0xEE9C9558UL,0x338C49D4UL,
		0xC3DF145CUL,0x1B91A89BUL,0xA6158E06UL,0x7C6F439FUL,
		0xEBD7825CUL,0x6615B326UL,0x57CF4140UL,0xD8DD57C1UL,
		0xCD38197DUL,0xB0F3B372UL,0xAE891C00UL,0xF1A1CBBAUL,
		0x0F15E9CDUL,0xD1DD5A08UL,0xCA5DFAA8UL,0xDA257067UL,
		0x54983EF3UL,0x98F84C4EUL,0xEE658CBDUL,0x345D12FAUL,
	},
	{
		32UL,
		0x1ABB7FA9UL,0xA5EF70EAUL,0x8416D267UL,0xC5B28801UL,
		0xBEB01413UL,0x84B2EE52UL,0x3174051DUL,0x2243F2A8UL,
		0xD09F9009UL,0xB1C004A5UL,0x4FFFCE1BUL,0x02B68260UL,
		0x5960F482UL,0x1FD99580UL,0x6969D74BUL,0x05C522CAUL,
		0x1E331D08UL,0x902FAFBAUL,0x7DC0D445UL,0x7C30D555UL,
		0xA49DFB17UL,0x2BA5A83DUL,0x8441CD44UL,0x42443663UL,
		0x6E401D09UL,0xA75D0581UL,0x18C311F8UL,0xD21AAE22UL,
		0xAD807E6FUL,0x3A1CDDD1UL,0x79F297BDUL,0xC7FF6F7DUL,
	},
	{
		32UL,
		0x05F2328EUL,0xBCD27D4CUL,0x77F5E127UL,0xE7FADF6DUL,
		0xDD8E2EB2UL,0x8AC22E8AUL,0x95B55248UL,0xD55D3842UL,
		0x446C93E0UL,0x2F127699UL,0x4A991495UL,0x5A4CE6BAUL,
		0x020CDB4EUL,0xE52F73A1UL,0xBB225AE5UL,0xDB31939EUL,
		0x53F34268UL,0x440EA84CUL,0x07FB118EUL,0x30584688UL,
		0x0D7F92A1UL,0xB4C09F8EUL,0xB503B548UL,0x1F32A473UL,
		0x656A316EUL,0xB4A06EE4UL,0x5BADB7F0UL,0x4064A01AUL,
		0x3D3F86F5UL,0x9B2482F4UL,0x686A6368UL,0x569021AFUL
	}
};

static const uint32_t gPkaRsa2kMessage[] = 
{
	64UL,
	0x12345678UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
	0x11111111UL, 0x11111111UL, 0x11111111UL, 0x11111111UL,
};

/* PKA handle for processing every api */
AsymCrypt_Handle			gPkaHandle = NULL;

static uint32_t gPkaRsaOutputResult[RSA_MAX_LENGTH ];

//! [include]
void pka_rsa_pub_priv(void)
{
//! [sa2ulpkarsa]

	/* Open PKA instance, enable PKA engine, Initialize clocks and Load PKA Fw */
    gPkaHandle = AsymCrypt_open(0U);
    DebugP_assert(gPkaHandle != NULL);

    AsymCrypt_RSAPublic(gPkaHandle, gPkaRsa2kMessage, &gPkaRsa2kPublicKey, gPkaRsaOutputResult);

	AsymCrypt_RSAPrivate(gPkaHandle, gPkaRsaOutputResult, &gPkaRsa2kPrivateKey, gPkaRsaOutputResult);

	/* Close PKA instance, disable PKA engine, deinitialize clocks*/
	AsymCrypt_close(gPkaHandle);

    if (0 != memcmp(gPkaRsaOutputResult, gPkaRsa2kMessage, sizeof(gPkaRsa2kMessage)))
	{
		DebugP_log("[AsymCrypt] AsymCrypt_RSAPrivate output did not match expected output\n");
	}
    
//! [sa2ulpkarsa]
    return;
}