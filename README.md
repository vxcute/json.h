# json.h

```c
  // Public API

  JsonValue JsonParse(const char *data);
  JsonValue JsonGetValue(JsonObject object, const char *key);
  void JsonPrintValue(JsonValue value);
```
