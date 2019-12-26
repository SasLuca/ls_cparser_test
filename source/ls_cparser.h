//region Interface
#ifndef LS_CPARSER_H
#define LS_CPARSER_H

#ifndef ls_bool
#define ls_bool int
#endif

#ifndef ls_int
#define ls_int signed long long
#endif

#ifndef ls_size_t
#define ls_size_t unsigned long long
#endif

#ifndef LS_CPARSER_API
#define LS_CPARSER_API extern
#endif

#ifndef LS_CPARSER_INTERNAL
#define LS_CPARSER_INTERNAL static
#endif

//region structs
typedef enum ls_cparser_error
{
    ls_cparser_unexpected_end_of_source = 1
} ls_cparser_error;

typedef enum ls_cparser_token_type
{
    ls_token_type_string_literal,
    ls_token_type_int_literal,
    ls_token_type_float_literal,
    ls_token_type_double_literal,
    ls_token_type_single_line_comment,
    ls_token_type_multi_line_comment,

    ls_token_type_backtick,              // `
    ls_token_type_tilde,                 // ~
    ls_token_type_exclamation_mark,      // !
    ls_token_type_at_symbol,             // @
    ls_token_type_hashtag,               // #
    ls_token_type_percent,               // %
    ls_token_type_hat,                   // ^
    ls_token_type_ampersand,             // &
    ls_token_type_star,                  // *
    ls_token_type_open_paren,           // (
    ls_token_type_close_paren,          // )
    ls_token_type_minus,                 // -
    ls_token_type_plus,                  // +
    ls_token_type_equal,                 // =
    ls_token_type_colon,                 // :
    ls_token_type_left_arrow,            // <
    ls_token_type_right_arrow,           // >
    ls_token_type_comma,                 // ,
    ls_token_type_dot,                   // .
    ls_token_type_open_square_bracket,   // [
    ls_token_type_close_square_breacket, // ]
    ls_token_type_open_curly,            // {
    ls_token_type_close_curly,           // }
    ls_token_type_bar,                   // |
    ls_token_type_semicolon,             // ;
    ls_token_type_forward_slash,         // /
    ls_token_type_backward_slash,        // \

} ls_cparser_token_type;

typedef struct ls_cparser_token ls_cparser_token;
struct ls_cparser_token
{
    ls_cparser_token_type type;
    ls_int                text_begin;
    ls_int                text_length;
    ls_int                line_of_code;
    ls_int                number_in_line; //The number of the token in the line its on.
};

typedef struct ls_cparser_tokenizer_context ls_cparser_tokenizer_context;
struct ls_cparser_tokenizer_context
{
    const char* source_code;
    ls_int      source_code_len;

    ls_cparser_error error;

    ls_int      current_pos; //Current position in the source code
    ls_int      current_line;
    ls_int      tokens_on_line_count;
    ls_int      ignored_utf8_bytes_count;
};
//endregion

LS_CPARSER_API inline ls_bool ls_cparser_tokenizer_is_done(ls_cparser_tokenizer_context tk_ctx)
{
    return tk_ctx.current_pos >= tk_ctx.source_code_len;
}

LS_CPARSER_API ls_cparser_token ls_cparser_get_next_token(ls_cparser_tokenizer_context* tk_ctx)
{
    #define current_char tk_ctx->source_code[tk_ctx->current_pos]
    #define reached_end_of_code (tk_ctx->current_pos >= tk_ctx->source_code_len)
    #define can_peek ((tk_ctx->current_pos + 1) >= tk_ctx->source_code_len)
    #define peek_next_char tk_ctx->source_code[tk_ctx->current_pos + 1]
    #define one_char_token_case(t, c) case c: return (ls_cparser_token) { .type = t, .line_of_code = tk_ctx->current_line, .number_in_line = tk_ctx->tokens_on_line_count++, .text_begin = tk_ctx->current_pos++, .text_length = 1, }

    if (tk_ctx->error)
    {
        return (ls_cparser_token) { };
    }

    ls_bool looking_for_token = true;

    while (looking_for_token && !reached_end_of_code)
    {
        switch (current_char)
        {
            //One character tokens
            //case '/': We dont handle / here because it is used to produce comments
            one_char_token_case(ls_token_type_backtick, '`');
            one_char_token_case(ls_token_type_tilde, '~');
            one_char_token_case(ls_token_type_exclamation_mark, '!');
            one_char_token_case(ls_token_type_at_symbol, '@');
            one_char_token_case(ls_token_type_hashtag, '#');
            one_char_token_case(ls_token_type_percent, '%');
            one_char_token_case(ls_token_type_hat, '^');
            one_char_token_case(ls_token_type_ampersand, '&');
            one_char_token_case(ls_token_type_star, '*');
            one_char_token_case(ls_token_type_open_paren, '(');
            one_char_token_case(ls_token_type_close_paren, ')');
            one_char_token_case(ls_token_type_minus, '-');
            one_char_token_case(ls_token_type_plus, '+');
            one_char_token_case(ls_token_type_equal, '=');
            one_char_token_case(ls_token_type_colon, ':');
            one_char_token_case(ls_token_type_left_arrow, '<');
            one_char_token_case(ls_token_type_right_arrow, '>');
            one_char_token_case(ls_token_type_comma, ',');
            one_char_token_case(ls_token_type_dot, '.');
            one_char_token_case(ls_token_type_open_square_bracket, '[');
            one_char_token_case(ls_token_type_close_square_breacket, ']');
            one_char_token_case(ls_token_type_open_curly, '{');
            one_char_token_case(ls_token_type_close_curly, '}');
            one_char_token_case(ls_token_type_bar, '|');
            one_char_token_case(ls_token_type_semicolon, ';');
            one_char_token_case(ls_token_type_backward_slash, '\\');

            case '\n':
            {
                tk_ctx->current_line++;
                tk_ctx->tokens_on_line_count = 0;

                tk_ctx->current_pos++;
                continue;
            }

            //Whitespace
            case '\t':
            case '\r':
            case ' ' :
            {
               tk_ctx->current_pos++;
               continue;
            }

            case '/':
            {
                if (can_peek)
                {
                    const ls_int begin     = tk_ctx->current_pos;
                    const char   next_char = peek_next_char;

                    if (next_char == '/') //Single line comment
                    {
                        while (current_char != '\n' && !reached_end_of_code)
                        {
                            tk_ctx->current_pos++;
                        }

                        return (ls_cparser_token)
                        {
                            .type           = ls_token_type_single_line_comment,
                            .line_of_code   = reached_end_of_code ? tk_ctx->current_line : tk_ctx->current_line++, //Only increase the current line counter if we didnt reach the end of code
                            .number_in_line = tk_ctx->tokens_on_line_count++,
                            .text_begin     = begin,
                            .text_length    = tk_ctx->current_pos - begin,
                        };
                    }
                    else if (next_char == '*') //Multi line comment
                    {

                    }
                }

                //if no characters left then return single char token
                return (ls_cparser_token)
                {
                    .type           = ls_token_type_forward_slash,
                    .line_of_code   = tk_ctx->current_line,
                    .number_in_line = tk_ctx->tokens_on_line_count++,
                    .text_begin     = tk_ctx->current_pos++,
                    .text_length    = 1,
                };
            }
        }
    }

    #undef current_char
    #undef reached_end_of_code
}

#endif //LS_CPARSER_H
//endregion

//region Implementation
#ifdef LS_CPARSER_IMPL
#endif
//endregion