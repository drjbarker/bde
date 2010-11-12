// bslscm_version.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSCM_VERSION
#define INCLUDED_BSLSCM_VERSION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
//  bslscm_Version: namespace for RCS and SCCS versioning information for 'bde'
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bde' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bde' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'bslscm_Version' 'struct' can be used to query version
// information for the 'bde' package group at runtime.  The following usage
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// '#include' this component header file to get 'bde' version information
// embedded in an executable.  It is only necessary to use one or more 'bde'
// components (and, hence, link in the 'bde' library).
//
///Usage
///-----
// A program can display the version of BSL that was used to build it by
// printing the version string returned by 'bslscm_Version::version()' to
// 'stdout' as follows:
//..
//  std::printf("BSL version: %s\n", bslscm_Version::version());
//..

#ifndef INCLUDED_BSLS_BUILDTARGET
#include <bsls_buildtarget.h>      // need to ensure consistent build options
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLSCM_VERSIONTAG
#include <bslscm_versiontag.h> // BSL_VERSION_MAJOR, BSL_VERSION_MINOR
#endif

#ifndef bdema_Allocator
#define bdema_Allocator bslma_Allocator
    // These preposterous macro definitions are placed here, to preempt clients
    // forward-declaring 'bdema_Allocator' (pre-migration to 'bsl') from
    // declaring their own class that differs from 'bslma_Allocator'.
#endif

#ifndef BSL_LEGACY
#define BSL_LEGACY 1
    // This macro controls whether we allow features which we must continue to
    // support for our clients but do not want to rely on in our own code base.
    // Clients who want to continue using these features should define
    // 'BSL_LEGACY' as 1, which is the default unless it is already defined.
    // In order to make sure an entire code base does not rely on these
    // features, recompile with this macro set to 0.  Examples of such features
    // are: including '<stdheader>' as opposed to '<bsl_stdheader.h>', or using
    // 'DEBUG' instead of 'BAEL_DEBUG'.
#elif BSL_LEGACY == 0
#define BDE_DONT_ALLOW_TRANSITIVE_INCLUDES 1
    // When we don't want to rely on legacy features, we also want to make sure
    // we are not picking up macros or type aliases via (direct or transitive)
    // includes of headers that have migrated from 'bde' to 'bsl' libraries.
#endif

namespace BloombergLP {

struct bslscm_Version {
    static const char *d_ident;
    static const char *d_what;

#define BSLSCM_CONCAT2(a,b,c,d,e,f) a ## b ## c ## d ## e ## f
#define BSLSCM_CONCAT(a,b,c,d,e,f)  BSLSCM_CONCAT2(a,b,c,d,e,f)

// 'BSLSCM_D_VERSION' is a symbol whose name warns users of version mismatch
// linking errors.  Note that the exact string "compiled_this_object" must be
// present in this version coercion symbol.  Tools may look for this pattern to
// warn users of mismatches.

#define BSLSCM_D_VERSION BSLSCM_CONCAT(d_version_BSL_,       \
                                       BSL_VERSION_MAJOR, _, \
                                       BSL_VERSION_MINOR, _, \
                                       compiled_this_object)

    static const char *BSLSCM_D_VERSION;

    static const char *d_dependencies;
    static const char *d_buildInfo;
    static const char *d_timestamp;
    static const char *d_sourceControlInfo;

    static const char *version();
};

inline
const char *bslscm_Version::version()
{
    return BSLSCM_D_VERSION;
}

// Force linker to pull in this component's object file.

#if defined(BSLS_PLATFORM__CMP_IBM)
static const char **bslscm_version_assertion =
                                             &bslscm_Version::BSLSCM_D_VERSION;
#else
namespace {
    extern const char **const bslscm_version_assertion =
                                             &bslscm_Version::BSLSCM_D_VERSION;
}
#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
