# json.h
simple header-only json parser 

```c
  // Public API

  JsonValue JsonParse(const char *data);
  JsonValue JsonGetValue(JsonObject object, char *key);
  void JsonPrintValue(JsonValue value);
```
