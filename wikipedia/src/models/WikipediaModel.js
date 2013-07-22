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
const DEFAULT_METADATA_FILE = CONTENT_DIRECTORY +  "pt-BR-brazil.json";

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

    initFromJsonFile: function(filename) {
        try {

            let app_content = JSON.parse(Utils.load_file (filename));
            this._app_name = app_content['app_name'];
            this._image_uri = app_content['image_uri'];
            this._lang_code = filename.substring(0, 2);
            let categories = app_content['categories'];
            let cat_length = categories.length
            for(let i = 0; i < cat_length; i++){
                let category = categories[i];
                this.addCategory(category);
            }
         
        } catch (e) {
            throw e;
        }
    },

    addCategory: function(category){
        let name = category['category_name'];
        let description = category['content_text'];
        let image_uri = category['image_uri'];
        let articles = category['articles'];
        this._categories.push(new CategoryModel.CategoryModel(name, description, image_uri, articles));
    },

    getCategories: function() {
        return this._categories;
    },

    getAppName: function(){
        return this._app_name;
    },

    getImageURI: function(){
        return this._image_uri;
    }
});
