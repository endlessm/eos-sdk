const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
const GObject = imports.gi.GObject;
const Json = imports.gi.Json;
const Lang = imports.lang;

// Local libraries
//const CategoryModel = imports.models.category_model;
const Utils = imports.utils;
const JsonUtils = imports.models.utils.json_utils;

const CONTENT_DIRECTORY = Endless.getCurrentFileDir() + "/../../data/";
const DEFAULT_METADATA_FILE = CONTENT_DIRECTORY +  "brazil_categories.json";

const CategoryModel = new Lang.Class({
    Name: "CategoryModel",
    Extends: GObject.Object,

    _init: function(name, desc, image_uri, articles) {
        print("in category init");
        
    },

    getArticles: function() {
        return this._categories;
    },
});
