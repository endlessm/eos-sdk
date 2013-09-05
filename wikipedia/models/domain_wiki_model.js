
const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
const GObject = imports.gi.GObject;
const Lang = imports.lang;

// Local libraries
const CategoryModel = imports.wikipedia.models.category_model;
const Utils = imports.wikipedia.utils;

const DomainWikiModel = new Lang.Class({

    Name: "DomainWikiModel",
    Extends: GObject.Object,

    //params should have the image-uri for the app's image, and the application name.
    _init: function(params) {
        this.parent(params);
    },

    setLinkedArticles:function(articles){
        this._linked_articles = articles;
    },

    getLinkedArticles:function(){
        return this._linked_articles;
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
    }
});