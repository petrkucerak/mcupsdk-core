# SA2UL PKA ECDSA Signing and Verification Test {#EXAMPLES_DRIVERS_SA2UL_PKA_ECDSA_SIGN_VERIFY}

[TOC]

# Introduction

This example demonstrates the SA2UL PKA ECDSA Signing and Verification operations.

# Supported Combinations {#EXAMPLES_DRIVERS_SA2UL_PKA_ECDSA_SIGN_VERIFY_COMBOS}

\cond SOC_AM64X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 nortos
 Toolchain      | ti-arm-clang
 Board          | @VAR_BOARD_NAME_LOWER
 Example folder | examples/security/crypto/sa2ul_pka/ecdsa_signing_verification/ecdsa_signing_verification.c
\endcond

\cond SOC_AM243X

 Parameter      | Value
 ---------------|-----------
 CPU + OS       | r5fss0-0 nortos
 Toolchain      | ti-arm-clang
 Board          | @VAR_BOARD_NAME_LOWER, @VAR_LP_BOARD_NAME_LOWER
 Example folder | examples/security/crypto/sa2ul_pka/ecdsa_signing_verification/ecdsa_signing_verification.c
\endcond

# Steps to Run the Example

- **When using CCS projects to build**, import the CCS project for the required combination
  and build it using the CCS project menu (see \ref CCS_PROJECTS_PAGE).
- **When using makefiles to build**, note the required combination and build using
  make command (see \ref MAKEFILE_BUILD_PAGE)
- Launch a CCS debug session and run the executable, see \ref CCS_LAUNCH_PAGE

# See Also

\ref SECURITY_PKA_MODULE_PAGE

# Sample Output

Shown below is a sample output when the application is run.


CCS Console:
\code

[AsymCrypt] ECDSA Signing and Verification example started ...
[AsymCrypt] ECDSA Signing and Verification example completed!!
All tests have passed!!

\endcode