/*
 * File:   main.cpp
 * Author: Saleem Edah-Tally - nmset@yandex.com
 * License: CeCILL
 * Copyright: Saleem Edah-Tally - © 2023
 *
 * Created on 20 september 2023, 18:31
 */

#include <iostream>
#include <getopt.h>
#include <utf8proc.h>
#include <format>
#include <map>
#include <libintl.h>

using namespace std;

#define STRIP_OPTIONS_DEFAULT (UTF8PROC_IGNORE | UTF8PROC_STRIPCC | UTF8PROC_STRIPMARK | UTF8PROC_STRIPNA | UTF8PROC_DECOMPOSE | UTF8PROC_STABLE | UTF8PROC_NULLTERM)
//https://www.labri.fr/perso/fleury/posts/programming/a-quick-gettext-tutorial.html
#define _(STRING) gettext(STRING)
#define _E(STRING) string(getenv("UTF8UTIL_RESULT_ONLY") != NULL ? "" : STRING)
#define _APPNAME_ "utf8util"
#define _APPVERSION_ 2

typedef map<int, string> KeyValuePair;
// Described in utf8proc.h.
KeyValuePair categoryDescription;
KeyValuePair bidirectional;
KeyValuePair decompositionType;
KeyValuePair boundClass;

string valueRepresentation(long nb, int baseHint) {
    // https://en.cppreference.com/w/cpp/utility/format/formatter
    string formatted;
    switch (baseHint)
    {
        case 2:
            formatted = std::format("{:08b}", nb);
            break;
        case 8:
            formatted = std::format("{}{:03o}", "\\", nb);
            break;
        case 10:
            formatted = std::format("{:d}", nb);
            break;
        case 16:
            formatted = std::format("{}{:0X}","0x", nb);
            break;
        case 17: // enforce 4-byte representation for UTF-16, even if codepoint < 0xFFFF.
            formatted = std::format("{}{:04X}","0x", nb);
            break;
        case 'U':
            formatted = std::format("{}{:04X}","U+", nb);
            break;
        case 'x':
            formatted = std::format("{}{:d}{}","&#", nb, ";");
            break;
        default:
            cout << _("Unhandled base: ") << baseHint << endl;
            return "";
    }
    
    return formatted;
}

void unaccentShowHelp()
{
    string message = _("This operational mode removes character markings, control characters, default ignorable characters and unassigned codepoints from an UTF-8 input."
    "The utf8proc library, on which this utility is based, refers to character markings as 'non-spacing, spacing and enclosing (accents)' marks, and to default ignorable characters 'such as SOFT-HYPHEN or ZERO-WIDTH-SPACE'. Control characters are stripped or converted to spaces."
    "\n\nBy default, every removable byte is stripped, the output characters are decomposed and Unicode Versioning Stability is enforced."
    "\n\n  -i, --ignore: do not strip 'default ignorable characters'"
    "\n  -c, --control: do not handle 'control characters'"
    "\n  -m, --mark: do not strip 'character markings'"
    "\n  -n, --na: do not strip 'unassigned codepoints'"
    "\n  -r, --recompose: output recomposed characters"
    "\n  -h, --help: show this message"
    "\n\nThe input can be piped in or read from stdin. It must be a single NULL terminated line.");
    
    cout << message << endl;
}

void normalizeShowHelp()
{
    string message = _("This operational mode normalizes the input string according to the specified type, the default being NFC."
    "\n\n  -t, --type: one of NFC, NFD, NFKC, NFKD, NFKC_Casefold"
    "\n  -h, --help: show this message"
    "\n\nThe input can be piped in or read from stdin. It must be a single NULL terminated line.");
    
    cout << message << endl;
}

void representationShowHelp()
{
    string message = _("This operational mode displays representations of the first identified codepoint."
    "\n\n  -p, --codepoint: hexadecimal representation of the codepoint"
    "\n  -e, --utf8: hexadecimal representation of each byte"
    "\n  -s, --utf16: hexadecimal representation of each surrogate"
    "\n  -b, --binary: binary representation of each byte"
    "\n  -o, --octal: octal representation of each byte"
    "\n  -d, --decimal: decimal representation of each byte"
    "\n  -x, --xml: XML decimal representation of each byte"
    "\n  -L, --tolower: displays the codepoint as a lower-case character if existent"
    "\n  -U, --toupper: displays the codepoint as an upper-case character if existent"
    "\n  -T, --totitle: displays the codepoint as a title-case character if existent"
    "\n  -h, --help: show this message"
    "\n\nThe input can be piped in or read from stdin. Pass in a single character for simplicity."
    "\nIf the environment variable 'UTF8UTIL_RESULT_ONLY' is set, only the result is printed on stdout.");
    
    cout << message << endl;
}

void propertiesShowHelp()
{
    string message = _("This operational mode displays properties of the first identified codepoint."
    "\n\n  -l, --islower: displays 1 if the codepoint refers to a lower-case character, 0 otherwise"
    "\n  -u, --isupper: displays 1 if the codepoint refers to an upper-case character, 0 otherwise"
    "\n  -c, --category: determines the category of a codepoint (Letter, Number, Symbol...)"
    "\n  -d, --direction: determines the bidirectional class of a codepoint; see utf8proc.h"
    "\n  -i, --decompositiontype: determines the decomposition type of a codepoint; see utf8proc.h"
    "\n  -b, --boundclass: determines the boundclass property of a codepoint; see utf8proc.h"
    "\n  -h, --help: show this message"
    "\n\nThe input can be piped in or read from stdin. Pass in a single character for simplicity."
    "\nIf the environment variable 'UTF8UTIL_RESULT_ONLY' is set, only the result is printed on stdout.");
    
    cout << message << endl;
}

int unaccent(int argc, char **argv) {
    string input;
    int options  = STRIP_OPTIONS_DEFAULT;
    
    // Use : --longopt=<val> -s <val>
    option longopts[] = {
        {"ignore", no_argument, 0, 'i'}, 
        {"control", no_argument, 0, 'c'}, 
        {"mark", no_argument, 0, 'm'},
        {"na", no_argument, 0, 'n'},
        {"recompose", no_argument, 0, 'r'},
        {"help", no_argument, 0, 'h'},
        {0}};
        
    while (1) {
        const int opt = getopt_long(argc, argv, "icmnrh", longopts, 0);
        
        if (opt == -1) {
            break;
        }
        
        switch (opt) {
            case 'i':
                options ^= UTF8PROC_IGNORE;
                break;
            case 'c':
                options ^= UTF8PROC_STRIPCC;
                break;
            case 'm':
                options ^= UTF8PROC_STRIPMARK;
                break;
            case 'n':
                options ^= UTF8PROC_STRIPNA;
                break;
            case 'r':
                // Interestingly, UTF8PROC_COMPOSE gives the same result as UTF8PROC_DECOMPOSE.
                // Probably the options mean : decompose, strip, compose ?
                options ^= UTF8PROC_DECOMPOSE;
                options |= UTF8PROC_COMPOSE;
                break;
            case 'h':
                unaccentShowHelp();
                return 0;
            case '?':
                return 30; // -5 to -1 are reserved by utf8proc; their absolute values are used here.
            default:
                options  = STRIP_OPTIONS_DEFAULT;
                break;
        }
    }
    
    //string fragment;
    // while (cin >> fragment)
    //     input += fragment + " ";
    // input.pop_back();
    std::getline(cin, input);
    utf8proc_uint8_t * result;
    utf8proc_ssize_t nb = utf8proc_map((const utf8proc_uint8_t *) input.data(),
                                        0, // Without UTF8PROC_NULLTERM, is number of bytes to process from input.
                                        &result,
                                        utf8proc_option_t (options)
    );
    if (nb < 0) // an error occured
    {
        cout << utf8proc_errmsg(nb) << endl;
        if (result)
            free((void*) result);
        return nb * -1;
    }
    
    cout << (const char*) result << endl;
    free((void*) result);
    
    return 0;
}

int normalize(int argc, char ** argv)
{
    string input;
    string type("NFC");
    
    // Use : --longopt=<val> -s <val>
    option longopts[] = {
        {"type", required_argument, 0, 't'}, 
        {"help", no_argument, 0, 'h'},
        {0}};
    
    while (1) {
        const int opt = getopt_long(argc, argv, "t:h", longopts, 0);
        
        if (opt == -1) {
            break;
        }
        
        switch (opt) {
            case 't':
                type = optarg;
                break;
            case 'h':
                normalizeShowHelp();
                return 0;
            case '?':
                return 40; // -5 to -1 are reserved by utf8proc; their absolute values are used here.
            default:
                type = "NFC";
                break;
        }
    }
    
    std::getline(cin, input);
    utf8proc_uint8_t * result = NULL;
    
    if (type == "NFC")
    {
        result = utf8proc_NFC((const utf8proc_uint8_t*) input.c_str());
    }
    else if (type == "NFD")
    {
        result = utf8proc_NFD((const utf8proc_uint8_t*) input.c_str());
    }
    else if (type == "NFKC")
    {
        result = utf8proc_NFKC((const utf8proc_uint8_t*) input.c_str());
    }
    else if (type == "NFKD")
    {
        result = utf8proc_NFKD((const utf8proc_uint8_t*) input.c_str());
    }
    else if (type == "NFKC_Casefold")
    {
        result = utf8proc_NFKC_Casefold((const utf8proc_uint8_t*) input.c_str());
    }
    else
    {
        cout << _("Unknown type; valid types are NFC, NFD, NFKC, NFKD and NFKC_Casefold.") << endl;
        return 41;
    }
    
    if (result)
    {
        cout << (const char*) result << endl;
        free((void*) result);
    }
    
    return 0;
}

int representation(int argc, char ** argv)
{
    for (uint i = 0; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            representationShowHelp();
            return 0;
        }
    }
    
    utf8proc_int32_t codepoint = 0;
    string input;
    cin >> input;
    const utf8proc_uint8_t * inputArray = (const utf8proc_uint8_t *) input.c_str();
    // This stops at the first codepoint; with 'عَ', the first retrieved codepoint is 'ع'.
    utf8proc_ssize_t nb = utf8proc_iterate(&inputArray[0], -1, &codepoint);
    if (nb < 0)
    {
        cout << utf8proc_errmsg(nb) << endl;
        return nb * -1;
    }
    utf8proc_uint8_t firstCharArray[5];
    utf8proc_ssize_t nbOfBytesInFirstChar = utf8proc_encode_char(codepoint, firstCharArray);
    if (nbOfBytesInFirstChar == 0)
    {
        cout << _("No valid bytes at start of input.") << endl;
        return 51;
    }
    firstCharArray[nbOfBytesInFirstChar] = '\0';
    
    option longopts[] = {
        {"codepoint", no_argument, 0, 'p'}, 
        {"utf8", no_argument, 0, 'e'},  // 'e'ight
        {"utf16", no_argument, 0, 's'}, // 's'ixteen
        {"binary", no_argument, 0, 'b'},
        {"octal", no_argument, 0, 'o'},
        {"decimal", no_argument, 0, 'd'},
        {"xml", no_argument, 0, 'x'},
        {"tolower", no_argument, 0, 'L'},
        {"toupper", no_argument, 0, 'U'},
        {"totitle", no_argument, 0, 'T'},
        {0}};
        
    while (1) {
        const int opt = getopt_long(argc, argv, "pesbodxLUT", longopts, 0);
        
        if (opt == -1) {
            break;
        }
        
        switch (opt) {
            case 'p':
                cout << _E(_("Codepoint: ")) << valueRepresentation(codepoint, 'U') << endl;
                break;
            case 'e':
                cout << _E("UTF-8: ");
                for (uint i = 0; i < nbOfBytesInFirstChar; i++)
                {
                    cout << valueRepresentation(firstCharArray[i], 16) << " ";
                }
                cout << endl;
                break;
            case 's':
                // https://en.wikipedia.org/wiki/UTF-16
                cout << _E("UTF-16: ");
                if (codepoint < 0xFFFF) // 2 bytes only
                {
                    cout << valueRepresentation(codepoint, 17);
                }
                else // 4 bytes
                {
                    utf8proc_int32_t intermediate = codepoint - 0x10000;
                    utf8proc_int32_t shifted = intermediate >> 10; // Divide by 0x400 (1024)(2^10)
                    utf8proc_int32_t highSurrogate = shifted +  0xD800;
                    utf8proc_int32_t lowTenBits = intermediate % 0x400; // Same result with (intermediate & 1023)
                    utf8proc_int32_t lowSurrogate = lowTenBits + 0xDC00;
                    cout << valueRepresentation(highSurrogate, 17) << " ";
                    cout << valueRepresentation(lowSurrogate, 17);
                }
                cout << endl;
                break;
            case 'b':
                cout << _E(_("Binary: "));
                for (uint i = 0; i < nbOfBytesInFirstChar; i++)
                {
                    cout << valueRepresentation(firstCharArray[i], 2) << " ";
                }
                cout << endl;
                break;
            case 'o':
                cout << _E(_("Octal: "));
                for (uint i = 0; i < nbOfBytesInFirstChar; i++)
                {
                    cout << valueRepresentation(firstCharArray[i], 8) << " ";
                }
                cout << endl;
                break;
            case 'd':
                cout << _E(_("Decimal: "));
                for (uint i = 0; i < nbOfBytesInFirstChar; i++)
                {
                    cout << valueRepresentation(firstCharArray[i], 10) << " ";
                }
                cout << endl;
                break;
            case 'x':
                cout << _E(_("XML decimal: ")) << valueRepresentation(codepoint, 'x') << endl;
                break;
            case 'L':
            {
                utf8proc_int32_t lowerCodepoint = utf8proc_tolower(codepoint);
                utf8proc_uint8_t dst[5];
                utf8proc_ssize_t bytesWritten = utf8proc_encode_char(lowerCodepoint, &dst[0]);
                dst[bytesWritten] = '\0';
                cout << _E(_("To lower: ")) << (const char*) dst << endl;
            }
                break;
            case 'U':
            {
                utf8proc_int32_t upperCodepoint = utf8proc_toupper(codepoint);
                utf8proc_uint8_t dst[5];
                utf8proc_ssize_t bytesWritten = utf8proc_encode_char( upperCodepoint, &dst[0]);
                dst[bytesWritten] = '\0';
                cout << _E(_("To upper: ")) << (const char*) dst << endl;
            }
                break;
            case 'T':
            {
                utf8proc_int32_t upperCodepoint = utf8proc_totitle(codepoint);
                utf8proc_uint8_t dst[5];
                utf8proc_ssize_t bytesWritten = utf8proc_encode_char( upperCodepoint, &dst[0]);
                dst[bytesWritten] = '\0';
                cout << _E(_("To title: ")) << (const char*) dst << endl;
            }
                break;
            case 'h':
                representationShowHelp();
                return 0;
            case '?':
                return 50; // -5 to -1 are reserved by utf8proc; their absolute values are used here.
            default:
                break;
        }
    }
    // Show the processed character.
    cout << _E(_("Character: ")) << (const char*) firstCharArray << endl;
    return 0;
}

int properties(int argc, char ** argv)
{
    for (uint i = 0; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            propertiesShowHelp();
            return 0;
        }
    }
    
    utf8proc_int32_t codepoint = 0;
    string input;
    cin >> input;
    const utf8proc_uint8_t * inputArray = (const utf8proc_uint8_t *) input.c_str();
    // This stops at the first codepoint; with 'عَ', the first retrieved codepoint is 'ع'.
    utf8proc_ssize_t nb = utf8proc_iterate(&inputArray[0], -1, &codepoint);
    if (nb < 0)
    {
        cout << utf8proc_errmsg(nb) << endl;
        return nb * -1;
    }
    utf8proc_uint8_t firstCharArray[5];
    utf8proc_ssize_t nbOfBytesInFirstChar = utf8proc_encode_char(codepoint, firstCharArray);
    if (nbOfBytesInFirstChar == 0)
    {
        cout << _("No valid bytes at start of input.") << endl;
        return 51;
    }
    firstCharArray[nbOfBytesInFirstChar] = '\0';
    
    option longopts[] = {
        {"islower", no_argument, 0, 'l'},
        {"isupper", no_argument, 0, 'u'},
        {"category", no_argument, 0, 'c'},
        {"direction", no_argument, 0, 'd'},
        {"decompositiontype", no_argument, 0, 'i'}, // decompos'i'tion
        {"boundclass", no_argument, 0, 'b'},
        {0}};
        
        while (1) {
            const int opt = getopt_long(argc, argv, "lucdib", longopts, 0);
            
            if (opt == -1) {
                break;
            }
            
            switch (opt) {
                case 'l':
                    cout << _E(_("Is lower: ")) << utf8proc_islower(codepoint) << endl;
                    break;
                case 'u':
                    cout << _E(_("Is upper: ")) << utf8proc_isupper(codepoint) << endl;
                    break;
                case 'c':
                {
                    utf8proc_category_t category = utf8proc_category(codepoint);
                    cout << _E(_("Category: ")) << "[" << utf8proc_category_string(codepoint) << "] ";
                    cout << categoryDescription[category] << endl;
                }
                break;
                case 'd':
                {
                    const utf8proc_property_t * property = utf8proc_get_property(codepoint);
                    cout << _E(_("Direction: ") )<< bidirectional[property->bidi_class] << endl;
                }
                break;
                case 'i':
                {
                    const utf8proc_property_t * property = utf8proc_get_property(codepoint);
                    cout << _E(_("Decomposition type: ")) << decompositionType[property->decomp_type] << endl;
                }
                break;
                case 'b':
                {
                    // property->boundclass is 1 (other) on all tested characters.
                    const utf8proc_property_t * property = utf8proc_get_property(codepoint);
                    cout << _E(_("Bound class: ")) << boundClass[property->boundclass] << endl;
                }
                break;
                case 'h':
                    representationShowHelp();
                    return 0;
                case '?':
                    return 50; // -5 to -1 are reserved by utf8proc; their absolute values are used here.
                default:
                    break;
            }
        }
        // Show the processed character.
        cout << _E(_("Character: ")) << (const char*) firstCharArray << endl;
        return 0;
}

int main(int argc, char ** argv)
{
    setlocale (LC_ALL, "");
    // TODO: Avoid hardcoding the path
    bindtextdomain (_APPNAME_, "/usr/local/share/locale"); // containing <language_code>/LC_MESSAGES/
    textdomain (_APPNAME_);
    
    // Translatable, but we won't do it on our own.
    categoryDescription[UTF8PROC_CATEGORY_CN] = _("Other, not assigned");
    categoryDescription[UTF8PROC_CATEGORY_LU] = _("Letter, uppercase");
    categoryDescription[UTF8PROC_CATEGORY_LL] = _("Letter, lowercase");
    categoryDescription[UTF8PROC_CATEGORY_LT] = _("Letter, titlecase");
    categoryDescription[UTF8PROC_CATEGORY_LM] = _("Letter, modifier");
    categoryDescription[UTF8PROC_CATEGORY_LO] = _("Letter, other");
    categoryDescription[UTF8PROC_CATEGORY_MN] = _("Mark, nonspacing");
    categoryDescription[UTF8PROC_CATEGORY_MC] = _("Mark, spacing combining");
    categoryDescription[UTF8PROC_CATEGORY_ME] = _("Mark, enclosing");
    categoryDescription[UTF8PROC_CATEGORY_NL] = _("Number, letter");
    categoryDescription[UTF8PROC_CATEGORY_NO] = _("Number, other");
    categoryDescription[UTF8PROC_CATEGORY_PC] = _("Punctuation, connector");
    categoryDescription[UTF8PROC_CATEGORY_PD] = _("Punctuation, dash");
    categoryDescription[UTF8PROC_CATEGORY_PS] = _("Punctuation, open");
    categoryDescription[UTF8PROC_CATEGORY_PE] = _("Punctuation, close");
    categoryDescription[UTF8PROC_CATEGORY_PI] = _("Punctuation, initial quote");
    categoryDescription[UTF8PROC_CATEGORY_PF] = _("Punctuation, final quote");
    categoryDescription[UTF8PROC_CATEGORY_PO] = _("Punctuation, other");
    categoryDescription[UTF8PROC_CATEGORY_SM] = _("Symbol, math");
    categoryDescription[UTF8PROC_CATEGORY_SC] = _("Symbol, currency");
    categoryDescription[UTF8PROC_CATEGORY_SK] = _("Symbol, modifier");
    categoryDescription[UTF8PROC_CATEGORY_SO] = _("Symbol, other");
    categoryDescription[UTF8PROC_CATEGORY_ZS] = _("Separator, space");
    categoryDescription[UTF8PROC_CATEGORY_ZL] = _("Separator, line");
    categoryDescription[UTF8PROC_CATEGORY_ZP] = _("Separator, paragraph");
    categoryDescription[UTF8PROC_CATEGORY_CC] = _("Other, control");
    categoryDescription[UTF8PROC_CATEGORY_CF] = _("Other, format");
    categoryDescription[UTF8PROC_CATEGORY_CS] = _("Other, surrogate");
    categoryDescription[UTF8PROC_CATEGORY_CO] = _("Other, private use");
    
    bidirectional[UTF8PROC_BIDI_CLASS_L] = _("Left-to-Right");
    bidirectional[UTF8PROC_BIDI_CLASS_LRE] = _("Left-to-Right Embedding");
    bidirectional[UTF8PROC_BIDI_CLASS_LRO] = _("Left-to-Right Override");
    bidirectional[UTF8PROC_BIDI_CLASS_R] = _("Right-to-Left");
    bidirectional[UTF8PROC_BIDI_CLASS_AL] = _("Right-to-Left Arabic");
    bidirectional[UTF8PROC_BIDI_CLASS_RLE] = _("Right-to-Left Embedding");
    bidirectional[UTF8PROC_BIDI_CLASS_RLO] = _("Right-to-Left Override");
    bidirectional[UTF8PROC_BIDI_CLASS_PDF] = _("Pop Directional Format");
    bidirectional[UTF8PROC_BIDI_CLASS_EN] = _("European Number");
    bidirectional[UTF8PROC_BIDI_CLASS_ES] = _("European Separator");
    bidirectional[UTF8PROC_BIDI_CLASS_ET] = _("European Number Terminator");
    bidirectional[UTF8PROC_BIDI_CLASS_AN] = _("Arabic Number");
    bidirectional[UTF8PROC_BIDI_CLASS_CS] = _("Common Number Separator");
    bidirectional[UTF8PROC_BIDI_CLASS_NSM] = _("Nonspacing Mark");
    bidirectional[UTF8PROC_BIDI_CLASS_BN] = _("Boundary Neutral");
    bidirectional[UTF8PROC_BIDI_CLASS_B] = _("Paragraph Separator");
    bidirectional[UTF8PROC_BIDI_CLASS_S] = _("Segment Separator");
    bidirectional[UTF8PROC_BIDI_CLASS_WS] = _("Whitespace");
    bidirectional[UTF8PROC_BIDI_CLASS_ON] = _("Other Neutrals");
    bidirectional[UTF8PROC_BIDI_CLASS_LRI] = _("Left-to-Right Isolate");
    bidirectional[UTF8PROC_BIDI_CLASS_RLI] = _("Right-to-Left Isolate");
    bidirectional[UTF8PROC_BIDI_CLASS_FSI] = _("First Strong Isolate");
    bidirectional[UTF8PROC_BIDI_CLASS_PDI] = _("Pop Directional Isolate");
    
    // Whatever it means! But does it concern decomposed form only?
    decompositionType[0] = _("Unknown"); // property->decomp_type is 0 on all tested characters, decomposed or not.
    decompositionType[UTF8PROC_DECOMP_TYPE_FONT] = _("Font"); // Starts at 1.
    decompositionType[UTF8PROC_DECOMP_TYPE_NOBREAK] = _("Nobreak");
    decompositionType[UTF8PROC_DECOMP_TYPE_INITIAL] = _("Initial");
    decompositionType[UTF8PROC_DECOMP_TYPE_MEDIAL] = _("Medial");
    decompositionType[UTF8PROC_DECOMP_TYPE_FINAL] = _("Final");
    decompositionType[UTF8PROC_DECOMP_TYPE_ISOLATED] = _("Isolated");
    decompositionType[UTF8PROC_DECOMP_TYPE_CIRCLE] = _("Circle");
    decompositionType[UTF8PROC_DECOMP_TYPE_SUPER] = _("Super");
    decompositionType[UTF8PROC_DECOMP_TYPE_SUB] = _("Sub");
    decompositionType[UTF8PROC_DECOMP_TYPE_VERTICAL] = _("Vertical");
    decompositionType[UTF8PROC_DECOMP_TYPE_WIDE] = _("Wide");
    decompositionType[UTF8PROC_DECOMP_TYPE_NARROW] = _("Narrow");
    decompositionType[UTF8PROC_DECOMP_TYPE_SMALL] = _("Small");
    decompositionType[UTF8PROC_DECOMP_TYPE_SQUARE] = _("Square");
    decompositionType[UTF8PROC_DECOMP_TYPE_FRACTION] = _("Fraction");
    decompositionType[UTF8PROC_DECOMP_TYPE_COMPAT] = _("Compat");
    
    // Whatever most values mean!
    boundClass[UTF8PROC_BOUNDCLASS_START] =  _("Start");
    boundClass[UTF8PROC_BOUNDCLASS_OTHER] =  _("Other");
    boundClass[UTF8PROC_BOUNDCLASS_CR] =  _("Cr");
    boundClass[UTF8PROC_BOUNDCLASS_LF] =  _("Lf");
    boundClass[UTF8PROC_BOUNDCLASS_CONTROL] =  _("Control");
    boundClass[UTF8PROC_BOUNDCLASS_EXTEND] =  _("Extend");
    boundClass[UTF8PROC_BOUNDCLASS_L] =  _("L");
    boundClass[UTF8PROC_BOUNDCLASS_V] =  _("V");
    boundClass[UTF8PROC_BOUNDCLASS_T] =  _("T");
    boundClass[UTF8PROC_BOUNDCLASS_LV] =  _("Lv");
    boundClass[UTF8PROC_BOUNDCLASS_LVT] = _("Lvt");
    boundClass[UTF8PROC_BOUNDCLASS_REGIONAL_INDICATOR] = _("Regional indicator");
    boundClass[UTF8PROC_BOUNDCLASS_SPACINGMARK] = _("Spacingmark");
    boundClass[UTF8PROC_BOUNDCLASS_PREPEND] = _("Prepend");
    boundClass[UTF8PROC_BOUNDCLASS_ZWJ] = _("Zero Width Joiner");
    boundClass[UTF8PROC_BOUNDCLASS_E_BASE] = _("Emoji Base");
    boundClass[UTF8PROC_BOUNDCLASS_E_MODIFIER] = _("Emoji Modifier");
    boundClass[UTF8PROC_BOUNDCLASS_GLUE_AFTER_ZWJ] = _("Glue_After_ZWJ");
    boundClass[UTF8PROC_BOUNDCLASS_E_BASE_GAZ] = _("E_BASE + GLUE_AFTER_ZJW");
    boundClass[UTF8PROC_BOUNDCLASS_EXTENDED_PICTOGRAPHIC] = _("Extended_Pictographic");
    boundClass[UTF8PROC_BOUNDCLASS_E_ZWG] = _("UTF8PROC_BOUNDCLASS_EXTENDED_PICTOGRAPHIC + ZWJ");
    
    const char * modeInfo = _("A mode of operation is required: unaccent, normalize, representation, properties, about."
     "\nPass '--help' for more information in each mode.");
    const int sargc = argc - 1;
    if (sargc == 0)
    {
        cout << modeInfo << endl;
        return 20;
    }
    char * sargv[sargc];
    for (uint i = 0; i < argc; i++)
    {
        if (i < 1)
        {
            sargv[i] = argv[i];
            continue;
        }
        if (i == 1)
            continue;
        sargv[i - 1] = argv[i];
    }
    
    int ret = 0;
    const string mode(argv[1]);
    if (mode == "unaccent")
    {
        ret = unaccent(sargc, sargv);
    }
    else if (mode == "normalize")
    {
        ret = normalize(sargc, sargv);
    }
    else if (mode == "representation")
    {
        ret = representation (sargc, sargv);
    }
    else if (mode == "properties")
    {
        ret = properties(sargc, sargv);
    }
    else if (mode == "about")
    {
        cout << "Author: Saleem Edah-Tally [Surgeon, Hobbyist developer]"
        "\nVersion: " << _APPVERSION_ << endl <<
        "License: CeCILL" << endl;
        ret = 0;
    }
    else
    {
        cout << modeInfo << endl;
        return 20;
    }
    
    return ret;
}
