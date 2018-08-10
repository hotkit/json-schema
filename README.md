# JSON Schema Validation


A JSON schema validator that builds on the JSON implementation found in [fost-base](https://github.com/KayEss/fost-base) and is built for use in [Fostgres](https://github.com/KayEss/fostgres).


## Assertions

The following assertions are supported: The specifications for them are taken from [_JSON Schema Validation: A Vocabulary for Structural Validation of JSON_ (draft 1)](https://www.ietf.org/id/draft-handrews-json-schema-validation-01.txt).

The schema used for testing is <http://json-schema.org/draft-07/schema#>.

* `not` -- negates the contained check.
* `type` -- type check against the JSON types (`null`, `object`).
* `properties` -- that the assertions for each property hold if present in the data
