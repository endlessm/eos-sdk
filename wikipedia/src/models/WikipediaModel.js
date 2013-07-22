const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
const GObject = imports.gi.GObject;
const Json = imports.gi.Json;
const Lang = imports.lang;

// Local libraries
const CategoryModel = imports.models.CategoryModel;
const Utils = imports.utils;
const JsonUtils = imports.models.utils.json_utils;

const CONTENT_DIRECTORY = Endless.getCurrentFileDir() + "/../../data/";
const DEFAULT_METADATA_FILE = CONTENT_DIRECTORY +  "brazil_categories.json";

const WikipediaModel = new Lang.Class({
    Name: "WikipediaModel",
    Extends: GObject.Object,

    _init: function(f) {
        print("in init");
        let jsonFile;
        if(f){
            f = CONTENT_DIRECTORY + f
        }
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
            let app_content = JSON.parse(Utils.load_file (theFile));
            let categories = app_content['categories'];
            let cat_length = categories.length
            this._cat_models = new Array();
            for(let i = 0; i < cat_length; i++){
                let category = categories[i]
                let name = category['category_name'];
                let description = category['content_text'];
                let image_uri = category['image_uri'];
                let articles = category['articles'];
                this._cat_models.push(new CategoryModel.CategoryModel(name, description, image_uri, articles));
            }
            reader.read_member("categories");
            let categoryCount = reader.count_elements();
         
            reader.end_member();
        } catch (e) {
            throw e;
        }
    },

    getCategories: function() {
        return this._categories;
    },
});
