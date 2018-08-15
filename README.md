# JSON Schema Validation


A JSON schema validator that builds on the JSON implementation found in [fost-base](https://github.com/KayEss/fost-base) and is built for use in [Fostgres](https://github.com/KayEss/fostgres). It is implemented in modern C++, specifically C++17.


## Assertions

The specifications for them are taken from [_JSON Schema Validation: A Vocabulary for Structural Validation of JSON_ (draft 1)](https://www.ietf.org/id/draft-handrews-json-schema-validation-01.txt). The following assertions are supported:

* `enum` -- Values must be in the specified set.
* `maximum` -- Maximum value for numeric data.
* `maxItems` and `minItems` -- counts for the number of items in a JSON array.
* `not` -- negates the contained check.
* `properties`, `patternProperties` and `additionalProperties` -- that the assertions for each property hold if present in the JSON object data.
* `required` -- Object properties must include the specified strings.
* `type` -- type check against the JSON types (`null`, `boolean`, `object`, `array`, `number`, `string` and `integer`).

The schema used for testing is <http://json-schema.org/draft-07/schema#>. Progress can be seen by looking at the [to do list in the schema test cmake file](./test/stress/CMakeLists.txt).

For type validation, if the underlying memory type is a `double` then it will only match against `number` even if there is no fractional part to the number. The JSON parser will correctly identify integers parsing JSON (and use the `int64_t` memory type for them), so this can only happen when JSON is produced by code that uses a `double`. The memory type `int64_t` will match both of the `number` and `integer` validation types.


## The JSON Schema Testsuite

The build target `json-schema-testsuite-v7` will download and run the tests that are found at the [_JSON Schema Test Suite_](https://github.com/json-schema-org/JSON-Schema-Test-Suite/tree/master/tests/draft7).

