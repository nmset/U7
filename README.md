## U7

This project links to the [utf8proc](https://github.com/JuliaStrings/utf8proc) library to perform some operations on an UTF-8 input.

---
    $ utf8util --help
    A mode of operation is required: unaccent, normalize, representation, properties, about.
    Pass '--help' for more information in each mode.
---
    $ utf8util unaccent --help
    This operational mode removes character markings, control characters, default ignorable
    characters and unassigned codepoints from an UTF-8 input.The utf8proc library, on which
    this utility is based, refers to character markings as 'non-spacing, spacing and
    enclosing (accents)' marks, and to default ignorable characters 'such as SOFT-HYPHEN or
    ZERO-WIDTH-SPACE'. Control characters are stripped or converted to spaces.
    
    By default, every removable byte is stripped, the output characters are decomposed and
    Unicode Versioning Stability is enforced.
    
      -i, --ignore: do not strip 'default ignorable characters'
      -c, --control: do not handle 'control characters'
      -m, --mark: do not strip 'character markings'
      -n, --na: do not strip 'unassigned codepoints'
      -r, --recompose: output recomposed characters
      -h, --help: show this message
    
    The input can be piped in or read from stdin. It must be a single NULL terminated line.
---
    $ utf8util normalize --help
    This operational mode normalizes the input string according to the specified type, the
    default being NFC.
    
      -t, --type: one of NFC, NFD, NFKC, NFKD, NFKC_Casefold
      -h, --help: show this message
    
    The input can be piped in or read from stdin. It must be a single NULL terminated line.
---
    $ utf8util representation --help
    This operational mode displays representations of the first identified codepoint.
    
      -p, --codepoint: hexadecimal representation of the codepoint
      -e, --utf8: hexadecimal representation of each byte
      -s, --utf16: hexadecimal representation of each surrogate
      -b, --binary: binary representation of each byte
      -o, --octal: octal representation of each byte
      -d, --decimal: decimal representation of each byte
      -x, --xml: XML decimal representation of each byte
      -L, --tolower: displays the codepoint as a lower-case character if existent
      -U, --toupper: displays the codepoint as an upper-case character if existent
      -T, --totitle: displays the codepoint as a title-case character if existent
      -h, --help: show this message
    
    The input can be piped in or read from stdin. Pass in a single character for simplicity.
---
    $ utf8util properties --help
    This operational mode displays properties of the first identified codepoint.
    
      -l, --islower: displays 1 if the codepoint refers to a lower-case character,
      0 otherwise
      -u, --isupper: displays 1 if the codepoint refers to an upper-case character,
      0 otherwise
      -c, --category: determines the category of a codepoint (Letter, Number, Symbol...)
      -n, --direction: determines the bidirectional class of a codepoint; see utf8proc.h
      -i, --decompositiontype: determines the decomposition type of a codepoint;
      see utf8proc.h
      -k, --boundclass: determines the boundclass property of a codepoint; see utf8proc.h
      -h, --help: show this message
    
    The input can be piped in or read from stdin. Pass in a single character for simplicity.
---
    $ utf8util about            
    Author: Saleem Edah-Tally [Surgeon, Hobbyist developer]
    Version: 1
    License: CeCILL
---

### Disclaimer

Use at your own risks.
