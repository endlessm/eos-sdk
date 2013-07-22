
const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
const GObject = imports.gi.GObject;
const Lang = imports.lang;

// Local libraries
const CategoryModel = imports.models.category_model;
const Utils = imports.utils;

const DomainWikiModel = new Lang.Class({

    Name: "DomainWikiModel",
    Extends: GObject.Object,
    Properties: {

        'image-uri': GObject.ParamSpec.string('image-uri',
            'Application image URI',
            'URI describing a path to the image for this application.',
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT_ONLY,
            ''),

        // Name of the Wikipedia-based application, e.g. 'Brazil', 'Math'
        'application-name': GObject.ParamSpec.string('application-name',
            'Application name',
            'Name of the Wikipedia-based application',
            GObject.ParamFlags.READABLE,
            '')
    },

    //params should have the image-uri for the app's image, and the application name.
    _init: function(params) {
        this.parent(params);
    },

    //categories should be a list of category models, already populated with article models.
    addCategories: function(categories){
        this._categories = categories;
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