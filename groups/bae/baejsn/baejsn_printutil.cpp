// baejsn_printutil.cpp                                               -*-C++-*-
#include <baejsn_printutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_printutil_cpp,"$Id$ $CSID$")

#include <bdede_base64encoder.h>
#include <bsl_sstream.h>

namespace BloombergLP {

namespace {

char getEscapeChar(char value)
    // Return the JSON escape character for the specified 'value' if 'value'
    // needs to be escaped, return 0 otherwise.
{

    switch (value) {
      case '"':
      case '\\':
      case '/': {
        return value;                                                 // RETURN
      }
      case '\b': {
        return 'b';                                                   // RETURN
      }
      case '\f': {
        return 'f';                                                   // RETURN
      }
      case '\n': {
        return 'n';                                                   // RETURN
      }
      case '\r': {
        return 'r';                                                   // RETURN
      }
      case '\t': {
        return 't';                                                   // RETURN
      }
      default: {
        if (value < 32) {
            // control characters

            return 'u';                                               // RETURN
        }
      }
    }
    return 0;
}

}  // close unnamed namespace

                           // ----------------------
                           // class baejsn_PrintUtil
                           // ----------------------

int baejsn_PrintUtil::printValueImp(bsl::ostream& stream, char value)
{
    bsl::string str = "\"";

    char ch = getEscapeChar(value);
    if (0 != ch) {
        str += '\\';
        str += ch;
        if ('u' == ch) {
            str += "00";
            bsl::ostringstream oss;
            oss << bsl::hex << bsl::setfill('0') << bsl::setw(2)
                << (static_cast<unsigned int>(value) & 0xff);
            str += oss.str();
        }
    }
    else {
        str += value;
    }
    str += '"';

    stream << str;
    return 0;
}

int baejsn_PrintUtil::printString(bsl::ostream&            stream,
                                  const bslstl::StringRef& value)
{
    stream << '"';

    for (bslstl::StringRef::const_iterator it = value.begin();
         it != value.end();
         ++it) {
        bsl::string str;

        char ch = getEscapeChar(*it);
        if (0 != ch) {
            str += '\\';
            str += ch;
            if ('u' == ch) {
                bslstl::StringRef::const_iterator next = it;
                ++next;

                bsl::ostringstream oss;

                if (*it == 0 || next == value.end())
                {
                    oss << "00"
                        << bsl::hex << bsl::setfill('0') << bsl::setw(2)
                        << (static_cast<unsigned int>(*it) & 0xff);
                }
                else
                {
                    oss << bsl::hex << bsl::setfill('0') << bsl::setw(2)
                        << (static_cast<unsigned int>(*it) & 0xff)
                        << bsl::hex << bsl::setfill('0') << bsl::setw(2)
                        << (static_cast<unsigned int>(*next) & 0xff);
                    ++it;
                }
                str += oss.str();
            }
        }
        else {
            str += *it;
        }

        stream << str;
    }

    stream << '"';
    return 0;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
