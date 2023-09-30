/*
 * File:   main.cpp
 * Author: Saleem Edah-Tally - nmset@yandex.com
 * License: CeCILL-C
 * Copyright: Saleem Edah-Tally - © 2023
 *
 * Created on 20 september 2023, 18:31
 */

#include <iostream>
#include <getopt.h>
#include <utf8proc.h>

using namespace std;

#define STRIP_OPTIONS_DEFAULT (UTF8PROC_IGNORE | UTF8PROC_STRIPCC | UTF8PROC_STRIPMARK | UTF8PROC_STRIPNA | UTF8PROC_DECOMPOSE | UTF8PROC_STABLE | UTF8PROC_NULLTERM)

void unaccentShowHelp()
{
    string message("This operational mode removes character markings, control characters, default ignorable characters and unassigned codepoints from an UTF-8 input."
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
    string message("This operational mode normalizes the input string according to the specified type, the default being NFC."
    "\n\n  -t, --type: one of NFC, NFD, NFKC, NFKD, NFKC_Casefold"
    "\n  -h, --help: show this message"
    "\n\nThe input can be piped in or read from stdin. It must be a single NULL terminated line.");
    
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
        cout << "Unknown type; valid types are NFC, NFD, NFKC, NFKD and NFKC_Casefold." << endl;
        return 41;
    }
    
    if (result)
    {
        cout << (const char*) result << endl;
        free((void*) result);
    }
    
    return 0;
}

int main(int argc, char ** argv)
{
    const char * modeInfo = "A mode of operation is required: unaccent, normalize."
     "Pass '--help' for more information on each mode.";
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
    else
    {
        cout << modeInfo << endl;
        return 20;
    }
    
    return ret;
}
