
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

    /**
     * Method: loadFromJson
     * Populate the model from the CMS's exported JSON file
     *
     * Parameters:
     *   json - an object created by parsing the JSON file
     *
     * Call this once, when creating the model, to populate it using the JSON
     * file defining the categories and articles.
     * The JSON file adheres to the following format:
     *
     * > <MAIN> =
     * > {
     * >     "categories": [ <CATEGORY>, <CATEGORY>, ... ],
     * >     "articles": [ <ARTICLE>, <ARTICLE>, ... ]
     * > }
     * > <CATEGORY> =
     * > {
     * >     "category_name": <string>,
     * >     "content_text": <string>,
     * >     "image_file": <string>,
     * >     "image_thumb_uri": <string>,
     * >     "is_main_category": <boolean>,
     * >     "subcategories": [ <string>, <string>, ... ]
     * > }
     *
     * "subcategories" is a list of "category_name" strings from other
     * categories.
     * "subcategories" can be empty.
     * "is_main_category" will probably disappear from a future version.
     *
     * > <ARTICLE> =
     * > {
     * >     "title": <string>,
     * >     "url": <string>,
     * >     "categories": [ <string>, <string>, ... ],
     * > }
     *
     * "categories" is a list of "category_name" strings from the categories this
     * article is associated with.
     * "categories" can be empty, but generally should not.
     */
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

    /**
     * Method: getArticles
     * Articles available in this library
     */
    getArticles: function () {
        return this._articles;
    },

    /**
     * Method: getArticlesForCategory
     * Articles belonging to a category in this library
     *
     * Parameters:
     *   id - The string ID of a category
     *
     * Returns:
     *   An array of <ArticleModels> belonging to the category signified by _id_
     *   or an empty array if there were none or _id_ was not found
     */
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

    /**
     * Method: getCategory
     * Category corresponding to a string ID
     *
     * Parameters:
     *   id - The string ID of a category
     *
     * Returns:
     *   A <CategoryModel> that corresponds to _id_, or undefined if _id_ was
     *   not found.
     */
    getCategory: function (id) {
        return this._categories[id];
    },

    /**
     * Method: getMainCategory
     * Category marked as "main" for this library
     *
     * Returns:
     *   A <CategoryModel> that has been marked as the "main" category, or null
     *   if the main category has not been set yet.
     */
    getMainCategory: function () {
        return this._mainCategory;
    }
});