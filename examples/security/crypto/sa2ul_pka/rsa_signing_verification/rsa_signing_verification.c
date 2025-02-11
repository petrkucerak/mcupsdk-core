/*
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* This example demonstrates the PKA RSA Signing and Verification operations*/

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <kernel/dpl/DebugP.h>
#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"

static uint32_t gPkaRsaSignOutputResult[RSA_MAX_LENGTH];
static uint32_t gPkaRsaVerifyOutputResult[RSA_MAX_LENGTH];
static uint32_t gPkaRsaShaHashWith32BitFormate[RSA_MAX_LENGTH];
static uint32_t gPkaRsaShaHashWithBigIntFormate[RSA_MAX_LENGTH];
static uint8_t gPkaRsaShaHashWithPadding[(RSA_MAX_LENGTH * 4)];

/* SHA512 length */
#define APP_SHA512_LENGTH               (64U)
/* We are using 4096 bit key*/
#define APP_PKA_RSA_KEY_SIZE_IN_BYTES	(512U)
#define APP_PKA_RSA_KEY_SIZE_IN_WORDS	(128U)
#define APP_PKA_RSA_MSG_SIZE_IN_BYTES	(512U)

/* Sa2ul Pka Instance */
#define SA2UL_PKA_INSTANCE				(0U)

/* Openssl command To generate public key : Openssl rsa -pubout -in private.pem -out public.pem
The below key is in Bigint format please check in Api guide to know about Bigint format*/
static const struct AsymCrypt_RSAPubkey gPkaRsaPublicKey =
{
	{
		128UL,
		0xB30F257DUL,0x6692D669UL,0xD60247BFUL,0x7C3EBCFEUL,
		0x4F2BFC65UL,0x37FBF89AUL,0xA7ECC69EUL,0x26665EE7UL,
		0x603C736DUL,0x6EB2EBEEUL,0x25278D41UL,0x0E2B5903UL,
		0x694D0019UL,0x145FAA33UL,0x6104C2F2UL,0x86EA4CEDUL,
		0x3F2BD18EUL,0xC910121BUL,0xDEC93E61UL,0x98AF8AE6UL,
		0x6CCB2488UL,0x24366867UL,0xB30AAB68UL,0x9F77FD18UL,
		0x16278D26UL,0xBE79536EUL,0x14ABAB7DUL,0x5F72B6BDUL,
		0xDE51DFAAUL,0xE5D0DE44UL,0xF3E86E58UL,0x2B783AE0UL,
		0xB83EDB77UL,0xC18B7CD4UL,0xEDEBB920UL,0xF98E6CEAUL,
		0x62F2B043UL,0x45B65BCCUL,0xC2C61DB4UL,0xD9C994E4UL,
		0x85A3414DUL,0x72B96FCEUL,0xF45CABC8UL,0x55E4C5AAUL,
		0x7C3F2DDDUL,0x9CFAE215UL,0x9874A88DUL,0x0A9DC2A0UL,
		0x3412E132UL,0x5C8D2660UL,0x655715EEUL,0x63C4D092UL,
		0x40CC7F1AUL,0x57B215BBUL,0x958DCE85UL,0x78CD8226UL,
		0xBE8E55C9UL,0x952E5F5EUL,0x51DA4F16UL,0x87109D31UL,
		0x71409FE7UL,0xC79921AFUL,0xCA64DC92UL,0xE8407330UL,
		0x2350722AUL,0xD24CFEC0UL,0xC5A03E59UL,0x2ACF6ECEUL,
		0x86C32F43UL,0x568BC62CUL,0x048F69ABUL,0xF0049084UL,
		0xDFEF9548UL,0x31B2B61EUL,0x4166D41FUL,0xE99C94D2UL,
		0x3F55C587UL,0x7B0EEDEBUL,0x0C671AC0UL,0x342B87ACUL,
		0x7343B2BDUL,0xBBE4B01DUL,0xF6903DBCUL,0x55E67FB7UL,
		0x273CB9F4UL,0xC8FB90C3UL,0xAD8D0C2BUL,0x50D10DA2UL,
		0x55988309UL,0x901C41CDUL,0x5B1869DBUL,0x12950770UL,
		0x56EB029FUL,0x561DD33BUL,0x745FC0D0UL,0xAB844DCDUL,
		0xA5877876UL,0x2CF97440UL,0x9465349CUL,0x2AA99F2EUL,
		0xA747C368UL,0x99B24F9BUL,0xC907DD56UL,0x8F9B71FAUL,
		0xE8FD6E6DUL,0x0A74987AUL,0xF36F5D08UL,0xB656726EUL,
		0x30481F17UL,0x06CEC2FAUL,0x7B08B46FUL,0x9631A585UL,
		0x8E6BF3CCUL,0xCFC77135UL,0xBCA8E8A1UL,0x7C69FE96UL,
		0x1BCD6B72UL,0x875D5D4FUL,0x13F6A707UL,0xC029CE5FUL,
		0x8CECF878UL,0x23F59212UL,0xFEA5C665UL,0xFB0A115AUL,
		0x90F896ABUL,0x9D495E78UL,0x4AEA1DFEUL,0xE7F43E38UL,
	},
	{   1UL,
		0x00010001UL,
	}
};

/* Openssl command To generate private key : Openssl genrsa -out private.pem
The below key is in Bigint format please check in Api guide to know about Bigint format*/
static const struct AsymCrypt_RSAPrivkey gPkaRsaPrivateKey =
{
	{
		128UL,
		0xB30F257DUL,0x6692D669UL,0xD60247BFUL,0x7C3EBCFEUL,
		0x4F2BFC65UL,0x37FBF89AUL,0xA7ECC69EUL,0x26665EE7UL,
		0x603C736DUL,0x6EB2EBEEUL,0x25278D41UL,0x0E2B5903UL,
		0x694D0019UL,0x145FAA33UL,0x6104C2F2UL,0x86EA4CEDUL,
		0x3F2BD18EUL,0xC910121BUL,0xDEC93E61UL,0x98AF8AE6UL,
		0x6CCB2488UL,0x24366867UL,0xB30AAB68UL,0x9F77FD18UL,
		0x16278D26UL,0xBE79536EUL,0x14ABAB7DUL,0x5F72B6BDUL,
		0xDE51DFAAUL,0xE5D0DE44UL,0xF3E86E58UL,0x2B783AE0UL,
		0xB83EDB77UL,0xC18B7CD4UL,0xEDEBB920UL,0xF98E6CEAUL,
		0x62F2B043UL,0x45B65BCCUL,0xC2C61DB4UL,0xD9C994E4UL,
		0x85A3414DUL,0x72B96FCEUL,0xF45CABC8UL,0x55E4C5AAUL,
		0x7C3F2DDDUL,0x9CFAE215UL,0x9874A88DUL,0x0A9DC2A0UL,
		0x3412E132UL,0x5C8D2660UL,0x655715EEUL,0x63C4D092UL,
		0x40CC7F1AUL,0x57B215BBUL,0x958DCE85UL,0x78CD8226UL,
		0xBE8E55C9UL,0x952E5F5EUL,0x51DA4F16UL,0x87109D31UL,
		0x71409FE7UL,0xC79921AFUL,0xCA64DC92UL,0xE8407330UL,
		0x2350722AUL,0xD24CFEC0UL,0xC5A03E59UL,0x2ACF6ECEUL,
		0x86C32F43UL,0x568BC62CUL,0x048F69ABUL,0xF0049084UL,
		0xDFEF9548UL,0x31B2B61EUL,0x4166D41FUL,0xE99C94D2UL,
		0x3F55C587UL,0x7B0EEDEBUL,0x0C671AC0UL,0x342B87ACUL,
		0x7343B2BDUL,0xBBE4B01DUL,0xF6903DBCUL,0x55E67FB7UL,
		0x273CB9F4UL,0xC8FB90C3UL,0xAD8D0C2BUL,0x50D10DA2UL,
		0x55988309UL,0x901C41CDUL,0x5B1869DBUL,0x12950770UL,
		0x56EB029FUL,0x561DD33BUL,0x745FC0D0UL,0xAB844DCDUL,
		0xA5877876UL,0x2CF97440UL,0x9465349CUL,0x2AA99F2EUL,
		0xA747C368UL,0x99B24F9BUL,0xC907DD56UL,0x8F9B71FAUL,
		0xE8FD6E6DUL,0x0A74987AUL,0xF36F5D08UL,0xB656726EUL,
		0x30481F17UL,0x06CEC2FAUL,0x7B08B46FUL,0x9631A585UL,
		0x8E6BF3CCUL,0xCFC77135UL,0xBCA8E8A1UL,0x7C69FE96UL,
		0x1BCD6B72UL,0x875D5D4FUL,0x13F6A707UL,0xC029CE5FUL,
		0x8CECF878UL,0x23F59212UL,0xFEA5C665UL,0xFB0A115AUL,
		0x90F896ABUL,0x9D495E78UL,0x4AEA1DFEUL,0xE7F43E38UL,
	},
	{
		1UL,
		0x00010001UL,
	},
	{
		128UL,
		0x51328009UL,0x3ED222F3UL,0x24233CB1UL,0x16E12104UL,
		0x1418250EUL,0x38F562D0UL,0x0ED1F95AUL,0x46292801UL,
		0xC6D43F90UL,0x3403773EUL,0xCD8D4A99UL,0x0C866B3CUL,
		0xA46AB12BUL,0xF0AA4CECUL,0xACDD1EA7UL,0xD2D11C97UL,
		0xC21A578DUL,0x4A003064UL,0x52D153B9UL,0xCF4F8462UL,
		0x0A8AD990UL,0x533263B2UL,0xB3BEE900UL,0x1DFC8A7BUL,
		0x5F8DCC4EUL,0x08EBA1C8UL,0xBAA7FE0AUL,0x76C3F5D9UL,
		0xB3B33281UL,0x95F18A02UL,0xA27262CBUL,0x15729747UL,
		0xB3A6557BUL,0x5FD1965EUL,0xFD41054CUL,0x22D3D951UL,
		0x7FF33D5EUL,0xB9E455D6UL,0xE585B404UL,0x3F04618BUL,
		0x1AE2CAC4UL,0xFD98BFECUL,0x938E8020UL,0xC416594EUL,
		0x8329B1DCUL,0x34CF9CA0UL,0x5EF31CDCUL,0x4D368464UL,
		0x435D2614UL,0x4624143EUL,0x582A1C7CUL,0xA9FC18F4UL,
		0x9112BCD9UL,0x7BEF257AUL,0x09ECDC33UL,0x470D699AUL,
		0xAB128A91UL,0xF7060499UL,0x961CA195UL,0x3AB967C4UL,
		0xCB023A7EUL,0xD23EB737UL,0x4AE4A275UL,0x7C987CB2UL,
		0xAFA5B00AUL,0x702C07E0UL,0x7D62CF2CUL,0x4A4BC6DDUL,
		0x636CDABCUL,0x0715FB64UL,0xD19C0FE7UL,0x5EABE33DUL,
		0x00CBC53BUL,0x252D1898UL,0x0F65DA96UL,0x069DA2BCUL,
		0xFE15912CUL,0x9D30ABBAUL,0x012724E1UL,0x9C4CFF1DUL,
		0x038D5D6CUL,0x85D37741UL,0xB9422A68UL,0x07012F87UL,
		0xF6D88E55UL,0xA9F89314UL,0xAA99C356UL,0xCD27A40BUL,
		0x310EC20EUL,0xFF6DB283UL,0x51877DDAUL,0x1BFFF33DUL,
		0x68315DDCUL,0xD6E8FAAEUL,0xEA73B4F6UL,0x2EF72202UL,
		0x01C9D2B7UL,0x6ED9B684UL,0x1670717CUL,0x629A95A2UL,
		0x69DB6D52UL,0xDE320C99UL,0xFD369B37UL,0x1CDDD80AUL,
		0xB5C75523UL,0x82DD2B67UL,0xF684AEE0UL,0x78763DCFUL,
		0x2E2089B6UL,0x237A70B2UL,0xAD426D6AUL,0xCC661875UL,
		0x986F8D9DUL,0x38528378UL,0x850D29E3UL,0x5CA0FEE0UL,
		0xA03CEFF7UL,0x1627A183UL,0x6EC4E3C8UL,0xDEF9D7A1UL,
		0xD6743229UL,0xBB246F5EUL,0xAB5B484CUL,0xDE1846B4UL,
		0x834493AAUL,0x369B6080UL,0xF451A319UL,0x06BB51EAUL,
	},
	{
		64UL,
		0xF89AC607UL,0xA3FFD690UL,0xF771CB8CUL,0x5554179EUL,
		0x432D220CUL,0x55D3E934UL,0x83B2435EUL,0x70151AA3UL,
		0x7FC39B5CUL,0x69AC9F69UL,0x8979DEE4UL,0xEC7CCE4FUL,
		0xDFF0A95FUL,0x22CA9327UL,0xFE7B8E3BUL,0xC1270436UL,
		0x58F4A72AUL,0x0E8BDA95UL,0x82D66270UL,0xF0F58327UL,
		0x27972659UL,0xA3F89FC1UL,0x0EB8702FUL,0x6FFE01AFUL,
		0x5EE63055UL,0x9F75EF05UL,0x816DAAFAUL,0x832E2AC4UL,
		0x2E51859CUL,0x215A8863UL,0x5AD0D430UL,0x7B979845UL,
		0x250A156DUL,0xE5BEAFD2UL,0x6ADF085BUL,0x228F1489UL,
		0xF626B00BUL,0xB5009BBAUL,0x19DBD95BUL,0xD68D5F95UL,
		0x0705120FUL,0xECFD2547UL,0x5BC699CEUL,0xC92E69E9UL,
		0xB55320B6UL,0xAFE5DFC6UL,0x1A1A08CEUL,0xD4934D6AUL,
		0x2C3D1429UL,0x61E2C47BUL,0xCE3E867AUL,0x2FDAD775UL,
		0xDF919071UL,0x18A7747DUL,0xF5C1C96CUL,0x41EB8BA2UL,
		0x1BA20DE7UL,0xD0EFA0E4UL,0x3002EB10UL,0x16DB47F1UL,
		0x2CE0B99EUL,0xE1F6B607UL,0x8D1E572DUL,0xFB21B840UL
	},
	{
		64UL,
		0x9AB6F75BUL,0x62498A8DUL,0x7E023DAEUL,0xDE123B24UL,
		0xCFAAB5F9UL,0x89EA2D59UL,0xEC191C1FUL,0xE98A5014UL,
		0x691530D3UL,0xDAABF362UL,0x3DFCCD85UL,0x005FB374UL,
		0x10DA602EUL,0xAE409E97UL,0x4333EFEDUL,0xD1364ECCUL,
		0x980090EDUL,0x6F7B8CBAUL,0xD2707F87UL,0xB3CBD681UL,
		0x2DEECBE3UL,0x5A30792EUL,0x71E67464UL,0x33F6D2BBUL,
		0xC7EC0C57UL,0xCB570409UL,0x537B48A6UL,0x988260ACUL,
		0xA6DCCA7CUL,0x9ADB04D2UL,0x32CC0FCCUL,0x87CFD8F4UL,
		0x82607FDBUL,0xF15723A6UL,0x534456ABUL,0x45A2FE39UL,
		0x69551C4DUL,0x92FD6429UL,0x1011A16DUL,0x5BB6E218UL,
		0x4C6D8203UL,0x91E24FCCUL,0xE8A8F4B3UL,0x293B2699UL,
		0xAA5C2A6EUL,0x4D7A07F1UL,0xF2FBA01BUL,0x35646BABUL,
		0xD1CE9B93UL,0x7A7BC2C1UL,0xDE5EBB47UL,0x2CF275F4UL,
		0x63BF1D72UL,0x4343820DUL,0xDFBE4632UL,0x8B95B1ACUL,
		0x02B9FD9CUL,0xBBE709E5UL,0x0E4BB8BCUL,0x1727CEA8UL,
		0x7CA1EB12UL,0xCA1AD226UL,0xCB8D67B8UL,0xEC7359ECUL,
	},
	{
		64UL,
		0x928301B3UL,0x402E5D95UL,0xCB23C20DUL,0xEC00A6D8UL,
		0x83B596B3UL,0xE0AC9BBAUL,0x76112109UL,0x2B2F64E6UL,
		0x08D2B7A6UL,0xD64B22BDUL,0x8CFF5C6EUL,0x945C5038UL,
		0x8FF0CB0FUL,0x3248AC03UL,0xA3F1E8ACUL,0x8DBA0CBDUL,
		0xED8AA77FUL,0x06954F73UL,0x8E397EBAUL,0xCADF5DBEUL,
		0xC51E2D63UL,0x9F9B83AEUL,0x98DC0E56UL,0x225E7E4FUL,
		0x8722C7CAUL,0x42F62E36UL,0xEB143520UL,0xBFEC756EUL,
		0x67914438UL,0xA9F7C351UL,0x11481B0DUL,0xCB3B0A6AUL,
		0xD528C0B1UL,0x67DA79E6UL,0x7B17C274UL,0xB720B6AAUL,
		0xA05D2C2FUL,0x48BEBBC1UL,0x0EBC33D1UL,0x5C185795UL,
		0x38192ADAUL,0xAE404DB5UL,0xF814A3B4UL,0xDE6EDE9DUL,
		0x10954D70UL,0x3597E0A8UL,0x3B546460UL,0x87E04F41UL,
		0xECC1D312UL,0x8AD47031UL,0x5272B29AUL,0x907C3B77UL,
		0xAD0B0237UL,0xA21FAD6AUL,0xD6894BDCUL,0x885F16AAUL,
		0x0C01418BUL,0x913A31E9UL,0x49EA46BDUL,0x4040F757UL,
		0x4DD1AA93UL,0x69CCEB3EUL,0x54E1EE8BUL,0x56468059UL,
	},
	{
		64UL,
		0x134429F7UL,0x41D2E0DCUL,0x2A35B178UL,0xEEFB101AUL,
		0xE6BD0B88UL,0x38D62988UL,0x4A7363F0UL,0x8AA06B44UL,
		0xA9B11705UL,0xAF45D14AUL,0x32EBB1AFUL,0x2FF1CB80UL,
		0x36AC2CF7UL,0x70472412UL,0x1371A96EUL,0x5390C282UL,
		0xE7A67B62UL,0x2E95EDD9UL,0xDBBE3544UL,0x4CEC5FC0UL,
		0xFFC9C045UL,0x55FE5EADUL,0x7FB8B67AUL,0x96871889UL,
		0x14B642C4UL,0xBB8E0338UL,0x0F652D4FUL,0x811EC250UL,
		0xB41A4564UL,0xB698361BUL,0x32F3CA0FUL,0xED5F2D69UL,
		0xA504B7AFUL,0x7285E320UL,0x69F47135UL,0x25B661C5UL,
		0xB2181802UL,0xCCC8E82EUL,0x22267C51UL,0x6285590DUL,
		0xA58509ABUL,0xEAFE7B30UL,0x4431E612UL,0x8013F702UL,
		0xAB930539UL,0x5A69B5A8UL,0x783224A2UL,0x5B633F34UL,
		0x875F9821UL,0x05D4399DUL,0x3399F4C1UL,0xF983FDA6UL,
		0xBA8E9368UL,0x4CE0C7ADUL,0xC3E69663UL,0xC8C1BDF0UL,
		0x2A3F265DUL,0x5BB89D60UL,0x2D178CF5UL,0x2EAB6953UL,
		0xFBA65CC1UL,0x0327EFA7UL,0x1327D333UL,0x60501036UL,
	},
	{
		64UL,
		0xB8210409UL,0x4AD457EBUL,0x5C90747EUL,0x9509C039UL,
		0xF21CD54CUL,0xA1D18EDEUL,0xD5EFC2B8UL,0xFFD852EFUL,
		0x234BD795UL,0xCB6EDF65UL,0x57DC0F3AUL,0x3AE9680AUL,
		0x5D014152UL,0xBB249CB9UL,0x20E2A9ADUL,0xAFE9749CUL,
		0xBB067659UL,0xF6FFEA47UL,0xDB62F67EUL,0xFFFC6A90UL,
		0x6966D4D7UL,0xE888FE42UL,0xCBC0EB8BUL,0x87BF0C10UL,
		0x150F1FE0UL,0x18E0F03BUL,0x6FD07777UL,0xF82D0943UL,
		0x7B46DB8AUL,0xF600D4CCUL,0x4CF56A64UL,0x782134B4UL,
		0x8EBCCAD1UL,0x1C81D83FUL,0xCBEFDB61UL,0x615E5199UL,
		0xDF61B654UL,0x5180C88CUL,0x011B0AE7UL,0xC10CCD80UL,
		0x06279688UL,0x02047D1BUL,0x0E03FA46UL,0x03F114DDUL,
		0xAD5FE46EUL,0xF71E7D4FUL,0xDADF21A9UL,0x6F2FC142UL,
		0x54368184UL,0x9769ECF5UL,0x4FA0364DUL,0xFAD9AAB1UL,
		0x41DFE4D5UL,0xAA059FF2UL,0x58632936UL,0x2912AB92UL,
		0x8D07A868UL,0x2E8E74FCUL,0xED9AC2F6UL,0x719CA7CBUL,
		0x24B94241UL,0x51F01BF3UL,0x1FDDFB40UL,0x488EEFEEUL,
	}
};

static uint8_t gPkaRsaMessage[APP_PKA_RSA_MSG_SIZE_IN_BYTES] =
{
	0x33, 0xcd, 0x12, 0x5a, 0x18, 0x6d, 0xc4, 0x49, 0x89, 0xeb, 0x04, 0xc7,
  	0x66, 0x17, 0x05, 0x3f, 0xd7, 0x70, 0x8f, 0x1f, 0x27, 0x51, 0x7c, 0x5c,
  	0xb6, 0x0c, 0xd7, 0xa4, 0xa0, 0xb6, 0xf5, 0xd3, 0xd1, 0x55, 0x0d, 0x71,
  	0x35, 0x0b, 0x51, 0xf8, 0x9f, 0x00, 0x9d, 0xa9, 0x75, 0x92, 0x3c, 0x34,
  	0xc6, 0xba, 0x6d, 0x8d, 0xc6, 0xe5, 0xfe, 0x6d, 0x37, 0x8e, 0xab, 0x5a,
  	0x06, 0x2d, 0xd8, 0x84, 0xb2, 0x70, 0x93, 0xd9, 0xf9, 0xae, 0x5c, 0x18,
  	0x7b, 0x67, 0xd4, 0x36, 0x6f, 0xe3, 0x0f, 0xbc, 0x96, 0xcb, 0x3e, 0x21,
  	0x64, 0xe3, 0x20, 0xaa, 0x0b, 0xcb, 0x11, 0x40, 0xc1, 0xc3, 0x38, 0xf5,
  	0x08, 0x3c, 0x6b, 0xda, 0xb3, 0x9f, 0xb1, 0x42, 0x7d, 0xbb, 0x45, 0xb9,
  	0x4b, 0xe2, 0x03, 0xfd, 0x68, 0xdd, 0xd4, 0xc0, 0xdc, 0x06, 0xb1, 0x28,
  	0xa0, 0x4b, 0x28, 0x98, 0x24, 0x46, 0x49, 0xaa, 0xb5, 0xfe, 0x0f, 0x7a,
  	0x7e, 0x62, 0xe4, 0xac, 0xff, 0xa0, 0xcd, 0xbd, 0xd6, 0x4f, 0xab, 0x29,
  	0x74, 0xb8, 0xac, 0x7c, 0x0b, 0x0f, 0xdf, 0x47, 0xb2, 0x6c, 0xeb, 0x01,
  	0x6b, 0x0f, 0x56, 0xa8, 0x5d, 0xee, 0xbe, 0x8b, 0x00, 0xd4, 0x06, 0xdd,
  	0x3b, 0x62, 0x74, 0xbc, 0x61, 0x12, 0x2f, 0xd1, 0x98, 0xd4, 0xf3, 0x8d,
  	0x84, 0x19, 0x8e, 0xa5, 0xc3, 0x91, 0xab, 0xd6, 0xf1, 0xa4, 0xe4, 0xb9,
  	0x79, 0xf8, 0x96, 0x7a, 0xc0, 0xbc, 0x27, 0x46, 0x73, 0x1a, 0x85, 0xa2,
  	0x6b, 0xe1, 0xb3, 0x5b, 0x7a, 0x4a, 0xc1, 0x24, 0xdd, 0x2b, 0xcb, 0xba,
  	0xed, 0x7e, 0x23, 0x7a, 0x46, 0xef, 0xb1, 0x73, 0x20, 0xbe, 0x38, 0x5b,
  	0x4f, 0x25, 0xcc, 0x21, 0xc0, 0x42, 0x1b, 0x16, 0x11, 0xe3, 0xd2, 0x35,
  	0x89, 0x22, 0x1a, 0xc2, 0x83, 0x9e, 0x9e, 0x41, 0x05, 0x0a, 0xc1, 0xf8,
  	0xb9, 0xf6, 0x64, 0x88, 0xf8, 0x83, 0x30, 0xd0, 0x97, 0xe1, 0x7b, 0xe7,
  	0xe4, 0x76, 0x40, 0x8e, 0x6d, 0x1c, 0xb2, 0x04, 0x8a, 0x72, 0x69, 0x77,
  	0x19, 0x76, 0x77, 0xf7, 0x5b, 0x63, 0x66, 0x90, 0x05, 0x5f, 0x61, 0xc1,
  	0x21, 0xd0, 0xf4, 0xdb, 0x92, 0x45, 0xc5, 0x8e, 0x25, 0xf9, 0x1f, 0x32,
  	0x5e, 0x57, 0x7c, 0xf0, 0x9b, 0xc2, 0x38, 0x7a, 0xaa, 0x81, 0x3a, 0x09,
  	0x87, 0x77, 0x34, 0x14, 0x82, 0xf0, 0x4c, 0x40, 0xa7, 0xea, 0x49, 0x04,
  	0x07, 0x0c, 0xae, 0xf1, 0x1f, 0x9e, 0x38, 0xf9, 0x6e, 0x57, 0x39, 0x39,
  	0x53, 0xee, 0x37, 0x2e, 0xff, 0xd2, 0x27, 0x24, 0x25, 0xd4, 0xa1, 0xcf,
  	0x3a, 0x3c, 0x75, 0x20, 0x1f, 0xe0, 0xa1, 0x0b, 0xf5, 0x58, 0x10, 0xe7,
  	0xb6, 0xb9, 0x44, 0xdc, 0x17, 0xb2, 0xee, 0x74, 0x09, 0xbe, 0x9d, 0xc0,
  	0x50, 0x7c, 0x35, 0x1a, 0xaf, 0xb7, 0x9d, 0xc1, 0xf8, 0x52, 0x2e, 0x1b,
  	0x10, 0x65, 0x1b, 0x26, 0x08, 0x18, 0xcc, 0xc8, 0x07, 0x9a, 0x22, 0x13,
  	0xf6, 0xe1, 0x3c, 0x99, 0x11, 0xf4, 0x9f, 0x48, 0x83, 0x5b, 0x66, 0xfc,
  	0x96, 0x87, 0xd7, 0x90, 0xe6, 0x3f, 0x53, 0x69, 0xb5, 0xb2, 0xdc, 0xc4,
  	0xb3, 0x69, 0xdc, 0x21, 0xe4, 0x68, 0xd0, 0xd5, 0x7a, 0xa1, 0x77, 0xd5,
  	0x0e, 0x53, 0x5b, 0x2d, 0x2b, 0x71, 0x1d, 0x6c, 0xa2, 0x67, 0x07, 0x46,
  	0x55, 0xf9, 0x00, 0xf4, 0xfd, 0x3f, 0xcc, 0x81, 0x69, 0xda, 0x9c, 0x0a,
  	0x21, 0xa5, 0x59, 0xbe, 0x6d, 0x0b, 0x04, 0xd4, 0x9b, 0xfb, 0x3b, 0x24,
  	0x70, 0xe2, 0x9f, 0xc8, 0x7f, 0xc3, 0xc9, 0xc2, 0x44, 0x5a, 0x88, 0x0e,
  	0x80, 0x7e, 0x43, 0xec, 0xdb, 0xb5, 0x2d, 0x99, 0x23, 0xc1, 0xb0, 0x5f,
  	0x3d, 0xa7, 0x47, 0xba, 0x19, 0xbd, 0xfa, 0x4e, 0x2b, 0xbb, 0xf7, 0x05,
  	0x3f, 0xbe, 0x92, 0x08, 0xef, 0xf1, 0x2e, 0xa5
};

/* PKA handle for processing PKA api's */
AsymCrypt_Handle			gPkaHandle = NULL;
/* Crypto handle for processing SHA api's */
Crypto_Handle       gShaHandle;

/* Context memory */
static Crypto_Context gCryptoShaContext __attribute__ ((aligned (SA2UL_CACHELINE_ALIGNMENT)));
/* Context Object */
SA2UL_ContextObject  gSa2ulCtxObj __attribute__ ((aligned (SA2UL_CACHELINE_ALIGNMENT)));

/*Output test buffer for sha computation */
uint8_t gCryptoShaOutputBuf[APP_PKA_RSA_MSG_SIZE_IN_BYTES] __attribute__ ((aligned (SA2UL_CACHELINE_ALIGNMENT)));
uint8_t gCryptoShaHashBufForCompare[APP_SHA512_LENGTH];

int32_t sha512(uint8_t *inputBuf, uint32_t inputLength, uint8_t *output);
void app_getHashFormVerifyOutput(uint32_t *verifyOutput, uint8_t *hash, uint8_t typeOfAlgo);

void rsa_signing_verification(void *args)
{
    Drivers_open();
    Board_driversOpen();

    AsymCrypt_Return_t            status = ASYM_CRYPT_RETURN_SUCCESS;

    DebugP_log("[AsymCrypt] RSA Signing and Verification example started ...\r\n");

	gShaHandle = Crypto_open(&gCryptoShaContext);
    DebugP_assert(gShaHandle != NULL);

	/* Open PKA instance, enable PKA engine, Initialize clocks and Load PKA Fw */
    gPkaHandle = AsymCrypt_open(SA2UL_PKA_INSTANCE);
    DebugP_assert(gPkaHandle != NULL);

	/* Perform SHA operation */
	/* Openssl Command for SHA operations: openssl dgst -sha512 -binary -out sha512.dgt msg.bin */
	status = sha512(gPkaRsaMessage, sizeof(gPkaRsaMessage), gCryptoShaOutputBuf);
	DebugP_assert(SystemP_SUCCESS == status);

	/* Padding operation */
	Crypto_PKCSPaddingForSign(gCryptoShaOutputBuf, APP_PKA_RSA_KEY_SIZE_IN_BYTES, HASH_ALG_SHA2_512, gPkaRsaShaHashWithPadding);
	/* Uint8_t to Uint32_t conversion */
	Crypto_Uint8ToUint32(gPkaRsaShaHashWithPadding, APP_PKA_RSA_KEY_SIZE_IN_BYTES, gPkaRsaShaHashWith32BitFormate);
	/* Uint32_t to BigInt conversion */
	Crypto_Uint32ToBigInt(gPkaRsaShaHashWith32BitFormate, APP_PKA_RSA_KEY_SIZE_IN_WORDS, gPkaRsaShaHashWithBigIntFormate);

	/* Openssl Command for Sign: openssl rsautl -sign -inkey privkey.pem -in sha512.dgt -out sha512_signed.dgt */
	status = AsymCrypt_RSAPrivate(gPkaHandle, gPkaRsaShaHashWithBigIntFormate, &gPkaRsaPrivateKey, gPkaRsaSignOutputResult);
	DebugP_assert(ASYM_CRYPT_RETURN_SUCCESS == status);

	/* Openssl Command for Verify: openssl rsautl -verify -pubin -inkey pubkey.pem -in sha512_signed.dgt -out sha512_decrypted.dgt */
    status = AsymCrypt_RSAPublic(gPkaHandle, gPkaRsaSignOutputResult, &gPkaRsaPublicKey, gPkaRsaVerifyOutputResult);
	DebugP_assert(ASYM_CRYPT_RETURN_SUCCESS == status);

	/* Close PKA instance, disable PKA engine, deinitialize clocks*/
	status = AsymCrypt_close(gPkaHandle);
	DebugP_assert(ASYM_CRYPT_RETURN_SUCCESS == status);

	/* Close SHA instance */
    status = Crypto_close(gShaHandle);
    DebugP_assert(SystemP_SUCCESS == status);

	/* Extracting Message hash from verify output */
	app_getHashFormVerifyOutput(gPkaRsaVerifyOutputResult, gCryptoShaHashBufForCompare, HASH_ALG_SHA2_512);
	/* Comparing both hash results */
	if (0 != memcmp(gCryptoShaOutputBuf, gCryptoShaHashBufForCompare, sizeof(gCryptoShaHashBufForCompare)))
	{
		DebugP_log("[AsymCrypt] Verification output did not match expected output\n");
	}

    DebugP_log("[AsymCrypt] RSA Signing and Verification example completed!!\r\n");
    DebugP_log("All tests have passed!!\r\n");

    Board_driversClose();
    Drivers_close();

    return;
}

int32_t sha512(uint8_t *inputBuf, uint32_t inputLength, uint8_t *output )
{
    int32_t             status;

    SA2UL_ContextParams ctxParams;

    /* Configure secure context */
    ctxParams.opType    = SA2UL_OP_AUTH;
    ctxParams.hashAlg   = SA2UL_HASH_ALG_SHA2_512;
    ctxParams.inputLen  = inputLength;
    gSa2ulCtxObj.totalLengthInBytes = inputLength;

    status = SA2UL_contextAlloc(gCryptoShaContext.drvHandle, &gSa2ulCtxObj, &ctxParams);
    DebugP_assert(SystemP_SUCCESS == status);

    /* Perform SHA operation */
    status = SA2UL_contextProcess(&gSa2ulCtxObj,&inputBuf[0], inputLength, output);
    DebugP_assert(SystemP_SUCCESS == status);

    /*Function to free secure context configuration*/
    status = SA2UL_contextFree(&gSa2ulCtxObj);
    DebugP_assert(SystemP_SUCCESS == status);

	memcpy((void *)output, gSa2ulCtxObj.computedHash, APP_SHA512_LENGTH);

	return (status);
}

void app_getHashFormVerifyOutput(uint32_t *verifyOutput, uint8_t *hash, uint8_t typeOfAlgo)
{
	static uint32_t shaLenInWords;
	uint32_t temp[16];

	switch(typeOfAlgo)
	{
		case 0:
			shaLenInWords = 20/4;
			break;
		case 1:
			shaLenInWords = 32/4;
			break;
		case 2:
			shaLenInWords = 64/4;
			break;
	}

	Crypto_bigIntToUint32(verifyOutput, shaLenInWords, temp);

	Crypto_Uint32ToUint8(temp, (shaLenInWords*4), hash);

	return;
}