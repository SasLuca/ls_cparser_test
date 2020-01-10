#include <catch2/catch.hpp>
#include <ls_cparser.h>

#define MAX_TOKEN_COUNT 1024

TEST_CASE("Tokenizer test", "[tokenizer]")
{
    const char* source_code = "#include <stdio.h>"
                              "#define NUM 0"
                              "int main() { return NUM; }";

    const int source_code_len = strlen(source_code);

    ls_cparser_preprocessor_context preprocessor_context = { source_code, source_code_len };

    ls_cparser_preprocessor_output preprocessed_output = ls_cparser_execute_preprocessor_with_allocator(
        source_code, source_code_len,
        (ls_cparser_string_array) { stdio_include },
        ls_cparser_mallocator
    );

    assert(preprocessed_output.error != ls_cparser_no_error);

    ls_cparser_tokenizer_context tokenizer_context = { preprocessed_output.text, preprocessed_output.text_len };
    ls_cparser_token tokens[MAX_TOKEN_COUNT];
    int tokens_count = 0;

    while (!ls_cparser_tokenizer_is_done(tokenizer_context))
    {
        const ls_cparser_token tk = ls_cparser_get_next_token(&tokenizer_context);
        assert(tokenizer_context.error == ls_cparser_no_error && tokens_count < MAX_TOKEN_COUNT);
        tokens[tokens_count++] = tk;
    }

    for (int i = 0; i < tokens_count; i++)
    {
        char text_buffer[1024];
        assert(ls_cparser_print_token(text_buffer, 1024, tokens[i]));

        printf("%s", text_buffer);
    }

    ls_cparser_context parser = { tokens };
    ls_cparser_parse(&parser);


}