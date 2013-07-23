<<<<<<< HEAD

const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
const GObject = imports.gi.GObject;
=======
const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
const GObject = imports.gi.GObject;
const Json = imports.gi.Json;
>>>>>>> f00c77b... Completed the view for GTK
const Lang = imports.lang;

// Local libraries
const CategoryModel = imports.models.category_model;
const Utils = imports.utils;
<<<<<<< HEAD

const DomainWikiModel = new Lang.Class({

    Name: "DomainWikiModel",
    Extends: GObject.Object,
    Properties: {

        'image-uri': GObject.ParamSpec.string('image-uri',
            'Application image URI',
            'URI describing a path to the image for this application.',
=======
const JsonUtils = imports.models.utils.json_utils;

const CONTENT_DIRECTORY = Endless.getCurrentFileDir() + "/../../data/";
const DEFAULT_METADATA_FILE = CONTENT_DIRECTORY +  "pt-BR-brazil.json";

const DomainWikiModel = new Lang.Class({
    Name: "DomainWikiModel",
    Extends: GObject.Object,
    Properties: {
        // resource:// URI for the categories JSON file
        'image-uri': GObject.ParamSpec.string('image-uri',
            'Category file URI',
            'URI for image of this cateogry',
>>>>>>> f00c77b... Completed the view for GTK
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT_ONLY,
            ''),

        // Name of the Wikipedia-based application, e.g. 'Brazil', 'Math'
        'application-name': GObject.ParamSpec.string('application-name',
            'Application name',
            'Name of the Wikipedia-based application',
            GObject.ParamFlags.READABLE,
            '')
    },

<<<<<<< HEAD
    //params should have the image-uri for the app's image, and the application name.
    _init: function(params) {
        this.parent(params);
    },

    //categories should be a list of category models, already populated with article models.
    addCategories: function(categories){
        this._categories = categories;
=======
    _init: function(filename) {
        let jsonFile;
        if(filename){
            filename = CONTENT_DIRECTORY + filename
        }
        jsonFile = filename || DEFAULT_METADATA_FILE;

        this._categories = new Array();

        this.initFromJsonFile(jsonFile);
    },

    initFromJsonFile: function(filename) {
        try {

            let app_content = JSON.parse(Utils.load_file (filename));
            this._application_name = app_content['app_name'];
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
>>>>>>> f00c77b... Completed the view for GTK
    },

    getArticlesForCategoryIndex: function(index){
        let category = this.getCategories()[index];
        return category.getArticles();
    },

    getCategories: function() {
        return this._categories;
    },

    get application_name(){
        return this._application_name;
    },

    get image_uri(){
        return this._image_uri;
    }
});
<<<<<<< HEAD

=======
>>>>>>> f00c77b... Completed the view for GTK
