// bdepu_iso8601.h              -*-C++-*-
#ifndef INCLUDED_BDEPU_ISO8601
#define INCLUDED_BDEPU_ISO8601

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a utility for converting date/time objects to/from ISO8601.
//
//@CLASSES:
//     bdepu_Iso8601: namespace for ISO8601 date/time conversion functions
//
//@SEE_ALSO: bdet_date, bdet_datetime, bdet_datetimetz, bdet_datetz, bdet_time,
//           bdet_timetz, baexml_encoder, baexml_decoder
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a namespace, 'bdepu_Iso8601'
// containing functions that convert 'bdet' date and time objects to and from
// a string representation.  The string representation generated and parsed by
// this component adheres to the ISO8601 standard for dates and time and is
// suitable for use in XML generation or parsing.
//
// Each 'generate' function takes an 'bsl::ostream' and a 'bdet' date or time
// object and writes the ISO8601 representation to the stream.  Each 'parse'
// function takes a pointer to a 'bdet' date or time object, a character
// string, and a length and parses the character string into the date or time
// object, returning a non-zero status on error.
//
///Usage
///-----
// The following example illustrates how to generate an ISO 8601-compliant
// string from a 'bdet_DatetimeTz' value:
//..
//  const bdet_DatetimeTz theDatetime(bdet_Datetime(2005, 1, 31,
//                                                  8, 59, 59, 123), 240);
//  bsl::stringstream ss;
//  bdepu_Iso8601::generate(ss, theDatetime);
//  assert(ss.str() == "2005-01-31T08:59:59.123+04:00");
//..
// The following example illustrates how to parse an ISO 8601-compliant string
// into a 'bdet_DatetimeTz' object:
//..
//  bdet_DatetimeTz dateTime;
//  const char dtStr[] = "2005-01-31T08:59:59.1226-04:00";
//  int ret = bdepu_Iso8601::parse(&dateTime, dtStr, bsl::strlen(dtStr));
//  assert(0 == ret);
//  assert(2005 == dateTime.localDatetime().year());
//  assert(   1 == dateTime.localDatetime().month());
//  assert(  31 == dateTime.localDatetime().day());
//  assert(   8 == dateTime.localDatetime().hour());
//  assert(  59 == dateTime.localDatetime().minute());
//  assert(  59 == dateTime.localDatetime().second());
//  assert( 123 == dateTime.localDatetime().millisecond());
//  assert(-240 == dateTime.offset());
//..
// Note that fractions of a second was rounded up to 123 milliseconds and that
// the offset from GMT was converted to minutes.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

namespace BloombergLP {

class bdet_Date;
class bdet_Datetime;
class bdet_DatetimeTz;
class bdet_DateTz;
class bdet_Time;
class bdet_TimeTz;

                            // ====================
                            // struct bdepu_Iso8601
                            // ====================

struct bdepu_Iso8601 {
    // namespace for ISO8601 date/time conversion functions

    // TYPES
    enum {
        // Fixed lengths for iso8601-formatted date and time values:
        BDEPU_DATE_STRLEN         = 10,
        BDEPU_DATETIME_STRLEN     = 23,
        BDEPU_DATETIMETZ_STRLEN   = 29,
        BDEPU_DATETZ_STRLEN       = 16,
        BDEPU_TIME_STRLEN         = 12,
        BDEPU_TIMETZ_STRLEN       = 18,
        BDEPU_MAX_DATETIME_STRLEN = BDEPU_DATETIMETZ_STRLEN

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , DATE_STRLEN         = BDEPU_DATE_STRLEN
      , DATETIME_STRLEN     = BDEPU_DATETIME_STRLEN
      , DATETIMETZ_STRLEN   = BDEPU_DATETIMETZ_STRLEN
      , DATETZ_STRLEN       = BDEPU_DATETZ_STRLEN
      , TIME_STRLEN         = BDEPU_TIME_STRLEN
      , TIMETZ_STRLEN       = BDEPU_TIMETZ_STRLEN
      , MAX_DATETIME_STRLEN = BDEPU_MAX_DATETIME_STRLEN
#endif
    };

    // CLASS METHODS
    static int generate(char             *buffer,
                        const bdet_Date&  object,
                        int               bufferLength);
    static int generate(char                 *buffer,
                        const bdet_Datetime&  object,
                        int                   bufferLength);
    static int generate(char                   *buffer,
                        const bdet_DatetimeTz&  object,
                        int                     bufferLength);
    static int generate(char               *buffer,
                        const bdet_DateTz&  object,
                        int                 bufferLength);
    static int generate(char             *buffer,
                        const bdet_Time&  object,
                        int               bufferLength);
    static int generate(char               *buffer,
                        const bdet_TimeTz&  object,
                        int                 bufferLength);
        // Write the ISO8601 representation of the specified 'object' to
        // specified 'buffer' truncating (if necessary) to the specified
        // 'bufferLength', and return the length of the formatted string
        // before truncation (not including a null terminator).  If
        // 'bufferLength' is larger than necessary to contain the string
        // representation of 'object', then a null terminator is appended to
        // the output.  The behavior is undefined unless '0 <= bufferLength'.
        // Note that, for each type of 'object', the return value is always the
        // same (as enumerated in the '_STRLEN' constants).  Also note that a
        // buffer of size 'BDEPU_MAX_DATETIME_STRLEN + 1' is large enough to
        // hold any formatted string, including the null terminator.

    static int generateRaw(char             *buffer,
                           const bdet_Date&  object);
    static int generateRaw(char                 *buffer,
                           const bdet_Datetime&  object);
    static int generateRaw(char                   *buffer,
                           const bdet_DatetimeTz&  object);
    static int generateRaw(char               *buffer,
                           const bdet_DateTz&  object);
    static int generateRaw(char             *buffer,
                           const bdet_Time&  object);
    static int generateRaw(char               *buffer,
                           const bdet_TimeTz&  object);
        // Write the ISO8601 representation of the specified 'object' to
        // specified 'buffer' and return the length of the formatted string
        // (not including a null terminator).  The behavior is undefined unless
        // 'buffer' holds enough characters.  Note that, for each type of
        // 'object', the return value is always the same (as enumerated in the
        // '_STRLEN' constants).  Also note that a buffer of size
        // 'BDEPU_MAX_DATETIME_STRLEN + 1' is large enough to hold any
        // formatted string, including any null terminator.  It is unspecified
        // whether the 'buffer' is null terminated on output or not.

    template <typename DATE_TYPE>
    static bsl::ostream& generate(bsl::ostream&    stream,
                                  const DATE_TYPE& object);
        // Write the ISO8601 representation of the specified 'object' to
        // specified 'stream'.  Return a reference to the modifiable
        // 'stream'.

    static int parse(bdet_Date  *result,
                     const char *input,
                     int         inputLength);
    static int parse(bdet_Datetime *result,
                     const char    *input,
                     int            inputLength);
    static int parse(bdet_DatetimeTz *result,
                     const char      *input,
                     int              inputLength);
    static int parse(bdet_DateTz *result,
                     const char  *input,
                     int          inputLength);
    static int parse(bdet_Time  *result,
                     const char *input,
                     int         inputLength);
    static int parse(bdet_TimeTz *result,
                     const char  *input,
                     int          inputLength);
        // Parse the string beginning at the specified 'input' and having the
        // specified 'inputLength' and load the result into the specified
        // 'result'.  Return 0 on success, and non-zero otherwise.  Do not
        // modify 'result' on failure.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

template <typename DATE_TYPE>
inline
bsl::ostream& bdepu_Iso8601::generate(bsl::ostream&    stream,
                                      const DATE_TYPE& object)
{
    char buffer[BDEPU_MAX_DATETIME_STRLEN + 1];
    int len = generate(buffer, object, BDEPU_MAX_DATETIME_STRLEN);
    BSLS_ASSERT_SAFE(BDEPU_MAX_DATETIME_STRLEN >= len);
    stream.write(buffer, len);
    return stream;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
