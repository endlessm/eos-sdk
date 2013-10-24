
const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
const GObject = imports.gi.GObject;
const Lang = imports.lang;

// Local libraries
const ArticleModel = imports.wikipedia.models.article_model;
const CategoryModel = imports.wikipedia.models.category_model;

const DomainWikiModel = new Lang.Class({

    Name: "DomainWikiModel",
    Extends: GObject.Object,

    _init: function(params) {
        this._articles = [];
        this._mainCategory = null;
        this._categories = {};
        this.parent(params);
    },

    loadFromJson: function (json) {
        // Load list of articles
        this._articles = json['articles'].map(function (article) {
            return ArticleModel.newFromJson(article);
        });

        // First create flat list of category models, indexed by ID
        json['categories'].forEach(function (category) {
            let modelObj = CategoryModel.newFromJson(category);
            if (modelObj.is_main_category)
                this._mainCategory = modelObj;
            this._categories[modelObj.id] = modelObj;
            modelObj.has_articles = this._getCategoryHasArticles(modelObj.id);
        }, this);
        // Create links between all the category models in a tree
        json['categories'].forEach(function (category) {
            category['subcategories'].forEach(function(subcatId) {
                this._categories[category['category_name']].addSubcategory(
                    this._categories[subcatId]);
            }, this);
        }, this);
    },

    setLinkedArticles:function(articles){
        this._linked_articles = articles;
    },

    getLinkedArticles:function(){
        return this._linked_articles;
    },

    getArticles: function () {
        return this._articles;
    },

    getArticlesForCategory: function (id) {
        return this._articles.filter(function (article) {
            return article.getCategories().indexOf(id) != -1;
        });
    },

    // A faster version of getArticlesForCategory() that just returns true if
    // there were any articles in this category
    _getCategoryHasArticles: function (id) {
        return this._articles.some(function (article) {
            return article.getCategories().indexOf(id) != -1;
        });
    },

    getCategory: function (id) {
        return this._categories[id];
    },

    getMainCategory: function () {
        return this._mainCategory;
    }
});