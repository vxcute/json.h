#ifndef JSON_H
#define JSON_H

#define STB_DS_IMPLEMENTATION

#include <assert.h>
#include <ctype.h>
#include <stb/stb_ds.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define internal static

typedef char *JsonString;
typedef char *JsonNumber;
typedef bool JsonBool;

typedef struct JsonValue JsonValue;
typedef struct JsonObject JsonObject;
typedef struct JsonPair JsonPair;
typedef struct JsonArray JsonArray;
typedef struct Token Token;
typedef struct Lexer Lexer;

typedef enum TokenType TokenType;
typedef enum JsonValueType JsonValueType;

#define JSON_GET_OBJECT(v) v.object

enum TokenType {
  TOKEN_L_BRACE = 0,
  TOKEN_R_BRACE,
  TOKEN_L_SQUARE_BRACKET,
  TOKEN_R_SQUARE_BRACKET,
  TOKEN_DOUBLE_QUOTE,
  TOKEN_COMMA,
  TOKEN_STRING,
  TOKEN_NUMBER,
  TOKEN_COLON,
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_NULL,
  TOKEN_EOF,
};

struct Token {
  TokenType type;
  char *token;
  size_t start;
  size_t end;
};

struct Lexer {
  const char *data;
  size_t cap;
  size_t pos;
};

enum JsonValueType {
  JSON_OBJECT = 0,
  JSON_ARRAY,
  JSON_STRING,
  JSON_NUMBER,
  JSON_BOOL,
  JSON_NULL,
  JSON_INVALID,
};

struct JsonObject {
  JsonPair *pairs;
  size_t pairs_count;
};

struct JsonArray {
  JsonValue *items;
  size_t size;
};

struct JsonValue {
  JsonValueType type;

  union {
    JsonObject object;
    JsonString string;
    JsonNumber number;
    JsonBool boolean;
    JsonArray array;
  };
};

struct JsonPair {
  char *string;
  JsonValue value;
};

// Internal API
internal Lexer *LexerNew(const char *data);
internal Token LexerNext(Lexer *l);
internal Token LexerPeek(Lexer *l);

internal const char *TokenTypeStr(TokenType type);
internal Token MakeToken(char *token, TokenType type, size_t start, size_t end);

internal JsonValue JsonParseString(Lexer *l);
internal JsonValue JsonParseNumber(Lexer *l);
internal JsonValue JsonParseBool(Lexer *l);
internal JsonPair JsonParsePair(Lexer *l);
internal JsonValue JsonParseObject(Lexer *l);
internal JsonValue _JsonParse(Lexer *lexer);

// Public API
JsonValue JsonParse(const char *data);
JsonValue JsonGetValue(JsonObject object, const char *key);
void JsonPrintValue(JsonValue value);

internal Lexer *LexerNew(const char *data) {
  Lexer *lexer = malloc(sizeof(Lexer));
  lexer->data = data;
  lexer->cap = strlen(data);
  lexer->pos = 0;
  return lexer;
}

internal const char *TokenTypeStr(TokenType type) {
  switch (type) {
  case TOKEN_L_BRACE:
    return "L_BRACE";
  case TOKEN_R_BRACE:
    return "R_BRACE";
  case TOKEN_R_SQUARE_BRACKET:
    return "R_BRACKET";
  case TOKEN_L_SQUARE_BRACKET:
    return "L_BRACKET";
  case TOKEN_STRING:
    return "STRING";
  case TOKEN_NUMBER:
    return "NUMBER";
  case TOKEN_COLON:
    return "COLON";
  case TOKEN_DOUBLE_QUOTE:
    return "DOUBLE QUOTE";
  case TOKEN_COMMA:
    return "COMMA";
  case TOKEN_TRUE:
    return "TRUE";
  case TOKEN_FALSE:
    return "FALSE";
  case TOKEN_NULL:
    return "NULL";
  default:
    return "";
  }
}

internal Token MakeToken(char *token, TokenType type, size_t start,
                         size_t end) {
  return (Token){
      .token = token,
      .type = type,
      .start = start,
      .end = end,
  };
}

internal Token LexerPeek(Lexer *l) {
  if (l->pos >= l->cap)
    return (Token){.type = TOKEN_EOF};

  Token t = LexerNext(l);
  l->pos = l->pos - (t.end - t.start);
  return t;
}

internal void SkipSpaces(Lexer *l) {
  while (isspace(l->data[l->pos]) && (l->pos < l->cap)) {
    l->pos++;
  }
}

internal Token LexerNext(Lexer *l) {
  if (l->pos >= l->cap)
    return (Token){.type = TOKEN_EOF};

  SkipSpaces(l);

  char c = l->data[l->pos];
  size_t pos = l->pos;
  char *token_s = NULL;
  Token token;

  switch (c) {
  case '{': {
    arrput(token_s, c);
    arrput(token_s, '\0');
    l->pos++;
    return MakeToken(token_s, TOKEN_L_BRACE, pos, pos + 1);
  }
  case '}': {
    arrput(token_s, c);
    arrput(token_s, '\0');
    l->pos++;
    return MakeToken(token_s, TOKEN_R_BRACE, pos, pos + 1);
  }
  case ':': {
    arrput(token_s, c);
    arrput(token_s, '\0');
    l->pos++;
    return MakeToken(token_s, TOKEN_COLON, pos, pos + 1);
  }
  case ',': {
    arrput(token_s, c);
    arrput(token_s, '\0');
    l->pos++;
    return MakeToken(token_s, TOKEN_COMMA, pos, pos + 1);
  }
  case '[': {
    arrput(token_s, c);
    arrput(token_s, '\0');
    l->pos++;
    return MakeToken(token_s, TOKEN_L_SQUARE_BRACKET, pos, pos + 1);
  }
  case ']': {
    arrput(token_s, c);
    arrput(token_s, '\0');
    l->pos++;
    return MakeToken(token_s, TOKEN_R_SQUARE_BRACKET, pos, pos + 1);
  }
  case '"': {
    arrput(token_s, c);
    arrput(token_s, '\0');
    l->pos++;
    return MakeToken(token_s, TOKEN_DOUBLE_QUOTE, pos, pos + 1);
  }
  }

  char last = l->data[l->pos - 1];
  if (last == '"') {
    size_t start = l->pos;
    c = l->data[l->pos];
    while (c != '"') {
      arrput(token_s, c);
      l->pos++;
      c = l->data[l->pos];
    }
    size_t end = l->pos;
    arrput(token_s, '\0');
    return MakeToken(token_s, TOKEN_STRING, start, end);
  }

  if (isalpha(c)) {
    size_t start = pos;
    while (isalpha(c) && (l->pos < l->cap)) {
      arrput(token_s, c);
      l->pos++;
      c = l->data[l->pos];
    }
    arrput(token_s, '\0');

    if (strncmp(token_s, "true", 4) == 0) {
      token = MakeToken(token_s, TOKEN_TRUE, start, l->pos);
    } else if (strncmp(token_s, "false", 4) == 0) {
      token = MakeToken(token_s, TOKEN_FALSE, start, l->pos);
    } else if (strncmp(token_s, "null", 4) == 0) {
      token = MakeToken(token_s, TOKEN_NULL, start, l->pos);
    } else {
      token = MakeToken(token_s, TOKEN_STRING, start, l->pos);
    }
  }

  if (isdigit(c) || c == '-') {
    size_t start = pos;
    while ((isdigit(c) || c == '.' || c == '-') && (l->pos < l->cap)) {
      arrput(token_s, c);
      l->pos++;
      c = l->data[l->pos];
    }

    arrput(token_s, '\0');
    token = MakeToken(token_s, TOKEN_NUMBER, start, l->pos);
  }

  return token;
}

internal JsonValue JsonParseString(Lexer *l) {
  LexerNext(l);
  JsonValue json_value = {.type = JSON_STRING};
  json_value.string = LexerNext(l).token;
  LexerNext(l);
  return json_value;
}

internal JsonValue JsonParseNumber(Lexer *l) {
  JsonValue json_value = {.type = JSON_NUMBER};
  json_value.number = LexerNext(l).token;
  return json_value;
}

internal JsonValue JsonParseBool(Lexer *l) {
  Token t = LexerNext(l);
  JsonValue json_value;
  json_value.type = JSON_BOOL;

  if (t.type == TOKEN_TRUE) {
    json_value.boolean = true;
  } else if (t.type == TOKEN_FALSE) {
    json_value.boolean = false;
  }

  return json_value;
}

internal JsonValue json_parse_array(Lexer *l) {
  Token t = LexerNext(l); // skip [
  JsonValue json_value = {.type = JSON_ARRAY};
  json_value.array.items = NULL;
  size_t size = 0;

  while (t.type != TOKEN_R_SQUARE_BRACKET) {
    JsonValue item = _JsonParse(l);
    arrput(json_value.array.items, item);
    t = LexerNext(l);
    size++;
  }

  json_value.array.size = size;

  return json_value;
}

internal JsonPair JsonParsePair(Lexer *l) {
  JsonPair json_pair = {0};
  JsonValue string = _JsonParse(l);
  LexerNext(l);
  JsonValue value = _JsonParse(l);
  json_pair.string = string.string;
  json_pair.value = value;
  return json_pair;
}

internal JsonValue JsonParseObject(Lexer *l) {
  Token t = LexerNext(l); // skip {
  JsonValue json_value = {.type = JSON_OBJECT};
  json_value.object.pairs = NULL;
  size_t pairs_count = 0;

  while (t.type != TOKEN_R_BRACE) {
    JsonPair json_pair = JsonParsePair(l);
    arrput(json_value.object.pairs, json_pair);
    t = LexerNext(l);
    pairs_count++;
  }

  json_value.object.pairs_count = pairs_count;

  return json_value;
}

internal JsonValue _JsonParse(Lexer *lexer) {
  Token peek = LexerPeek(lexer);

  switch (peek.type) {
  case TOKEN_L_BRACE:
    return JsonParseObject(lexer);
  case TOKEN_DOUBLE_QUOTE:
    return JsonParseString(lexer);
  case TOKEN_NUMBER:
    return JsonParseNumber(lexer);
  case TOKEN_TRUE:
  case TOKEN_FALSE:
    return JsonParseBool(lexer);
  case TOKEN_L_SQUARE_BRACKET:
    return json_parse_array(lexer);
  default:
    return (JsonValue){.type = JSON_INVALID};
  }
}

JsonValue JsonParse(const char *data) {
  Lexer *l = LexerNew(data);
  return _JsonParse(l);
}

void JsonPrintValue(JsonValue value) {
  if (value.type == JSON_NUMBER) {
    printf("%s", value.number);
  } else if (value.type == JSON_STRING) {
    printf("\"%s\"", value.string);
  } else if (value.type == JSON_NULL) {
    printf("null");
  } else if (value.type == JSON_BOOL) {
    const char *bval = value.boolean ? "true" : "false";
    printf("%s", bval);
  } else if (value.type == JSON_ARRAY) {
    JsonArray arr = value.array;
    JsonValue *items = arr.items;
    printf("[ ");
    for (int i = 0; i < arr.size; i++) {
      JsonPrintValue(items[i]);
      if (i < arr.size - 1)
        printf(", ");
    }
    printf("]\n");
  } else if (value.type == JSON_OBJECT) {
    JsonObject obj = value.object;
    JsonPair *pairs = obj.pairs;

    printf("{ ");

    for (size_t i = 0; i < obj.pairs_count; i++) {
      printf("\"%s\" : ", pairs[i].string);
      JsonPrintValue(pairs[i].value);
      if (i < obj.pairs_count - 1) {
        printf(", ");
      }
    }

    printf(" }");
  }
}

JsonValue JsonGetValue(JsonObject object, const char *key) {
  JsonPair *pairs = object.pairs;

  for (size_t i = 0; i < object.pairs_count; i++) {
    if (strcmp(pairs[i].string, key) == 0) {
      return pairs[i].value;
    }
  }
}

#endif /* END OF JSON_H */
