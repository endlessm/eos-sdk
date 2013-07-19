const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
const GObject = imports.gi.GObject;
const Json = imports.gi.Json;
const Lang = imports.lang;

// Local libraries
//const CategoryModel = imports.models.category_model;
const JsonUtils = imports.models.utils.json_utils;

const CONTENT_DIRECTORY = Endless.getCurrentFileDir() + "/../../content/";
const DEFAULT_METADATA_FILE = CONTENT_DIRECTORY +  "metadata.json";

const WikipediaModel = new Lang.Class({
    Name: "WikipediaModel",
    Extends: GObject.Object,

    _init: function(f) {

        let jsonFile;
        jsonFile = f || DEFAULT_METADATA_FILE;

        this._categories = new Array();

        this.initFromJsonFile(jsonFile);
    },

    initFromJsonFile: function(theFile) {

        let parser = new Json.Parser();

        try {
            parser.load_from_file(theFile);

            let root = parser.get_root();
            let reader = new Json.Reader();
            reader.root = root;

            reader.read_member("categories");
            let categoryCount = reader.count_elements();
            for(let i=0; i<categoryCount; i++) {
                reader.read_element(i);
                print("hello");
                let vId = reader.get_string_value();
                //let newCategory = new CategoryModel.CategoryModel({ id: vId });
                //this._categories.push(newCategory);

                reader.end_element();
            }
            reader.end_member();
        } catch (e) {
            throw e;
        }
    },

    getCategories: function() {
        return this._categories;
    },
});
