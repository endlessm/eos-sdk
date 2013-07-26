const Endless = imports.gi.Endless;
const GObject = imports.gi.GObject;
const Lang = imports.lang;

// Local libraries
const ArticleModel = imports.models.article_model;

const CategoryModel = new Lang.Class({
    Name: "CategoryModel",
    Extends: GObject.Object,
   	Properties: {
        'description': GObject.ParamSpec.string('description', 'Category Description', 'This is the text that the user reads on the category page.',
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                ""),
        'title': GObject.ParamSpec.string('title', 'Category Name', 'This is the name that is displayed on the front page and as the title on the category page.', 
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                ""),
        'image_uri': GObject.ParamSpec.string('image_uri', 'Category Image URI', 'Path to image for this category in the GResource', 
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                "")
    },

    _init: function(params) {
        this.parent(params);
    },

    addArticles: function(articles) {
        this._articles = articles;
    }

    getArticles: function() {
        return this._articles;
    },
});