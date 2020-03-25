/*
 * FreeRTOS V202002.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

/*
 * @brief PEM-encoded client certificate.
 *
 * @todo If you are running one of the FreeRTOS demo projects, set this
 * to the certificate that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyCLIENT_CERTIFICATE_PEM                   ""

#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

#include <stdint.h>

/*
 * PEM-encoded client certificate.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWTCCAkGgAwIBAgIUU+a92WtxVtuc7i+gW3NeaSkYfo0wDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MTExNDAxMTUx\n"\
"MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALvu/fcXko/NFcNnltcv\n"\
"caSCxhB/0efLKGXSGXi7BUuOmylCgU+3K8eCsBs03xyUndqdwgwOVMy4lgDPoTPA\n"\
"Whpcw9wyu7dx0jd/Bd3MpUP3yssXDltI6lUJA0dOKDdllBINQU3LT1gQhvuZA9vX\n"\
"WYHbX66dHKjPddb5uh14kcatypTeTv8mKOueEw7abb8maMbaRq9ce7zDl4Wchtge\n"\
"8LymNuz6eEIQeFmNxkkj8BtwbsB5v21BebVqZat8E020JZX5/GK2PlYNQaHW/C/d\n"\
"2KNHsIFbezqVcQsnP26hs8L2BsnQA2SHKRejeh3vwb4aEhk8qI5i6rJm49Rbb6az\n"\
"1Y0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUXdayOAnfvowDoualhhsdEA7Z74kwHQYD\n"\
"VR0OBBYEFBfgUJ4NTxUqb9WaT1fWxMYQbbFeMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCmCtbdJyZhjVl4FayzyE9tN+We\n"\
"pzPTEbwoyZqKjn74lZs7lF61gI0010jEguvZ83EO5XAnIQ4lXZErXJ9A4kI23Yk2\n"\
"2vwvaANGDpRooEOOmSLzZ8tqhgkFWyv1EZeRhoXfkwIEHJXLvZiDvvdYXP864jzT\n"\
"1SesGt0ocqOOgkqPmFHCjBnTlN5u5LT+bGkFeymS6l7XzlT9op6/XI0WmbT6O2h0\n"\
"2rJiJS7zpCyqKP5tq98uf8vZT75ZaGzsuSaOS8vcQHE3IFn3EZhwXwCn7tOWnPeX\n"\
"oaCoabB8Iiw3BNJiMBDL2BIKXm3FDSE53ZS6xvffaF06b0v2b+dYS2N2Dg51\n"\
"-----END CERTIFICATE-----"

 /*
  * PEM-encoded client private key.
  *
  * Must include the PEM header and footer:
  * "-----BEGIN RSA PRIVATE KEY-----\n"\
  * "...base64 data...\n"\
  * "-----END RSA PRIVATE KEY-----"
  */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEpAIBAAKCAQEAu+799xeSj80Vw2eW1y9xpILGEH/R58soZdIZeLsFS46bKUKB\n"\
"T7crx4KwGzTfHJSd2p3CDA5UzLiWAM+hM8BaGlzD3DK7t3HSN38F3cylQ/fKyxcO\n"\
"W0jqVQkDR04oN2WUEg1BTctPWBCG+5kD29dZgdtfrp0cqM911vm6HXiRxq3KlN5O\n"\
"/yYo654TDtptvyZoxtpGr1x7vMOXhZyG2B7wvKY27Pp4QhB4WY3GSSPwG3BuwHm/\n"\
"bUF5tWplq3wTTbQllfn8YrY+Vg1Bodb8L93Yo0ewgVt7OpVxCyc/bqGzwvYGydAD\n"\
"ZIcpF6N6He/BvhoSGTyojmLqsmbj1FtvprPVjQIDAQABAoIBAQCG65s0a53y0x+O\n"\
"u63VKuCBZMlhSwKdUILeJqnWvR2eEvFd+7ruSeiKQU7BGYIb+K4jJyMKmLdUiihW\n"\
"yLlIeA2342VRjrKpjnOHGBBUHfmABEN47kKGUqC5D9Fkvb2tl6VoGFAL06xQ4vyl\n"\
"YEMe70n5Kk9VAsJyhw5ombx+L/MoGJtimBpRlh2TJ8cKKpJGOTrIVkZs6QlZwE7P\n"\
"uYr17HZzbHjdoBN7xWWTsrtH4Sg+ngHuak1GHL2U41mju4yrkkw6LWswDDbA6LhN\n"\
"IGRUz2rP+xBCFQi2N/ry+MAPe439e/tDBCQ3Co4IAQs1IDJcG8hJ3DUg6l0O+D6Z\n"\
"Uns1CS8ZAoGBAPjp5CAMjy71rDTa8djVykPInKg5KrGDsAhmiU0of8jVlSd8PxVS\n"\
"g9Zh8LLjE0GSGy8YtVT3Y1FHPjhy6IT2V8zbdzBLDnBm+CrmNTV0T0hvhqyg6dug\n"\
"XWj6R6WJAd3CITsTz7P5BF/nfmrUjN4vEP6Kf1YWZByYCUelxWAqF84fAoGBAMFI\n"\
"q/VUMwS/sMJ5wzBH8D5cjLHTRYG7v/MEsfJxt4eT/77TX19uu91B7OPlL9CLR45a\n"\
"GdHexUW6EdmhUgh22+Vl6naHootGepnMDJYjRmVJY1zt2rfGhtrKnBggm2uzqrX5\n"\
"XUIHU54G38zAwS9Apr5+ntuheDSM1m+L1BibyM7TAoGBANDyVeNpZ5B9/i7oU/g3\n"\
"jvDTqF22j1YwO0bx+/PfOVV3pRteOv5FJBKEaK9h2rpCUbt5PE601JRX/60XGIT0\n"\
"3cNDv4u0lcxZQ52PXb8fgaZGqIClG2qX9ZG39wjIN97VpVuYR9TGBpwTEZXQEug4\n"\
"DkX+MOItLaCrJ68TQtp0u0L1AoGAcWprPPHDXi39UqCQYGxWFMHJ7S7DwnShFcDT\n"\
"iiggQXYew6LUe/NGWXatwqxhekXAH0/Phs7pgxjEEKgQstbMAbfj9NlyD7eziTuq\n"\
"xw676ANAQu9vQBdGWmw5XA8GJBSxP0ar6mrSLhBqkoMo2MlfDrjODqKZHnzl915d\n"\
"fnMrSfECgYBg0uBE7FlL6BFJmu6E/liP5837JEPe5hFcTlikEVsy/VFufThbf2p6\n"\
"RUV28AOXoX6ZKufSBDP4FvjuAeJTGjIse64nSCtYrrkKjspyzRP0MyqL8ZOE28zj\n"\
"2duF/pw9scYQVbPSeZSmEGupsxge/ABEEt7rbx5fz4GNpOAMbUO5Mw==\n"\
"-----END RSA PRIVATE KEY-----"

  /*
   * PEM-encoded Just-in-Time Registration (JITR) certificate (optional).
   *
   * If used, must include the PEM header and footer:
   * "-----BEGIN CERTIFICATE-----\n"\
   * "...base64 data...\n"\
   * "-----END CERTIFICATE-----"
   */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  ""

/*
 * @brief PEM-encoded client private key.
 *
 * @todo If you are running one of the FreeRTOS demo projects, set this
 * to the private key that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM                   ""

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
