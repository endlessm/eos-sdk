const Gio = imports.gi.Gio;
const Json = imports.gi.Json;
const Gettext = imports.gettext;

const _JSON_VAL_STRING = 0;
const _JSON_VAL_INT = 1;
const _JSON_VAL_DOUBLE = 2;
const _JSON_VAL_BOOLEAN = 3;


function getJsonMemberStringValue(reader, key) {

    return _getJsonMemberValue(reader, key, _JSON_VAL_STRING);
}

function getJsonMemberIntValue(reader, key) {

    return _getJsonMemberValue(reader, key, _JSON_VAL_INT);
}

function getJsonMemberDoubleValue(reader, key) {

    return _getJsonMemberValue(reader, key, _JSON_VAL_DOUBLE);
}

function getJsonMemberBooleanValue(reader, key) {

    return _getJsonMemberValue(reader, key, _JSON_VAL_BOOLEAN);
}

function getJsonMemberLocalizedValue(reader, key) {

    return Gettext.gettext(_getJsonMemberValue(reader, key + '_', _JSON_VAL_STRING));
}

function _getJsonMemberValue(reader, key, type) {

    reader.read_member(key);
    let value;
    switch(type) {
    case _JSON_VAL_STRING:
        value = reader.get_string_value();
        break;
    case _JSON_VAL_INT:
        value = reader.get_int_value();
        break;
    case _JSON_VAL_DOUBLE:
        value = reader.get_double_value();
        break;
    case _JSON_VAL_BOOLEAN:
        value = reader.get_boolean_value();
        break;
    }
    reader.end_member();
    return value;
}
