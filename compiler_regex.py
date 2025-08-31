import re

token_specification = [
    ("T_FUNCTION",  r'\bfn\b'),
    ("T_INT",       r'\bint\b'),
    ("T_FLOAT",     r'\bfloat\b'),
    ("T_STRING",    r'\bstring\b'),
    ("T_BOOL",      r'\bbool\b'),
    ("T_RETURN",    r'\breturn\b'),
    ("T_IF",        r'\bif\b'),
    ("T_ELSE",      r'\belse\b'),
    ("T_WHILE",     r'\bwhile\b'),
    ("T_FOR",       r'\bfor\b'),

    ("T_IDENTIFIER", r'[A-Za-z_][A-Za-z0-9_]*'),
    ("T_FLOATLIT",  r'\d+\.\d+'),
    ("T_INTLIT",    r'\d+'),
    ("T_STRINGLIT", r'"([^"\\]|\\.)*"'),   # handles escapes

    ("T_EQUALSOP",  r'=='),
    ("T_ASSIGNOP",  r'='),
    ("T_NEQ",       r'!='),
    ("T_LE",        r'<='),
    ("T_GE",        r'>='),
    ("T_AND",       r'&&'),
    ("T_OR",        r'\|\|'),

    ("T_PLUS",      r'\+'),
    ("T_MINUS",     r'-'),
    ("T_MUL",       r'\*'),
    ("T_DIV",       r'/'),

    ("T_PARENL",    r'\('),
    ("T_PARENR",    r'\)'),
    ("T_BRACEL",    r'\{'),
    ("T_BRACER",    r'\}'),
    ("T_BRACKETL",  r'\['),
    ("T_BRACKETR",  r'\]'),
    ("T_COMMA",     r','),
    ("T_SEMICOLON", r';'),

    ("T_COMMENT",   r'//.*|/\*[\s\S]*?\*/'),  # single & multi-line
    ("T_NEWLINE",   r'\n'),
    ("T_SKIP",      r'[ \t]+'),
    ("T_ERROR",     r'.'),  # catch-all
]


tok_regex = '|'.join(f'(?P<{name}>{pattern})' for name, pattern in token_specification)

compiled_re = re.compile(tok_regex)


def tokenize(code):
    tokens = []
    for mo in compiled_re.finditer(code):
        kind = mo.lastgroup
        value = mo.group()
        if kind == "T_SKIP" or kind == "T_NEWLINE" or kind == "T_COMMENT":
            continue
        elif kind == "T_ERROR":
            raise SyntaxError(f"Unexpected token: {value}")
        else:
            tokens.append((kind, value))
    return tokens



code = '''
    fn int my_fn(int x, float y) {
        string my_str = "hmm";
        bool my_bool = x == 40;
        return x;
    }
    '''

token_stream = tokenize(code)
for token in token_stream:
    print(token)
