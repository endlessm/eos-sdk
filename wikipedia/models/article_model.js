const Endless = imports.gi.Endless;
const GObject = imports.gi.GObject;
const Lang = imports.lang;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const ArticleModel = new Lang.Class({
    Name: "ArticleModel",
    Extends: GObject.Object,
   	Properties: {
        'title': GObject.ParamSpec.string('title', 'Article Title', 'Human Readable Article Title',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                    ""),
        'uri': GObject.ParamSpec.string('uri', 'Article URI', 'Title URI as stored in wikipedia database', 
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                    "")
    },

    _init: function(params) {
        this._categoryList = [];
        this.parent(params);
    },

    setCategories: function (categoryList) {
        this._categoryList = categoryList;
    },

    addCategory: function (categoryId) {
        if (!this.hasCategory(categoryId))
            this._categoryList.push(categoryId);
    },

    getCategories: function () {
        return this._categoryList;
    },

    hasCategory: function (categoryId) {
        return this._categoryList.indexOf(categoryId) != -1;
    }

});

function newFromJson(json) {
    let retval = new ArticleModel({
        title: json['title'],
        uri: json['url']
    });
    retval.setCategories(json['categories']);
    return retval;
}
